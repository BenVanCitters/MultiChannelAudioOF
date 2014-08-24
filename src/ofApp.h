#pragma once

#include "ofMain.h"
#include "ofxGui.h"


#define OUT_CHANNEL_COUNT 2

class ofApp : public ofBaseApp
{

public:

    void setup();
    void update();
    void draw();
    void exit();
    void keyPressed  (int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    void audioOut(float * input, int bufferSize, int nChannels);
    void audioIn(float * input, int bufferSize, int nChannels);
    void sampleLenChanged(int & sampleLenChanged);
    void spacingChanged(float & spacing);
    void drawSoundrect(string name, vector <float> samples,ofVec2f pos, ofVec2f sz,ofColor color = ofColor(245, 58, 135));

    //GUI
    ofxPanel gui;
    ofxIntSlider sampleLenSlider;
    ofxFloatSlider sampleFadeSlider;
    bool bufferInUse;
    ofxFloatSlider channelSpacingSlider;
    
    ofSoundStream soundStream;
    int bufferSize;
    int sampleRate;

	//output
    vector<float> outputBuffers[OUT_CHANNEL_COUNT];
    int outputBufferCurrentIndex[OUT_CHANNEL_COUNT];

    vector <float> delaySamples;
    
    // input
    vector <float> input1;
    vector <float> input2;
    int delayMic1InsertionIndex;
    int delayMic2InsertionIndex;
    
    void compressor();
//    void ofApp::compressor();
    void bitcrusher();
    void bitcrusher(int start, int end);
    void fade();
    void fade(int start, int end);
};
