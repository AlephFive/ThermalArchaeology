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

	pos.x = 0;
	pos.y = 0;

	//pos.x = ofGetMouseX();
	//pos.y = ofGetMouseY();

	vel.x = 0;
	vel.y = 0;

	
	timer = 0;

	frc = ofPoint(0, 0, 0);
	downfrc = ofPoint(0, 1, 0);
	origin = ofPoint(0, 0, 0);

	scale = ofRandom(2, 5);
	

	expansionRate = ofRandom(0.008, 0.015);
	drag = ofRandom(0.985, 0.995);
	//vel.y = fabs(vel.y) * 1.01; //make the particles all be going down
	
	alpha = 30;
	/*
	r = (int)ofRandom(230, 240);
	g = (int)ofRandom(220, 225);
	b = (int)ofRandom(160, 165);
	*/
	r = (int)ofRandom(150, 165);
	g = (int)ofRandom(100, 110);
	b = (int)ofRandom(60, 65);

	dust.load("dust.png");

	framesProcessed = 0;
}

//------------------------------------------------------------------
void dustParticle::update(ofPoint *mPos) {


	//1 - APPLY THE FORCES 
	if (isVisible) {
		framesProcessed++;

		if (framesProcessed == 10) {
			//find mouse velocity vector
			//mousePos.x = ofGetMouseX();
			//mousePos.y = ofGetMouseY();
			vel = (origin - *mPos)*-0.1;//*-0.2;
			vel.limit(4);
			//expansionRate += 0.005 * vel.lengthSquared();
			vel *= 1;
			
			goodVel = (vel.lengthSquared() > 0.04) ? true : false;
			//vel.x *= ofSignedNoise(uniqueVal)*0.1;
			//vel.y *= ofSignedNoise(uniqueVal)*0.1;
			vel.x += ofRandom(-0.5, 0.5);
			vel.y += ofRandom(-0.5, 0.5);
			pos += vel * 10;

		}
		else if (framesProcessed > 10 && goodVel) {
			if (scale < 10) {
				scale = scale + expansionRate * 4;
			}
			/*
			else if (scale >= 25) {
				scale = scale + expansionRate * 5;
			}
			*/
			else if (scale >= 10) {
				scale = scale + expansionRate * 2;
			}

			
			frc = origin - pos;

			//let get the distance and only repel points close to the mouse
			float dist = frc.length();
			frc.normalize();
			
			vel *= drag;
			/*
			if (dist < 5) {
				vel += -frc * 0.01; //notice the frc is negative 
			}

			else {
				//if the particles are not close to us, lets add a little bit of random movement using noise. this is where uniqueVal comes in handy. 			
				frc.x = ofSignedNoise(uniqueVal, pos.y * 0.01, ofGetElapsedTimef()*0.2);
				frc.y = ofSignedNoise(uniqueVal, pos.x * 0.01, ofGetElapsedTimef()*0.2);
				vel += frc * 0.001;
			}
			*/

			//2 - UPDATE OUR POSITION

			pos += vel;
			pos += downfrc;

			//3 - (optional) LIMIT THE PARTICLES TO STAY ON SCREEN 
			//we could also pass in bounds to check - or alternatively do this at the ofApp level
			/*
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
			*/
		}

		else if (framesProcessed > 20 && !goodVel) {
			isVisible = false;
		}





		
	}
}

void dustParticle::emit(ofPoint *xy) {
	reset();

	lifeStart = ofGetElapsedTimeMillis();
	lifeEnd = lifeStart + (int)ofRandom(6000, 12000);
	origin = *xy;
	timer = 0;
	isVisible = true;
	pos.x = origin.x + ofRandom(-2, 2);
	pos.y = origin.y + ofRandom(-2, 2);
}
/*
void dustParticle::emit() {
	reset();

	lifeStart = ofGetElapsedTimeMillis();
	lifeEnd = lifeStart + (int)ofRandom(1000, 5000);
	origin = ofPoint(ofGetMouseX(), ofGetMouseY());
	timer = 0;
	isVisible = true;
}
*/
//------------------------------------------------------------------
void dustParticle::draw() {

	if (isVisible && framesProcessed > 5 && goodVel) {
		//if at end of lifespan, set isVisible to false
		float timer = ofGetElapsedTimeMillis();
		if (timer >= lifeEnd && isVisible) {
			isVisible = false;
		}
		

		if (alpha > 50) {
			if (framesProcessed % 8 == 0) {
				alpha = alpha - 1;
			}
		}
		else if (alpha <= 50 && alpha > 25) {
			if (framesProcessed % 5 == 0) {
				alpha = alpha - 1;
			}
		}
		else if (alpha <= 25 && alpha > 15) {
			if (framesProcessed % 7 == 0) {
				alpha = alpha - 1;
			}
		}
		else if (alpha <= 15 && alpha > 5) {
			if (framesProcessed % 9 == 0) {
				alpha = alpha - 1;
			}
		}
		else if (alpha <= 5 && alpha > 0) {
			if (framesProcessed % 12 == 0) {
				alpha = alpha - 1;
			}
		}
		else if (alpha == 0) {
			isVisible = false;
		}
		

		//ofEnableAlphaBlending();
		ofSetColor(r, g, b, alpha);

		float drawscale = scale * 30;
		//
		dust.draw((pos.x - drawscale / 2), (pos.y - drawscale / 2), drawscale, drawscale);
		//ofDrawCircle(pos.x, pos.y, scale * 30);
		//ofDisableAlphaBlending();
	}
	
}

bool dustParticle::isAlive() {
	return isVisible;
}

