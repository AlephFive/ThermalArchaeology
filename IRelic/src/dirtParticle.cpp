#include "dirtParticle.h"

//------------------------------------------------------------------
dirtParticle::dirtParticle() {

	timer = 0;
	isVisible = false;
	framesProcessed = 0;

}



void dirtParticle::reset() {
	//the unique val allows us to set properties slightly differently for each particle
	isVisible = false;
	framesProcessed = 0;
	uniqueVal = ofRandom(-10000, 10000);

	pos.x = ofGetMouseX();
	pos.y = ofGetMouseY();

	vel.x = ofRandom(-0.01, 0.01);
	vel.y = ofRandom(-0.01, 0.01);


	timer = 0;

	frc = ofPoint(0, 5, 0);
	origin = ofPoint(0, 0, 0);
	mousePos.x = ofGetMouseX();
	mousePos.y = ofGetMouseY();

	scale = ofRandom(5, 10);


	drag = ofRandom(0.96, 0.99);
	vel.y = fabs(vel.y) * 1.02; //make the particles all be going down


	r = (int)ofRandom(140, 160);
	g = (int)ofRandom(100, 120);
	b = (int)ofRandom(60, 70);

}

//------------------------------------------------------------------
void dirtParticle::update() {
	
	//1 - APPLY THE FORCES 
	if (isVisible) {
		framesProcessed++;

		if (framesProcessed == 50) {
			//find mouse velocity vector
			mousePos.x = ofGetMouseX();
			mousePos.y = ofGetMouseY();
			vel = (origin - mousePos)*-0.1;
			//vel.x = ofSignedNoise(uniqueVal, mousePos.y * 0.001, ofGetElapsedTimef()*0.2);
			//vel.y = ofSignedNoise(uniqueVal, mousePos.x * 0.001, ofGetElapsedTimef()*0.2);
		}
		else if (framesProcessed > 50) {
			

			

			vel *= drag;

			vel += frc * 0.01; //notice the frc is negative 
			


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
}

void dirtParticle::emit() {
	reset();

	lifeStart = ofGetElapsedTimeMillis();
	lifeEnd = lifeStart + 7000;
	origin = ofPoint(ofGetMouseX(), ofGetMouseY());
	timer = 0;
	isVisible = true;
}

//------------------------------------------------------------------
void dirtParticle::draw() {

	if (isVisible) {
		//if at end of lifespan, set isVisible to false
		float timer = ofGetElapsedTimeMillis();
		if (timer >= lifeEnd && isVisible) {
			isVisible = false;
		}

		

		ofSetColor(r, g, b);
		ofDrawCircle(pos.x, pos.y, scale * 10.0);
	}
}

bool dirtParticle::isAlive() {
	return isVisible;
}