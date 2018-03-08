import socket
import cv2
import numpy as np

global sock
# global remain
global header
# remain=str()
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


def socket_receive(connection,remain):
    # global remain
    global header
    header_len= len(header)
    mode_size = 2 # bytes
    data_size = 7
    pack_size = 4096
    if(len(remain)>=0):
        print(remain[0:10])

    message = str()
    data   = str(remain)
    print( "remain size:"+str(len(remain)))
    # remain = str()
    while(True):
        if(len(data)<=(header_len+data_size+mode_size)):
            curr_data =connection.recv(pack_size)
            #curr_data, addr = sock.recv(pack_size)
            data += curr_data
        if(data[0:header_len]==header):
            data = data[header_len:]
            break
        else:
            if(len(data) > 16000 or len(data) < 100):
                print("data len:"+str(len(data)))
            data = data[1:]

    mode     = int(data[0:mode_size])
    img_size = int(data[mode_size:data_size+mode_size])
    data     = data[mode_size+data_size:]
   # print(data[mode_size:mode_size+data_size])
    count=0
    print("resuming")
    while(img_size > 0):
        print("receiving packet: "+str(count)+"  data size: "+str(len(data)))
        if(len(data) > img_size):
            message+=data[0:img_size]
            remain = data[img_size:]
            # print("Remain-end "+remain)
            break
        else:
    	    message+=data
            img_size-=len(data)
            if(img_size<=0):
                remain = str()
                break
    	print("waiting data")
	data  = connection.recv(pack_size)
    	#data, addr = sock.recv(pack_size)
        count+=1
    print("got everything!")
    mat   = np.fromstring(message, np.uint8)
    image = cv2.imdecode(mat, cv2.IMREAD_COLOR)
    print("Reamin size before leaving: "+str(len(remain)))
    return mode, np.copy(image), remain
