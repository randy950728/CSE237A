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


