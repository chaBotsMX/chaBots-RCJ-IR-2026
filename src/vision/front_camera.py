# front_camera.py

import sensor, time
from pyb import UART

TARGET = "YELLOW"

YELLOW_THRESHOLD = (50, 100, 11, 127, 39, 127)
BLUE_THRESHOLD = (0, 32, -2, 23, -128, 0)

IMG_CX = 160

ROI = (0, 80, 320, 80)

FOV = 140

# Calibration points: (top_y_pixel, distance_in_cm)
# CRITICAL: Keep this list sorted from LOWEST top_y to HIGHEST top_y
CALIBRATION_POINTS = [
    (0, 0),
    (0, 0),
    (0, 0),
    (0, 0),
    (0, 0)
]

sensor.reset()

sensor.set_contrast(0)
sensor.set_brightness(0)
sensor.set_saturation(0)

sensor.set_auto_gain(False)
sensor.set_auto_rotation(False)

sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)

sensor.skip_frames(time=2000)

clock = time.clock()

uart = UART(3, 115200, timeout_char=0)

sensor.set_windowing(ROI)

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

def process_goal(blobs):
    if not blobs:
        return None

    largest_blob = max(blobs, key=lambda b: b.pixels())

    if largest_blob.pixels() < 150:
        return None

    angle = int((largest_blob.cx() - 160) * (FOV / 320))
    distance = estimate_distance(largest_blob.y())

    aspect_ratio = largest_blob.w() / largest_blob.h()
    confidence = (aspect_ratio * 100) / 2.5

    return (angle, distance, confidence)

while True:
    clock.tick()
    img = sensor.snapshot()

    img.lens_corr(strenght=1.8)

    yellow_blobs = img.find_blobs([YELLOW_THRESHOLD], pixels_threshold=100, merge=True)
    blue_blobs   = img.find_blobs([BLUE_THRESHOLD], pixels_threshold=100, merge=True)

    yellow_goal = process_goal(yellow_blobs)
    blue_goal = process_goal(blue_blobs)

    angle = 254
    distance = 254
    confidence = 254

    if TARGET == "YELLOW" and yellow_goal:
        angle = yellow_goal[0]
        distance = yellow_goal[1]
        confidence = yellow_goal[2]
        img.draw_rectangle(yellow_blobs.rect(), color=(255, 255, 0))
    elif TARGET == "BLUE" and blue_goal:
        angle = blue_goal[0]
        distance = blue_goal[1]
        confidence = blue_goal[2]
        img.draw_rectangle(blue_blobs.rect(), color=(0, 0, 255))

    received = 0
    while uart.any():
        # read(1) gets 1 byte as a bytes object (e.g., b'\xff')
        # indexing [0] converts it to an integer (0-255)
        received = uart.read(1)[0]

        # 3. Check if the trigger byte (255) was received
        if received == 255:
            uart.write(bytes([angle]))
            uart.write(bytes([distance]))
            uart.write(bytes([confidence]))

    print(yellow_goal, blue_goal, clock.fps())
