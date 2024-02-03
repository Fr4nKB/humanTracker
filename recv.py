import cv2
import socket
import pickle

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.setblocking(False)
sock.bind(('', 1337))

while True:
    try:
        data = sock.recvfrom(65535)[0]
        encFrame = pickle.loads(data, fix_imports=True, encoding="bytes")
        frame = cv2.imdecode(encFrame, cv2.IMREAD_COLOR)
        frame = cv2.cvtColor(frame, cv2.COLOR_RGB2BGR)
        cv2.imwrite("feed.jpg", frame)
    except:
        pass
