import os
import cv2
import sys
import numpy as np
from udp_receiver import *
# pip install playsound
# from playsound import playsound 

UDP_IP = "127.0.0.1"
UDP_PORT = 5005

# set up socket
socket_init(UDP_IP, UDP_PORT)

# receive message
while True:
    mode, image = socket_receive()
    if(int(mode) == 0):  # known face
        # playsound(".mp3")
        cv2.imshow("Known face", image)
    else:
        # play_sound
        cv2.imshow("Unknown face", image)

    cv2.waitKey(5000)
    cv2.destroyAllWindows() 

