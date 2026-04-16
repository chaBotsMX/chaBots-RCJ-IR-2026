# back_camera.py

import sensor, time
from pyb import UART

sensor.set_contrast(0)
sensor.set_brightness(0)
sensor.set_saturation(0)

sensor.set_auto_gain(False, gain_db=20)  # Fixed gain

sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)

sensor.skip_frames(time=2000)

clock = time.clock()

yellow_threshold = (26, 100, -35, 127, 36, 127)
blue_threshold = (0, 54, -17, 35, -71, -8)

goal_threshold = yellow_threshold
#goal_threshold = blue_threshold

roi = (0, 100, 320, 100)

FOV = 140

approximate_angle = 200

uart = UART(3, 115200, timeout_char=0)

sensor.set_windowing(roi)

while True:
    clock.tick()
    img = sensor.snapshot()

    for blob in img.find_blobs([goal_threshold], pixels_threshold=5, area_threshold=300):
        if blob is not None:
            approximate_angle = int((blob.cx() * FOV) / 320)

            img.draw_rectangle(blob.rect())
            img.draw_cross(blob.cx(), blob.cy())
        else:
            approximate_angle = 200

    print(approximate_angle, clock.fps())
    uart.write(bytes([255]))
    uart.write(bytes([approximate_angle]))
