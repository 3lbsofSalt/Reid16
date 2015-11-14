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

/* MOTOR CONSTANTS

1 = Front Right Drive
2 = Conveyor
3 = Left Flywheel Back
4 = Back Left Drive
5 = Intake
6 = Front Left Drive
7 = Right Flywheel Back
8 = Left Flywheel Front
9 = Back Right Drive
10 = Right Flywheel Front*/

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
void operatorControl() {
	//Front Drive motor is toward intake
	//Front Flywheel motor is toward flywheel

	const int frontLeftDrive = 6;
	const int frontRightDrive = 1;
	const int backLeftDrive = 4;
	const int backRightDrive = 9;
	const int conveyor = 2;
	const int frontRightFlywheel = 10;
	const int backRightFlywheel = 7;
	const int frontLeftFlywheel = 8;
	const int backLeftFlywheel = 3;
	const int intake = 5;

	int deadzone = 20; //Sets joystick deadzone in case of incorrect analog positioning
	int xAxis; //Holds X axis for drive analog stick
	int yAxis; //Holds Y axis for drive analog stick
	int intakeForward; //Holds 1 or 0 from one of the left joystick shoulder buttons to tell if the intake should run forward
	int intakeBackward; //Holds 1 or 0 from other left joystick shoulder button to tell if intake should run backward
	int flyWheel = 0; //Holds integer checking flywheel speed
	int halfSpeed = 0;

	while (1) {

		xAxis = joystickGetAnalog(1, 1); //Assigns joystick value to X Axis variable
		yAxis = joystickGetAnalog(1, 2); //Assigns joystick value to Y Axis variable

		//DRIVE

		if(halfSpeed != 1){
			//Runs Drive at FULL SPEED
			if(abs(xAxis) > deadzone || abs(yAxis) > deadzone){ //Checks to see if joystick is past deadzone, if it is then it engages drive
				motorSet(frontLeftDrive, yAxis + xAxis); //Front Left Drive
				motorSet(backLeftDrive, yAxis + xAxis); //Back Left Drive
				motorSet(backRightDrive, -yAxis + xAxis); //Back Right Drive
				motorSet(frontRightDrive, yAxis - xAxis); //Front Right Drive
				halfSpeed = joystickGetDigital(1, 8, JOY_LEFT); //Gets if driver wants HALF speed
			} else { //Turns of drive motors if joystick is not being pressed
				motorSet(frontLeftDrive, 0); //Front Left Drive
				motorSet(backLeftDrive, 0); //Back Left Drive
				motorSet(backRightDrive, 0); //Back Right Drive
				motorSet(frontRightDrive, 0); //Front Right Drive
				halfSpeed = joystickGetDigital(1, 8, JOY_LEFT); //Gets if driver wants HALF speed
			}
		} else if (halfSpeed == 1) {
			//Runs Drive at HALF SPEED

			if(abs(xAxis) > deadzone || abs(yAxis) > deadzone){ //Checks to see if joystick is past deadzone, if it is then it engages drive
				xAxis = xAxis/2; //Sets speed to half
				yAxis = yAxis/2;
				motorSet(frontLeftDrive, yAxis + xAxis); //Front Left Drive
				motorSet(backLeftDrive, yAxis + xAxis); //Back Left Drive
				motorSet(backRightDrive, -yAxis + xAxis); //Back Right Drive
				motorSet(frontRightDrive, yAxis - xAxis); //Front Right Drive
				if(joystickGetDigital(1, 8, JOY_RIGHT)){
					//Gets if driver wants FULL speed
					halfSpeed = 0;
				}
			} else { //Turns of drive motors if joystick is not being pressed
				motorSet(frontLeftDrive, 0); //Front Left Drive
				motorSet(backLeftDrive, 0); //Back Left Drive
				motorSet(backRightDrive, 0); //Back Right Drive
				motorSet(frontRightDrive, 0); //Front Right Drive
				if(joystickGetDigital(1, 8, JOY_RIGHT)){
					//Gets if driver wants FULL speed
					halfSpeed = 0;
				}
			}
		}

		//INTAKE

		intakeForward = joystickGetDigital(1, 5, JOY_DOWN); //Checks to see if left bottom joystick shoulder button is pressed, if so, it assigns a value of one to intakeForward
		intakeBackward = joystickGetDigital(1, 5, JOY_UP); //Checks to see if left top joystick shoulder button is pressed if so, it assigns a value of 1 to intakeBackward

		if(intakeForward){
			motorSet(intake, 127); //Intake
			motorSet(conveyor, 127); //Conveyor
		} else if(intakeBackward){
			motorSet(intake, -127); //Intake
			motorSet(conveyor, -127); //Conveyor
		} else {
			motorSet(intake, 0); //Intake
			motorSet(conveyor, 0); //Conveyor
		}

		//FLYWHEEL
		/*
		 * Flywheel Speeds
		 * 0 = Off
		 * 1 = Highest Speed Possible
		 * 2 = Med-High (Doesn't shoot completely over field
		 * 3 = Medium
		 * 4 = Medium-Low
		 * 5 = Low (Barely makes it over goal height)
		 */
		if(joystickGetDigital(1, 8, JOY_DOWN)){ //if bottom button on right d-pad is pressed
			flyWheel = 0; //Turn Flywheel off
		} else if(joystickGetDigital(1, 8, JOY_UP)){ //If Top Button on right d-pad is pressed
			flyWheel = 1; //Set flywheel to highest possible speed
		} else if(joystickGetDigital(1, 7, JOY_UP)){ //If top button on left d-pad is pressed
			flyWheel = 2; //Set flywheel to med-high speed
		} else if(joystickGetDigital(1, 7, JOY_LEFT)){ //If left button on left d-pad is pressed
			flyWheel = 3; //Set flywheel to medium speed
		} else if(joystickGetDigital(1, 7, JOY_DOWN)){ //If bottom button on left d-pad is pressed
			flyWheel = 4; //Set flywheel to low speed
		} else if(joystickGetDigital(1, 7, JOY_RIGHT)){
			flyWheel = 5;
		}


		if(flyWheel == 1){  //Highest Speed
			motorSet(frontLeftFlywheel, -125); //Left FlyWheel Front
			motorSet(frontRightFlywheel, -125); //Right FlyWheel Front
			motorSet(backRightFlywheel, -125); //Right FlyWheel Back
			motorSet(backLeftFlywheel, 125); //Left FlyWheel Back
		} else if(flyWheel == 2){ //Makes it from about middle of the field
			motorSet(frontLeftFlywheel, -54); //Left FlyWheel Front
			motorSet(frontRightFlywheel, -54); //Right FlyWheel Front
			motorSet(backRightFlywheel, -54); //Right FlyWheel Back
			motorSet(backLeftFlywheel, 54); //Left FlyWheel Back
		} else if(flyWheel == 3){ //Makes it from starting tile
			motorSet(frontLeftFlywheel, -58); //Left FlyWheel Front
			motorSet(frontRightFlywheel, -58); //Right FlyWheel Front
			motorSet(backRightFlywheel, -58); //Right FlyWheel Back
			motorSet(backLeftFlywheel, 58); //Left FlyWheel Back
		} else if(flyWheel == 4){ //Makes it in between middle and goal (Close score)
			motorSet(frontLeftFlywheel, -50); //Left FlyWheel Front
			motorSet(frontRightFlywheel, -50); //Right FlyWheel Front
			motorSet(backRightFlywheel, -50); //Right FlyWheel Back
			motorSet(backLeftFlywheel, 50); //Left FlyWheel Back
		} else if(flyWheel == 5){ //Makes it in between starting tiles and middle (intermediate score)
			motorSet(frontLeftFlywheel, -60); //Left FlyWheel Front
			motorSet(frontRightFlywheel, -60); //Right FlyWheel Front
			motorSet(backRightFlywheel, -60); //Right FlyWheel Back
			motorSet(backLeftFlywheel, 60); //Left FlyWheel Back
		} else if(flyWheel == 0){ //Flywheel off
			motorSet(frontLeftFlywheel, 0); //Left FlyWheel Front
			motorSet(frontRightFlywheel, 0); //Right FlyWheel Front
			motorSet(backRightFlywheel, 0); //Right FlyWheel Back
			motorSet(backLeftFlywheel, 0); //Left FlyWheel Back
		}

		delay(20);
	}
		}
