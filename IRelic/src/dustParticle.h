#pragma once
#include "ofMain.h"

class dustParticle {

public:
	dustParticle();
	
	void reset();
	void update(ofPoint *mPos);
	void emit(ofPoint *xy);
	//void emit();
	void draw();
	bool isAlive();

	ofPoint pos;
	ofPoint vel;
	ofPoint frc;
	ofPoint downfrc;

	ofPoint origin;

	float drag;
	float uniqueVal;
	
	float scale;

	float lifespan;
	float lifeStart;
	float lifeEnd;
	float timer;

	float transparency;
	float gravity;

	bool isVisible;
	
	bool goodVel;
	int framesProcessed;

	float expansionRate;

	int alpha;
	int r;
	int g;
	int b;

	ofImage dust;

	
};