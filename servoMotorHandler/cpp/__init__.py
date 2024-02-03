import ctypes
import os


class pwmPI5:
    def __init__(self, pin):
        location = os.path.join(os.path.dirname(os.path.abspath(__file__)), "pwm.so")
        self.lib = ctypes.CDLL(location)

        self.lib.pwmPI5_new.argtypes = [ctypes.c_uint8]
        self.lib.pwmPI5_new.restype = ctypes.c_void_p

        self.lib.setDutyCycle.argtypes = [ctypes.c_void_p, ctypes.c_uint16]
        self.lib.setDutyCycle.restype = ctypes.c_bool

        self.lib.pwmPI5_delete.argtypes = [ctypes.c_void_p]
        self.lib.pwmPI5_delete.restype = None

        self.obj = self.lib.pwmPI5_new(pin)


    def set_duty_cycle(self, arg):
        return self.lib.setDutyCycle(self.obj, arg)


    def __del__(self):
        print("s")
        self.lib.pwmPI5_delete(self.obj)

