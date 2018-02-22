import cv2
import glob

WIDTH = 360
HEIGHT = 270

faceCascade = cv2.CascadeClassifier("haarcascade_frontalface_default.xml") # initialize the camera and grab a reference to the raw camera capture

i = 0
for filename in glob.glob("photos/*.jpg"):
    image = cv2.imread(filename)
    image = cv2.resize(image, (1920, 1080))
    #print(a1.shape)
    print(1) 
    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    print(2)
    faces = faceCascade.detectMultiScale(
        gray,
        scaleFactor=1.1,
        minNeighbors=5,
        minSize=(30, 30),
        flags = cv2.cv.CV_HAAR_SCALE_IMAGE
        )
    print(3)
    if(len(faces) > 0):
        for (x, y, w, h) in faces:
            cv2.rectangle(image, (x, y), (x+w, y+h), (0, 255, 0), 2)
            face = cv2.equalizeHist(gray[y:y+h, x:x+w])
            cv2.imwrite(str(i)+".jpg", cv2.resize(face, (WIDTH, HEIGHT)))
            i = i+1

    else:
        print("no faces")

