/** @file opcontrol.c
 * @brief File for operator control code
 *
 * This file should contain the user operatorControl() function and any functions related to it.
 *
 * Copyright (c) 2011-2014, Purdue University ACM SIG BOTS.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Purdue University ACM SIG BOTS nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL PURDUE UNIVERSITY ACM SIG BOTS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Purdue Robotics OS contains FreeRTOS (http://www.freertos.org) whose source code may be
 * obtained from http://sourceforge.net/projects/freertos/files/ or on request.
 */

#include "main.h"
/*
 * Runs the user operator control code. This function will be started in its own task with the
 * default priority and stack size whenever the robot is enabled via the Field Management System
 * or the VEX Competition Switch in the operator control mode. If the robot is disabled or
 * communications is lost, the operator control task will be stopped by the kernel. Re-enabling
 * the robot will restart the task, not resume it from where it left off.
 *
 * If no VEX Competition Switch or Field Management system is plugged in, the VEX Cortex will
 * run the operator control task. Be warned that this will also occur if the VEX Cortex is
 * tethered directly to a computer via the USB A to A cable without any VEX Joystick attached.
 *
 * Code running in this task can take almost any action, as the VEX Joystick is available and
 * the scheduler is operational. However, proper use of delay() or taskDelayUntil() is highly
 * recommended to give other tasks (including system tasks such as updating LCDs) time to run.
 *
 * This task should never exit; it should end with some kind of infinite loop, even if empty.
 */
Encoder speedEnc;
int encoderSpeedOp(){
  int old;
  int new;
  
  old = encoderGet(speedEnc);
  if(old > 100000){
    encoderReset(speedEnc);
    old = 0;
  }
  delay(20);
  new = encoderGet(speedEnc);
  return new - old;
}

void operatorControl() {
  //Front Drive motor is toward intake
  //Flywheel motor numbers are from bottom to top
  
  const int frontLeftDrive = 4;
  const int frontRightDrive = 7;
  const int backLeftDrive = 1;
  const int backRightDrive = 6;
  const int ballControl = 10;
  const int flywheelTwo = 2;
  const int flywheelThree = 3;
  const int flywheelOne = 9;
  const int flywheelFour = 8;
  const int intake = 5;
  
  //LCD Backlight
  lcdSetBacklight(uart1, true);
  
  //Encoder Variables/Init
  if(!speedEnc){
    speedEnc = encoderInit(1, 2, 0);
  }
  
  encoderReset(speedEnc);
  int speed = 0;
  
  
  int deadzone = 20; //Sets joystick deadzone in case of incorrect analog positioning
  int xAxis; //Holds X axis for drive analog stick
  int yAxis; //Holds Y axis for drive analog stick
  int intakeForward; //Holds 1 or 0 from one of the left joystick shoulder buttons to tell if the intake should run forward
  int intakeBackward; //Holds 1 or 0 from other left joystick shoulder button to tell if intake should run backward
  int targetSpeed = 0;
  
  while (1) {
    
    speed = encoderSpeedOp(); //Get the flywheel distance
    /* lcdPrint(uart1, 1, "%d TargetSpeed", targetSpeed); */
    lcdPrint(uart1, 2, "%d Speed", speed);
    
    xAxis = joystickGetAnalog(1, 1); //Assigns joystick value to X Axis variable
    yAxis = joystickGetAnalog(1, 2); //Assigns joystick value to Y Axis variable
    
    /////////
    //DRIVE//
    /////////
    
    if(abs(xAxis) > deadzone || abs(yAxis) > deadzone){ //Checks to see if joystick is past deadzone, if it is then it engages drive
      motorSet(frontLeftDrive, yAxis + xAxis);
      motorSet(backLeftDrive, yAxis + xAxis);
      motorSet(backRightDrive, yAxis - xAxis);
      motorSet(frontRightDrive, -yAxis + xAxis);
    } else { //Turns of drive motors if joystick is not being pressed
      motorSet(frontLeftDrive, 0);
      motorSet(backLeftDrive, 0);
      motorSet(backRightDrive, 0);
      motorSet(frontRightDrive, 0);
    }
    
    //////////
    //INTAKE//
    //////////
    
    intakeForward = joystickGetDigital(1, 5, JOY_DOWN); //Checks to see if left bottom joystick shoulder button is pressed, if so, it assigns a value of one to intakeForward
    intakeBackward = joystickGetDigital(1, 5, JOY_UP); //Checks to see if left top joystick shoulder button is pressed if so, it assigns a value of 1 to intakeBackward
    if(intakeForward){			//Intake
      motorSet(intake, 127);
    } else if(intakeBackward){	//Outtake
      motorSet(intake, -127);
    } else {					//Stop Conveyor
      motorSet(intake, 0);
    }
    
    /////////////////////
    //BALL CONTROL LOOP//
    /////////////////////
    
    if((joystickGetDigital(1, 6, JOY_DOWN) && (speed > targetSpeed - 1) && (speed < targetSpeed + 2)) || joystickGetDigital(1, 7, JOY_UP)){ //Won't the ball shoot unless flywheel is at +- 3 to the correct RPM
      motorSet(ballControl, 127);
    } else {
      motorSet(ballControl, 0);
    }
    
    ////////////
    //FLYWHEEL//
    ////////////
    
    //From square = 86
    //Midfield = 69
    //1 Square away = 63
    
    
    if(joystickGetDigital(1, 8, JOY_UP)){ //Set target speed to 85
      targetSpeed = 82;
    }
    
    if(joystickGetDigital(1, 8, JOY_LEFT)){ //Set target speed to 69
      targetSpeed = 64;
    }
    
    if(joystickGetDigital(1, 8, JOY_RIGHT)){ //Set target speed to 63
      targetSpeed = 59;
    }
    
    if(joystickGetDigital(1, 8, JOY_DOWN)){ //Set flywheels to off
      targetSpeed = 0;
    }
    
    
    if(speed < targetSpeed){ //If flywheel isn't fast enough, speed up
      motorSet(flywheelOne, 127);
      motorSet(flywheelTwo, 127);
      motorSet(flywheelThree, 127);
      motorSet(flywheelFour, 127);
    } else if(speed > targetSpeed && targetSpeed > 80) {
      motorSet(flywheelOne, 50);
      motorSet(flywheelTwo, 50);
      motorSet(flywheelThree, 50);
      motorSet(flywheelFour, 50);
    } else if (speed > targetSpeed + 4){ //If flywheel is too fast, slow down
      motorSet(flywheelOne, 0);
      motorSet(flywheelTwo, 0);
      motorSet(flywheelThree, 0);
      motorSet(flywheelFour, 0);
    }

    /////////////
    //TEST CODE//
    /////////////
    if(joystickGetDigital(1, 7, JOY_DOWN)){
      lcdPrint(uart1, 1, "%d TargetSpeed", encoderGet(speedEnc));
    }
    delay(20);
  }
}  
