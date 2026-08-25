def main():
    import struct
    import argparse
    from scrutiny.sdk.client import ScrutinyClient
    from scrutiny import sdk

    parser = argparse.ArgumentParser(description='Set the LED blink frequency via Scrutiny user command')
    parser.add_argument('--host', type=str, default='localhost', help='The Scrutiny server hostname')
    parser.add_argument('--port', type=int, default=8765, help='The Scrutiny server port')
    parser.add_argument('frequency', type=float, help='LED blink frequency in Hz (0.1 to 10)')
    args = parser.parse_args()

    if not (0.1 <= args.frequency <= 10.0):
        parser.error(f'Frequency must be between 0.1 and 10 Hz, got {args.frequency}')

    data = struct.pack('<f', args.frequency)

    client = ScrutinyClient()
    with client.connect(args.host, args.port, wait_status=True):
        server_status = client.get_latest_server_status()
        wanted_config = sdk.RTTLinkConfig(
            target_device="STM32F411CE",
            jlink_interface=sdk.RTTLinkConfig.JLinkInterface.SWD,
            buffer_index=0
        )
                
        if server_status.device_link.config != wanted_config:
            print("Connecting to RTT... ", end='')
            client.configure_device_link(sdk.DeviceLinkType.RTT, wanted_config)
            print("Done")
       
        print("Waiting for device... ",  end='')
        client.wait_device_ready(10)    # Leaves some time to establish a new connection if the link is being changed.
        print("Device ready!")

        print(f"Requesting a frequency change to {args.frequency}Hz... ", end='')
        client.user_command(1, data)
        print("Success!")

if __name__ == '__main__':
    main()
