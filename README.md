# humanTracker
A software for RPI5 to track a human face and follow his/her movement.

It uses a camera to detect humans and two servo motors to move in the x, y directions (pan tilt).
The software is mostly written in Python, however, a part is in low level C++ since there's no python module (at the time of writing) to move the servos accurately on the RPI5 due to the change of the nature of GPIO compared to previous PIs.

## Requirements
- A compliant camera for the RPI5
- Two servo motors that work at 50Hz and with min and max pulse width 0.5ms and 2.5ms
- The following modules (use sudo as the program itself has to be run with sudo):
  - ```sudo pip install opencv-python```
  - ```sudo pip install picamera2```
  - ```sudo pip install mediapipe```

In case there's some error use the option ```--break-system-packages```.

## Configuration
- Use ```./docs/config.json``` to configure the program:
  - ```width``` and ```height```: image resolution
  - ```threshold```: is a percentage which defines a squared area around the center of the image, when the human is inside this area the motor are not moved. A smaller percentage will result in a more accurate placement but more energy consumed etc...
  - ```output_ip``` and ```output_port```: address at which the image seen from the camera will be displayed
- Connect the servo motor for the x axis at GPIO12 and the servo for y axis at GPIO13.

## Usage
Start ```ht.py``` on RPI5 with ```sudo```:

```sudo python ht.py```

you can see the output by executing ```recv.py``` on the machine with address ```output_ip, output_port``` set during the configuration.
