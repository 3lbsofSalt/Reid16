/** @file auto.c
 * @brief File for autonomous code
 *
 * This file should contain the user autonomous() function and any functions related to it.
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
 * Runs the user autonomous code. This function will be started in its own task with the default
 * priority and stack size whenever the robot is enabled via the Field Management System or the
 * VEX Competition Switch in the autonomous mode. If the robot is disabled or communications is
 * lost, the autonomous task will be stopped by the kernel. Re-enabling the robot will restart
 * the task, not re-start it from where it left off.
 *
 * Code running in the autonomous task cannot access information from the VEX Joystick. However,
 * the autonomous function can be invoked from another task if a VEX Competition Switch is not
 * available, and it can access joystick information if called in this way.
 *
 * The autonomous task may exit, unlike operatorControl() which should never exit. If it does
 * so, the robot will await a switch to another mode or disable/enable cycle.
 */

//Motor Constants

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

int encoderSpeed(){				//Gets encoder speed.
  int old;
  int new;
  
  old = encoderGet(speedEnc); //Gets encoder before time delay
  delay(20);
  new = encoderGet(speedEnc); //Get encoder after time delay
  return new - old;			//Get Rotations per delay ms
}

void conveyorForward() {        //Intakes
  motorSet(intake, 127);
}

void conveyorStop() {			//Stop conveyor
  motorSet(intake, 0);
}

void conveyorBackward() { 		//Outtake
  motorSet(intake, -127);
}

void flywheelStart() {			//Start Flywheel
  motorSet(flywheelOne, 127);
  motorSet(flywheelTwo, 127);
  motorSet(flywheelThree, 127);
  motorSet(flywheelFour, 127);
}

void flywheelHalf() {			//Start Flywheel
  motorSet(flywheelOne, 50);
  motorSet(flywheelTwo, 50);
  motorSet(flywheelThree, 50);
  motorSet(flywheelFour, 50);
}

void flywheelStop() {			//Stop Flywheel
  motorSet(flywheelOne, 0);
  motorSet(flywheelTwo, 0);
  motorSet(flywheelThree, 0);
  motorSet(flywheelFour, 0);
}

void runBallControl() {  		//Allows a ball to shoot
  motorSet(ballControl, 127);
}

void stopBallControl() {		//Stop balls from shooting
  motorSet(ballControl, 0);
}

void forwardOuttake() {
  motorSet(frontLeftDrive, 127);
  motorSet(backLeftDrive, 127);
  motorSet(backRightDrive, 127);
  motorSet(frontRightDrive, -127);
  motorSet(intake, -127);
}

void stopAll() {
  motorSet(frontLeftDrive, 0);
  motorSet(backLeftDrive, 0);
  motorSet(backRightDrive, 0);
  motorSet(frontRightDrive, 0);
  motorSet(intake, 0);
  motorSet(flywheelOne, 0);
  motorSet(flywheelTwo, 0);
  motorSet(flywheelThree, 0);
  motorSet(flywheelFour, 0);
  motorSet(ballControl, 0);
}

Encoder speedEnc;
void autonomous() {
  int speed;
  int targetSpeed = 82;
  Encoder left;
  Encoder right;

  left = encoderInit(3, 4, 1);
  right = encoderInit(5, 6, 1);
  speedEnc = encoderInit(1, 2, 0);

  encoderReset(left);
  encoderReset(right);
  encoderReset(speedEnc);
  
  lcdInit(uart1);
  lcdSetBacklight(uart1, true);

  ///////////////MATCH AUTONOMOUS////////////////
  if(digitalRead(7) == HIGH){
    while(1){
      lcdPrint(uart1, 1, "SWEET AUTO");
      lcdPrint(uart1, 2, "%d", encoderGet(speedEnc));
      speed = encoderSpeed();
      
      if(speed < targetSpeed){ //Flywheel control statements. Turns them on if they aren't fast enough, turns it of if it's too fast.
	flywheelStart();
      }else if (speed > targetSpeed) {
	flywheelHalf();
      }else if (speed > targetSpeed + 4){ //If flywheel is too fast, slow down
      flywheelStop();
      }
      
      if((speed > targetSpeed - 1) && (speed < targetSpeed + 2)){ //Ball control loop. Make the added values larger to make it less accurate
	runBallControl();
      } else {
	stopBallControl();
      }
      
      if((speed > targetSpeed - 2) && (speed < targetSpeed + 2)){ //Conveyor control loop. Make the conveyor run a little more than the ballControl.
	conveyorForward();
      } else {
	conveyorStop();
      }
      if(encoderGet(speedEnc) > 50000){
	stopAll();
        lcdPrint(uart1, 1, "Stopped");
	break;
      }
    }
    while(encoderGet(left) < 2000 && encoderGet(right) < 2000){
      lcdPrint(uart1, 1, "%d Right", encoderGet(right));
      lcdPrint(uart1, 2, "%d Left", encoderGet(left));
      forwardOuttake();
    }
    while(1) {
      
    }

    /////////////////////////SKILLS AUTONOMOUS/////////////////////////////////
  } else if (digitalRead(7) == LOW) {
    while(1){
      lcdPrint(uart1, 1, "SKILLS AUTO");
      speed = encoderSpeed();
      
      if(speed < targetSpeed){ //Flywheel control statements. Turns them on if they aren't fast enough, turns it of if it's too fast.
	flywheelStart();
      }else if (speed > targetSpeed) {
	flywheelHalf();
      }else if (speed > targetSpeed + 4){ //If flywheel is too fast, slow down
	flywheelStop();
      }
      
      if((speed > targetSpeed - 1) && (speed < targetSpeed + 2)){ //Ball control loop. Make the added values larger to make it less accurate
	runBallControl();
      } else {
	stopBallControl();
      }
      
      if((speed > targetSpeed - 3) && (speed < targetSpeed + 3)){ //Conveyor control loop. Make the conveyor run a little more than the ballControl.
	conveyorForward();
      } else {
	conveyorStop();
      }
    }
  }
}
