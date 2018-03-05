import socket
import cv2
import numpy as np

global sock
global remain
remain=str()

def socket_init(UDP_IP, UDP_PORT):
    global sock
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind((UDP_IP, UDP_PORT))

def socket_receive():
    global remain
    mode_size = 2 # bytes
    data_size = 7
    pack_size = 2048
    message = str()
    data, addr = sock.recvfrom(pack_size) # buffer size is 1024 bytes
    print("Remain-start: "+remain)
    data = remain+data
    remain = str()
    mode = data[0:mode_size]
    print(data[mode_size:mode_size+data_size])
    img_size = int(data[mode_size:mode_size+data_size])
    data = data[mode_size+data_size:]

    
    while(img_size > 0):
        if(len(data) > img_size):
            message+=data[0:img_size]
            remain = data[img_size:]
            print("Remain-end "+remain)
            break
        else:
    	    message+=data
            img_size-=len(data)
            
    	data, addr = sock.recvfrom(pack_size)

 #       if(len(data)<pack_size):
 #           message+=data
 #           break
    mat = np.fromstring(message, np.uint8)
    image = cv2.imdecode(mat, cv2.IMREAD_COLOR)
    return mode, np.copy(image)
