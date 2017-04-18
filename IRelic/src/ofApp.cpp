#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){
	ofSetVerticalSync(true);
	int numDust = 100;
	int numDirt = 250;

	dusts.assign(numDust, dustParticle());
	dirts.assign(numDirt, dirtParticle());
	dustIndex = 0;
	dirtIndex = 0;

	brushDown = false;
	scrapeDown = false;

	resetParticles();
	

	resetTimer();
}

void ofApp::resetParticles() {
	//reset dust
	for (unsigned int i = 0; i < dusts.size(); i++) {
		dusts[i].reset();
	}

	//reset dirt
	for (unsigned int i = 0; i < dirts.size(); i++) {
		dirts[i].reset();
	}
}

void ofApp::resetTimer() {
	startTime = ofGetElapsedTimeMillis();
	timer = ofGetElapsedTimeMillis() - startTime;
}

//--------------------------------------------------------------
void ofApp::update(){
	
	
	timer = ofGetElapsedTimeMillis() - startTime;
	
	brushParticleEffects();
	scrapeParticleEffects();
	

	


	



}

void ofApp::brushParticleEffects() {
	//brush effects
	for (unsigned int i = 0; i < dusts.size(); i++) {
		dusts[i].update();
	}

	if (brushDown && timer>80) {
		bool found = false;
		resetTimer();

		for (unsigned int i = 0; i < dusts.size(); i++) {
			if (!dusts[i].isAlive()) {
				//find index where there is unutilized particle objects
				dustIndex = i;
				found = true;
				break;
			}
		}

		if (!found) {
			dusts[0].reset();
			dustIndex = 0;
		}

		dusts[dustIndex].emit();
	}
}

void ofApp::scrapeParticleEffects() {
	//dirt effects
	for (unsigned int i = 0; i < dirts.size(); i++) {
		dirts[i].update();
	}

	if (scrapeDown && timer>80) {
		bool found = false;
		resetTimer();

		for (unsigned int i = 0; i < dirts.size(); i++) {
			if (!dirts[i].isAlive()) {
				//find index where there is unutilized particle objects
				dirtIndex = i;
				found = true;
				break;
			}
		}

		if (!found) {
			dirts[0].reset();
			dirtIndex = 0;
		}

		dirts[dirtIndex].emit();
	}
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofBackgroundGradient(ofColor(60, 60, 60), ofColor(10, 10, 10));

	
	
	for (unsigned int i = 0; i < dusts.size(); i++) {
		dusts[i].draw();
	}

	for (unsigned int i = 0; i < dirts.size(); i++) {
		dirts[i].draw();
	}


	
	ofSetColor(190);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	//brushDown = true;
	scrapeDown = true;
	
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
	//brushDown = false;
	scrapeDown = false;

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

// A class to describe a group of Particles
// An ArrayList is used to manage the list of Particles 

