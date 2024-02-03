from servoMotorHandler.pwmC import ServoRPI5
import time
from multiprocessing import Queue


class ServoMotorHandler:
    def __init__(self, screen_width: int, screen_height: int, threshold_perc: float, queue: Queue):
        self.__screen_width = screen_width
        self.__screen_height = screen_height

        self.__x_threshold = screen_width * threshold_perc
        self.__y_threshold = screen_height * threshold_perc

        self.__queue = queue

        self.__servo_x = ServoRPI5(12, 0.5, 2.5, 0)
        self.__servo_y = ServoRPI5(13, 0.5, 2.5, 0.3)

        
    def __convert_coordinates(self, coordinates):
        return [(coordinates[0] - 0.5)*self.__screen_width, -(coordinates[1] - 0.5)*self.__screen_height]
    

    def __move_motors(self, orig_coordinates):
        coordinates = self.__convert_coordinates(orig_coordinates)

        # CHECK X COORDINATE
        if -self.__x_threshold < coordinates[0] and coordinates[0] < self.__x_threshold:
            pass
        elif coordinates[0] > 0:
            self.__servo_x.step(0.025, True)
            time.sleep(0.1)
        else:
            self.__servo_x.step(0.025, False)
            time.sleep(0.1)
        
        # CHECK Y COORDINATE
        if -self.__y_threshold < coordinates[1] and coordinates[1] < self.__y_threshold:
              pass
        elif coordinates[1] > 0:
            self.__servo_y.step(0.025, True)
            time.sleep(0.1)
        else:
            self.__servo_y.step(0.025, False)
            time.sleep(0.1)


    def run(self):
        latest_ts = time.time()

        while True:
            try:
                coordinates_array = self.__queue.get(False)
                self.__move_motors(coordinates_array)
                latest_ts = time.time()
            except:
                if time.time() - latest_ts > 2:
                    self.__servo_y.value(0.3)
                    self.__servo_x.continuos_rotation()
