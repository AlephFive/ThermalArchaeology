#include "dustParticle.h"

//------------------------------------------------------------------
dustParticle::dustParticle() {
	
	timer = 0;
	isVisible = false;
	
}



void dustParticle::reset() {
	//the unique val allows us to set properties slightly differently for each particle
	isVisible = false;
	
	uniqueVal = ofRandom(-10000, 10000);

	pos.x = ofGetMouseX();
	pos.y = ofGetMouseY();

	vel.x = ofRandom(-0.01, 0.01);
	vel.y = ofRandom(-0.01, 0.01);

	
	timer = 0;

	frc = ofPoint(0, 0, 0);
	origin = ofPoint(0, 0, 0);

	scale = ofRandom(10, 15);

	
	drag = ofRandom(0.97, 0.99);
	vel.y = fabs(vel.y) * 1.02; //make the particles all be going down
	
	alpha = 255;
	r = (int)ofRandom(230, 255);
	g = (int)ofRandom(150, 190);
	b = (int)ofRandom(50, 100);

}

//------------------------------------------------------------------
void dustParticle::update() {

	//1 - APPLY THE FORCES 
	if (isVisible) {
		scale = scale + 0.005;
		

		frc = origin - pos;

		//let get the distance and only repel points close to the mouse
		float dist = frc.length();
		frc.normalize();

		vel *= drag;
		if (dist < 60) {
			vel += -frc * 0.01; //notice the frc is negative 
		}
		
		else {
			//if the particles are not close to us, lets add a little bit of random movement using noise. this is where uniqueVal comes in handy. 			
			frc.x = ofSignedNoise(uniqueVal, pos.y * 0.01, ofGetElapsedTimef()*0.2);
			frc.y = ofSignedNoise(uniqueVal, pos.x * 0.01, ofGetElapsedTimef()*0.2);
			vel += frc * 0.001;
		}
		

		//2 - UPDATE OUR POSITION

		pos += vel;

		//3 - (optional) LIMIT THE PARTICLES TO STAY ON SCREEN 
		//we could also pass in bounds to check - or alternatively do this at the ofApp level
		if (pos.x > ofGetWidth()) {
			pos.x = ofGetWidth();
			vel.x *= -1.0;
		}
		else if (pos.x < 0) {
			pos.x = 0;
			vel.x *= -1.0;
		}
		if (pos.y > ofGetHeight()) {
			pos.y = ofGetHeight();
			vel.y *= -1.0;
		}
		else if (pos.y < 0) {
			pos.y = 0;
			vel.y *= -1.0;
		}
	}
}

void dustParticle::emit() {
	reset();

	lifeStart = ofGetElapsedTimeMillis();
	lifeEnd = lifeStart + (int)ofRandom(1000, 5000);
	origin = ofPoint(ofGetMouseX(), ofGetMouseY());
	timer = 0;
	isVisible = true;
}

//------------------------------------------------------------------
void dustParticle::draw() {

	if (isVisible) {
		//if at end of lifespan, set isVisible to false
		float timer = ofGetElapsedTimeMillis();
		if (timer >= lifeEnd && isVisible) {
			isVisible = false;
		}

		if (alpha != 0) {
			alpha = alpha - 0.5;
		}

		ofSetColor(r, g, b, alpha);
		ofDrawCircle(pos.x, pos.y, scale * 10.0);
	}
}

bool dustParticle::isAlive() {
	return isVisible;
}

