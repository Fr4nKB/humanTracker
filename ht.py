import sys
import os
import signal
from streamer import Streamer
from servoMotorHandler import ServoMotorHandler
from jsonHandler import loadJSON
from multiprocessing import Process, freeze_support, Queue

child_pid = 0

config = loadJSON("config")
if not config:
    sys.exit(-1)

def servo(q):
    s = ServoMotorHandler(config["width"], config["height"], config["threshold"], q)
    s.run()

def signal_handler(sig, frame):
    global child_pid
    os.kill(child_pid, signal.SIGKILL)
    os.kill(0, signal.SIGKILL)

if __name__ == '__main__':
    freeze_support()
    q = Queue()
    signal.signal(signal.SIGINT, signal_handler)
    h = Streamer(config["width"], config["height"], config["output_ip"], config["output_port"], q)
    p = Process(target=servo, args={q})
    p.start()
    child_pid = p.pid
    h.run()

