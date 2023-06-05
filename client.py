import cv2
import numpy as np
import socket
import time

# Open the webcam
cap = cv2.VideoCapture(0)

# # # Create a TCP/IP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Connect to the C++ server
server_address = ('localhost', 8000)
sock.connect(server_address)

while True:
    # Capture a frame from the webcam
    ret, frame = cap.read()

    # Encode the frame as a JPEG image
    retval, buffer = cv2.imencode('.jpg', frame)

    # Convert the encoded frame to a byte array
    data = np.array(buffer).tobytes()

    # print(type(data))

    # Send the frame size to the server
    frame_size = len(data)
    # print(f"frame_size : {frame_size}")
    # print(buffer.shape)
    sock.sendall(frame_size.to_bytes(4, 'little'))

    img_str = cv2.imencode('.jpg', frame)[1].tostring()
    # # Önemli loglar
    # print(len(img_str))
    # print(type(img_str))
    # print(img_str == data)
    # print(img_str)
    # print first 10 bytes
    # for i in range(10):
    #     print(f"byte number {i} : {img_str[i]}")
    # print(f"last byte : {img_str[-1]}")

    # Send the frame data to the server
    sock.sendall(data)

    cv2.imshow('Sent Frame', frame)

    # Break the loop if the 'q' key is pressed
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break 
    # time.sleep(3600)

# Release the webcam and close any open windows
cap.release()
cv2.destroyAllWindows()
