#Requires -Version 5.1
$ErrorActionPreference = 'Stop'

# This script executes the Scrutiny postbuild steps using powershell
# Usage : powershell scrutiny_postbuild.ps1 <path_to_elf>
# Can be invoked by CCS as a post-build step

function fatal($msg, $code = 1) { Write-Error "[Error] $msg"; exit $code }
function info($msg)             { Write-Host "[Info] $msg" }

# Work from where the script is
$SCRIPT_DIR = Split-Path -Parent $MyInvocation.MyCommand.Path
Set-Location $SCRIPT_DIR

info "Running Scrutiny post build script : $($MyInvocation.MyCommand.Name)"

if (-not (Get-Command scrutiny -ErrorAction SilentlyContinue)) {
    fatal "scrutiny not in PATH"
}

$ELFFILE = $args[0]

if ([string]::IsNullOrEmpty($ELFFILE)) { fatal "Missing .elf firmware path" }
$ELFFILE = Resolve-Path $ELFFILE | Select-Object -ExpandProperty Path
if (-not (Test-Path $ELFFILE -PathType Leaf)) { fatal "File does not exist" }

$BUILD_DIR               = Split-Path -Parent $ELFFILE
$ELFFILE_BASENAME_NO_EXT = [System.IO.Path]::GetFileNameWithoutExtension($ELFFILE)
$ELFFILE_TAGGED_NAME     = "${ELFFILE_BASENAME_NO_EXT}_tagged.out"
$SFD_FILENAME            = "${ELFFILE_BASENAME_NO_EXT}.sfd"
Set-Location $BUILD_DIR

$WORKFOLDER = "sfd_workfolder"
if (Test-Path $WORKFOLDER) { Remove-Item -Recurse -Force $WORKFOLDER }
New-Item -ItemType Directory -Name $WORKFOLDER | Out-Null

scrutiny get-firmware-id $ELFFILE --output $WORKFOLDER
scrutiny tag-firmware-id $ELFFILE $ELFFILE_TAGGED_NAME
scrutiny elf2varmap $ELFFILE    `
    --output $WORKFOLDER        `
    --dereference-pointers      `
    --path_ignore_patterns      `
        "/global/typeinfo*"     `
        "/global/vtable*"       `
        "*__vptr"               `
        "/global/__TI*"         `
        "/global/std/*"         `
        "/global/_sys_memory/*"
scrutiny add-alias $WORKFOLDER --file ..\aliases\alias_hardware.json ..\aliases\alias_plant.json        
scrutiny make-metadata --project "C2000 Testapp" --version "1.0" --output $WORKFOLDER
scrutiny make-sfd $WORKFOLDER $SFD_FILENAME --install

info "Scrutiny post build script executed successfully!"
info "  - Tagged firmware: ${BUILD_DIR}\${ELFFILE_TAGGED_NAME}"
info "  - SFD file: ${BUILD_DIR}\${SFD_FILENAME}"
