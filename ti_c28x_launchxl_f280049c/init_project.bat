@echo off
setlocal

cd /d "%~dp0"
git submodule update --init scrutiny-embedded

if not exist "scrutiny_build_config.hpp" (
    copy "scrutiny-embedded\lib\inc\scrutiny_build_config.hpp.default" "scrutiny_build_config.hpp"
)
