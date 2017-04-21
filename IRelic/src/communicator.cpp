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
		bool held = (bytesReadString[1] == 1) ? true : false;
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
		}
		
		
		switch (tool) {
			case 'b':
				brush = held;
				brushForce = value;
				break;
			case 'k':
				knife = held;
				knifeForce = value;
				break;
			case 'p':
				pip = held;
				break;

			default:
				//bytes are offset, flush the serial read buffer
				serial.flush(true, false);
		
		}



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



//------------------------------------------------------------------


