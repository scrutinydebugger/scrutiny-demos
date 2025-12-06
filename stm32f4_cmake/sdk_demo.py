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
            ld3 = client.watch('/IO/Output/LD3')
            ld4 = client.watch('/IO/Output/LD4')
            ld5 = client.watch('/IO/Output/LD5')
            ld6 = client.watch('/IO/Output/LD6')

            all_leds = [ld3, ld4, ld6, ld5] # Rotating order
            active_led = 0

            while True:
                with client.batch_write():  # Do all 4 write at the same time in a single server request.
                    for i in range(len(all_leds)):
                        if i == active_led:
                            all_leds[i].value_enum = 'GPIO_PIN_SET' 
                        else:
                            all_leds[i].value_enum = 'GPIO_PIN_RESET'
                    
                active_led = (active_led + 1) % len(all_leds)
                time.sleep(0.3)

    except KeyboardInterrupt:
        pass    # Silent exit

if __name__ == '__main__':
    main()