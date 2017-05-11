import numpy as np
import cv2
import redis
from FaceDependencies import *


#face_cascade = cv2.CascadeClassifier('haarcascade_frontalface_default.xml')
#eye_cascade = cv2.CascadeClassifier('haarcascade_eye.xml')

#Prereq for Detection of People
#hog = cv2.HOGDescriptor()
#hog.setSVMDetector( cv2.HOGDescriptor_getDefaultPeopleDetector() )

#cap = cv2.VideoCapture("stabilized.avi")
def main():
    while True:
        #ret, img = cap.read()
        gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)

        #Detects People
        found,w=hog.detectMultiScale(img, winStride=(8,8), padding=(32,32), scale=1.5)
        draw_detections(img,found)

    #Detects face
        faces = face_cascade.detectMultiScale(
            gray,
            scaleFactor = 1.1,
            minNeighbors = 5,
            minSize = (30,30),
            flags = 0)

        for (x,y,w,h) in faces:
            cv2.rectangle(img,(x,y),(x+w,y+h),(255,0,0),2)
            roi_gray = gray[y:y+h, x:x+w]
            roi_color = img[y:y+h, x:x+w]

            #Detects Eyes
            eyes = eye_cascade.detectMultiScale(roi_gray)
            for (ex,ey,ew,eh) in eyes:
                cv2.rectangle(roi_color,(ex,ey),(ex+ew,ey+eh),(0,255,0),2)

    

        #Prints image
        cv2.imshow('Image',img)
    
        #k = cv2.waitKey(30) & 0xff
        #if k == 27:
        #    break

    #cap.release()
    #cv2.destroyAllWindows()
