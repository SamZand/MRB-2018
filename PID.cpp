#include "Serial.h"
#include "Tracker.h"
#include <chrono>
#include <thread>
#include <string>
#include <iostream>

#define outMin 0
#define outMax 35
#define r3 1.73205 // square root of 3

Tracker tracker;
double xO = 240; // = screen.size.x /2;
double yO = 320; // = screen.size.y /2;
double xCor, yCor; //ball position read by object tracking
double xI, yI; //actual (median filtered)
double lastXI = 0, lastYI = 0;

double errorx[3];//holds current error
double lastError[3] = {0,0,0};
double lastLastError[3] = {0,0,0};

double output[3]; //Hold actual output

double ITerm[3] = {0,0,0};
double lastDVal[3] = {0,0,0};
double lastLastDVal[3] = {0,0,0};

double kp[3] = {1.05,1.05,1.05};
double ki[3] = {0,0,0};
double kd[3] = {0,0,0};
double ka[3] = {1,1,1};

void getCoordinates();
void getError(int i);
void computePID(int i);
void limitOutput(int i);
void controlPID();
void sendDataToServos(Serial* SP); 

int main()
{
	int nBaudRate = CBR_115200;
	Serial* SP = new Serial("COM5", nBaudRate);
	std::thread trackThread(&Tracker::run, &tracker);
	trackThread.detach();


	int i = 0;

	while (true)
	{
		controlPID();
		sendDataToServos(SP);
		i++;
		//std::cout << i << std::endl;
		Sleep(3);
	}
}

void controlPID()
{
    for(int i=0; i<3; i++) // for each servo
    { 
		getCoordinates();
		getError(i); // finds error values
		computePID(i); // computes output from PID algorithm
		limitOutput(i); // limit the output
    }    
}

void computePID(int i)
{
	// time wordt tijd in milisec

	static auto startTime = std::chrono::system_clock::now();
	std::chrono::duration<float> tim = std::chrono::system_clock::now() - startTime;

	float deltaT = tim.count()*0.001;

  double PTerm = errorx[i] * kp[i];
  //ITerm[i] += (ki[i] * errorx[i]);
	ITerm[i] += (deltaT * errorx[i]);
  double DVal = (errorx[i] - lastError[i])/deltaT;
  double DTerm = kd[i] * DVal;
	//double DTerm = kd[i] * (((5*DVal) + (3*lastDVal[i]) + (2*lastLastDVal[i]))/10.0);
  //double ATerm = ka[i] * (((DVal - lastDVal[i]) + (lastDVal[i] - lastLastDVal[i]))/2.0);
      
  output[i] = (PTerm + ITerm[i] * ki[i] + DTerm); // + ATerm;

  lastError[i] = errorx[i];
  lastLastDVal[i] = lastDVal[i];
  lastDVal[i] = DVal;

	startTime = std::chrono::system_clock::now();
}

void limitOutput(int i)
{
  if(output[i] > outMax) output[i] = outMax;
  else if(output[i] < outMin) output[i] = outMin;
  if(ITerm[i] > outMax) ITerm[i] = outMax;
  else if(ITerm[i] < outMin) ITerm[i] = outMin;
}

void getError(int i)
{
 	if (i == 0) // for servo1: (on y axis)
    errorx[0] = (yI - yO);

  if (i == 1) // for servo2 (120deg CW from servo1)
    errorx[1] = ((2*r3)*(xI - xO) + (yO - yI))/2.0;
    //errorx[1] = (r3*sqrt((xI - xO)*(xI - xO) + (yO + yI)*(yO + yI)));

  if (i == 2) // for servo3 (120deg CCW from servo1)
    errorx[2] = (r3*(xO - xI) + (yO - yI))/2.0;
		//errorx[1] = (r3*sqrt((xO - xI)*(xO - xI) + (yO + yI)*(yO + yI)));
  //std::cout << errorx[0] << " - " << errorx[1] << " - " << errorx[2] << std::endl;
}

void sendDataToServos(Serial* SP)
{
	std::string error1, error2, error3;

	if(output[0] < 10)
	{
		error1 += "0";
	}
	if(output[1] < 10)
	{
		error2 += "0";
	}
	if(output[2] < 10)
	{
		error3 += "0";
	}
	error1 += std::to_string((int) output[0]);
	error2 += std::to_string((int) output[1]);
	error3 += std::to_string((int) output[2]);
	std::string totalString = "B" + error1 + error2 + error3 + "E";
	SP->WriteData(totalString.c_str(), 8);

	//std::cout << totalString << std::endl;
}

void getCoordinates()
{
	xI = tracker.getX();
	std:: cout << "X: " << xI << std::endl;
	yI = 640 - tracker.getY();
	std:: cout << "Y: " << yI << std::endl;

	//Sleep(3);
}