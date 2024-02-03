import time
from servoMotorHandler.cpp import pwmPI5


class ServoRPI5():
    def __init__(self, pin, min_pulse_width = 1, max_pulse_width = 2, start_pos = 0):
        
        if min_pulse_width > max_pulse_width:
            raise Exception("min_pulse_width must be LESS than max_pulse_width")

        self._min_pw = int(min_pulse_width * 1000)
        self._max_pw = int(max_pulse_width * 1000)
        self.__range = self._max_pw  - self._min_pw
        self.__ang_coeff = self.__range / 2

        self.__current_dc = self._min_pw
        self.__direction = -1

        self.__servo = pwmPI5(pin)
        self.value(start_pos)


    def value(self, value):
        if -1 <= value <= 1:
            # convert from value to duty cycle in ms
            dc = int(self.__ang_coeff * (value + 1) + self._min_pw)
            self.__current_dc = dc
            self.__servo.set_duty_cycle(dc)
            return True
        else:
            print("value must be between -1 and 1")
            return False
        

    def step(self, step_size: float, direction: bool):
        if step_size <= 0 or step_size > 1:
            return False
        
        if direction:
            if self.__current_dc == self._max_pw:
                return False
        elif self.__current_dc == self._min_pw:
            return False
    
        step = step_size * self.__range
        if not direction:
            step = -step

        dc = int(self.__current_dc + step)

        if dc > self._max_pw:
            dc = self._max_pw        
        elif dc < self._min_pw:
            dc = self._min_pw

        self.__current_dc = dc
        self.__servo.set_duty_cycle(dc)
        return True
    
    
    def continuos_rotation(self):
        if self.__direction == -1:
            ret = self.step(0.05, False)
            if not ret:
                self.__direction = 1
                time.sleep(2)
            else:
                time.sleep(1)
        else:
            ret = self.step(0.05, True)
            if not ret:
                self.__direction = -1
                time.sleep(2)
            else:
                time.sleep(1)
