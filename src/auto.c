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
/*
const int frontLeftDrive = 4;
const int frontRightDrive = 7;
const int backLeftDrive = 5;
const int backRightDrive = 6;
const int ballControl = 10;
const int flywheelTwo = 2;
const int flywheelThree = 3;
const int flywheelOne = 9;
const int flywheelFour = 8;
const int intake = 1;

int encoderSpeed(){
	int old;
	int new;

	old = encoderGet(speedEnc); //Gets encoder before time delay
	if(old > 100000){	 		//If too many ticks, reset for accuracy and so that the int value will hold
		encoderReset(speedEnc);
		old = 0;
	}
	delay(20);					//Allow 20 ms to pass
	new = encoderGet(speedEnc); //Get encoder after time delay
	return new - old;			//Get Rotations per 20 ms
}

void conveyorStart() {
	motorSet(intake, -127);
}

void conveyorStop() {
	motorSet(intake, 0);
}

void flywheelStart() {			//Start Flywheel
	motorSet(flywheelOne, 127);
	motorSet(flywheelTwo, 127);
	motorSet(flywheelThree, 127);
	motorSet(flywheelFour, 127);
}

void flywheelStop() {			//Stop Flywheel
	motorSet(flywheelOne, 0);
	motorSet(flywheelTwo, 0);
	motorSet(flywheelThree, 0);
	motorSet(flywheelFour, 0);
}

void runBallControl() {  		//Allows a ball to shoot
	motorSet(ballControl, -127);
}

void stopBallControl() {
	motorSet(ballControl, 0);
}*/

void autonomous() {
/*
	const int ballControl = 10;
	const int flywheelTwo = 2;
	const int flywheelThree = 3;
	const int flywheelOne = 9;
	const int flywheelFour = 8;
	const int intake = 1;

	motorSet(flywheelOne, 86);
	motorSet(flywheelTwo, 86);
	motorSet(flywheelThree, 86);
	motorSet(flywheelFour, 86);
	motorSet(intake, -127);
	motorSet(ballControl, -127);*/
}
