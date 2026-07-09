/**
 * @file AttackerControl.cpp
 * @brief Implementation of attacker control
 *
 * @author Alfonso De Anda / chaBotsMX
 * @date 2026-04-12
 */

#include "AttackerControl.h"

AttackerControl::AttackerControl() {}

MovementCommandAtk AttackerControl::calculateMovement(int currentLineAngle, int irAngle, int irDistance, int cameraAngle, float yaw) {
    MovementCommandAtk cmd;
  
    if(line.lineDetected(currentLineAngle)) {
        if(!firstDetected) {
            initialLineAngle = currentLineAngle;
            firstDetected = true;
        }
        cmd.angle = line.getAvoidLineAngle(currentLineAngle, initialLineAngle);
        cmd.power = 100; // Line detected
        cmd.rotation = 0;
    }
    else if(irAngle <= 360) {
        //cmd.angle = getBallChasingAngleNoDistance(irAngle, irDistance);
        //cmd.angle = getBallChasingAngle(irAngle, irDistance);
        cmd.angle = getBallChasingAngleNew(irAngle, irDistance);
        cmd.power = getBallChasingPower(irAngle, irDistance);
        firstDetected = false; // Reset line detection when ball is detected
    }
    else {
        cmd.angle = 0;
        cmd.power = 0; // No ball detected
        cmd.rotation = 0;
        firstDetected = false; // Reset line detection when no ball is detected
    }
  
    return cmd;
}

int AttackerControl::getBallChasingAngle(int irAngle, int irDistance) {
    if(isBallOnFront(irAngle, irDistance)) return 90; // Ball is close, go straight
    int xBall = irDistance * cos(radians(irAngle));
    int yBall = irDistance * sin(radians(irAngle));

    double t = ((xBall * xBall) + (yBall * yBall - kIRDistanceOffset)) / ((xBall * xBall) + (yBall - kIRDistanceOffset) * (yBall - kIRDistanceOffset));
    t = max(0.0, min(1.0, t)); // Clamp t to [0, 1]
    int xClosest = xBall * t;
    int yClosest = (yBall - kIRDistanceOffset) * t;

    bool shouldOrbit = ((xClosest - xBall) * (xClosest - xBall) + (yClosest - yBall) * (yClosest - yBall)) < (kAvoidDistance * kAvoidDistance);

    if (shouldOrbit) {
        int offsetAngle = asin(kIRDistanceOffset / irDistance);
        if(irAngle > 270 or irAngle < 90) return irAngle - offsetAngle;
        return irAngle + offsetAngle;
    }
    return (int)degrees(atan2(yBall - kIRDistanceOffset, xBall));
}

int AttackerControl::getBallChasingAngleNoDistance(int irAngle, int irDistance) {
    if(irAngle > 360 || irAngle < 0){
        return 500;  // Invalid angle
    }
    
    if(isBallOnFront(irAngle, irDistance)) return 90; // Ball is close, go straight
    
    if(irDistance > 210) return irAngle;
    
    // Right side
    if(irAngle > 270 || irAngle < 75){
        int adjusted = irAngle - 90;
        // Fix negative modulo
        if(adjusted < 0) adjusted += 360;
        return adjusted;
    }
    // Left side
    else if(irAngle > 105 && irAngle < 270){
        int adjusted = irAngle + 90;
        // Handle wrap-around
        if(adjusted >= 360) adjusted -= 360;
        return adjusted;
    }
    else{
        return 90; // Ball is directly in front, go straight
    }
}

int AttackerControl::getBallChasingAngleNew(int irAngle, int irDistance) {
    float proximity = map(float(irDistance), 0.0, 90, 1.0, 0.0); // Closer ball gives higher proximity
    
    if(irAngle > 270 || irAngle < apertureLeft){ //right side
        int adjusted = irAngle - (90 * proximity); // Adjust angle based on proximity
        // Fix negative modulo
        if(adjusted < 0) adjusted += 360;
        return adjusted;
    }
    // Left side
    else if(irAngle > apertureRight && irAngle < 270){
        int adjusted = irAngle + (90 * proximity);
        // Handle wrap-around
        if(adjusted >= 360) adjusted -= 360;
        return adjusted;
    }
    else{
        return irAngle;
    }
}

/* int AttackerControl::getBallChasingPower(int irAngle, int irDistance) {
    // If the ball is completely lost, safe return to minimum power or 0
    if (irDistance > 90) return minPower;

    // 1. Constrain raw distance to the valid 0-90 scale just in case
    int constrainedDist = constrain(irDistance, 0, 90);

    // 2. Map distance directly to power: 
    // 0 (closest) -> minPower
    // 90 (furthest) -> maxPower
    int calculatedPower = map(constrainedDist, 0, 90, minPower, maxPower);

    // 3. Override rule: If the ball is right in front, charge! 
    if (isBallOnFront(irAngle, irDistance)) {
        return maxPower; 
    }

    return calculatedPower;
} */

int AttackerControl::getBallChasingPower(int irAngle, int irDistance) {
    if (irDistance > maxDistance) return minPower;

    // 1. Calculate the shortest angular error from the front (90 degrees)
    int angleError = abs(irAngle - 90);
    if (angleError > 180) {
        angleError = 360 - angleError;
    }

    // 2. Base Factors (0.0 to 1.0)
    float distanceFactor = constrain(irDistance, 0, maxDistance) / maxDistance;
    float angleFactor = angleError / 180.0f;

    // 3. The "Front-Channel Strike" Modification
    // If the ball is closely lined up with the front (+/- 15 degrees) 
    // and within striking distance (< 80)
    if (angleError <= 20 && irDistance < 60) {
        // Invert the distance so: 0 (closest) -> 1.0, 80 (limit) -> 0.0
        float proximity = (60.0f - constrain(irDistance, 0, 60)) / 60.0f;
        
        // Boost the distance factor using a smooth curve.
        // As proximity approaches 1.0 (very close), distanceFactor drops to 0 
        // normally, but we blend in this boost to force the power higher.
        distanceFactor = distanceFactor + (proximity * 0.6f); 
        if (distanceFactor > 1.0f) distanceFactor = 1.0f;
        
        // Visually/Logically: This forces the robot to ignore the "slow down 
        // when close" rule ONLY when the ball is perfectly lined up to be captured.
    }

    // 4. Combine with your tuned weights
    const float DISTANCE_WEIGHT = 0.65f;
    float combinedFactor = (distanceFactor * DISTANCE_WEIGHT) + (angleFactor * (1.0f - DISTANCE_WEIGHT));

    // 5. Final Output calculation
    int calculatedPower = minPower + (int)(combinedFactor * (maxPower - minPower));

    // debug
    debug_distanceFactor = distanceFactor;
    debug_angleFactor = angleFactor;
    debug_combinedFactor = combinedFactor;

    return constrain(calculatedPower, minPower, maxPower);
}

bool AttackerControl::isBallOnFront(int irAngle, int irDistance) {
    if((irAngle >= apertureLeft and irAngle <= apertureRight) and irDistance < 80) return true;
    return false;
}