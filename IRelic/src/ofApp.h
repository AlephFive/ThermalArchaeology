#pragma once

#include "ofMain.h"
#include "Lib\ImagerIPC2.h"
#include "dustParticle.h"
#include "dirtParticle.h"
#include "communicator.h"


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

		
		
		
};
