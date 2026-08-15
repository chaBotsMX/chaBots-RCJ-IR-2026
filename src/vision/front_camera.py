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

BOTTOM_CENTER_X = 160
BOTTOM_CENTER_Y = 239

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
distance = 254
confidence = 254


def process_goal(blobs):
    if not blobs:
        return None

    largest_blob = max(blobs, key=lambda b: b.pixels)

    if largest_blob.pixels < 150:
        return None

    angle = int((largest_blob.cx - 160) * (DEGREES_IN_PIXEL))

    dx = largest_blob.cx - BOTTOM_CENTER_X
    dy = largest_blob.cy - BOTTOM_CENTER_Y
    distance = int((dx**2 + dy**2) ** 0.5)

    aspect_ratio = largest_blob.w / largest_blob.h
    confidence = int((aspect_ratio * 100) / 2.5)

    return (angle, distance, confidence, largest_blob)


while True:
    clock.tick()
    img = csi0.snapshot()

    img.lens_corr(strength=1.8)

    if TARGET == "YELLOW":
        current_goal = process_goal(
            img.find_blobs(
                [YELLOW_THRESHOLD],
                pixels_threshold=100,
                merge=True,
                x_stride=4,
                y_stride=2,
            )
        )
    else:
        current_goal = process_goal(
            img.find_blobs(
                [BLUE_THRESHOLD],
                pixels_threshold=100,
                merge=True,
                x_stride=4,
                y_stride=2,
            )
        )

    if current_goal:
        angle, distance, confidence, goal_blob = current_goal
        img.draw_rectangle(goal_blob.rect)

        # Visual Overlay: Draw tracking line from bottom-center to goal center
        img.draw_line(
            (BOTTOM_CENTER_X, BOTTOM_CENTER_Y, goal_blob.cx, goal_blob.cy), thickness=2
        )

        # Visual Overlay: Draw target center point
        img.draw_cross(
            (goal_blob.cx, goal_blob.cy), size=5
        )

    else:
        angle = 254
        distance = 254
        confidence = 254
        goal_blob = None

    received = 0
    while uart.any():
        received = uart.read(1)[0]

    if received == 255:
        uart.write(bytes([angle + 70]))
        uart.write(bytes[distance])
        uart.write(bytes[confidence])

    print("Angle:", angle, "Dist:", distance, "FPS:", clock.fps())
