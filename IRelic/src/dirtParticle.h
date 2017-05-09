#pragma once
#include "ofMain.h"

class dirtParticle {

public:
	dirtParticle();

	void reset();
	void update(ofPoint * mPos);
	void emit(ofPoint *xy);
	//void emit();
	void draw();
	bool isAlive();
	double gaussrand(double V, double E);

	
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

	int framelimit;
};