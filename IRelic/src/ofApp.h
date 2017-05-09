#pragma once

#include "ofMain.h"
#include "Lib\ImagerIPC2.h"

#include <stdio.h>
#include <math.h>
#include <queue>
#include <vector>
#include <map>
#include <cstdlib>

#include "ofxOpenCv.h"
#include "ofxGui.h"



#include "dustParticle.h"
#include "dirtParticle.h"
#include "communicator.h"


void InitIPC(void);
void ReleaseIPC(void);
void Idle(void);
void HandleEvents(void);
void Init(int frameWidth, int frameHeight, int frameDepth);
BYTE clip(int val);
void GetBitmap_Limits(short* buf, int FrameSize, short *min, short *max, bool Sigma);

HRESULT WINAPI OnServerStopped(int reason);
HRESULT WINAPI OnInitCompleted(void);
HRESULT WINAPI OnFrameInit(int frameWidth, int frameHeight, int frameDepth);
HRESULT WINAPI OnNewFrame(void * pBuffer, FrameMetadata *pMetadata);

enum ToolStyle { none = 0, knife, brush, dropper };
enum GameStage { START = 0, PROCESS, END, GAMEOVER };
enum RelicType { CAITAO = 0, HUASHI };

struct RelicPackage {//for each relic package's files management
	string imageFolder;
	int imageNum;
	//ofImage outlineImage;//160*120 pixels  the file should be named as imageFolder_mask.jpg
	//map<string,ToolStyle> outlinePair;
	RelicPackage(string a, int b) { imageFolder = a; imageNum = b; }
};


//______________________________CLASSES_____________________________________

class Tool {
public:
	ToolStyle ID;
	ofImage icon_on;
	ofImage icon_off;
	Tool() { ID = none; }
	void setToolStyle(int num) { ID = (ToolStyle)num; }
};

class button {
public:
	bool state = false;
	int x, y;
	int w, h;
	ofImage icon;
	string name;
	ofPoint toucharea;
	void setposition(int x1, int y1, int w1, int h1) { x = x1; y = y1; w = w1; h = h1; }
	void setposition(int x1, int y1) { x = x1; y = y1; }
	void draw() {icon.draw(x, y, w, h); }
};

class Process {
public:
	int TotalStepsNum;
	int TotalImgsNum;
	string imgdirectory;
	int currentStep;
	//ofImage temp;
	vector<ofImage> ProcessImages;
	vector<ofImage> OutlineImages;
	vector<ToolStyle> Toollist;

	void setup(int stepsnum, string imgfolder, int * toollist);
};

class Widgets {//the widgets drawn on the top layer
public:
	ofImage instruction;
	ofImage tips;
	ofImage finishpercent;
	ofImage health;
	ofImage toolpara;
	ofTrueTypeFont  font;
	float thres1;//for knife
	float thres2;//for brush
	float workingPercent = 1.0;//working left percent
	float healthPercent = 1.0;
	float toolparaPercent = 0.0;
	 int healthBarWidth = 700;
	 int workingBarWidth = 260;
	 int toolparaBarWidth = 260;
	ToolStyle currentToolStyle;
	void setup();
	void reset() {
		 workingPercent = 1.0;//working left percent
		 healthPercent = 1.0;
		 toolparaPercent = 0.0;

	}
	void update(Process cai, bool finish);
	void setThres(float x, float y) { thres1 = x; thres2 = y; }
	void draw();
};


//====================================BASEAPP========================================
class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
		
		void resetTimer();

		void resetParticles();
		void drawParticles();
		//void brushParticleEffects();
		//void scrapeParticleEffects();
		void particleEffects();
		void emitParticles();
		

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		void exit();


		//Kilo start from here

		/*For Arduino Serial Communication*/
		
		//string str;
		ToolStyle ToolNow;

		int IRimage_w = 160;
		int IRimage_h = 120;

		/*************     For Motion Detection      *************/
		ofxCvGrayscaleImage diff;		//Absolute difference of the frames
		ofxCvFloatImage diffFloat;		//Amplified difference images
		ofxCvFloatImage bufferFloat;	//Buffer image
		ofxCvGrayscaleImage binaryMotion; //binary mask of motion
		void IRtoMotion(ofxCvGrayscaleImage IR, ofxCvGrayscaleImage IRprev);
		bool Moved;
		void MotionDraw();
		bool newMotion;

		//GUI  for motion detecting experiments
		ofxPanel Experiment;
		ofxFloatSlider Amplify;
		ofxFloatSlider Damping;
		ofxIntSlider Threshold;
		ofxIntSlider AdaptiveThreshold;
		ofxToggle Adap;
		ofxIntSlider outlineth;
		ofxIntSlider IRthreshold;
		//ofxIntSlider IRfanwei;

		/****************************       For mask shader         *******************/
		ofShader shaderMask;
		ofShader shaderMotion;
		ofImage screenshot;
		ofImage backgroundImage;
		ofImage foregroundImage;
		ofImage bF;
		ofxCvGrayscaleImage outlineImage;     // outlineImage is a const image for each map/stage
		ofxCvGrayscaleImage outlineImageTBD;// this one is used in shader to detect whether 
		//ofImage brushImage; //these three haven't been used
		//ofImage knifeImage;
		//ofImage waterImage;
		ofFbo maskFbo;
		ofFbo existFbo;
		ofFbo fbo;
		void  maskShaderUpdate();
		float times = 6;
		float shiftx = 160;
		float shifty = 0;
		/*****************               PREPARE FOR PARTICLE SYS                       *******************/
		ofxCvContourFinder contourFinder;
		ofPoint GetMotionCenter();

		/**************************************   Tool Switch   *******************************************/
		Tool Dropper, Knife, Brush;
		ofPoint TSPosition;
		void ToolSwitchSetup();
		void ToolSwitchUpdate();
		void ToolSwitchDraw();

		/***************************************   Game Logic   ********************************************/
		bool stepend=false;
		bool firsttimehere=true;
		bool firsttimeend=false;
		//int GameStage;
		GameStage stage;
		RelicType relic;
		Process caitao;

		ofSoundPlayer shali;
		ofSoundPlayer saotu;
		ofSoundPlayer kouxue;
		ofSoundPlayer baojing;
		ofSoundPlayer daojishi;
		ofSoundPlayer gamelogicsound;
		

		//START
		ofImage startbackground;

		//PROCESS
		Widgets caitaoWidgets;

		//END
		ofImage endbackground;

		//GAMEOVER
		ofImage gameoverbackground;
		ofTrueTypeFont  appfont;
		/************************************   Game Data    ***************************************/
		void resetGameData();
		const float healthTotal = 5000;
		float healthLeft=5000;
		int workingTotal[5] = { 4151,4151,1449,1449,4151 };
		int workingLeft;


		float forceTotal1= 30;
		float forceTotal2 = 100;
		float currentForce=0;
		float safeThres1 = 15;
		float safeThres2 = 50;
		void setForce(float total1, float thres1, float total2, float thres2) {
			forceTotal1 = total1;
			forceTotal2 = total2;
			safeThres1 = thres1;
			safeThres2 = thres2;
		}
		float timeLimit = 120.0; //120 seconds
		float dropperstarttime=0;
		float droppertimer=0;
		float changingtimeLimit = 4000;//2seconds 2000millisecs
		float changingstarttime=0;
		float changingtimer=0;
		
		/****************************************    Button    ********************************************/

		//button ButtonCaitao;
		//button ButtonHuashi;
		button ButtonStart;
		//button ButtonPause;
		button ButtonRestartpro;
		button ButtonRestartend;
		button ButtonRestartgameover;
		//button ButtonHelp;
		//void getSwitchStage();
		void ButtonSetup();
		bool getButtonState(button bu);

		//void RelicPackageSetup();
		
		
		//Brian start from here

		
};
