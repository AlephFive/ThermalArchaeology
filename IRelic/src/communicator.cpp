#include "communicator.h"

//------------------------------------------------------------------
communicator::communicator() {
	reset();
}

void communicator::setup(int baud, string port) {
	serial.setup(port, baud);
}

void communicator::reset() {
	nTimesRead = 0;
	nBytesRead = 0;
	readTime = 0;
	memset(bytesReadString, 0, 4);

	brushForce = 0;
	knifeForce = 0;

	brush = false;
	knife = false;
	pip = false;
	prev = '0';
	//prevprev = '0';

	toolChangeCount = 0;
	discard = false;
	held = false;

	brushCounter = 0;
	knifeCounter = 0;
	pipCounter = 0;
}

//------------------------------------------------------------------
void communicator::update() {
	timer = ofGetElapsedTimeMillis() - startTime;

	if (serial.available() >= 6) {
		
		unsigned char bytesReturned[6];

		memset(bytesReadString, 0, 7);
		memset(bytesReturned, 0, 6);

		int nRead = 0;
		nRead = serial.readBytes(bytesReturned, 6);
		memcpy(bytesReadString, bytesReturned, 6);

		
		printf("echo");
		printf(bytesReadString);
		printf("\n");
		

		//identify what tool is being used and parse values as needed

		//xv0000 <- format
		//x: tool identifying char
		//v: 1 if is being held, 0 if not
		//0000: data
		int value;
		char tool = bytesReadString[0];
		held = (bytesReadString[1] == 1) ? true : false;
		int d1 = bytesReadString[2] - '0';
		int d2 = bytesReadString[3] - '0';
		int d3 = bytesReadString[4] - '0';
		int d4 = bytesReadString[5] - '0';
		if (d1<10&&d2<10&&d3<10&&d4<10) {
			value = (d1 * 1000) + (d2 * 100) + (d3 * 10) + d4;
		}
		else {
			value = 0;
			printf("last 4 bytes are not numeric \n");
			printf(bytesReadString);
			printf("\n");
			//serial.flush(true, false);
			discard = true;
		}
		
		if (!discard) {
			switch (tool) {
			case 'b':
				if (prev == 'b' && brushCounter > 4) {
					brush = true;
					knife = false;
					pip = false;
					
				}
				if (brushCounter < 100) {
					brushCounter++;
				}
				
				knifeCounter = 0;
				pipCounter = 0;
				resetTimer();
				prev = 'b';
				brushForce = value;

				break;
			case 'k':
				if (prev == 'k' && knifeCounter > 4) {
					brush = false;
					knife = true;
					pip = false;
					
				}
				if (knifeCounter < 100) {
					knifeCounter++;
				}

				brushCounter = 0;
				pipCounter = 0;
				resetTimer();
				knifeForce = value;
				prev = 'k';
				
				break;
			case 'p':
				if (prev == 'p' && pipCounter > 4) {
					pip = true;
					brush = false;
					knife = false;
					
				}
				if (pipCounter < 100) {
					pipCounter++;
				}

				brushCounter = 0;
				knifeCounter = 0;
				resetTimer();
				prev = 'p';

				break;

			default:
				//bytes are offset, flush the serial read buffer
				discard = true;

			}

			
		}

		if (discard) {
			serial.flush(true, false);
			discard = false;
		}
		
			sentCounter = 0;
		

		

	}

	//tool timeout
	if (timer > 1000) {
		brush = false;
		knife = false;
		pip = false;
		printf("no tools held \n");
	}
	
	sentCounter++;

}



bool communicator::isBrushHeld() {
	return brush;
}

bool communicator::isKnifeHeld() {
	return knife;
}

bool communicator::isPipHeld() {
	return pip;
}

int communicator::getBrushForce(){
	return brushForce;
}

int communicator::getKnifeForce(){
	return knifeForce;
}

char communicator::whatTool() {
	if (brush == 1) {
		return 'b';
	}
	else if (knife == 1) {
		return 'k';
	}
	else if (pip == 1) {
		return 'p';
	}
	else {
		//printf("whattool else\n");
		return '0';
	}
}



void communicator::resetTimer() {
	startTime = ofGetElapsedTimeMillis();
	timer = ofGetElapsedTimeMillis() - startTime;
}



//------------------------------------------------------------------


