# File: simulators/LauncherSimulator.py
import socket
import time
from datetime import datetime

AKS_PORT = 5000  # Port for AKS communication
LAUNCHER_PORT = 5002  # Port for Launcher Simulator

# Create UDP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)  # Initialize a UDP socket
sock.bind(('localhost', LAUNCHER_PORT))  # Bind the socket to the local host and specified port

print(f"{datetime.now().strftime('%Y-%m-%d %H:%M:%S.%f')[:-3]} Launcher Simulator running on port {LAUNCHER_PORT}")

aks_heartbeat_received = False  # Flag to track if AKS heartbeat is received
connected = False  # Flag to track connection status
missed_heartbeats = 0  # Counter for missed heartbeats

print(f"{datetime.now().strftime('%Y-%m-%d %H:%M:%S.%f')[:-3]} Waiting for initial AKS heartbeat...")

while True:
    try:
        # Check for incoming messages (non-blocking)
        sock.setblocking(False)  # Set socket to non-blocking mode
        try:
            data, addr = sock.recvfrom(1024)  # Receive data from the socket
            message = data.decode()  # Decode the received message
            print(f"{datetime.now().strftime('%Y-%m-%d %H:%M:%S.%f')[:-3]} Received from AKS: {message}")
            if message == "AKS_HEARTBEAT":  # Check if the received message is a heartbeat
                if not connected:  # If not already connected
                    print(f"{datetime.now().strftime('%Y-%m-%d %H:%M:%S.%f')[:-3]} Connected to AKS")  # Log connection
                    connected = True  # Update connection status
                aks_heartbeat_received = True  # Update heartbeat status
                missed_heartbeats = 0  # Reset missed heartbeats counter
        except socket.error:  # Handle socket errors
            if connected:  # If connected, increment missed heartbeats
                missed_heartbeats += 1
        sock.setblocking(True)  # Set socket back to blocking mode

        if connected:  # If connected to AKS
            if aks_heartbeat_received:  # If a heartbeat was received
                # Send heartbeat
                sock.sendto(b"LAUNCHER_HEARTBEAT", ('localhost', AKS_PORT))  # Send heartbeat to AKS

            if missed_heartbeats >= 5:  # Check if too many heartbeats were missed
                print(f"{datetime.now().strftime('%Y-%m-%d %H:%M:%S.%f')[:-3]} Lost connection with AKS")  # Log disconnection
                connected = False  # Update connection status
                aks_heartbeat_received = False  # Reset heartbeat status

        time.sleep(1)  # Sleep for a second before the next iteration
    except KeyboardInterrupt:  # Handle keyboard interrupt
        print(f"{datetime.now().strftime('%Y-%m-%d %H:%M:%S.%f')[:-3]} Launcher Simulator shutting down")  # Log shutdown
        break

sock.close()  # Close the socket when done