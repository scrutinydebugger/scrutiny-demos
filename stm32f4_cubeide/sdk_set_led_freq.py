def main():
    import struct
    import argparse
    from scrutiny.sdk.client import ScrutinyClient

    parser = argparse.ArgumentParser(description='Set the LED blink frequency via Scrutiny user command')
    parser.add_argument('--host', type=str, default='localhost', help='The Scrutiny server hostname')
    parser.add_argument('--port', type=int, default=8765, help='The Scrutiny server port')
    parser.add_argument('frequency', type=float, help='LED blink frequency in Hz (0.1 to 10)')
    args = parser.parse_args()

    if not (0.1 <= args.frequency <= 10.0):
        parser.error(f'Frequency must be between 0.1 and 10 Hz, got {args.frequency}')

    data = struct.pack('<f', args.frequency)

    client = ScrutinyClient()
    with client.connect(args.host, args.port):
        print(f"Requesting a frequency change to {args.frequency}Hz...", end='')
        client.user_command(1, data)
        print(" Success!")

if __name__ == '__main__':
    main()
