import cv2
from picamera2 import Picamera2
import mediapipe as mp
import numpy as np
import socket
import pickle
import time
from multiprocessing import Queue


encode_param = [int(cv2.IMWRITE_JPEG_QUALITY), 30]


class Streamer:
    def __init__(self, queue: Queue, screen_width: int = 1280, screen_height: int = 720, ip: str = None, port: int = None):
      mp_face_detection = mp.solutions.face_detection
      self.__detector = mp_face_detection.FaceDetection(model_selection=1, min_detection_confidence=0.6)

      self.__screen_width = screen_width
      self.__screen_height = screen_height
      
      self.__queue = queue
      self.__sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
      self.__output_ip = ip
      self.__output_port = port

    def __send_coordinates(self, coordinates):
      self.__queue.put(coordinates)

    def __visualize(self, frame, detections):
      if detections.detections:
        tmp = detections.detections[0].location_data.relative_keypoints[2]
        face_react = np.multiply(
            [
                tmp.x,
                tmp.y,
                1,
                1,
            ],
            [self.__screen_width, self.__screen_height, 50, 50]).astype(int)
        return cv2.rectangle(frame, face_react, color=(255, 255, 255), thickness=2)
      else:
        return frame

    def __detect_human(self, frame):
      try:
        detections = self.__detector.process(frame)
        
        if self.__output_ip and self.__output_port:
          bb_img = self.__visualize(frame, detections)
          img = cv2.imencode('.jpg', bb_img, encode_param)[1]
          data = pickle.dumps(img, 0)
          self.__sock.sendto(data, (self.__output_ip, self.__output_port))
        
      except:
        return

      try:
        detections_coordinates = detections.detections[0].location_data.relative_keypoints[2]
        tmp = [detections_coordinates.x, detections_coordinates.y]
        self.__send_coordinates(tmp)
      except:
        pass


    def run(self):
      self.__cam = Picamera2()
      camera_config = self.__cam.create_still_configuration(main={"size": (self.__screen_width, self.__screen_height)})
      self.__cam.configure(camera_config)
      self.__cam.start()

      counter = 0
      while True:

          frame = self.__cam.capture_array()
          if frame.any():
            if(counter % 2 == 0):
              frame = cv2.flip(frame, 0)
              self.__detect_human(frame)
            counter += 1
            time.sleep(1/30)
          else:
            print("Couldn't fetch frame from camera")
          
