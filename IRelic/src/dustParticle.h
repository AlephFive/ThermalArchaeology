#pragma once
#include "ofMain.h"

class dustParticle {

public:
	dustParticle();

	void reset();
	void update();
	void emit();
	void draw();
	bool isAlive();

	ofPoint pos;
	ofPoint vel;
	ofPoint frc;

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

	int alpha;
	int r;
	int g;
	int b;


};