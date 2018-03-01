import socket
import numpy
import cv2
import sys
UDP_IP = "127.0.0.1"
UDP_PORT = 5005
MESSAGE = "Hello, World!"
 
print "UDP target IP:", UDP_IP
print "UDP target port:", UDP_PORT
print "message:", MESSAGE
 
sock = socket.socket(socket.AF_INET, # Internet
                     socket.SOCK_DGRAM) # UDP

image = cv2.imread("0.jpg")
MESSAGE = cv2.imencode('.png', image)[1].tostring()
print "image size", sys.getsizeof(MESSAGE)
#sock.sendto(MESSAGE, (UDP_IP, UDP_PORT))
