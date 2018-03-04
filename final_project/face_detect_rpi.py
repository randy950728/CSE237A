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
WIDTH  = 360
HEIGHT = 270
F_WIDTH  = 200
F_HEIGHT = 200
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

# Read in training files
filenames   = os.listdir("./"+training_image_dir)
num_of_user = len(filenames)
user_name   = []
user_label  = []
user_face   = []

# Read in user name
for i in range(num_of_user):
    file = open("./"+training_image_dir+"/"+str(i+1)+"/name.txt")
    temp_name = file.readline()
    user_name.append(temp_name)
    file.close()


# Read in training image
for i in range(num_of_user):
    for j in range(image_per_face):
        file = "./"+training_image_dir+"/"+str(i+1)+"/"+str(j)+".jpg"
        image = cv2.imread(file)
        image = cv2.resize(cv2.cvtColor(image, cv2.COLOR_BGR2GRAY), (200,200))
        user_face.append(image)
        user_label.append(i)
        cv2.imshow("Reading training images...", cv2.resize(image, (400, 400)))
        cv2.waitKey(100)
cv2.destroyAllWindows()

#Train on training data
face_model = cv2.face.createLBPHFaceRecognizer()
face_model.train(user_face, np.asarray(user_label))

# set up socket
UDP_IP = "127.0.0.1"
UDP_PORT = 5005
pack_size= 2048

count = 0
threshold = 130
face_time_out = 100e6
sent_list = dict()
collected_list=dict()

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

        # Recodnize face
        face = cv2.resize(gray[y:y+h, x:x+w], (F_WIDTH, F_HEIGHT))
        label, conf = recog_face(face_model,face)
        draw_text(image, user_name[label], (x), (y))

        # Print result
        print(user_name[label],conf)

        #Append to collected face
        if(conf>threshold):
            collected_list[-1]=face

        if not(label in collected_list):
            collected_list[label]=face


            # Result display
    # ----------------------------------#
    # show the frame
    cv2.imshow("Frame", cv2.resize(image, (WIDTH, HEIGHT)))



            # image sending protoccol
    # ----------------------------------#
    if(count > 100):
        count = 0
        curr_time = time.time()

        # Remove face from sent list if it has been >timeout
        for key in sent_list:
            if((curr_time-sent_list[key][2])> face_time_out):
                del sent_list[key]

        # add in new faces
        for key in collected_list:
            if not(key in sent_list):
                sent_list[key] = [collected_list[key],curr_time,0]

        # Clear up collected dict
        collected_list = dict()

        # Sending logic
        socket_send(UDP_IP, UDP_PORT, pack_size, sent_list)

    # count ++
    count += 1

    # Wait for keyboard input
    key = cv2.waitKey(1) & 0xFF

    # clear the stream in preparation for the next frame
    rawCapture.truncate(0)

    # if the `q` key was pressed, break from the loop
    if key == ord("q"):
        break
