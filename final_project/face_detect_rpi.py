import os
import cv2
import sys
import time
import imutils
import numpy as np
# from picamera import PiCamera
# from picamera.array import PiRGBArray

# Get user supplied values
training_image_dir  = "training_data"
detection_template  = "haarcascade_frontalface_default.xml"
WIDTH  = 640
HEIGHT = 480
Frame_rate = 20
image_per_face = 6

def draw_text(frame, string, x, y):
    cv2.putText(frame, string, (x, y), cv2.FONT_HERSHEY_PLAIN, 1.5, (0, 255, 0), 2)

def recog_face(model, input):
    label, prob = model.predict(input)
    return label,prob

# Create the haar cascade
faceCascade = cv2.CascadeClassifier(detection_template) # initialize the camera and grab a reference to the raw camera capture

# Initialize Pi Camera
# camera = PiCamera()
# camera.framerate  = Frame_rate
# camera.resolution = (WIDTH, HEIGHT)
# rawCapture = PiRGBArray(camera, size=(WIDTH, HEIGHT))

# allow the camera to warmup
# time.sleep(0.1)
# time_stamp = time.time()*1000.0

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
        user_face.append(cv2.cvtColor(image, cv2.COLOR_BGR2GRAY))
        user_label.append(i)
        cv2.imshow("Reading training images...", cv2.resize(image, (400, 500)))
        cv2.waitKey(100)
cv2.destroyAllWindows()

#Train on training data
#face_model = cv2.face.LBPHFaceRecognizer_create()
face_model = cv2.face.createLBPHFaceRecognizer()
face_model.train(user_face, np.asarray(user_label))

test_a =  cv2.cvtColor(cv2.imread("./test_data/1.jpg"), cv2.COLOR_BGR2GRAY)
test_b =  cv2.cvtColor(cv2.imread("./test_data/2.jpg"), cv2.COLOR_BGR2GRAY)
label, conf = recog_face(face_model,test_a)
print(user_name[label],conf)
label, conf = recog_face(face_model,test_b)
print(user_name[label],conf)
# for frame in camera.capture_continuous(rawCapture, format="bgr", use_video_port=rue):
#     # Get image pixels
#     image = frame.array

#     # Convert to grayscale
#     gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)

#     # Get faces within the frame
#     faces = faceCascade.detectMultiScale(
#         gray,
#         scaleFactor=1.1,
#         minNeighbors=5,
#         minSize=(30, 30),
#         flags = cv2.cv.CV_HAAR_SCALE_IMAGE
#         )


#     print time.time()*1000.0-time_stamp," Found {0} faces!".format(len(faces))
#     time_stamp = time.time()*1000.0
#     for (x, y, w, h) in faces:
#         cv2.rectangle(image, (x, y), (x+w, y+h), (0, 255, 0), 2)

#     # show the frame
#     #cv2.imshow("Frame", image)
#     cv2.imshow("Frame", cv2.resize(image, (640, 480)))

#     # Wait for keyboard input
#     key = cv2.waitKey(1) & 0xFF

#     # clear the stream in preparation for the next frame
#     rawCapture.truncate(0)

#     # if the `q` key was pressed, break from the loop
#     if key == ord("q"):
#         break
