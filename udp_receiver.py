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
