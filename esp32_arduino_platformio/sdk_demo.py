#    sdk_demo.py
#        A demonstration of the Scrutiny Python SDK connected to the ESP32 Demo
#
#   - License : MIT - See LICENSE file.
#   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-demos)

import argparse
import sys
import time
from scrutiny.sdk.client import ScrutinyClient

def main():
    parser = argparse.ArgumentParser(description="Scrutiny ESP32 SDK Demo")
    parser.add_argument('--host', type=str, default='localhost', help='Scrutiny server hostname')
    parser.add_argument('--port', type=int, default=8765, help='Scrutiny server port')
    args = parser.parse_args()

    client = ScrutinyClient()
    try:
        print(f"Connecting to Scrutiny server at {args.host}:{args.port}...")
        client.connect(args.host, args.port)

        print("Waiting for ESP32 device to be ready...")
        client.wait_device_ready(timeout=10.0)

        sfd = client.get_loaded_sfd()
        if sfd:
            print(f"Connected to firmware: {sfd.metadata.project_name} v{sfd.metadata.version} ({sfd.firmware_id})")

        print("Watching demo variables via aliases...")
        v_counter = client.watch('/Demo/Cycle_Counter')
        v_sine = client.watch('/Demo/Sine_Wave')
        v_heartbeat = client.watch('/Demo/Heartbeat')
        client.wait_new_value_for_all(timeout=5.0)

        print("\nStreaming live values (Press Ctrl+C to stop):")
        print(f"{'Time (s)':<10} | {'Cycle Counter':<18} | {'Sine Wave':<15} | {'Heartbeat':<10}")
        print("-" * 60)

        start = time.time()
        while True:
            elapsed = time.time() - start
            print(f"{elapsed:<10.2f} | {v_counter.value:<18} | {v_sine.value:<15.4f} | {str(v_heartbeat.value):<10}", flush=True)
            time.sleep(0.2)

    except KeyboardInterrupt:
        print("\nStopping demo...")
    finally:
        client.disconnect()

if __name__ == '__main__':
    main()
