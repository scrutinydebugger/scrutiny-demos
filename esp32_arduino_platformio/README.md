# ESP32 Arduino PlatformIO Demo

This project demonstrates how to instrument an **ESP32** microcontroller using the **Scrutiny Debugger** framework with **PlatformIO** and the **Arduino framework**.

With this demo, the ESP32 runs firmware that updates live variables in memory, while Scrutiny reads and writes those variables over a standard USB serial connection and plots them in real time in a desktop GUI—without stopping CPU execution.

---

## 1. What This Demo Does

When this demo is running:
- The ESP32 continuously updates four telemetry variables:
  - `g_cycle_counter`: A 32-bit unsigned integer incremented on every pass through `loop()`.
  - `g_sine_wave`: A 32-bit float generating a smooth sine wave.
  - `g_heartbeat`: A boolean toggling true/false every 500 ms.
  - `g_analog_read`: A 16-bit unsigned integer simulating an analog sensor value.
- Scrutiny Embedded runs inside `loop()`, exchanging data with your computer over the USB serial port at 115200 baud.
- The Scrutiny desktop GUI connects to the ESP32 through the Scrutiny background server, letting you:
  - Watch variable values live with zero CPU halts or breakpoints.
  - Plot the sine wave and cycle counter in real time using Continuous Graphs.
  - Interact with convenient human-friendly aliases (such as `/Demo/Sine_Wave` instead of `/global/g_sine_wave`).
  - Automate testing or headless data collection using the Python SDK.

---

## 2. How the System Works

A Scrutiny setup consists of four main parts:

```text
+-----------------------+              USB Serial (e.g. COM7 or /dev/ttyUSB0)
|      ESP32 Board      | <=====================================================> +-----------------------------+
|  (Firmware + Scrutiny |                                                         |    Scrutiny PC Server       |
|   Embedded Library)   |                                                         |  (Runs in background on PC) |
+-----------------------+                                                         +-----------------------------+
                                                                                                 ^
                                                                                                 | TCP localhost:8765
                                                                                                 v
                                              +-----------------------+           +-----------------------------+
                                              | Scrutiny Firmware     |           |     Desktop GUI / Python    |
                                              | Description (.sfd)    | --------> |     (Visual Graph & Watch)  |
                                              +-----------------------+           +-----------------------------+
```

1. **ESP32 Firmware**: Contains your application code plus the lightweight Scrutiny Embedded C++ library.
2. **Serial Connection**: The ESP32 and PC communicate using Scrutiny's binary protocol over the board's existing USB serial chip (CP2102, CH340, etc.).
3. **Scrutiny Server**: A local PC background process that manages the serial link, decodes telemetry packets, and serves a WebSocket/TCP API on port `8765`.
4. **Scrutiny Firmware Description (`.sfd`)**: A zip archive created during compilation that contains the memory map (variable names, data types, and RAM addresses extracted from DWARF debug symbols) and a unique **Firmware ID**. When the ESP32 connects, the server reads the Firmware ID from the board and automatically loads the matching `.sfd`.
5. **Scrutiny GUI / SDK**: Desktop tools running on your PC that connect to the server to display graphs, watch windows, and dashboards.

---

## 3. Requirements

### Hardware
- Any standard ESP32 development board (e.g., ESP32 Dev Module, NodeMCU-32S, ESP32-WROOM-32, ESP32 DevKitC).
- A micro-USB or USB-C data cable (ensure it supports data transfer, not charging only).

### Software (on your PC)
1. **Python 3.10 to 3.13**: [python.org](https://www.python.org/) *(Note: Scrutiny supports Python 3.10 up to 3.13; Python 3.14+ is not currently supported).*
2. **PlatformIO Core (CLI) or PlatformIO IDE (VS Code)**: [platformio.org](https://platformio.org/)
3. **Scrutiny Debugger**: Install via pip:
   ```bash
   pip install scrutinydebugger
   ```

---

## 4. Clone and Dependency Setup

This demo uses `scrutiny-embedded` as a Git submodule. You must clone recursively so that the library source files are downloaded.

### If you are cloning for the first time:
```bash
git clone --recurse-submodules https://github.com/scrutinydebugger/scrutiny-demos.git
cd scrutiny-demos/esp32_arduino_platformio
```

### If you already cloned without submodules:
```bash
cd scrutiny-demos/esp32_arduino_platformio
git submodule update --init --recursive
```

Verify that the submodule directory exists at `scrutiny-embedded/lib/inc/scrutiny.hpp`.

---

## 5. Build the Firmware

Run the build command from the `esp32_arduino_platformio` directory:

```bash
# Run from scrutiny-demos/esp32_arduino_platformio
pio run
```

### What Happens During the Build:
PlatformIO compiles the ESP32 Arduino framework and the embedded Scrutiny library with `-gdwarf-4` debug symbols. Then, the custom SCons script ([`scrutiny_postbuild.py`](scrutiny_postbuild.py)) automatically executes:
1. `scrutiny get-firmware-id`: Computes a unique 16-byte hash representing this specific binary build.
2. `scrutiny elf2varmap`: Parses DWARF symbols from the ELF file to extract all variable addresses and types.
3. `scrutiny make-metadata`: Embeds project metadata (name, version, author).
4. `scrutiny add-alias`: Imports human-readable paths defined in [`aliases/alias.json`](aliases/alias.json).
5. `scrutiny make-sfd`: Packages these components into `.pio/build/esp32dev/firmware.sfd` and installs it into your local Scrutiny registry.
6. `scrutiny tag-firmware-id`: Injects the unique Firmware ID into `.pio/build/esp32dev/firmware_tagged.elf`.
7. `esptool.py elf2image`: Generates the final flashable `.pio/build/esp32dev/firmware.bin` directly from the tagged ELF.

### Build Outputs:
- `.pio/build/esp32dev/firmware.bin` — The flashable binary with embedded firmware ID.
- `.pio/build/esp32dev/firmware_tagged.elf` — The tagged ELF containing symbols and firmware ID.
- `.pio/build/esp32dev/firmware.sfd` — The matching Scrutiny Firmware Description file.

---

## 6. Flash the Board

Connect your ESP32 board to your computer with the USB cable.

### Step 1: Identify Your Serial Port
- **Windows**: Open Device Manager -> **Ports (COM & LPT)** (for example, `COM7`).
- **Linux**: Run `ls /dev/ttyUSB*` or `ls /dev/ttyACM*` (for example, `/dev/ttyUSB0`).
- **macOS**: Run `ls /dev/cu.usbserial*` or `ls /dev/cu.wchusbserial*`.

### Step 2: Upload Firmware
PlatformIO will usually detect the port automatically:

```bash
# Run from scrutiny-demos/esp32_arduino_platformio
pio run --target upload
```

If you have multiple devices connected, specify your port explicitly:

**Windows (PowerShell):**
```powershell
pio run --target upload --upload-port COM7
```

**Linux / macOS:**
```bash
pio run --target upload --upload-port /dev/ttyUSB0
```

Once upload finishes, the ESP32 automatically restarts and begins running the demo.

---

## 7. Run Scrutiny

### Step 1: Configure the Serial Port in the Server Config
Open [`scrutiny_server_config.json`](scrutiny_server_config.json) in a text editor and make sure `"portname"` matches your ESP32 port:

```json
{
    "name": "ESP32 Arduino Demo",
    "api": {
        "client_interface_config": {
            "host": "127.0.0.1",
            "port": 8765
        }
    },
    "device": {
        "response_timeout": 1.0,
        "link_type": "serial",
        "link_config": {
            "portname": "COM7",
            "baudrate": 115200,
            "stopbits": 1,
            "databits": 8,
            "parity": "none"
        }
    }
}
```
*(On Linux, change `"COM7"` to `"/dev/ttyUSB0"`).*

### Step 2: Launch the Scrutiny Server
Open a terminal and start the server:

```bash
# Run from scrutiny-demos/esp32_arduino_platformio
scrutiny server --config scrutiny_server_config.json
```

Expected output:
```text
[INFO] <ScrutinyServer> Starting server instance "ESP32 Arduino Demo"
[INFO] <TCPClientHandler> Starting TCP socket listener on 127.0.0.1:8765
[INFO] <DeviceHandler> Found a device. "ESP32 PlatformIO Demo" (ID: 49089a7f...)
[INFO] <DeviceHandler> Connected to device with session ID 0x00000000
[INFO] <DeviceHandler> Communication with device fully ready.
[INFO] <ActiveSFDHandler> Loading firmware description file (SFD)...
```

Leave this terminal window open. The server must remain running while you use the GUI or SDK.

> [!NOTE]
> **When is `scrutiny install-sfd` needed?**
> - **Local build**: `pio run` automatically installs the generated `.sfd` into your local machine's Scrutiny registry during post-build. You do not need to run `install-sfd` manually.
> - **Prebuilts / Second PC**: If you are using the precompiled binaries in [`prebuilt/`](prebuilt/) or running the Scrutiny server on a computer different from where compilation took place, install the SFD once:
>   ```bash
>   scrutiny install-sfd prebuilt/esp32_demo.sfd
>   ```

### Step 3: Launch the GUI
Open a **second terminal** and launch the desktop interface:

```bash
scrutiny gui
```

Click **Connect** in the GUI toolbar to connect to the local server (`localhost:8765`).

---

## 8. Open the Dashboard & Plot Live Graphs

### Option A: Load the Preconfigured Dashboard
This demo includes a preconfigured dashboard layout in [`gui_dashboards/esp32_demo_dashboard.scdb`](gui_dashboards/esp32_demo_dashboard.scdb).

1. In the Scrutiny GUI, click **File -> Open Dashboard**.
2. Select `gui_dashboards/esp32_demo_dashboard.scdb`.

### What You Will See:
- **Continuous Graph**: A rolling live chart plotting:
  - `alias:/Demo/Sine_Wave` — Smooth oscillating waveform.
  - `alias:/Demo/Cycle_Counter` — Continuously climbing loop count.
- **Watch Window**: A structured table displaying live values, types, and updates for:
  - `Cycle Counter` (`alias:/Demo/Cycle_Counter`)
  - `Sine Wave` (`alias:/Demo/Sine_Wave`)
  - `Heartbeat` (`alias:/Demo/Heartbeat`)
  - `Analog Read` (`alias:/Demo/Analog_Read`)
- **Variable List**: Searchable hierarchy on the right panel showing all variables extracted from firmware DWARF symbols.

### Option B: Plot `/Demo/Sine_Wave` Manually from Scratch
If you prefer to configure your own graph:
1. Open a new Continuous Graph widget: Click **Components -> Continuous Graph**.
2. Locate the variable: On the right-hand **Variable List** panel, expand the **alias** folder, then expand **Demo**.
3. Add to graph: Drag `Sine_Wave` directly onto the graph area, or right-click `Sine_Wave` and choose **Add to Graph**.
4. The graph will immediately begin plotting the live sine wave in real time.

---

## 9. Use the Python SDK Example

Scrutiny provides a full Python SDK for automated testing, scripts, and headless telemetry streaming.

A complete example script is provided in [`sdk_demo.py`](sdk_demo.py). With the Scrutiny server running, open another terminal and run:

```bash
# Run from scrutiny-demos/esp32_arduino_platformio
python sdk_demo.py --host 127.0.0.1 --port 8765
```

Sample output:
```text
Connecting to Scrutiny server at 127.0.0.1:8765...
Waiting for ESP32 device to be ready...
Connected to firmware: ESP32 Arduino Demo v1.0.0 (49089a7f1f4dece33efb9bdad55b0ccf)
Watching demo variables via aliases...

Streaming live values (Press Ctrl+C to stop):
Time (s)   | Cycle Counter      | Sine Wave       | Heartbeat 
------------------------------------------------------------
0.00       | 13960804           | 0.7832          | False     
0.30       | 13963459           | 0.9320          | True      
0.60       | 13966183           | 0.9976          | True      
0.90       | 13968851           | 0.9734          | False     
1.20       | 13971532           | 0.8618          | True      
```

*(Note: The exact firmware ID and counter values will reflect your build and uptime; the values above illustrate a successful stream).*

Press `Ctrl+C` to cleanly exit the script.

---

## 10. Use Prebuilt Artifacts

If you do not have PlatformIO installed or want to verify hardware immediately, precompiled artifacts from the reference build are provided in [`prebuilt/`](prebuilt/):
- `esp32_demo.bin` — Tagged flashable binary.
- `esp32_demo_tagged.elf` — Tagged ELF with debug symbols.
- `esp32_demo.sfd` — Matching Scrutiny Firmware Description.

### Step 1: Install the Prebuilt SFD
```bash
scrutiny install-sfd prebuilt/esp32_demo.sfd
```

### Step 2: Flash the Prebuilt Binary
On ESP32 boards using standard partition tables, application firmware is loaded at flash address **`0x10000`**. You can flash using `esptool.py`:

**Windows (PowerShell):**
```powershell
esptool.py --chip esp32 --port COM7 write_flash 0x10000 prebuilt/esp32_demo.bin
```

**Linux / macOS:**
```bash
esptool.py --chip esp32 --port /dev/ttyUSB0 write_flash 0x10000 prebuilt/esp32_demo.bin
```

> [!WARNING]
> Flashing only the app binary at offset `0x10000` assumes your ESP32 already contains standard Arduino bootloader (`0x1000`), partition table (`0x8000`), and `boot_app0` (`0xe000`) images from prior Arduino or ESP-IDF use. If your ESP32 has been completely erased (`esptool.py erase_flash`), build and upload via PlatformIO (`pio run --target upload`), which automatically writes all necessary partition images.

---

## 11. Troubleshooting

### 1. Serial Port Not Found or Device Not Detected
- Ensure your cable supports data transfer (many cheap USB cables are charge-only).
- Check that the USB-UART bridge driver is installed:
  - **CP210x**: Silicon Labs CP210x VCP driver.
  - **CH340/CH341**: WCH CH340 driver.
- On Linux, ensure your user has permission to open serial ports:
  ```bash
  sudo usermod -a -G dialout $USER
  ```
  *(Log out and log back in for changes to take effect).*

### 2. "Access is Denied" / Port Busy
Only one program can open a serial port at a time:
- Close serial monitors (such as Arduino IDE Serial Monitor, PuTTY, or PlatformIO Monitor).
- Close any other running instances of `scrutiny server`.

### 3. Missing SFD or Firmware ID Mismatch
If the server reports:
```text
No SFD matching firmware ID <hash> could be found
```
This means the firmware on your board was built separately from the SFD registered with the server.
- Re-run `pio run` and `pio run --target upload` so the board and server use the same build.
- Or install the SFD generated during your build:
  ```bash
  scrutiny install-sfd .pio/build/esp32dev/firmware.sfd
  ```
- Check installed SFDs at any time with:
  ```bash
  scrutiny list-sfd
  ```

### 4. Submodule / Compilation Errors
If files like `scrutiny.hpp` are missing during build:
```bash
git submodule update --init --recursive
```

---

## 12. How the Build Pipeline Works

On microcontrollers like the ESP32, the linking process produces an ELF executable, but the chip boots from a flashable binary image generated by `esptool.py elf2image`.

If Scrutiny tags the ELF *after* PlatformIO has already converted it to `.bin`, the flashed binary will be missing the tag, causing a Firmware ID mismatch when the server connects.

The [`scrutiny_postbuild.py`](scrutiny_postbuild.py) script solves this cleanly:
1. **Source Inclusion (`PRE` hook)**: Calls SCons `env.BuildSources()` to compile `scrutiny-embedded` directly into the project object graph with `-gdwarf-4` symbols without altering system paths.
2. **Post-Action (`env.AddPostAction`)**: Hooks into the `${PROGNAME}.bin` build target.
3. **Symbol & Metadata Extraction**: Generates the varmap and metadata, attaches aliases, and builds the `.sfd`.
4. **Firmware Tagging**: Writes the Firmware ID into a dedicated section of the ELF (`firmware_tagged.elf`).
5. **Binary Re-generation**: Re-runs `esptool.py elf2image` on the *tagged* ELF, ensuring that `.pio/build/esp32dev/firmware.bin` contains the exact same Firmware ID. Flashing through PlatformIO (`pio run --target upload`) flashes the verified tagged binary every time.

---

## 13. Demo Video

This walkthrough video demonstrates how to set up and run the ESP32 Scrutiny demo, monitor live variables, and view the sine-wave plot in the Scrutiny GUI:

[![ESP32 Scrutiny Demo Walkthrough](https://img.youtube.com/vi/bMgFpknRrbE/maxresdefault.jpg)](https://www.youtube.com/watch?v=bMgFpknRrbE "Watch the ESP32 Scrutiny Demo on YouTube")

*(Click the thumbnail above or [watch directly on YouTube](https://www.youtube.com/watch?v=bMgFpknRrbE))*


