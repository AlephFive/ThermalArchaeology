#include "ofApp.h"


//#define POLLING
#define MAXLEN 1024
#define SAFE_DELETE(x) { if (x) delete x; x = NULL; }				

// Global Vars:
HINSTANCE hInst;
HWND ghWnd = NULL;
const TCHAR szWindowClass[] = TEXT("StartIPCWin32");
TCHAR labelConnected[MAXLEN];
TCHAR labelFrameCounter[MAXLEN];
TCHAR labelPIF[MAXLEN];
TCHAR labelFlag[MAXLEN];
TCHAR labelTarget[MAXLEN];

bool ipcInitialized = false;
bool frameInitialized = false;
bool Connected = false;
bool Colors = false;
bool Painted = false;
short FrameWidth = 160, FrameHeight = 120, FrameDepth = 2;
int FrameSize = 19200;

ofImage IRimage;
unsigned char* pixels = new unsigned char[FrameSize];


vector <dustParticle> dusts;
vector <dirtParticle> dirts;

int dustIndex;
int dirtIndex;

communicator com;

float startTime;
float timer;

bool brushDown;
bool scrapeDown;



std::string TCHAR2STRING(TCHAR *STR)

{

	int iLen = WideCharToMultiByte(CP_ACP, 0, STR, -1, NULL, 0, NULL, NULL);

	char* chRtn = new char[iLen * sizeof(char)];

	WideCharToMultiByte(CP_ACP, 0, STR, -1, chRtn, iLen, NULL, NULL);

	std::string str(chRtn);

	return str;

}




//--------------------------------------------------------------
void ofApp::setup(){
	ofSetVerticalSync(true);

	//Init(160, 120, 2);//for ipc frame  w,h,depth
	/*For IPC Connection*/
	//SetImagerIPCCount(1);
	InitIPC();   //这个里面有 init和run
	SetIPCMode(0, 1);
	
	//setup particles
	int numDust = 100;
	int numDirt = 250;

	dusts.assign(numDust, dustParticle());
	dirts.assign(numDirt, dirtParticle());
	dustIndex = 0;
	dirtIndex = 0;

	brushDown = false;
	scrapeDown = false;
	
	resetParticles();
	resetTimer();

	

	//setup comms
	com.setup(38400, "COM9");
	com.reset();

	

}

//--------------------------------------------------------------
void ofApp::update(){
	
	
	timer = ofGetElapsedTimeMillis() - startTime;
	
	brushParticleEffects();
	scrapeParticleEffects();
	
	com.update();
	


	



}

//--------------------------------------------------------------
void ofApp::draw(){

	ofBackground(0, 0, 0); //Set up white background
	ofSetColor(255, 255, 255); //Set color for image drawing
	IRimage.draw(0, 0, FrameWidth * 5, FrameHeight * 5); //Draw image
	ofDrawBitmapString(TCHAR2STRING(labelFrameCounter), 200, 200);
	//printf("drawing\n");

	//ofBackgroundGradient(ofColor(60, 60, 60), ofColor(10, 10, 10));

	drawParticles();
	

	
	//ofSetColor(190);

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	//brushDown = true;
	scrapeDown = true;
	
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
	//brushDown = false;
	scrapeDown = false;

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}


/*********************************** FOR   IPC  PROCESSING  ********************************/
void InitIPC(void)
{
	HRESULT hr;
	while (!ipcInitialized) //   IS THERE ANY BETTER SOLUTIONS HERE  ?     easy to be endless loop
	{
		hr = InitImagerIPC(0);

		if (FAILED(hr))
		{
			ipcInitialized = frameInitialized = false;
			printf("failed in initImagerIPC\n");
		}
		else
		{
#ifndef POLLING 
			SetCallback_OnServerStopped(0, OnServerStopped);
			SetCallback_OnFrameInit(0, OnFrameInit);
			SetCallback_OnNewFrameEx(0, OnNewFrame);
			SetCallback_OnInitCompleted(0, OnInitCompleted);
#endif
			hr = RunImagerIPC(0);
			ipcInitialized = SUCCEEDED(hr);
		}
		if (ipcInitialized) { printf("IPC Connect!\n"); }
		else { printf("if initImagerIPC didn't fail,RunImagerIPC failed\n"); }
	}
}

void ReleaseIPC(void)
{
	Connected = false;
	if (ipcInitialized)
	{
		ReleaseImagerIPC(0);
		delete[] pixels;
		ipcInitialized = false;
	}
}

void Idle(void)
{
#ifdef POLLING 
	if (Connected && frameInitialized && (FrameBuffer != NULL))
	{
		FrameMetadata Metadata;
		if (GetFrameQueue(0))
			if (SUCCEEDED(GetFrame(0, 0, FrameBuffer, FrameSize * FrameDepth, &Metadata)))
				OnNewFrame(FrameBuffer, &Metadata);
	}
#endif
}

void HandleEvents(void)
{
#ifdef POLLING 
	if (ipcInitialized)
	{
		WORD State = GetIPCState(0, true);
		if (State & IPC_EVENT_SERVER_STOPPED)
			OnServerStopped(0);
		if (!Connected && (State & IPC_EVENT_INIT_COMPLETED))
			OnInitCompleted();
		if (State & IPC_EVENT_FRAME_INIT)
		{
			int frameWidth, frameHeight, frameDepth;
			if (SUCCEEDED(GetFrameConfig(0, &frameWidth, &frameHeight, &frameDepth)))
				Init(frameWidth, frameHeight, frameDepth);
		}
	}
#endif
}



void Init(int frameWidth, int frameHeight, int frameDepth)
{
	FrameWidth = frameWidth;
	FrameHeight = frameHeight;
	FrameSize = FrameWidth * FrameHeight;
	FrameDepth = frameDepth;

	frameInitialized = true;
#ifdef POLLING 
	SAFE_DELETE(FrameBuffer);
	int Size = FrameWidth * FrameHeight * FrameDepth;
	FrameBuffer = new char[FrameSize * FrameDepth];
#endif
}

HRESULT WINAPI OnServerStopped(int reason)
{
	ReleaseIPC();
	return 0;
}

HRESULT WINAPI OnInitCompleted(void)
{
	_stprintf_s(labelConnected, MAXLEN, TEXT("Connected with #%d"), GetSerialNumber(0));
	InvalidateRect(ghWnd, NULL, FALSE);
	//	Colors = (TIPCMode(GetIPCMode(1)) == ipcColors);
	Connected = true;
	return S_OK;
}

HRESULT WINAPI OnFrameInit(int frameWidth, int frameHeight, int frameDepth)
{
	Init(frameWidth, frameHeight, frameDepth);
	return 0;
}

HRESULT WINAPI OnNewFrame(void * pBuffer, FrameMetadata *pMetadata)//pBuffer is the buffer for all the temperature pixels  ; and pMetadate is  the information of the IPC process
{
	_stprintf_s(labelFrameCounter, MAXLEN, TEXT("Frame counter HW/SW: %d/%d"), pMetadata->CounterHW, pMetadata->Counter);
	_stprintf_s(labelPIF, MAXLEN, TEXT("PIF   DI:%d     AI1:%d     AI2:%d"), (pMetadata->PIFin[0] >> 15) == 0, pMetadata->PIFin[0] & 0x3FF, pMetadata->PIFin[1] & 0x3FF);

	_tcscpy_s(labelFlag, MAXLEN, TEXT("Flag: "));
	switch (pMetadata->FlagState)
	{
	case fsFlagOpen: _tcscat_s(labelFlag, MAXLEN, TEXT("open")); break;
	case fsFlagClose: _tcscat_s(labelFlag, MAXLEN, TEXT("closed")); break;
	case fsFlagOpening: _tcscat_s(labelFlag, MAXLEN, TEXT("opening")); break;
	case fsFlagClosing: _tcscat_s(labelFlag, MAXLEN, TEXT("closing")); break;
	}
	//	printf("b\n");
	//ofApp::IRimage = ;



	unsigned char* pixels = new unsigned char[FrameSize];

	short* buf = (short*)pBuffer;
	for (int i = 0; i < FrameSize; i++) {
		// temp=(buf[i]-1000)/10    we want tmin~tmax   now tmin=20 tmax=45.5 so tmin mapped to unsigned char 0 tmax map to unsigned char 255
		pixels[i] = (unsigned char)clip((int)buf[i] - 1200);
	}

	IRimage.setFromPixels(pixels, FrameWidth, FrameHeight, OF_IMAGE_GRAYSCALE);



	//short mn, mx;
	//GetBitmap_Limits(buf, FrameWidth*FrameHeight, &mn, &mx, true);
	//double Fact = 255.0 / (mx - mn);

	//for (int dst = 0, src = 0, y = 0; y < FrameHeight; y++, dst += stride_diff)
	//	for (int x = 0; x < FrameWidth; x++, src++, dst += 4)
	//		ptr[dst] =
	//		ptr[dst + 1] =
	//		ptr[dst + 2] = (char)min(max((int)(Fact * (buf[src] - mn)), 0), 255);



	return 0;


}

void GetBitmap_Limits(short* buf, int FrameSize, short *min, short *max, bool Sigma)
{
	int y;
	double Sum, Mean, Variance;
	if (!buf) return;

	if (!Sigma)
	{
		*min = SHRT_MAX;
		*max = SHRT_MIN;
		for (y = 0; y < FrameSize; y++)
		{
			*min = MIN(buf[y], *min);
			*max = MAX(buf[y], *max);
		}
		return;
	}

	Sum = 0;
	for (y = 0; y < FrameSize; y++)
		Sum += buf[y];
	Mean = (double)Sum / FrameSize;
	Sum = 0;
	for (y = 0; y < FrameSize; y++)
		Sum += (Mean - buf[y]) * (Mean - buf[y]);
	Variance = Sum / FrameSize;
	Variance = sqrt(Variance);
	Variance *= 3;  // 3 Sigma
	*min = short(Mean - Variance);
	*max = short(Mean + Variance);
	unsigned short d = *max - *min;
	if (d < 40)
	{
		d >>= 1;
		*min = *min - d;
		*max = *max + d;
	}
}

BYTE clip(int val)
{
	return (val <= 255) ? ((val > 0) ? val : 0) : 255;
};

// A class to describe a group of Particles
// An ArrayList is used to manage the list of Particles 


void ofApp::resetParticles() {
	//reset dust
	for (unsigned int i = 0; i < dusts.size(); i++) {
		dusts[i].reset();
	}

	//reset dirt
	for (unsigned int i = 0; i < dirts.size(); i++) {
		dirts[i].reset();
	}
}

void ofApp::drawParticles() {
	for (unsigned int i = 0; i < dusts.size(); i++) {
		dusts[i].draw();
	}

	for (unsigned int i = 0; i < dirts.size(); i++) {
		dirts[i].draw();
	}
}

void ofApp::resetTimer() {
	startTime = ofGetElapsedTimeMillis();
	timer = ofGetElapsedTimeMillis() - startTime;
}

void ofApp::brushParticleEffects() {
	//brush effects
	for (unsigned int i = 0; i < dusts.size(); i++) {
		dusts[i].update();
	}

	if (brushDown && timer>80) {
		bool found = false;
		resetTimer();

		for (unsigned int i = 0; i < dusts.size(); i++) {
			if (!dusts[i].isAlive()) {
				//find index where there is unutilized particle objects
				dustIndex = i;
				found = true;
				break;
			}
		}

		if (!found) {
			dusts[0].reset();
			dustIndex = 0;
		}

		dusts[dustIndex].emit();
	}
}

void ofApp::scrapeParticleEffects() {
	//dirt effects
	for (unsigned int i = 0; i < dirts.size(); i++) {
		dirts[i].update();
	}

	if (scrapeDown && timer>80) {
		bool found = false;
		resetTimer();

		for (unsigned int i = 0; i < dirts.size(); i++) {
			if (!dirts[i].isAlive()) {
				//find index where there is unutilized particle objects
				dirtIndex = i;
				found = true;
				break;
			}
		}

		if (!found) {
			dirts[0].reset();
			dirtIndex = 0;
		}

		dirts[dirtIndex].emit();
	}
}

