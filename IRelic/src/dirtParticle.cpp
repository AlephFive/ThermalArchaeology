#include "dirtParticle.h"

//------------------------------------------------------------------
dirtParticle::dirtParticle() {

	reset();

}
double dirtParticle::gaussrand(double V, double E)
{

	static double V1, V2, S;

	static int phase = 0;

	double X;



	if (phase == 0) {

		do {

			double U1 = (double)rand() / RAND_MAX;

			double U2 = (double)rand() / RAND_MAX;



			V1 = 2 * U1 - 1;

			V2 = 2 * U2 - 1;

			S = V1 * V1 + V2 * V2;

		} while (S >= 1 || S == 0);



		X = V1 * sqrt(-2 * log(S) / S);

	}
	else

		X = V2 * sqrt(-2 * log(S) / S);



	phase = 1 - phase;
	X = X * V + E;

	return X;

}


void dirtParticle::reset() {
	//the unique val allows us to set properties slightly differently for each particle
	isVisible = false;
	framesProcessed = 0;
	uniqueVal = ofRandom(-10000, 10000);

	pos.x = 0;
	pos.y = 0;

	vel.x = ofRandom(-0.01, 0.01);
	vel.y = ofRandom(-0.001, 0.001);

	goodVel = false;
	timer = 0;

	frc = ofPoint(0, 50, 0);
	origin = ofPoint(0, 0, 0);
	
	//mousePos.x = ofGetMouseX();
	//mousePos.y = ofGetMouseY();
	//mousePos.x = 0;
	//mousePos.y = 0;
	int sizedecider = (int)ofRandom(1, 50);
	if (sizedecider != 10) {
		scale = ofRandom(0.05, 0.1);
	}
	else {
		scale = ofRandom(0.2, 0.4);
	}



	




	drag = ofRandom(0.96, 0.971);
	vel.y = fabs(vel.y) * 1.02; //make the particles all be going down


	r = (int)ofRandom(150, 200);
	g = (int)ofRandom(100, 120);
	b = (int)ofRandom(60, 75);

	framelimit = (int)ofRandom(5, 15);
}

//------------------------------------------------------------------
void dirtParticle::update(ofPoint *origin2) {
	
	//1 - APPLY THE FORCES 
	if (isVisible) {
		framesProcessed++;
		
		if (framesProcessed == 5) {
			//find mouse velocity vector
			//mousePos.x = ofGetMouseX();
			//mousePos.y = ofGetMouseY();
			vel = (origin - *origin2)*-0.4;
			pos += vel * 10;
			vel.limit(5);
			goodVel = (vel.lengthSquared() > 0.01) ? true : false;
			vel.x += ofRandom(-0.5, 0.5);
			vel.y += ofRandom(-0.5, 0.5);
			
			
		}
		else if (framesProcessed > 5 && goodVel) {
			

			

			vel *= drag;

			vel += frc * 0.01; //notice the frc is negative 
			


			//2 - UPDATE OUR POSITION

			pos += vel;

			//3 - (optional) LIMIT THE PARTICLES TO STAY ON SCREEN 
			//we could also pass in bounds to check - or alternatively do this at the ofApp level
			
			if (pos.x > ofGetWidth()) {
				isVisible = false;
			}
			else if (pos.x < 0) {
				isVisible = false;
			}
			if (pos.y > ofGetHeight()) {
				isVisible = false;
			}
			else if (pos.y < 0) {
				isVisible = false;
			}
			
		}
		else if (framesProcessed > 20 && !goodVel) {
			isVisible = false;
		}
	}
}

void dirtParticle::emit(ofPoint *xy) {
	reset();

	lifeStart = ofGetElapsedTimeMillis();
	lifeEnd = lifeStart + 7000;
	origin = *xy;
	timer = 0;
	isVisible = true;
	pos.x = origin.x + ofRandom(-20, 20);
	pos.y = origin.y + ofRandom(-20, 20);
}
/*
void dirtParticle::emit() {
	reset();

	lifeStart = ofGetElapsedTimeMillis();
	lifeEnd = lifeStart + 7000;
	origin = ofPoint(ofGetMouseX(), ofGetMouseY());
	timer = 0;
	isVisible = true;
}
*/
//------------------------------------------------------------------
void dirtParticle::draw() {

	if (isVisible && goodVel) {
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