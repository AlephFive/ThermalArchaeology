#include "ofApp.h"


//#define POLLING
#define MAXLEN 1024
#define SAFE_DELETE(x) { if (x) delete x; x = NULL; }				
#define DEFER_FPS 5
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

int deferfps = 0;

// function prototypes:

//caitao information
int caitao_toollist[5] = { 1,2,3,1,3 };


ofxCvGrayscaleImage IRimage;
ofxCvGrayscaleImage IRimagePrev;
ofxCvGrayscaleImage IRimagePrevQueue[5];
unsigned char* pixels = new unsigned char[FrameSize];
unsigned char* pixelBuffer = new unsigned char[FrameSize]; //use for blending of the motion detect area
														   //ofPixels pixelBuffer;
bool newIR;
ofxCvColorImage outloadImage;






//====================Brian==============
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

	/*********************game data setup**********************/


	/****************widgets setup******************/
	// should be after the data setup
	caitaoWidgets.setup();
	caitaoWidgets.setThres(safeThres1 / forceTotal, safeThres2 / forceTotal);
	ToolSwitchSetup();


	/***********************     Button Setup    ****************************/
	ButtonSetup();

	/**********************For Motion Experiment gui**********************/
	Experiment.setup("Parameters", "settings.xml");
	Experiment.add(Amplify.setup("Amplify", 5.0, 1.0, 10.0));
	Experiment.add(Damping.setup("Damping", 0.85, 0.0, 1.0));
	Experiment.add(Threshold.setup("Threshold", 128, 0, 255));
	Experiment.add(Adap.setup("Adaptive", false));
	Experiment.add(AdaptiveThreshold.setup("AdaptiveThreshold", 30, 0, 255));
	Experiment.add(outlineth.setup("outlineThreshold", 128, 0, 255));
	Experiment.add(IRthreshold.setup("IRthreshold", 90, 0, 255));

	//************   For Mask Shader   ***********/
#ifdef TARGET_OPENGLES
	shaderMask.load("shadersES2/shader");
#else
	if (ofIsGLProgrammableRenderer()) {
		shaderMask.load("shadersGL3/shaderMask");
		shaderMotion.load("shadersGL3/shaderMotion");
	}
	else {
		shaderMask.load("shadersGL2/shaderMask");
		shaderMotion.load("shadersGL2/shaderMotion");
	}
#endif

	/******************          image loading           *****************/
	startbackground.loadImage("a1.jpg");//should be startbackground.png
	endbackground.loadImage("interface/endbackground.png");
	gameoverbackground.loadImage("interface/gameoverbackground.png");



	//pixelBuffer.allocate(FrameWidth, FrameHeight, OF_IMAGE_GRAYSCALE);
	for (int i = 0; i < FrameSize; i++) { pixelBuffer[i] = 0; }
	int width = ofGetWidth();
	int height = ofGetHeight();
	existFbo.allocate(IRimage_w, IRimage_h);
	maskFbo.allocate(width, height);
	fbo.allocate(width, height);

	// Clear the FBO's
	// otherwise it will bring some junk with it from the memory
	existFbo.begin();
	ofClear(0, 0, 0, 255);
	existFbo.end();
	maskFbo.begin();
	ofClear(0, 0, 0, 255);
	maskFbo.end();

	fbo.begin();
	ofClear(0, 0, 0, 255);
	fbo.end();

	//Init(160, 120, 2);//for ipc frame  w,h,depth

	/*For IPC Connection*/
	//SetImagerIPCCount(1);
	InitIPC();   //这个里面有 init和run
	SetIPCMode(0, 1);
	newIR = false;

	//deferfps = 0;
	ofEnableAlphaBlending();
	



	//============Brian==================
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
	com.setup(38400, "COM10");
	com.reset();

	

}

//--------------------------------------------------------------
void ofApp::update(){
	IRtoMotion(IRimage, IRimagePrev);
	switch (stage) {
	case START: {
		//getSwitchStage();
		if (getButtonState(ButtonStart)) {
			/**********Game setup***********/
			caitao.setup(5, "caitao", caitao_toollist);
			//----------------------------------------
			stage = PROCESS;
			healthLeft = healthTotal;
			workingLeft = workingTotal[0];
		}
	}
	case PROCESS: {
		if (!stepend) {


			if (firsttimehere) { //update the step data
				workingLeft = workingTotal[caitao.currentStep];
				caitaoWidgets.update(caitao, stepend);
				backgroundImage = caitao.ProcessImages[caitao.currentStep + 1];
				foregroundImage = caitao.ProcessImages[caitao.currentStep];

				for (int i = 0; i < FrameSize; i++) { pixelBuffer[i] = 0; }//clean the pixel Buffer for recording a new step motion
				if (caitao.Toollist[caitao.currentStep] == dropper) { starttime = ofGetElapsedTimef(); }
				firsttimehere = false;
			}


			if (caitao.Toollist[caitao.currentStep] == dropper) { timer = ofGetElapsedTimef(); }

			ToolSwitchUpdate();

			if (ToolNow == caitao.Toollist[caitao.currentStep]) { //If user is not using the right tool,then nothing updates.
				maskShaderUpdate();//workingleft is calculated here.
			}
			caitaoWidgets.healthPercent = ofMap(healthLeft, 0.0, healthTotal, 0.0, 1.0);
			caitaoWidgets.workingPercent = ofMap((float)workingLeft, 0.0, (float)workingTotal[caitao.currentStep], 0.0, 1.0);
			if (caitao.Toollist[caitao.currentStep] != dropper) {
				caitaoWidgets.toolparaPercent = ofMap(currentForce, 0.0, forceTotal, 0.0, 1.0);
				if (Moved) {
					if (caitao.Toollist[caitao.currentStep] == knife && currentForce > safeThres1) { healthLeft = healthLeft - 1; }//如果力大于thres1 用刀 并且 motion有数
					if (caitao.Toollist[caitao.currentStep] == brush && currentForce > safeThres2) { healthLeft = healthLeft - 1; }//如果力大于thres2 用刷 并且 motion有数
				}
			}
			else {
				caitaoWidgets.toolparaPercent = ofMap(timeLimit - timer + starttime, 0.0, timeLimit, 0.0, 1.0);
				if (caitaoWidgets.toolparaPercent < 0.001) { healthLeft = healthLeft - 1; }
			}
			if (healthLeft < 1) {//gameover
				screenshot.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
				stage = GAMEOVER;
			}
			if (caitaoWidgets.workingPercent > 0.9) {
				stepend = true;
				firsttimeend = true;
			}


		}
		else {


			if (firsttimeend) {
				caitaoWidgets.update(caitao, stepend);
				changingstarttime = ofGetElapsedTimeMillis();
				firsttimeend = false;
			}
			changingtimer = ofGetElapsedTimeMillis();
			if (changingtimer - changingstarttime > changingtimeLimit) {
				caitao.currentStep++;
				firsttimehere = true;
				stepend = false;
				if (caitao.currentStep > 4) {
					stage = END;
				}
			}


		}
	}
	case END: {
		if (getButtonState(ButtonRestartend)) {
			stage = START;
		}
	}
	case GAMEOVER: {
		if (getButtonState(ButtonRestartgameover)) {
			stage = START;
		}
	}
	}
	



	//===================Brian=============================
	timer = ofGetElapsedTimeMillis() - startTime;
	
	brushParticleEffects();
	scrapeParticleEffects();
	
	com.update();
	


	



}

//--------------------------------------------------------------
void ofApp::draw(){

	ofBackground(0, 0, 0); //Set up white background
	ofSetColor(255, 255, 255); //Set color for image drawing



	switch (stage) {
	case START: {
		startbackground.draw(0, 0, 1280, 800);
		ButtonStart.draw();
	}
	case PROCESS: {
		// FIRST draw the background image
		foregroundImage.draw(0, 0);
		// THEN draw the masked fbo on top
		fbo.draw(0, 0);
		caitaoWidgets.draw();
		ToolSwitchDraw();
		ButtonRestartpro.draw();
	}
	case END: {
		endbackground.draw(0, 0, 1280, 800);
		ButtonRestartend.draw();

	}
	case GAMEOVER: {
		screenshot.draw(0, 0);
		gameoverbackground.draw(0, 0, ofGetWidth(), ofGetHeight());//to see if there is any differences
		ButtonRestartgameover.draw();
	}
	}



	//----------------------------------------------------------
	//Here is For Test 

	MotionDraw();
	existFbo.draw(0, 4 * IRimage_h);
	Experiment.draw();

	//IRimage.draw(1600-320, 900-240,FrameWidth*2,FrameHeight*2); //Draw image
	//ofDrawBitmapString(TCHAR2STRING(labelFrameCounter), 100, 800);
	//printf("drawing\n");

	//ofBackgroundGradient(ofColor(60, 60, 60), ofColor(10, 10, 10));





	//=================Brian======================
	drawParticles();
	

	
	//ofSetColor(190);

}

void ofApp::exit() {
	Experiment.saveToFile("settings.xml");
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




void ofApp::ButtonSetup()
{
	ButtonStart.setposition(ofGetWindowSize().x / 2 - 100, ofGetWindowSize().y / 2 - 50, 200, 100);
	ButtonStart.icon.loadImage("interface/start.png");
	ButtonStart.name = "Start";
	ButtonStart.toucharea.set((ButtonStart.x + ButtonStart.w / 2) / 6, (ButtonStart.y + ButtonStart.h / 2) / 6);
	ButtonRestartpro.setposition(190, 30, 30, 30);
	ButtonRestartpro.icon.loadImage("interface/restart.png");
	ButtonRestartpro.name = "Restart";
	ButtonRestartpro.toucharea.set((ButtonRestartpro.x + ButtonRestartpro.w / 2) / 6, (ButtonRestartpro.y + ButtonRestartpro.h / 2) / 6);
	//	ButtonHelp.setposition(ofGetWindowSize().x / 2, ofGetWindowSize().y / 2, 200, 100);
	//	ButtonHelp.icon.loadImage("interface/help.png");
	//	ButtonHelp.name = "Help";
	ButtonRestartend.setposition(ofGetWindowSize().x / 2 - 100, ofGetWindowSize().y / 2 - 50, 200, 100);
	ButtonRestartend.icon.loadImage("interface/restartend.png");
	ButtonRestartend.name = "Restart";
	ButtonRestartend.toucharea.set((ButtonRestartend.x + ButtonRestartend.w / 2) / 6, (ButtonRestartend.y + ButtonRestartend.h / 2) / 6);
	ButtonRestartgameover.setposition(ofGetWindowSize().x / 2 - 100, ofGetWindowSize().y / 2 - 50, 200, 100);
	ButtonRestartgameover.icon.loadImage("interface/restartgameover.png");
	ButtonRestartgameover.name = "Restart";
	ButtonRestartgameover.toucharea.set((ButtonRestartend.x + ButtonRestartend.w / 2) / 6, (ButtonRestartend.y + ButtonRestartend.h / 2) / 6);
}
bool ofApp::getButtonState(button bu) {
	ofPixels temp = binaryMotion.getPixels();
	//unsigned char value;
	if (temp[(int)(bu.toucharea.x + bu.toucharea.y * 120)] > 0) { return true; }
	else { return false; }

}

void ofApp::ToolSwitchSetup()
{
	Knife.icon_off.loadImage("interface/chan_off.png");
	Knife.icon_on.loadImage("interface/chan_on.png");
	Knife.ID = knife;
	Brush.icon_off.loadImage("interface/brush_off.png");
	Brush.icon_on.loadImage("interface/brush_on.png");
	Brush.ID = brush;
	Dropper.icon_off.loadImage("interface/dropper_off.png");
	Dropper.icon_on.loadImage("interface/dropper_on.png");
	Dropper.ID = dropper;
	TSPosition.set(1180, 268);

}

void ofApp::ToolSwitchUpdate()
{
	//ToolNow = ??
	//currentForce=??
	//For Brian
}

void ofApp::ToolSwitchDraw()
{
	switch (ToolNow) {
	case none: {
		Knife.icon_off.draw(TSPosition);
		Brush.icon_off.draw(TSPosition.x, TSPosition.y + Knife.icon_off.getHeight());
		Dropper.icon_off.draw(TSPosition.x, TSPosition.y + Knife.icon_off.getHeight() * 2);
	}
	case knife: {
		Knife.icon_on.draw(TSPosition);
		Brush.icon_off.draw(TSPosition.x, TSPosition.y + Knife.icon_off.getHeight());
		Dropper.icon_off.draw(TSPosition.x, TSPosition.y + Knife.icon_off.getHeight() * 2);
	}
	case brush: {
		Knife.icon_off.draw(TSPosition);
		Brush.icon_on.draw(TSPosition.x, TSPosition.y + Knife.icon_off.getHeight());
		Dropper.icon_off.draw(TSPosition.x, TSPosition.y + Knife.icon_off.getHeight() * 2);
	}
	case dropper: {
		Knife.icon_off.draw(TSPosition);
		Brush.icon_off.draw(TSPosition.x, TSPosition.y + Knife.icon_off.getHeight());
		Dropper.icon_on.draw(TSPosition.x, TSPosition.y + Knife.icon_off.getHeight() * 2);
	}
	}
}

ofxCvGrayscaleImage ofApp::IRtoMotion(ofxCvGrayscaleImage IR, ofxCvGrayscaleImage IRprev)
{
	if (newIR) {
		//Store the previous frame, if it exists till now


		//Do processing if grayImagePrev is inited
		if (IRimagePrev.bAllocated) {
			//Get absolute difference
			diff.absDiff(IRimage, IRimagePrev);

			//We want to amplify the difference to obtain
			//better visibility of motion
			//We do it by multiplication. But to do it, we
			//need to convert diff to float image first
			diffFloat = diff;	//Convert to float image
			diffFloat *= Amplify;	//Amplify the pixel values

									//Update the accumulation buffer
			if (!bufferFloat.bAllocated) {
				//If the buffer is not initialized, then
				//just set it equal to diffFloat
				bufferFloat = diffFloat;
			}
			else {
				//Slow damping the buffer to zero
				bufferFloat *= 0.85;
				//Add current difference image to the buffer
				bufferFloat += diffFloat;
			}
			//get binary image
			binaryMotion = bufferFloat;
			if (Adap) {
				binaryMotion.adaptiveThreshold(AdaptiveThreshold);
			}
			else {
				binaryMotion.threshold(Threshold); //we need to experiment
			}
			for (int i = 0; i < FrameSize; i++) {
				if (binaryMotion.getPixels()[i] > 0) { Moved = true; break; }
				Moved = false;
			}
			newIR = false;
			newMotion = true;
		}
	}
	return binaryMotion;
}

void ofApp::maskShaderUpdate()
{
	float times = 6; //finally we make the touch area 960*720, 6 times to IRimage 
					 //here the drawing parameters decide the camera view on the whole screen

					 //------------------------------------------------------------------------
					 // we need to accumulate the motion areas .here we store all the motions into existFbo
	ofxCvGrayscaleImage abc;
	/*
	abc = IRimage;
	abc.threshold(IRthreshold);

	*/
	abc = binaryMotion;

	ofPixels ab = abc.getPixels();

	//unsigned char value;
	int counter = 0;
	for (int i = 0; i < FrameSize; i++) {
		if (pixelBuffer[i] < ab[i]) {
			pixelBuffer[i] = ab[i];
			if (caitao.OutlineImages[caitao.currentStep].getPixels()[i] > 0)
			{
				workingLeft--;
			}
		}
	}

	ofxCvGrayscaleImage pBimage;
	pBimage.allocate(160, 120);
	pBimage.setFromPixels(pixelBuffer, FrameWidth, FrameHeight);
	ofSetColor(255);
	existFbo.begin();
	pBimage.draw(0, 0);
	existFbo.end();
	//----------------------------------------------------------
	//then draw 160-120 size existFbo expand it "times" times and put it on the middletop of maskFbo
	maskFbo.begin();
	ofClear(0, 0, 0, 0);
	existFbo.draw(160, 0, IRimage_w * times, IRimage_h * times);
	maskFbo.end();
	//----------------------------------------------------------
	// HERE the shader-masking happends
	fbo.begin();
	// Cleaning everthing with alpha mask on 0 in order to make it transparent by default
	ofClear(0, 0, 0, 0);

	shaderMask.begin();
	// here is where the fbo is passed to the shader
	shaderMask.setUniformTexture("maskTex", maskFbo.getTextureReference(), 1);

	backgroundImage.draw(0, 0);

	shaderMask.end();
	fbo.end();
	ofEnableAlphaBlending();

}
void ofApp::MotionDraw()
{
	if (diffFloat.bAllocated) {
		//Get image dimensions
		int w = IRimage.width;
		int h = IRimage.height;

		//Set color for images drawing
		ofSetColor(255, 255, 255);

		//Draw images grayImage,  diffFloat, bufferFloat
		IRimage.draw(0, 0, w, h);
		diffFloat.draw(0, h, w, h);
		bufferFloat.draw(0, 2 * h, w, h);
		binaryMotion.draw(0, 3 * h, w, h);
	}
}

void Process::setup(int stepsnum, string imgfolder, int *toollist) {
	TotalImgsNum = stepsnum + 1;
	TotalStepsNum = stepsnum;
	currentStep = 0;
	string imgdirectory;
	ofImage temp;
	for (int i = 0; i<TotalImgsNum; i++)
	{
		imgdirectory = imgfolder + "/" + ofToString(i) + ".jpg";
		temp.loadImage(imgdirectory);
		ProcessImages.push_back(temp);
		imgdirectory = imgfolder + "/outline" + ofToString(i) + ".jpg";
		temp.loadImage(imgdirectory);
		OutlineImages.push_back(temp);
		if (i<TotalStepsNum)
		{
			Toollist.push_back((ToolStyle)toollist[i]);
		}
	}
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



	unsigned char* pixels = new unsigned char[FrameSize];

	short* buf = (short*)pBuffer;
	for (int i = 0; i < FrameSize; i++) {
		// temp=(buf[i]-1000)/10    we want tmin~tmax   now tmin=20 tmax=45.5 so tmin mapped to unsigned char 0 tmax map to unsigned char 255
		pixels[i] = (unsigned char)clip((int)buf[i] - 1200);
	}

	IRimagePrev = IRimage;

	//	IRimage.setFromPixels(pixels, FrameWidth, FrameHeight, OF_IMAGE_GRAYSCALE); // ofImage IRimage
	IRimage.setFromPixels(pixels, FrameWidth, FrameHeight);                                             // ofxCvGrayscaleImage IRimage
	newIR = true;
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

void Widgets::setup()
{
	font.load("HYQuHeiW 2.ttf", 12);
}

void Widgets::update(Process cai, bool finish)
{
	finishpercent.loadImage("interface/finishpercent.png");
	health.loadImage("interface/health.png");
	toolpara.loadImage("interface/toolpara" + ofToString(cai.currentStep) + ".png");
	currentToolStyle = cai.Toollist[cai.currentStep];
	if (finish) {
		instruction.loadImage("interface/step" + ofToString(cai.currentStep) + ".png");
		tips.loadImage("interface/tips" + ofToString(cai.currentStep) + ".png");

	}
	else {
		instruction.loadImage("interface/stepend" + ofToString(cai.currentStep) + ".png");
		tips.loadImage("interface/tipsend" + ofToString(cai.currentStep) + ".png");
	}
}

void Widgets::draw()
{
	instruction.draw(50, 720);
	tips.draw(900, 720);
	finishpercent.draw(260, 80);
	health.draw(260, 30);
	toolpara.draw(700, 80);
	//drawing the health bar
	ofSetColor(255 * (1 - healthPercent), 255 * healthPercent, 30);
	ofRect(310, 30, healthBarWidth*healthPercent, 30);
	ofSetColor(255, 255, 255);
	font.drawString((int)(healthPercent * 100) + "%", 310 + healthBarWidth + 5, 30);
	//draw the working bar
	ofSetColor(134, 216, 63);
	ofRect(310, 80, workingBarWidth*workingPercent, 30);
	ofSetColor(255, 255, 255);
	font.drawString((int)(workingPercent * 100) + "%", 310 + workingBarWidth + 5, 80);

	switch (currentToolStyle) {
	case knife:
	{
		//ofSetColor(255, 255, 255);
		if (toolparaPercent > thres1) {
			ofSetColor(255, 0, 0);
			ofRect(750, 80, toolparaBarWidth*toolparaPercent, 30);
			font.drawString("用力过猛", 750 + toolparaBarWidth + 5, 80);
		}
		else {
			ofSetColor(134, 216, 63);
			ofRect(750, 80, toolparaBarWidth*toolparaPercent, 30);
			ofSetColor(255, 255, 255);
			font.drawString("力度安全", 750 + toolparaBarWidth + 5, 80);
		}

	}
	case brush:
	{
		if (toolparaPercent > thres2) {
			ofSetColor(255, 0, 0);
			ofRect(750, 80, toolparaBarWidth*toolparaPercent, 30);
			font.drawString("用力过猛", 750 + toolparaBarWidth + 5, 80);
		}
		else {
			ofSetColor(134, 216, 63);
			ofRect(750, 80, toolparaBarWidth*toolparaPercent, 30);
			ofSetColor(255, 255, 255);
			font.drawString("力度安全", 750 + toolparaBarWidth + 5, 80);
		}
	}
	case dropper:
	{
		ofSetColor(134, 216, 63);
		ofRect(750, 80, toolparaBarWidth*toolparaPercent, 30);
	}
	}
}




//=====================Brian==========================

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

