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
	int flywheelSpeed = 127;
	lcdInit(uart1);


	int deadzone = 20; //Sets joystick deadzone in case of incorrect analog positioning
	int xAxis; //Holds X axis for drive analog stick
	int yAxis; //Holds Y axis for drive analog stick
	int intakeForward; //Holds 1 or 0 from one of the left joystick shoulder buttons to tell if the intake should run forward
	int intakeBackward; //Holds 1 or 0 from other left joystick shoulder button to tell if intake should run backward
	int flyWheel = 0; //Holds integer checking flywheel speed

	while (1) {

		xAxis = joystickGetAnalog(1, 1); //Assigns joystick value to X Axis variable
		yAxis = joystickGetAnalog(1, 2); //Assigns joystick value to Y Axis variable

		//DRIVE

		if(abs(xAxis) > deadzone || abs(yAxis) > deadzone){ //Checks to see if joystick is past deadzone, if it is then it engages drive
			motorSet(6, yAxis + xAxis); //Front Left Drive
			motorSet(4, yAxis + xAxis); //Back Left Drive
			motorSet(9, -yAxis + xAxis); //Back Right Drive
			motorSet(1, yAxis - xAxis); //Front Right Drive
		} else { //Turns of drive motors if joystick is not being pressed
			motorSet(6, 0); //Front Left Drive
			motorSet(4, 0); //Back Left Drive
			motorSet(9, 0); //Back Right Drive
			motorSet(1, 0); //Front Right Drive
		}

		//INTAKE

		intakeForward = joystickGetDigital(1, 5, JOY_DOWN); //Checks to see if left bottom joystick shoulder button is pressed, if so, it assigns a value of one to intakeForward
		intakeBackward = joystickGetDigital(1, 5, JOY_UP); //Checks to see if left top joystick shoulder button is pressed if so, it assigns a value of 1 to intakeBackward

		if(intakeForward){
			motorSet(5, 127); //Intake
			motorSet(2, 127); //Conveyor
		} else if(intakeBackward){
			motorSet(5, -127); //Intake
			motorSet(2, -127); //Conveyor
		} else {
			motorSet(5, 0); //Intake
			motorSet(2, 0); //Conveyor
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
		} else if(joystickGetDigital(1, 7, JOY_LEFT)){
			flyWheel = 5;
		}

		//flyWheel = joystickGetDigital(1, 7, JOY_UP); //Checks to see if flywheel button is engaged. If it is, then the flywheel will run continually

		if(flyWheel == 1){  //Highest Speed
			motorSet(8, -127); //Left FlyWheel Front
			motorSet(10, -127); //Right FlyWheel Front
			motorSet(7, -127); //Right FlyWheel Back
			motorSet(3, 127); //Left FlyWheel Back
		} else if(flyWheel == 2){ //Should make it from starting squares
			motorSet(8, -62); //Left FlyWheel Front
			motorSet(10, -62); //Right FlyWheel Front
			motorSet(7, -62); //Right FlyWheel Back
			motorSet(3, 62); //Left FlyWheel Back
		} else if(flyWheel == 3){ //Should make it from one square from starting tiles
			motorSet(8, -52); //Left FlyWheel Front
			motorSet(10, -52); //Right FlyWheel Front
			motorSet(7, -52); //Right FlyWheel Back
			motorSet(3, 52); //Left FlyWheel Back
		} else if(flyWheel == 4){ //Should make it from two squares from starting tiles
			motorSet(8, -47); //Left FlyWheel Front
			motorSet(10, -47); //Right FlyWheel Front
			motorSet(7, -47); //Right FlyWheel Back
			motorSet(3, 47); //Left FlyWheel Back
		} else if(flyWheel == 5){ //Should make it from three squares from starting tiles
			motorSet(8, -42); //Left FlyWheel Front
			motorSet(10, -42); //Right FlyWheel Front
			motorSet(7, -42); //Right FlyWheel Back
			motorSet(3, 42); //Left FlyWheel Back
		} else if(flyWheel == 0){ //Flywheel off
			motorSet(8, 0); //Left FlyWheel Front
			motorSet(10, 0); //Right FlyWheel Front
			motorSet(7, 0); //Right FlyWheel Back
			motorSet(3, 0); //Left FlyWheel Back
		}

		delay(20);
	}
		}
