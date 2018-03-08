import os
import cv2
import sys
import numpy as np
from udp_receiver import *
from multiprocessing import  Queue
import threading
from pygame import mixer

UDP_IP = "192.168.1.9"
UDP_PORT = 5006

image_queue = Queue()

class display_thread(threading.Thread):
    def __init__(self, queue):
        threading.Thread.__init__(self)
        self.queue = queue
    def run(self):
        mixer.init()
        up = mixer.Sound('up.wav')
        dead = mixer.Sound('dead.wav')
        print('Starting Display thread')
        while True:
            if(self.queue.empty()==True):
                continue
            else:
                data = self.queue.get()
		print(data[0])
                if(data[0]==1):
                    up.play()
                    cv2.imshow("Known face", data[1])
                else:
                    dead.play()
                    cv2.imshow("Unknown face", data[1])
                cv2.waitKey(5000)

class comm_thread(threading.Thread):
    def __init__(self, queue):
        threading.Thread.__init__(self)
        self.queue = queue

    def run(self):
        global UDP_IP
        global UDP_PORT
        connection = socket_init(UDP_IP, UDP_PORT)
        print('Starting comm thread')
        while True:
            print("start receive")
            mode, image = socket_receive(connection)
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
thread_b = comm_thread(image_queue)

thread_a.start()
thread_b.start()


