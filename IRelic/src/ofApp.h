#pragma once

#include "ofMain.h"
#include "Lib\ImagerIPC2.h"

#include <stdio.h>

#include <math.h>

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

enum ToolStyle { none, knife, brush, dropper };

class Tool {
public:
	ToolStyle ID;
	Tool() { ID = none; }
	void setToolStyle(int num) { ID = (ToolStyle)num; }
};






class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
		
		void resetTimer();

		void resetParticles();
		void drawParticles();
		void brushParticleEffects();
		void scrapeParticleEffects();

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



		//Kilo start from here

		/*For Arduino Serial Communication*/
		
		string str;
		Tool tool;

		int IRimage_w = 160;
		int IRimage_h = 120;





		//Brian start from here

		
};
