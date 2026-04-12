# Goalkeeper Defender - README

## What Changed

Your goalkeeper now uses a smooth, controlled algorithm instead of simple angle-based movement. It stays on the line while blocking shots.

---

## Files Modified

### 1. main.cpp (This Folder)
- The main control code for your goalkeeper
- Changed to use vector-based movement instead of simple angle following
- Result: Smoother, better defense

### 2. Line Sensors (lib/inputs/line-sensor/)
- Detects the white boundary line with 32 sensors
- Added methods to access raw sensor data
- Needed for the goalkeeper to know where the line is

### 3. Ball Sensors (lib/inputs/ir-sensor/)
- Detects the ball position using infrared sensors
- Added methods to access raw sensor data
- Needed for the goalkeeper to know where to block

---

## New Files Created

### GoalkeeperController (lib/control/goalkeeper/)
- Two new files: GoalkeeperController.h and GoalkeeperController.cpp
- The core algorithm that controls goalkeeper movement
- Combines line tracking and ball blocking into smooth motion

---

## How It Works

Your goalkeeper balances two forces:

1. Stay on the line (keeps it centered)
2. Move toward the ball (blocks shots)

When the ball comes from the left, the robot slides left while staying on the line.

---

## Before Testing

- Code compiles with zero errors
- Default settings are already configured must be tuned

---

---

## Quick Tuning

If the robot needs adjustment:

File: src/main_defender/main.cpp
Look for: setLineCoefficient(1.2)

Adjust if needed:
- Robot drifts off line: increase to 1.3 or 1.4
- Robot is slow: decrease to 1.0 or 1.1

Usually 1.2 works well apparently. Max values of 1.5 - 2.0 it will be stable but with low response

---

