import os
import cv2
import sys
import time
import imutils
import threading
import numpy as np
from udp_sender import *
from picamera import PiCamera
from multiprocessing import  Queue
from picamera.array import PiRGBArray

# Get user supplied values
# ---------------------------------------------------------------------#
training_image_dir  = "training_data"
detection_template  = "haarcascade_frontalface_default.xml"
WIDTH  = 1280
HEIGHT = 720
F_WIDTH  = 300
F_HEIGHT = 300
Frame_rate = 5
image_per_face = 6

UDP_IP = "192.168.1.9"    #UDP IP
UDP_PORT = 5006         #UDP Port
pack_size= 16384         #UDP Packet size

count = 0               #Data collecting interval
threshold = 55          #Unknown guest threshold
face_time_out = 5   #Time to clear out existing user


# ---------------------------------------------------------------------#
def draw_text(frame, string, x, y, mode=0):
    if mode == 0:
        cv2.putText(frame, string, (x, y), cv2.FONT_HERSHEY_PLAIN, 1.5, (0, 255, 0), 2)
    elif mode == 1:
        cv2.putText(frame, string, (x, y), cv2.FONT_HERSHEY_PLAIN, 1.5, (0, 0 , 255), 2)
    else:
        cv2.putText(frame, string, (x, y), cv2.FONT_HERSHEY_PLAIN, 1.5, (255, 255 , 255), 2)

# ---------------------------------------------------------------------#
def recog_face(model, input):
    label, prob = model.predict(input)
    return label,prob


# ---------------------------------------------------------------------#
class display_thread(threading.Thread):
    def __init__(self, queue):
        threading.Thread.__init__(self)
        self.queue = queue
    def run(self):
        print('Starting Display thread')
        global WIDTH
        global HEIGHT
        while True:
            if(self.queue.empty()==True):
                time.sleep(0.01)
                continue
            else:
                data = self.queue.get()
            cv2.imshow("Frame", cv2.resize(image, (WIDTH, HEIGHT)))
            key = cv2.waitKey(1) & 0xFF
            if key == ord("q"):
                break


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
# -------------------------------------------------#
for i in range(num_of_user):
    for j in range(image_per_face):
        file = "./"+training_image_dir+"/"+str(i+1)+"/"+str(j)+".jpg"
        image = cv2.imread(file)
        #image = cv2.resize(cv2.cvtColor(image, cv2.COLOR_BGR2GRAY), (200,200))
        image = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
        user_face.append(image)
        user_label.append(i)
        cv2.imshow("Reading training images...", cv2.resize(image, (400, 400)))
        cv2.waitKey(100)
cv2.destroyAllWindows()

#Train on training data
# -------------------------------------------------#
face_model = cv2.face.createLBPHFaceRecognizer()
face_model_fisher = cv2.face.createFisherFaceRecognizer()
face_model.train(user_face, np.asarray(user_label))
face_model_fisher.train(user_face, np.asarray(user_label))

# Setup UDP protocol
# -------------------------------------------------#
socket_init(UDP_IP, UDP_PORT)


unknown         = False
image_queue     = Queue()
sent_list       = dict()
collected_list  = dict()

# Start thread
# -------------------------------------------------#
thread_a = display_thread(image_queue)
thread_a.start()

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
    #print time.time()*1000.0-time_stamp," Found {0} faces!".format(len(faces))
    time_stamp = time.time()*1000.0


            # Face Recognition
    # ----------------------------------#
    face_count=0
    # for each face detected, draw rectangle and peform recognition
    for (x, y, w, h) in faces:
        # Draw rectangle
        cv2.rectangle(image, (x, y), (x+w, y+h), (0, 255, 0), 2)

        # Recognize face
        face = cv2.resize(gray[y:y+h, x:x+w], (F_WIDTH, F_HEIGHT), interpolation = cv2.INTER_CUBIC)
        label, conf = recog_face(face_model,face)
        #label_2, conf_2 = recog_face(face_model_fisher,face)


        # Print result
        print(face_count,user_name[label],conf)
        face_count+=1

        #Append to collected face
        if(conf>threshold):
            unknown = True
            draw_text(image, "unknown", (x), (y), 1)

        else:
            draw_text(image, user_name[label], (x), (y))
            if not(label in collected_list):
                draw_text(face, user_name[label], 0, 20, 2)
                collected_list[label]=face

    if(unknown):
        unknown_frame = np.copy(image)

            # Result display
    # ----------------------------------#
    # show the frame
    # cv2.imshow("Frame", cv2.resize(image, (WIDTH, HEIGHT)))
    image_queue.put(image)

            # image sending protoccol
    # ----------------------------------#
    if(count > 5):
        count = 0
        curr_time = time.time()
        # Remove face from sent list if it has been >timeout
        del_key = []
        for key in sent_list:
            if((curr_time-sent_list[key][1])> face_time_out):
                del_key.append(key)

        for key in del_key:
            del sent_list[key]


        # add in new faces
        for key in collected_list:
            if not(key in sent_list):
                sent_list[key] = [collected_list[key],curr_time,0]


        # Clear up collected dict
        collected_list = dict()

        # Send faces
        if(unknown==False):
            socket_send(UDP_IP, UDP_PORT, pack_size, sent_list, False)

        else:
            socket_send(UDP_IP, UDP_PORT, pack_size, None, True, cv2.resize(unknown_frame,(640, 480)))
            unknown = False

    # count ++
    count += 1

    # Wait for keyboard input
    # key = cv2.waitKey(1) & 0xFF

    # clear the stream in preparation for the next frame
    rawCapture.truncate(0)

    # if the `q` key was pressed, break from the loop
    # if key == ord("q"):
    #     break
