#include "ofApp.h"




//--------------------------------------------------------------
void ofApp::setup(){
	ofSetVerticalSync(true);

	int num = 100;
	p.assign(num, dustParticle());
	indexWhereThereIsInvisibleParticles = 0;
	framesProcessed = 0;

	resetParticles();
}

void ofApp::resetParticles() {
	for (unsigned int i = 0; i < p.size(); i++) {
		p[i].reset();
	}
}

//--------------------------------------------------------------
void ofApp::update(){
	framesProcessed++;

	for (unsigned int i = 0; i < p.size(); i++) {
		p[i].update();
	}


	if (brushDown && framesProcessed>framesPerEmit) {
		bool found = false;
		framesProcessed = 0;
		//focus on this place after

	

		for (unsigned int i = 0; i < p.size(); i++) {
			if (!p[i].isAlive()) {
				indexWhereThereIsInvisibleParticles = i;
				found = true;
				break;
			}
			
		}

		if (!found) {
			p[0].reset();
			indexWhereThereIsInvisibleParticles = 0;
		}

		p[indexWhereThereIsInvisibleParticles].emit();
	}
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofBackgroundGradient(ofColor(60, 60, 60), ofColor(10, 10, 10));


	
	for (unsigned int i = 0; i < p.size(); i++) {
		p[i].draw();
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
	brushDown = true;
	
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
	brushDown = false;
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

