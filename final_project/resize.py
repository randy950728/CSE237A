import cv2
import glob

WIDTH = 300
HEIGHT = 300

# faceCascade = cv2.CascadeClassifier("haarcascade_frontalface_default.xml")
# faceCascade = cv2.CascadeClassifier("haarcascade_frontalcatface_extended.xml")
# faceCascade = cv2.CascadeClassifier("haarcascade_frontalcatface.xml")
faceCascade = cv2.CascadeClassifier("haarcascade_frontalface_default.xml")

 # initialize the camera and grab a reference to the raw camera capture

i = 0
for filename in glob.glob("cary_test/*.JPG"):
    image = cv2.imread(filename)
    #print(a1.shape)
    print(1)
    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    # gray = cv2.equalizeHist(gray)
    print(2)
    faces = faceCascade.detectMultiScale(
        gray,
        scaleFactor=1.1,
        minNeighbors=5,
        minSize=(30, 30),
        flags = cv2.CASCADE_SCALE_IMAGE
        )
    print(3)
    if(len(faces) > 0):
        for (x, y, w, h) in faces:
            # cv2.rectangle(image, (x, y), (x+w, y+h), (0, 255, 0), 2)
            face = gray[y:y+h, x:x+w]
            cv2.imwrite(str(i)+".jpg", cv2.resize(face, (WIDTH, HEIGHT)))
            i = i+1

    else:
        print("no faces")

