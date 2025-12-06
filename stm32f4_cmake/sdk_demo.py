#    sdk_demo.py
#        A demonstration of the Scrutiny SDK that make a rotating pattern with the LEDs on
#        the board
#
#   - License : MIT - See LICENSE file
#   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-demos)
#
#    Copyright (c) 2025 Scrutiny Debugger

def main():
    import time
    import argparse
    from scrutiny.sdk.client import ScrutinyClient

    parser = argparse.ArgumentParser()
    parser.add_argument('--host', type=str, default='localhost', help='The Scrutiny server hostname')
    parser.add_argument('--port', type=int, default=8765, help='The Scrutiny server port')
    args = parser.parse_args()
    
    client = ScrutinyClient()
    try:
        with client.connect(args.host, args.port):
            x = client.watch('/IMU/Accel_X (G)')
            y = client.watch('/IMU/Accel_Y (G)')
            z = client.watch('/IMU/Accel_Z (G)')
            client.wait_new_value_for_all() # Ensure reading does not fail if no data has been received yet

            ld3 = client.watch('/IO/Output/LD3')
            ld4 = client.watch('/IO/Output/LD4')
            ld5 = client.watch('/IO/Output/LD5')
            ld6 = client.watch('/IO/Output/LD6')

            all_leds = [ld3, ld4, ld6, ld5] # Rotating order
            active_led = 0

            last_timestamp = time.monotonic()
            while True:
                timestamp = time.monotonic()
                if timestamp - last_timestamp > 0.3:    # Next LED
                    last_timestamp = timestamp
                    with client.batch_write():  # Do all 4 write at the same time in a single server request.
                        for i in range(len(all_leds)):
                            all_leds[i].value_enum = 'GPIO_PIN_SET' if i == active_led else 'GPIO_PIN_RESET' 
                    active_led = (active_led + 1) % len(all_leds)
                
                # Print to the accelerometer values to the console
                print(f"\rAcc: X={x.value:+1.02f}, Y={y.value:+1.02f}, Z={z.value:+1.02f}     ", flush=True, end='')
                time.sleep(0.05)   

    except KeyboardInterrupt:
        pass    # Silent exit

if __name__ == '__main__':
    main()