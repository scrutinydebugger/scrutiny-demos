import time
from scrutiny.sdk.client import ScrutinyClient

def main():
    client = ScrutinyClient()
    with client.connect('localhost', 8765):
        x = client.watch('IMU/Accelerometer/si/Accel_X (m_s^2)')
        y = client.watch('IMU/Accelerometer/si/Accel_Y (m_s^2)')
        z = client.watch('IMU/Accelerometer/si/Accel_Z (m_s^2)')
        client.wait_new_value_for_all()
        
        while True:
            print(f"\rAcc: X={x.value:+1.02f}, Y={y.value:+1.02f}, Z={z.value:+1.02f}     ", flush=True, end='')
            time.sleep(0.1)

if __name__ == '__main__':
    main()