import socket
import time
import random
from datetime import datetime

AKS_PORT = 5000
ANS_PORT = 5001

# Create UDP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind(('localhost', ANS_PORT))  # Bind the socket to the ANS_PORT

# Log the start of the simulator
print(f"{datetime.now().strftime('%Y-%m-%d %H:%M:%S.%f')[:-3]} ANS Simulator running on port {ANS_PORT}")

latitude = 0.0
longitude = 0.0
altitude = 10.0

# Initialize connection status variables
aks_heartbeat_received = False
connected = False
missed_heartbeats = 0

# Log waiting for AKS heartbeat
print(f"{datetime.now().strftime('%Y-%m-%d %H:%M:%S.%f')[:-3]} Waiting for initial AKS heartbeat...")

while True:
    try:
        # Check for incoming messages (non-blocking)
        sock.setblocking(False)
        try:
            data, addr = sock.recvfrom(1024)  # Receive data from the socket
            message = data.decode()  # Decode the received message
            print(f"{datetime.now().strftime('%Y-%m-%d %H:%M:%S.%f')[:-3]} Received from AKS: {message}")
            if message == "AKS_HEARTBEAT":  # Check for AKS heartbeat message
                if not connected:
                    print(f"{datetime.now().strftime('%Y-%m-%d %H:%M:%S.%f')[:-3]} Connected to AKS")
                    connected = True  # Update connection status
                aks_heartbeat_received = True  # Heartbeat received
                missed_heartbeats = 0  # Reset missed heartbeats counter
        except socket.error:
            if connected:
                missed_heartbeats += 1  # Increment missed heartbeats if connected
        sock.setblocking(True)  # Set socket to blocking mode

        if connected:
            if aks_heartbeat_received:
                # Send heartbeat to AKS
                sock.sendto(b"ANS_HEARTBEAT", ('localhost', AKS_PORT))

                # Send platform info to AKS
                platform_info = "PLATFORM_INFO,{:.6f},{:.6f},{:.2f}".format(latitude, longitude, altitude).encode()
                sock.sendto(platform_info, ('localhost', AKS_PORT))

                # Update position
                latitude = (latitude + 1) % 91
                longitude = (longitude + 1) % 181
                altitude += random.uniform(-4, 10)

            # Check for lost connection
            if missed_heartbeats >= 5:
                print(f"{datetime.now().strftime('%Y-%m-%d %H:%M:%S.%f')[:-3]} Lost connection with AKS")
                connected = False
                aks_heartbeat_received = False

        time.sleep(1)  # Sleep for a second before the next iteration
    except KeyboardInterrupt:
        # Handle shutdown gracefully
        print(f"{datetime.now().strftime('%Y-%m-%d %H:%M:%S.%f')[:-3]} ANS Simulator shutting down")
        break

sock.close()  # Close the socket when done