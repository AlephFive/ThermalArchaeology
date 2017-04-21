#pragma once
#include "ofMain.h"

class dirtParticle {

public:
	dirtParticle();

	void reset();
	void update();
	void emit();
	void draw();
	bool isAlive();

	ofPoint pos;
	ofPoint vel;
	ofPoint frc;

	ofPoint origin;
	ofPoint mousePos;

	int framesProcessed;
	

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

	int r;
	int g;
	int b;

	bool goodVel;


};