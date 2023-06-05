import socket
import struct
impor

# Create a socket
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Connect to the C++ server
server_address = ('localhost', 8000)
sock.connect(server_address)

# Send byte data continuously
while True:
    print("test...")
    # Get the byte data from some source (e.g., a file, sensor, etc.)
    byte_data = b'\x01\x02\x03\x04'  # Replace with your byte data

    # Get the size of the byte data
    data_size = len(byte_data)

    print(data_size)

    print(struct.pack('!I', data_size))

    # Send the size as a 4-byte unsigned integer (assuming the size will fit within 4 bytes)
    sock.sendall(struct.pack('!I', data_size))

    # Send the byte data
    sock.sendall(byte_data)

# Close the socket (optional if the loop is infinite)
sock.close()
