#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
        ofPoint findFrame(ofRectangle frame);

        ofxCvColorImage sourceImg;
        ofxCvColorImage targetImg;
        ofxCvColorImage templateImg;
    
        ofImage panoLeft;
        ofImage panoRight;
        ofImage disparity;
        string outfilename;

        ofRectangle sourceFrame;
        ofRectangle templateFrame;
        ofPoint templateLocation;
        int templateSize;
        float regionScale;
    
        int curpixel;
        bool tracking;
        bool dragging;
    
        ofRectangle trackFrame;
    
        float zoom;
        int Lw, Lh, Rw, Rh;
};
