# front_camera.py

import sensor
import time
from pyb import UART

TARGET = "YELLOW"

YELLOW_THRESHOLD = (23, 100, -5, 127, 20, 127)
BLUE_THRESHOLD = (0, 11, -128, 17, -128, -3)
THRESHOLDS = [YELLOW_THRESHOLD, BLUE_THRESHOLD]

ROI = (0, 80, 320, 80)

FOV = 140
DEGREES_IN_PIXEL = 0.4375

# Calibration points: (top_y_pixel, distance_in_cm)
# Keep this list sorted from LOWEST top_y to HIGHEST top_y
# needs to calibrate
CALIBRATION_POINTS = [
    (0, 0),
    (0, 0),
    (0, 0),
    (0, 0),
    (0, 0)
]

# init sensor

sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.set_framerate(120)

sensor.set_auto_gain(False, gain_db=50)

#sensor.set_auto_whitebal(False)
sensor.set_auto_exposure(False, exposure_us=200000)

sensor.set_contrast(1)
sensor.set_brightness(3)
sensor.set_saturation(2)
sensor.skip_frames(time=500)

sensor.set_windowing(ROI)

clock = time.clock()

# uart = UART(3, 115200, timeout_char=0)

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


def process_goal(blobs, min_confidence):
    if not blobs:
        return None

    largest_blob = max(blobs, key=lambda b: b.pixels())

    if largest_blob.pixels() < 150:
        return None

    angle = int((largest_blob.cx() - 160) * (DEGREES_IN_PIXEL))
    distance = estimate_distance(largest_blob.y())

    aspect_ratio = largest_blob.w() / largest_blob.h()
    confidence = min(int((aspect_ratio * 100) / 6.0), 100)
    if confidence <= min_confidence:
        return

    return (angle, distance, confidence, largest_blob)


while True:
    clock.tick()
    img = sensor.snapshot()

    # img.lens_corr(strength=1.8)

    # yellow_blobs = img.find_blobs([YELLOW_THRESHOLD], pixels_threshold=100, merge=True)
    # blue_blobs = img.find_blobs([BLUE_THRESHOLD], pixels_threshold=100, merge=True)
    all_blobs = img.find_blobs(THRESHOLDS, pixels_threshold=100, merge=True, x_stride=4, y_stride=2)
    yellow_blobs = [b for b in all_blobs if b.code() == 1]  # 1st threshold in list
    blue_blobs = [b for b in all_blobs if b.code() == 2]  # 2nd threshold in list

    yellow_goal = process_goal(yellow_blobs, 75)
    blue_goal = process_goal(blue_blobs, 75)

    current_goal = yellow_goal if TARGET == "YELLOW" else blue_goal

    if current_goal:
        angle, distance, confidence, goal_blob = current_goal
        img.draw_rectangle(goal_blob.rect())  # Keeps visual feedback overhead low
    else:
        angle, distance, confidence = 254, 254, 254

    received = 0
    while uart.any():
        received = uart.read(1)[0]

    if received == 255:
        uart.write(bytes([angle + 70]))
        uart.write(bytes([distance]))
        uart.write(bytes([confidence]))

    print(angle, distance, confidence, clock.fps())
