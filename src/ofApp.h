#pragma once

#include "ofMain.h"

class ofApp : public ofBaseApp{

	public:

		void setup();
		void update();
		void draw();

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


    void drawSoundrect(string name, vector <float> samples,ofVec2f pos, ofVec2f sz);
    
        ofSoundStream soundStream;

		float 	pan;
		int		sampleRate;
		bool 	bNoise;
		float 	volume;

	//output

    vector <float> lAudio;
    int speaker1Index;
    
		vector <float> rAudio;
    int speaker2Index;
    vector <float> nAudio;
    
    
    int delayMic1InsertionIndex;
    vector <float> delaySamples;
    
    // input
    vector <float> left;
    vector <float> right;
    
    
		
		//------------------- for the simple sine wave synthesis
		float 	targetFrequency;
		float 	phase;
		float 	phaseAdder;
		float 	phaseAdderTarget;
};
