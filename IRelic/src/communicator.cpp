#include "communicator.h"

//------------------------------------------------------------------
communicator::communicator(int baud, string port) {
	serial.setup(port, baud);

	reset();
}

void communicator::reset() {
	nTimesRead = 0;
	nBytesRead = 0;
	readTime = 0;
	memset(bytesReadString, 0, 4);
}

//------------------------------------------------------------------
void communicator::update() {
	nTimesRead = 0;
	nBytesRead = 0;
	int nRead = 0;  // a temp variable to keep count per read

	unsigned char bytesReturned[3];

	memset(bytesReadString, 0, 4);
	memset(bytesReturned, 0, 3);

	while ((nRead = serial.readBytes(bytesReturned, 3)) > 0) {
		nTimesRead++;
		nBytesRead = nRead;
	};

	memcpy(bytesReadString, bytesReturned, 3);

	bSendSerialMessage = false;
	readTime = ofGetElapsedTimef();

}

bool communicator::isBrushHeld() {
	return false;
}



//------------------------------------------------------------------


