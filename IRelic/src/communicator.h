#pragma once
#include "ofMain.h"

class communicator {

public:
	communicator(int baud, string port);

	void reset();
	void update();

	bool isBrushHeld();
	bool isKnifeHeld();
	bool isPipHeld();

	int getBrushForce();
	int getKnifeForce();


	char bytesRead[3];				// data from serial, we will be trying to read 3
	char bytesReadString[4];			// a string needs a null terminator, so we need 3 + 1 bytes
	int	nBytesRead;					// how much did we read?
	int	nTimesRead;					// how many times did we read?
	float readTime;					// when did we last read?				

	ofSerial	serial;

	bool brush;
	bool knife;
	bool pip;

	int brushForce;
	int knifeForce;


};