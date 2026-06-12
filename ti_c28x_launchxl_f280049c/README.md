

# Steps to recreate this project from scratch

1. Created an empty project for LaunchXL F280049C
2. Edited c2000.sysconfig to enable SCIA on pins 28/29 (UART through debug probe)
3. Edited the project configuration to select XDS110 USB debug probe. Default value given by the project creator is wrong for this board.
4. Deleted all build configs except CPU1_Flash
5. Fetch Scrutiny and copy the default config (see ``init_scrutiny.[bat|sh]``)
6. Exclude from build every folder from scrutiny-embedded, except the ``lib`` folder.
7. Added include paths
    - ${PROJECT_ROOT}/scrutiny-embedded/lib/inc
    - ${PROJECT_ROOT}
8. Configure compiler options: Optimization, ELF eabi, debug symbols in DWARF v4
9. Write code and linker script