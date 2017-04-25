#pragma once
#include "ofMain.h"

class communicator {

public:
	communicator();

	void setup(int baud, string port);
	void reset();
	void update();

	bool isBrushHeld();
	bool isKnifeHeld();
	bool isPipHeld();

	int getBrushForce();
	int getKnifeForce();

	char whatTool();

	void toolFilter(char t);


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

	int toolChangeCount;

	bool discard;
	bool held;

	//counters for turning tools off
	int brushOffCounter;
	int knifeOffCounter;
	int pipOffCounter;



};