import socket
import cv2
import numpy as np

global sock
global remain
global header
remain=str()
header = "CSE237A"

def socket_init(UDP_IP, UDP_PORT):
    global sock
    #sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    #sock.connect((UDP_IP, UDP_PORT))
    sock.bind((UDP_IP, UDP_PORT))
    sock.listen(1)
    connection, clt_addr = sock.accept()
    return connection


def socket_receive(connection):
    global remain
    global header
    header_len= len(header)
    mode_size = 2 # bytes
    data_size = 7
    pack_size = 16384

    # print("Remain-start: "+remain)
    message = str()
    data   = remain
    remain = str()
    while(True):
        if(len(data)<=(header_len+data_size+mode_size)):
            curr_data =connection.recv(pack_size)
            #curr_data, addr = sock.recv(pack_size)
            data += curr_data
        if(data[0:header_len]==header):
            data = data[header_len:]
            break
        else:
            data = data[1:]


    mode     = int(data[0:mode_size])
    img_size = int(data[mode_size:data_size+mode_size])
    data     = data[mode_size+data_size:]
   # print(data[mode_size:mode_size+data_size])
    count=0
    while(img_size > 0):
        print("receiving packet: "+str(count))
        if(len(data) > img_size):
            message+=data[0:img_size]
            remain = data[img_size:]
            # print("Remain-end "+remain)
            break
        else:
    	    message+=data
            img_size-=len(data)
            if(img_size<=0):
                break
    	data  = connection.recv(pack_size)
    	#data, addr = sock.recv(pack_size)
        count+=1
    mat   = np.fromstring(message, np.uint8)
    image = cv2.imdecode(mat, cv2.IMREAD_COLOR)

    return mode, np.copy(image)
