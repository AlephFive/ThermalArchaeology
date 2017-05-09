#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
	//ofSetupOpenGL(1280,800,OF_WINDOW);			// <-------- setup the GL context
	ofGLWindowSettings settings;
	settings.setGLVersion(3, 2);
	settings.width = 1280;
	settings.height = 800;
	settings.windowMode = OF_FULLSCREEN;
	
	ofCreateWindow(settings);
	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp(new ofApp());

}
