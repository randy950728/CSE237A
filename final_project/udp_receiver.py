import socket
import cv2
import numpy as np

global sock

def socket_init(UDP_IP, UDP_PORT):
    global sock
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind((UDP_IP, UDP_PORT))

def socket_receive():
    mode_size = 2 # bytes
    pack_size = 2048
    message = str()
    data, addr = sock.recvfrom(pack_size) # buffer size is 1024 bytes
    mode = data[0:mode_size]
    data = [mode_size:]
    while(len(data)!=0):
    	message+=data
    	data, addr = sock.recvfrom(pack_size)
        if(len(data)<pack_size):
            message+=data
            break
    mat = np.fromstring(message, np.uint8)
    image = cv2.imdecode(mat, cv2.IMREAD_COLOR)
    return mode, image
