import os
import cv2
import sys
import numpy as np
from udp_receiver import *
from queue import  Queue
import threading
# pip install playsound
# from playsound import playsound

UDP_IP = "127.0.0.1"
UDP_PORT = 5005

image_queue = Queue()

class display_thread(threading.Thread):
    def __init__(self, queue):
        threading.Thread.__init__(self)
        self.queue = queue
    def run(self):
        print('Starting Display thread')
        while True:
            if(self.queue.empty()==True):
                continue
            else:
                data = self.queue.get()
                if(data[0]==1):
                    cv2.imshow("Known face", image)
                else:
                    cv2.imshow("Unknown face", image)
                cv2.waitKey(5000)

class comm_thread(threading.Thread):
    def __init__(self, queue):
        threading.Thread.__init__(self)
        self.queue = queue

    def run(self):
        global UDP_IP
        global UDP_PORT
        socket_init(UDP_IP, UDP_PORT)
        print('Starting comm thread')
        while True:
            print("start receive")
            mode, image = socket_receive()
            self.queue.put((mode,image))


# set up socket


# # receive message
# while True:
#     print("start receive")
#     mode, image = socket_receive()
#     # print("image: "+str(type(image)))

#     if(int(mode) == 1):  # known face
#         # playsound(".mp3")
#         cv2.imshow("Known face", image)
#     else:
#         # play_sound
#         cv2.imshow("Unknown face", image)

#     cv2.waitKey(3000)
#     cv2.destroyAllWindows()

thread_a = display_thread(image_queue)
thread_b = receive_thread(image_queue)

thread_a.start()
thread_b.start()


