#include "ofApp.h"
#include <string>
#include <algorithm>
#include <iostream>

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate(200000);
    ofSetVerticalSync(false);
    
    panoLeft.loadImage("test_L.png");
    panoRight.loadImage("test_R.png");
    
    Lw = panoLeft.getWidth();
    Lh = panoLeft.getHeight();
    Rw = panoRight.getWidth();
    Rh = panoRight.getHeight();
    
    sourceImg.allocate(Lw, Lh);
    targetImg.allocate(Rw, Rh);
    
    disparity.allocate(Lw, Lh, OF_IMAGE_COLOR_ALPHA);
    disparity.setColor(ofColor(0,0,0,0));
    disparity.reloadTexture();

    curpixel = 0;
    templateSize = 32;
    regionScale = 12;
    
    stringstream ts;
    ts << "disparity_" << templateSize << ".png";
    outfilename = ts.str();

    zoom = 1.0;
    if (sourceImg.getWidth() > ofGetWindowWidth() )
        zoom = ofGetWindowWidth()/sourceImg.getWidth();
    ofSetWindowShape(sourceImg.getWidth()*zoom*2.0, sourceImg.getHeight()*zoom);
    
    // template rect in sourceImage
    templateFrame.set(0,0,templateSize,templateSize);
    sourceFrame.set(0,0,sourceImg.getWidth(),sourceImg.getHeight());
    trackFrame = sourceFrame;
    
    sourceImg.setFromPixels(panoLeft.getPixels(), Lw, Lh);
    targetImg.setFromPixels(panoRight.getPixels(), Rw, Rh);

    templateImg.allocate(templateFrame.width, templateFrame.height);
}

ofPoint ofApp::findFrame(ofRectangle frame) {

    // clip source ROI to source image
    ofRectangle croppedframe = frame.getIntersection(sourceFrame);
    sourceImg.setROI(croppedframe);
    
    if (frame != croppedframe)
        templateImg.resize(croppedframe.width, croppedframe.height);
    else
        templateImg.resize(frame.width, frame.height);
    
    templateImg = sourceImg;
    sourceImg.resetROI();
    
    // set ROI on target to speed things up
    // take templateSize * regionScale
    ofRectangle targetFrame = templateFrame;
    targetFrame.scaleFromCenter(regionScale);
    targetFrame = targetFrame.getIntersection(sourceFrame);
    targetImg.setROI(targetFrame);
    
    // create the target image and match
    IplImage *result = cvCreateImage(cvSize(targetFrame.width - templateImg.width + 1, targetFrame.height - templateImg.height + 1), 32, 1);
    cvMatchTemplate(targetImg.getCvImage(), templateImg.getCvImage(), result, CV_TM_SQDIFF);
    
    targetImg.resetROI();
    
    // collect stats from match process
    double minVal, maxVal;
    CvPoint minLoc, maxLoc;
    cvMinMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, 0);
    cvReleaseImage(&result);
    
    double weight = minVal/500000.0;
    return ofPoint(targetFrame.x + minLoc.x, targetFrame.y + minLoc.y, weight);
}

//--------------------------------------------------------------
void ofApp::update() {
    if (tracking) {
        
        // if finished tracking
        if (curpixel >= sourceImg.getWidth()*sourceImg.getHeight() ) {
            disparity.saveImage(outfilename);
            tracking = false;
            return;
        }

        
        int px = curpixel % int(sourceImg.getWidth());
        int py = int(floor(curpixel / sourceImg.getWidth()));
        
        // iterate over pixels inside the custom track frame
        if (trackFrame.inside(px,py)) {

            templateFrame.set(px-templateFrame.width/2, py-templateFrame.height/2, templateFrame.width, templateFrame.height);
            
            // find the corresponding frame in targetImg, using openCV
            // stored in templateLocation
            templateLocation = ofApp::findFrame(templateFrame);

            // display disparity on top
            
            // image colours
            int dx = 128 + (templateLocation.x - px);
            int dy = 128 + (templateLocation.y - py);
            int weight = templateLocation.z * 0.1 * 255;
            
            
            disparity.setColor( px, py, ofColor(dx, dy, weight));
            
            
            // periodically update the disparity map
            if (curpixel % 20 == 0)
                disparity.reloadTexture();
            
            // periodically save out the disparity map
            if (curpixel % int(trackFrame.width*10) == 0) {
                disparity.saveImage(outfilename);
                cout << "Saving image " << endl;
            }
        } else {
            while(!trackFrame.inside(px,py)) {
                curpixel++;
                px = curpixel % int(sourceImg.getWidth());
                py = int(floor(curpixel / sourceImg.getWidth()));
                if (curpixel >= sourceImg.getWidth()*sourceImg.getHeight() )
                    return;
            }
        }
        
        curpixel++;
    }

}

//--------------------------------------------------------------
void ofApp::draw(){

    ofScale(zoom,zoom,zoom);
    
    int Sw = sourceImg.getWidth();
    
    sourceImg.draw(0, 0);
    targetImg.draw(Sw, 0);
    disparity.draw(0, 0);

    ofNoFill();
    
    ofSetColor(ofFloatColor(1,0,0));
    ofRect(templateFrame.x, templateFrame.y, templateFrame.width, templateFrame.height);
    
    ofSetColor(ofFloatColor(0,1,0));
    ofRect(Sw+templateLocation.x, templateLocation.y, templateFrame.width, templateFrame.height);
    
    ofSetColor(ofFloatColor(1,1,0));
    ofRect(trackFrame.x, trackFrame.y, trackFrame.width, trackFrame.height);
    
    ofSetColor(ofFloatColor(1,1,1));
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

    if (key == 't')
        tracking = !tracking;
    else if (key == 's')
        disparity.saveImage(outfilename);
    else if (key == '+') {
        templateSize *= 2;
        templateImg.resize(templateSize, templateSize);
    }
    else if (key == '-') {
        templateSize /= 2;
        templateImg.resize(templateSize, templateSize);
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
    if (!tracking) {
        int px = x/zoom;
        int py = y/zoom;
        trackFrame.set(ofPoint(trackFrame.x,trackFrame.y), ofPoint(px,py));
    }

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

    if (!tracking) {
        int px = x/zoom;
        int py = y/zoom;
        
        trackFrame.set(ofPoint(px,py), ofPoint(px,py));
    }
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

    if (!tracking) {
        curpixel = trackFrame.x + trackFrame.y*sourceFrame.width;
        trackFrame.set(trackFrame.x-1, trackFrame.y-1, trackFrame.width+2, trackFrame.height+2);
    }
    tracking = !tracking;

    
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
