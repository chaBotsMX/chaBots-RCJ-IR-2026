# front_camera.py

import csi
import time
from pyb import UART

TARGET = "YELLOW"

YELLOW_THRESHOLD = (81, 100, -16, 127, 15, 127)
BLUE_THRESHOLD = (0, 21, -10, 127, -128, 0)
THRESHOLDS = [YELLOW_THRESHOLD, BLUE_THRESHOLD]

CROP_FROM_TOP = 70

ROI = (0, CROP_FROM_TOP, 320, 240 - CROP_FROM_TOP)

FOV_HORIZONTAL = 71.8
DEGREES_IN_PIXEL = FOV_HORIZONTAL / 320

# Calibration points: (top_y_pixel, distance_in_cm)
# Keep this list sorted from LOWEST top_y to HIGHEST top_y
# needs to calibrate
CALIBRATION_POINTS = [
    (14, 200),
    (16, 150),
    (20, 100),
    (22, 50)
]

# Init CSI Camera
csi0 = csi.CSI()

csi0.sleep(True)
time.sleep(0.01)

csi0.reset()
csi0.pixformat(csi.RGB565)
csi0.framesize(csi.QVGA)

csi0.vflip(True)

csi0.auto_gain(False, gain_db=22)
# csi0.auto_whitebal(False, rgb_gain_db=(4.2, 0.0, 4.0))
# csi0.auto_exposure(False, exposure_us=20000)

# Warm up / skip initial frames
csi0.snapshot(time=1500)

# csi0.window(ROI)

clock = time.clock()

uart = UART(3, 115200, timeout_char=0)

angle = 254


def estimate_distance(blob_y):
    if blob_y <= CALIBRATION_POINTS[0][0]:
        return CALIBRATION_POINTS[0][1]
    if blob_y >= CALIBRATION_POINTS[-1][0]:
        return CALIBRATION_POINTS[-1][1]

    for i in range(len(CALIBRATION_POINTS) - 1):
        y0, d0 = CALIBRATION_POINTS[i]
        y1, d1 = CALIBRATION_POINTS[i + 1]

        if y0 <= blob_y <= y1:
            # Linear Interpolation Formula
            fraction = (blob_y - y0) / (y1 - y0)
            interpolated_distance = d0 + fraction * (d1 - d0)
            return interpolated_distance
    return 254


def map_value(x, in_min, in_max, out_min, out_max):
    if in_max == in_min:
        return out_min

    mapped = out_min + (x - in_min) * (out_max - out_min) / (in_max - in_min)
    return max(min(int(mapped), out_max), out_min)


def process_goal(blobs, min_confidence):
    if not blobs:
        return None

    largest_blob = max(blobs, key=lambda b: b.pixels)

    if largest_blob.pixels < 150:
        return None

    angle = int((largest_blob.cx - 160) * (DEGREES_IN_PIXEL))

    return (angle, largest_blob)


while True:
    clock.tick()
    img = csi0.snapshot()

    img.lens_corr(strength=1)

    all_blobs = img.find_blobs(THRESHOLDS, pixels_threshold=100, merge=True, x_stride=4, y_stride=2)
    yellow_blobs = [b for b in all_blobs if b.code == 1]  # 1st threshold in list
    blue_blobs = [b for b in all_blobs if b.code == 2]    # 2nd threshold in list

    yellow_goal = process_goal(yellow_blobs, 0)
    blue_goal = process_goal(blue_blobs, 0)

    current_goal = yellow_goal if TARGET == "YELLOW" else blue_goal

    if current_goal:
        angle, goal_blob = current_goal
        # draw_rectangle now expects a coordinate tuple; blob.rect() returns one: (x, y, w, h)
        img.draw_rectangle(goal_blob.rect())
    else:
        angle = 254
        goal_blob = None

    received = 0
    while uart.any():
        received = uart.read(1)[0]

    if received == 255:
        uart.write(bytes([angle + 70]))

    print(angle, clock.fps())
    print(csi0.gain_db(), csi0.exposure_us(), csi0.rgb_gain_db())
