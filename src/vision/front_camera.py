# front_camera.py

import sensor
import time
from pyb import UART

TARGET = "YELLOW"

YELLOW_THRESHOLD = (65, 100, -128, 127, 11, 127)
BLUE_THRESHOLD = (0, 21, -10, 127, -128, 0)
THRESHOLDS = [YELLOW_THRESHOLD, BLUE_THRESHOLD]

CROP_FROM_TOP = 120

ROI = (0, CROP_FROM_TOP, 320, 240 - CROP_FROM_TOP)

FOV_HORIZONTAL = 71.8
DEGREES_IN_PIXEL = FOV_HORIZONTAL/320

# Calibration points: (top_y_pixel, distance_in_cm)
# Keep this list sorted from LOWEST top_y to HIGHEST top_y
# needs to calibrate
CALIBRATION_POINTS = [
    (14, 200),
    (16, 150),
    (20, 100),
    (22, 50)
]

# init sensor

sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.set_framerate(120)
sensor.set_vflip(True)
sensor.set_hmirror(True)

sensor.set_auto_gain(False, gain_db=24)

sensor.set_auto_whitebal(False, rgb_gain_db=(64.2, 60.2, 63.0))
sensor.set_auto_exposure(False, exposure_us=50000)

#sensor.set_contrast(1)
#sensor.set_brightness(3)
#sensor.set_saturation(2)
sensor.skip_frames(time=500)

sensor.set_windowing(ROI)

clock = time.clock()

uart = UART(3, 115200, timeout_char=0)

angle = 254
distance = 254
confidence = 254


def estimate_distance(blob_y):
    if blob_y <= CALIBRATION_POINTS[0][0]:
        return CALIBRATION_POINTS[0][1]
    if blob_y >= CALIBRATION_POINTS[-1][0]:
        return CALIBRATION_POINTS[-1][1]

    for i in range(len(CALIBRATION_POINTS) - 1):
        y0, d0 = CALIBRATION_POINTS[i]
        y1, d1 = CALIBRATION_POINTS[i+1]

        if y0 <= blob_y <= y1:
            # Linear Interpolation Formula
            # Determines the percentage of progress between y0 and y1
            fraction = (blob_y - y0) / (y1 - y0)
            # Apply that percentage to the distance gap
            interpolated_distance = d0 + fraction * (d1 - d0)
            return interpolated_distance
    return 254

def map_value(x, in_min, in_max, out_min, out_max):
    # Prevent division by zero if in_min equals in_max
    if in_max == in_min:
        return out_min

    # Calculate the mapped value
    mapped = out_min + (x - in_min) * (out_max - out_min) / (in_max - in_min)

    # Constrain the value to the output range (optional but recommended)
    return max(min(int(mapped), out_max), out_min)

MIN_AREA = 1000
MAX_AREA = 20000

def process_goal(blobs, min_confidence):
    if not blobs:
        return None

    largest_blob = max(blobs, key=lambda b: b.pixels())

    if largest_blob.pixels() < 150:
        return None

    angle = int((largest_blob.cx() - 160) * (DEGREES_IN_PIXEL))
    distance = map_value(largest_blob.area(), MIN_AREA, MAX_AREA, 0, 255) # estimate_distance(largest_blob.y())

    aspect_ratio = largest_blob.w() / largest_blob.h()
    confidence = min(int((aspect_ratio * 100) / 6.0), 100)
    if confidence <= min_confidence:
        return

    return (angle, distance, confidence, largest_blob)


while True:
    clock.tick()
    img = sensor.snapshot()

    img.lens_corr(strength=1.8)

    # yellow_blobs = img.find_blobs([YELLOW_THRESHOLD], pixels_threshold=100, merge=True)
    # blue_blobs = img.find_blobs([BLUE_THRESHOLD], pixels_threshold=100, merge=True)
    all_blobs = img.find_blobs(THRESHOLDS, pixels_threshold=100, merge=True, x_stride=4, y_stride=2)
    yellow_blobs = [b for b in all_blobs if b.code() == 1]  # 1st threshold in list
    blue_blobs = [b for b in all_blobs if b.code() == 2]  # 2nd threshold in list

    yellow_goal = process_goal(yellow_blobs, 0)
    blue_goal = process_goal(blue_blobs, 0)

    current_goal = yellow_goal if TARGET == "YELLOW" else blue_goal

    if current_goal:
        angle, distance, confidence, goal_blob = current_goal
        img.draw_rectangle(goal_blob.rect())  # Keeps visual feedback overhead low
    else:
        angle, distance, confidence = 254, 254, 254
        goal_blob = None

    received = 0
    while uart.any():
        received = uart.read(1)[0]

    if received == 255:
        uart.write(bytes([angle + 70]))
        uart.write(bytes([distance]))
        uart.write(bytes([confidence]))

    print(angle, distance, confidence, goal_blob.y(), clock.fps(), goal_blob.area())
    # print(sensor.get_gain_db(), sensor.get_exposure_us(), sensor.get_rgb_gain_db(), )
