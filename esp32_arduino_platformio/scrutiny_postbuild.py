#    scrutiny_postbuild.py
#        PlatformIO SCons build script for Scrutiny Debugger integration
#
#   - License : MIT - See LICENSE file.
#   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-demos)

Import("env")
import os
import subprocess
import shutil
import sys

# 1. Locate scrutiny-embedded library relative to this project
project_dir = env.subst("$PROJECT_DIR")
submodule_lib = os.path.join(project_dir, "scrutiny-embedded", "lib")
sibling_lib = os.path.abspath(os.path.join(project_dir, "..", "..", "scrutiny-embedded", "lib"))

if os.path.isdir(os.path.join(submodule_lib, "inc")):
    scrutiny_lib_dir = submodule_lib
elif os.path.isdir(os.path.join(sibling_lib, "inc")):
    scrutiny_lib_dir = sibling_lib
else:
    sys.stderr.write(
        f"\n[Scrutiny] Error: scrutiny-embedded not found at:\n"
        f"  {submodule_lib}\n"
        f"  {sibling_lib}\n"
        "Please initialize git submodules using:\n"
        "  git submodule update --init --recursive\n\n"
    )
    sys.exit(1)

env.Append(CPPPATH=[os.path.join(scrutiny_lib_dir, "inc")])
env.BuildSources(
    os.path.join("$BUILD_DIR", "scrutiny_embedded"),
    os.path.join(scrutiny_lib_dir, "src")
)

def find_scrutiny_cmd():
    # 1. System PATH
    scrutiny_bin = shutil.which("scrutiny")
    if scrutiny_bin:
        return [scrutiny_bin]

    # 2. Workspace virtual environment (relative to project)
    venv_win = os.path.abspath(os.path.join(project_dir, "..", "..", "scrutiny-main", ".venv", "Scripts", "python.exe"))
    if os.path.isfile(venv_win):
        return [venv_win, "-m", "scrutiny"]
    venv_posix = os.path.abspath(os.path.join(project_dir, "..", "..", "scrutiny-main", ".venv", "bin", "python"))
    if os.path.isfile(venv_posix):
        return [venv_posix, "-m", "scrutiny"]

    # 3. Active Python running SCons
    try:
        res = subprocess.run([sys.executable, "-m", "scrutiny", "version"], capture_output=True, text=True)
        if res.returncode == 0:
            return [sys.executable, "-m", "scrutiny"]
    except Exception:
        pass

    # 4. Fallback
    return ["python", "-m", "scrutiny"]

# 2. Define post-action on the binary target
def scrutiny_postbuild_action(source, target, env):
    bin_path = str(target[0])
    build_dir = os.path.dirname(bin_path)
    progname = env.subst("$PROGNAME")
    elf_path = os.path.join(build_dir, f"{progname}.elf")
    tagged_elf_path = os.path.join(build_dir, f"{progname}_tagged.elf")
    sfd_path = os.path.join(build_dir, f"{progname}.sfd")
    workfolder = os.path.join(build_dir, "_scrutiny_work")

    print("\n" + "=" * 50)
    print(f"[Scrutiny Postbuild] Running Scrutiny postbuild for {progname}.elf")
    print("=" * 50)

    if os.path.exists(workfolder):
        shutil.rmtree(workfolder)
    os.makedirs(workfolder, exist_ok=True)

    scrutiny_cmd = find_scrutiny_cmd()

    # Step A: get-firmware-id
    cmd = scrutiny_cmd + ["get-firmware-id", elf_path, "--output", workfolder]
    res = subprocess.run(cmd, capture_output=True, text=True)
    if res.returncode != 0:
        print(f"[Scrutiny] get-firmware-id failed:\n{res.stderr}")
        sys.exit(1)
    with open(os.path.join(workfolder, "firmwareid"), "r") as f:
        fw_id = f.read().strip()
    print(f"[Scrutiny] Firmware ID: {fw_id}")

    # Step B: elf2varmap
    cmd = scrutiny_cmd + ["elf2varmap", elf_path, "--output", workfolder, "--dereference-pointers"]
    cxxfilt = env.subst("$CXXFILT")
    if cxxfilt and os.path.exists(cxxfilt):
        cmd.extend(["--cppfilt", cxxfilt])
    res = subprocess.run(cmd, capture_output=True, text=True)
    if res.returncode != 0:
        print(f"[Scrutiny] elf2varmap failed:\n{res.stderr}")
        sys.exit(1)
    print(f"[Scrutiny] VarMap generated.")

    # Step C: make-metadata
    cmd = scrutiny_cmd + [
        "make-metadata",
        "--project-name", "ESP32 Arduino Demo",
        "--version", "1.0.0",
        "--author", "Scrutiny Debugger",
        "--output", workfolder
    ]
    res = subprocess.run(cmd, capture_output=True, text=True)
    if res.returncode != 0:
        print(f"[Scrutiny] make-metadata failed:\n{res.stderr}")
        sys.exit(1)
    print(f"[Scrutiny] Metadata generated.")

    # Step D: add-alias (if aliases/alias.json exists)
    alias_path = os.path.join(project_dir, "aliases", "alias.json")
    if os.path.isfile(alias_path):
        cmd = scrutiny_cmd + ["add-alias", workfolder, "--file", alias_path]
        res = subprocess.run(cmd, capture_output=True, text=True)
        if res.returncode != 0:
            print(f"[Scrutiny] Warning: add-alias failed:\n{res.stderr}")
        else:
            print(f"[Scrutiny] Aliases added from {alias_path}.")

    # Step E: make-sfd
    cmd = scrutiny_cmd + ["make-sfd", workfolder, sfd_path, "--install"]
    res = subprocess.run(cmd, capture_output=True, text=True)
    if res.returncode != 0:
        print(f"[Scrutiny] make-sfd failed:\n{res.stderr}")
        sys.exit(1)
    print(f"[Scrutiny] SFD generated and installed: {sfd_path}")

    # Step F: tag-firmware-id
    cmd = scrutiny_cmd + ["tag-firmware-id", elf_path, tagged_elf_path]
    res = subprocess.run(cmd, capture_output=True, text=True)
    if res.returncode != 0:
        print(f"[Scrutiny] tag-firmware-id failed:\n{res.stderr}")
        sys.exit(1)
    print(f"[Scrutiny] Tagged ELF written to: {tagged_elf_path}")

    # Step G: Re-generate bin from tagged ELF
    print(f"[Scrutiny] Converting tagged ELF to binary: {bin_path}...")
    python_exe = env.subst("$PYTHONEXE")
    objcopy = env.subst("$OBJCOPY")
    if not os.path.isfile(objcopy) and os.path.isfile(objcopy + ".exe"):
        objcopy = objcopy + ".exe"

    board_mcu = env.subst("${BOARD_MCU}") or "esp32"
    flash_mode = env.BoardConfig().get("build.flash_mode", "dio")

    raw_freq = str(env.BoardConfig().get("build.f_flash", "40m")).replace("L", "")
    if raw_freq.isdigit():
        flash_freq = f"{int(raw_freq) // 1000000}m"
    else:
        flash_freq = raw_freq

    flash_size = env.BoardConfig().get("upload.flash_size", "4MB")

    if objcopy.endswith(".py"):
        esptool_cmd = [python_exe, objcopy]
    elif os.path.isfile(objcopy):
        esptool_cmd = [objcopy]
    else:
        esptool_cmd = [python_exe, "-m", "esptool"]

    esptool_cmd.extend([
        "--chip", board_mcu,
        "elf2image",
        "--flash-mode", flash_mode,
        "--flash-freq", flash_freq,
        "--flash-size", flash_size,
        "-o", bin_path,
        tagged_elf_path
    ])

    res = subprocess.run(esptool_cmd, capture_output=True, text=True)
    if res.returncode != 0:
        print(f"[Scrutiny] esptool execution failed:\n{res.stderr}\nCommand: {' '.join(esptool_cmd)}")
        sys.exit(1)

    print(f"[Scrutiny] Tagged binary ready: {bin_path}")
    print("=" * 50 + "\n")

env.AddPostAction(
    os.path.join("$BUILD_DIR", "${PROGNAME}.bin"),
    scrutiny_postbuild_action
)
