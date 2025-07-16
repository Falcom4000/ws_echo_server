import requests
import json
import argparse

def send_post_request(server_ip, port=80, data=None):
    """
    Send POST request to ESP32 server
    """
    # Construct the URL
    url = f'http://{server_ip}:{port}/post'
    
    try:
        # Convert data to JSON if it's a dictionary
        if isinstance(data, dict):
            headers = {'Content-Type': 'application/json'}
            response = requests.post(url, json=data, headers=headers)
        else:
            # Send as plain text
            headers = {'Content-Type': 'text/plain'}
            response = requests.post(url, data=str(data), headers=headers)

        # Print response details
        print(f"Status Code: {response.status_code}")
        print(f"Response: {response.text}")
        
        return response

    except requests.exceptions.ConnectionError:
        print(f"Error: Could not connect to server at {url}")
    except Exception as e:
        print(f"Error occurred: {str(e)}")

def main():
    # Parse command line arguments
    parser = argparse.ArgumentParser(description='Send POST request to ESP32 server')
    parser.add_argument('--ip', required=True, help='ESP32 IP address')
    parser.add_argument('--port', type=int, default=80, help='Server port (default: 80)')
    parser.add_argument('--data', default='Hello from Python!', help='Data to send')
    
    args = parser.parse_args()

    # Example data to send
    test_data = args.data

    # Send the request
    send_post_request(args.ip, args.port, test_data)

if __name__ == "__main__":
    main()
# python http_client.py --ip 192.168.247.56 --port 80 --data "+"