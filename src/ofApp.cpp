#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

	ofBackground(34, 34, 34);

	// 8+ output channels,
	// 2+ input channels
	// 22050 samples per second
	// 512 samples per buffer
	// 4 num buffers (latency)
	
	int bufferSize		= 512;
	sampleRate 			= 44100;
	phase 				= 0;
	phaseAdder 			= 0.0f;
	phaseAdderTarget 	= 0.0f;
	volume				= 0.1f;
	bNoise 				= false;

    //output
	lAudio.assign(bufferSize, 0.0);
    speaker1Index = 0;
	
    rAudio.assign(bufferSize, 0.0);
    speaker2Index = 10000;
    
    nAudio.assign(bufferSize, 0.0);
    delaySamples.assign(2048*16, 0.0);
	
    //input
	left.assign(bufferSize, 0.0);
	right.assign(bufferSize, 0.0);
    
	soundStream.listDevices();
	
	//if you want to set the device id to be different than the default
//    soundStream.setDeviceID(9); 	//note some devices are input only and some are output only

	soundStream.setup(this, 2, 2, sampleRate, bufferSize, 5);

	ofSetFrameRate(60);
}


//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){

	ofSetColor(225);
	ofDrawBitmapString("press 's' to unpause the audio\npress 'e' to pause the audio", 31, 32);
	
	ofNoFill();
	
    int leftMargin = 32;
    int currentTop = 50;
    ofVec2f boxSz(900, 100);
    drawSoundrect("Left Channel", lAudio,ofVec2f(leftMargin, currentTop),boxSz);
    drawSoundrect("Right Channel", rAudio,ofVec2f(leftMargin, currentTop+=100),boxSz);
//	drawSoundrect("nAudio", nAudio,ofVec2f(leftMargin, currentTop+=100),boxSz);
    drawSoundrect("LeftIn", left,ofVec2f(leftMargin, currentTop+=100),boxSz);
    drawSoundrect("delaySamples", delaySamples,ofVec2f(leftMargin, currentTop+=100),boxSz);
    
	ofSetColor(225);
//	string reportString = "volume: ("+ofToString(volume, 2)+") modify with -/+ keys\npan: ("+ofToString(pan, 2)+") modify with mouse x\nsynthesis: ";
//	if( !bNoise ){
//		reportString += "sine wave (" + ofToString(targetFrequency, 2) + "hz) modify with mouse y";
//	}else{
//		reportString += "noise";	
//	}
//	ofDrawBitmapString(reportString, 32, 579);

}

void ofApp::drawSoundrect(string name, vector <float> samples,ofVec2f pos, ofVec2f sz)
{
    ofPushStyle();
    ofPushMatrix();
    ofTranslate(pos.x, pos.y, 0);
    
    ofSetColor(225);
    ofDrawBitmapString(name, 4, 18);
    
    ofSetLineWidth(1);
    ofRect(0, 0, sz.x, sz.y);
    
    ofSetColor(245, 58, 135);
    ofSetLineWidth(3);
    
    ofBeginShape();
    for (unsigned int i = 0; i < samples.size(); i++){
        float x =  ofMap(i, 0, samples.size(), 0, sz.x, true);
        ofVertex(x, (sz.y/2) - samples[i]*sz.y*.9);
    }
    ofEndShape(false);
    
    ofPopMatrix();
	ofPopStyle();

}

//--------------------------------------------------------------
void ofApp::keyPressed  (int key){
	if (key == '-' || key == '_' ){
		volume -= 0.05;
		volume = MAX(volume, 0);
	} else if (key == '+' || key == '=' ){
		volume += 0.05;
		volume = MIN(volume, 1);
	}
	
	if( key == 's' ){
		soundStream.start();
	}
	
	if( key == 'e' ){
		soundStream.stop();
	}
	
}

//--------------------------------------------------------------
void ofApp::keyReleased  (int key)
{

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y )
{
	int width = ofGetWidth();
	pan = (float)x / (float)width;
	float height = (float)ofGetHeight();
	float heightPct = ((height-y) / height);
	targetFrequency = 2000.0f * heightPct;
	phaseAdderTarget = (targetFrequency / (float) sampleRate) * TWO_PI;
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button)
{
	int width = ofGetWidth();
	pan = (float)x / (float)width;
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button)
{
	bNoise = true;
}


//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button)
{
	bNoise = false;
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h)
{

}
//--------------------------------------------------------------
void ofApp::audioIn(float * input, int bufferSize, int nChannels){
	
	float curVol = 0.0;
	
	// samples are "interleaved"
	int numCounted = 0;
    

    for (int i = 0; i < delaySamples.size(); i++)
    {
        delaySamples[i] *= .99;
    }
	//lets go through each sample and calculate the root mean square which is a rough way to calculate volume
    //interleaved samples
	for (int i = 0; i < bufferSize; i++)
    {
		left[i]		= input[i*2]*0.5;
		right[i]	= input[i*2+1]*0.5;
		curVol += left[i] * left[i];
		curVol += right[i] * right[i];
		numCounted+=2;
        int myIndex = (delayMic1InsertionIndex+i)%delaySamples.size();
        //add current left samples into delay buffer
        delaySamples[myIndex] += input[i*2]*2;
//        delaySamples.pop_back();
//        delaySamples.insert(delaySamples.begin(), input[i*2]);
	}
	delayMic1InsertionIndex = (delayMic1InsertionIndex+bufferSize)%delaySamples.size();
	//this is how we get the mean of rms :)
	curVol /= (float)numCounted;
	
	// this is how we get the root of rms :)
	curVol = sqrt( curVol );
	
//	smoothedVol *= 0.93;
//	smoothedVol += 0.07 * curVol;
//	
//	bufferCounter++;
	
}

//--------------------------------------------------------------
void ofApp::audioOut(float * output, int bufferSize, int nChannels)
{
	//pan = 0.5f;
	float leftScale = 1 - pan;
	float rightScale = pan;

	// sin (n) seems to have trouble when n is very large, so we
	// keep phase in the range of 0-TWO_PI like this:
	while (phase > TWO_PI)
    {
		phase -= TWO_PI;
	}

	if ( bNoise == true)
    {
		// ---------------------- noise --------------
		for (int i = 0; i < bufferSize; i++)
        {
			lAudio[i] = output[i*nChannels    ] = ofRandom(0, 1) * volume * leftScale;
			rAudio[i] = output[i*nChannels + 1] = ofRandom(0, 1) * volume * rightScale;
		}
	} else {
		phaseAdder = 0.95f * phaseAdder + 0.05f * phaseAdderTarget;
		for (int i = 0; i < bufferSize; i++)
        {

            
            int myIndex = (speaker1Index+i) % delaySamples.size();
            lAudio[i] = output[i*nChannels    ] = delaySamples[myIndex];
            myIndex = (speaker2Index+i) % delaySamples.size();
            rAudio[i] = output[i*nChannels + 1] = delaySamples[myIndex];
            
            phase += phaseAdder;
			float sample = sin(phase);
//			lAudio[i] = output[i*nChannels    ] = sample * volume * leftScale;
//			rAudio[i] = output[i*nChannels + 1] = sample * volume * rightScale;
//            nAudio[i] = output[i*nChannels + 2] = sample * volume * rightScale;//sin(phase*.4) * volume * rightScale;
		}
	}
    
    speaker1Index = (speaker1Index+bufferSize) % delaySamples.size();
    speaker2Index = (speaker2Index+bufferSize) % delaySamples.size();
    ofLog(OF_LOG_ERROR,"nChannels: " + ofToString(nChannels));
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
