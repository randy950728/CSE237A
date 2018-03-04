import socket
import numpy
import cv2
import sys
UDP_IP = "127.0.0.1"
UDP_PORT = 5005
pack_size= 2048
MESSAGE = "Hello, World!"

print "UDP target IP:", UDP_IP
print "UDP target port:", UDP_PORT
print "message:", MESSAGE

sock = socket.socket(socket.AF_INET, # Internet
                     socket.SOCK_DGRAM) # UDP

image = cv2.imread("0.jpg")
MESSAGE = cv2.imencode('.png', image)[1].tostring()

total_size = sys.getsizeof(MESSAGE)
num_packet = total_size/pack_size
off_set = 0
for i in range(num_packet):
	bytes_sent = sock.sendto(MESSAGE[off_set:(off_set+pack_size)], (UDP_IP, UDP_PORT))
	off_set +=bytes_sent
	total_size-=bytes_sent
if(total_size!=0):
	sock.sendto(MESSAGE[off_set:], (UDP_IP, UDP_PORT))


def socket_init( ):
	sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)




# ---------------------------------------------------------------------#



import socket
import cv2
import numpy as np
UDP_IP = "127.0.0.1"
UDP_PORT = 5005

sock = socket.socket(socket.AF_INET, # Internet
                      socket.SOCK_DGRAM) # UDP
sock.bind((UDP_IP, UDP_PORT))
message = str()
while True:
    data, addr = sock.recvfrom(2048) # buffer size is 1024 bytes
    while(len(data)!=0):
    	message+=data
    	data, addr = sock.recvfrom(2048)
        if(len(data)<2048):
            message+=data
            break
    mat = np.fromstring(message, np.uint8)
    image = cv2.imdecode(mat, cv2.IMREAD_COLOR)
    cv2.imshow("frame",image)
    cv2.waitKey(5000)
    cv2.destroyAllWindows()
    message=str()
    #print "received message:", data
