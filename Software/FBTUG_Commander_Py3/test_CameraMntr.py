import sys
sys.path.insert(0,'/usr/local/lib/python3.5/site-packages')
import threading
import time
import tkinter
import cv2
import tkinter.font
import numpy as np
from PIL import Image
from PIL import ImageTk

from class_CameraMntr import *

cam= CameraLink()

while True:
    frame= cam.get_frame()
    cv2.namedWindow("Test", cv2.WINDOW_NORMAL)
    cv2.imshow("Test", frame)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break
    time.sleep(0.01)

cv2.destroyAllWindows()
cam.stop_clean_buffer()
del(cam.thread_clean_buffer)
print("delete thread_clean_buffer")
