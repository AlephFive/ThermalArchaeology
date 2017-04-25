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

	toolChangeCount = 0;
	discard = false;
	held = false;
}

//------------------------------------------------------------------
void communicator::update() {


	if (serial.available() >= 6) {
		unsigned char bytesReturned[6];

		memset(bytesReadString, 0, 7);
		memset(bytesReturned, 0, 6);

		int nRead = 0;
		nRead = serial.readBytes(bytesReturned, 6);
		memcpy(bytesReadString, bytesReturned, 6);

		/*
		printf("echo");
		printf(bytesReadString);
		printf("\n");
		*/

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
			serial.flush(true, false);
			discard = true;
		}
		
		if (!discard && held) {
			switch (tool) {
			case 'b':
				if (!brush) {
					toolChangeCount++;
				}
				else {
					toolChangeCount = 0;
				}

				if (toolChangeCount > 2) {
					brush = true;
					knife = false;
					pip = false;
				}
				
				brushForce = value;

				break;
			case 'k':
				if (!knife) {
					toolChangeCount++;
				}
				else {
					toolChangeCount = 0;
				}

				if (toolChangeCount > 2) {
					knife = true;
					brush = false;
					pip = false;
				}

				knifeForce = value;
				
				break;
			case 'p':
				if (!pip) {
					toolChangeCount++;
				}
				else {
					toolChangeCount = 0;
				}

				if (toolChangeCount > 2) {
					pip = true;
					brush = false;
					knife = false;
				}

				break;

			default:
				//bytes are offset, flush the serial read buffer
				discard = true;

			}
		}
		else if (!discard && !held) {
			switch (tool) {
			case 'b':
				brush = false;

				break;
			case 'k':
				knife = false;

				break;
			case 'p':
				pip = false;

				break;

			default:
				//bytes are offset, flush the serial read buffer
				discard = true;

			}
		}

		if (discard) {
			serial.flush(true, false);
		}

		discard = false;

	}

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
	if (brush) {
		return 'b';
	}
	else if (knife) {
		return 'k';
	}
	else if (pip) {
		return 'p';
	}
	else {
		return '0';
	}
}

void communicator::toolFilter(char t) {
	if (toolChangeCount > 4) {
		switch (t) {
			case 'b':
				brush = held;
				
				break;
			case 'k':
				knife = held;

				break;
			case 'p':
				pip = held;

				break;
		}
	}
	
}


//------------------------------------------------------------------


