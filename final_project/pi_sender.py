import os
import cv2
import sys
import time
import imutils
import numpy as np
from udp_sender import *
from picamera import PiCamera
from picamera.array import PiRGBArray

# Get user supplied values
# ---------------------------------------------------------------------#
training_image_dir  = "training_data"
detection_template  = "haarcascade_frontalface_default.xml"
WIDTH  = 540
HEIGHT = 420
F_WIDTH  = 300
F_HEIGHT = 300
Frame_rate = 20
image_per_face = 6


# ---------------------------------------------------------------------#
def draw_text(frame, string, x, y):
    cv2.putText(frame, string, (x, y), cv2.FONT_HERSHEY_PLAIN, 1.5, (0, 255, 0), 2)


# ---------------------------------------------------------------------#
def recog_face(model, input):
    label, prob = model.predict(input)
    return label,prob


                        #Initialization
# ---------------------------------------------------------------------#
# Initialize Pi Camera
camera = PiCamera()
camera.framerate  = Frame_rate
camera.resolution = (WIDTH, HEIGHT)
rawCapture = PiRGBArray(camera, size=(WIDTH, HEIGHT))

# allow the camera to warmup
time.sleep(0.1)
time_stamp = time.time()*1000.0


                # Face recognition initialization
# ---------------------------------------------------------------------#
# Create the haar cascade
faceCascade = cv2.CascadeClassifier(detection_template) # initialize the camera and grab a reference to the raw camera capture

# set up socket
UDP_IP = "127.0.0.1"
UDP_PORT = 5005
pack_size= 2048

count = 0
threshold = 130
face_time_out = 100e6
sent_list = dict()
collected_list=[]

# Setup UDP protocol
socket_init()


        # Start running real time face detection and recognition
# ---------------------------------------------------------------------#
for frame in camera.capture_continuous(rawCapture, format="bgr", use_video_port=True):

            # Face Detection
    # ----------------------------------#
    # Get image pixels
    image = frame.array

    # Convert to grayscale
    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)

    # Get faces within the frame
    faces = faceCascade.detectMultiScale(
        gray,           scaleFactor=1.1,
        minNeighbors=5, minSize=(30, 30),
        flags = cv2.CASCADE_SCALE_IMAGE)

    # Print time stamp
    print time.time()*1000.0-time_stamp," Found {0} faces!".format(len(faces))
    time_stamp = time.time()*1000.0


            # Face Recognition
    # ----------------------------------#
    # for each face detected, draw rectangle and peform recognition
    for (x, y, w, h) in faces:
        # Draw rectangle
        cv2.rectangle(image, (x, y), (x+w, y+h), (0, 255, 0), 2)

        # Compare face
        face = cv2.resize(gray[y:y+h, x:x+w], (F_WIDTH, F_HEIGHT))
        flag = False
        print(len(collected_list))
        for mod in collected_list:
            label, conf = mod[1].predict(face)
            print("Label: "+str(label)+" conf: "+str(conf))
            if(conf<100):
                draw_text(image, "Old", (x), (y))
                flag = True

        if(flag==False):
            draw_text(image, "New", (x), (y))
            face_model = cv2.face.createEigenFaceRecognizer()
            face_model.train([face], np.asarray(1))
            collected_list.append([face,face_model])
            face_model = None

            # Result display
    # ----------------------------------#
    # show the frame
    cv2.imshow("Frame", cv2.resize(image, (WIDTH, HEIGHT)))



            # image sending protoccol
    # ----------------------------------#
    if(count > 50):
        count = 0
        curr_time = time.time()

        # Remove face from sent list if it has been >timeout
        del_key = []
        for key in sent_list:
            if((curr_time-sent_list[key][2])> face_time_out):
                del_key.append(key)

        for key in del_key:
            del sent_list[key]


        # add in new faces
        # for key in collected_list:
        #     if not(key in sent_list):
        #         sent_list[key] = [collected_list[key],curr_time,0]


        # Clear up collected dict
        collected_list = []

        # Send faces
        # socket_send(UDP_IP, UDP_PORT, pack_size, sent_list)

    # count ++
    count += 1

    # Wait for keyboard input
    key = cv2.waitKey(1) & 0xFF

    # clear the stream in preparation for the next frame
    rawCapture.truncate(0)

    # if the `q` key was pressed, break from the loop
    if key == ord("q"):
        break
