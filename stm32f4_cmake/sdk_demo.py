import time
from scrutiny.sdk.client import ScrutinyClient

client = ScrutinyClient()
try:
    with client.connect('localhost', 8765):
        ld3 = client.watch('/IO/Output/LD3')
        ld4 = client.watch('/IO/Output/LD4')
        ld5 = client.watch('/IO/Output/LD5')
        ld6 = client.watch('/IO/Output/LD6')

        all_leds = [ld3, ld4, ld5, ld6]
        active_led = 0

        while True:
            for i in range(len(all_leds)):
                if i == active_led:
                    all_leds[i].value_enum = 'GPIO_PIN_SET' 
                else:
                    all_leds[i].value_enum = 'GPIO_PIN_RESET'
                
                active_led = (active_led + 1) % len(all_leds)
                time.sleep(0.5)

except KeyboardInterrupt:
    pass