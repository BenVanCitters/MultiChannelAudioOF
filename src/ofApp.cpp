#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{

	ofBackground(34, 34, 34);

	// 8+ output channels,
	// 2+ input channels
	// 22050 samples per second
	// 512 samples per buffer
	// 4 num buffers (latency)
	
	bufferSize		= 512;
	sampleRate 			= 44100;

    //output
    delaySamples.assign(2048*16, 0.0);
    bufferInUse = false;
    
    for(int i = 0; i < OUT_CHANNEL_COUNT; i++)
    {
        outputBuffers[i] = vector<float>();
        outputBuffers[i].assign(bufferSize, 0.0);
        outputBufferCurrentIndex[i] = (int)(delaySamples.size()*i*1.f/OUT_CHANNEL_COUNT);
    }
    
    //input
	input1.assign(bufferSize, 0.0);
	input2.assign(bufferSize, 0.0);

    delayMic1InsertionIndex = 0;
    delayMic2InsertionIndex = delaySamples.size()/2;
    
    
	soundStream.listDevices();
	//if you want to set the device id to be different than the default
//    soundStream.setDeviceID(9); 	//note some devices are input only and some are output only

	soundStream.setup(this, OUT_CHANNEL_COUNT, 2, sampleRate, bufferSize, 5);

    
    //gUI
    gui.setup(); // most of the time you don't need a name
    gui.setSize(900, 30);
    gui.add(sampleLenSlider.setup( "sampleLen", 2048*16, 500,  2048*128,900));
    gui.add(sampleFadeSlider.setup( "sampleFade", .99, .98,  1,900));
    gui.add(channelSpacingSlider.setup( "channel spacing", .5, 0,  1.f,900));
    channelSpacingSlider.addListener(this,&ofApp::spacingChanged);
    
    sampleLenSlider.addListener(this,&ofApp::sampleLenChanged);
    
	ofSetFrameRate(60);
}

void ofApp::exit()
{
    soundStream.stop();
    sampleLenSlider.removeListener(this,&ofApp::sampleLenChanged);
    channelSpacingSlider.removeListener(this,&ofApp::spacingChanged);
}

void ofApp::spacingChanged(float & spacing)
{
    for(int i = 0; i < OUT_CHANNEL_COUNT; i++)
    {
        outputBufferCurrentIndex[i] = spacing*delaySamples.size()*i*1.f/OUT_CHANNEL_COUNT;
    }
}

void ofApp::sampleLenChanged(int & sampleLenChanged)
{
    //stop the music!! so we don't hve threading problems.
    soundStream.stop();
    while(bufferInUse);
    delaySamples.resize(sampleLenChanged);
    for(int i = 0; i < OUT_CHANNEL_COUNT; i++)
    {
        outputBufferCurrentIndex[i] = channelSpacingSlider*delaySamples.size()*i*1.f/OUT_CHANNEL_COUNT;
    }
    delayMic1InsertionIndex = 0;
    delayMic2InsertionIndex = delaySamples.size()/2;
    soundStream.start();
}

//--------------------------------------------------------------
void ofApp::update()
{

}

//--------------------------------------------------------------
void ofApp::draw()
{
	ofSetColor(225);
	ofDrawBitmapString("press 's' to unpause the audio\npress 'e' to pause the audio", 31, 32);
	
	ofNoFill();
	
    int leftMargin = 32;
    int currentTop = 50;
    ofVec2f boxSz(900, 100);
    
    drawSoundrect("delaySamples", delaySamples,ofVec2f(leftMargin, currentTop),ofVec2f(900,600),ofColor(0,255,0));
    for(int j = 0; j < OUT_CHANNEL_COUNT; j++)
    {
        ofPushMatrix();
        float xPos = leftMargin+boxSz.x*outputBufferCurrentIndex[j]*1.f/delaySamples.size();
        ofTranslate(xPos, currentTop+600/2);
        ofEllipse(0,0, 50, 50);
        ofTranslate(-40,-40);
        ofDrawBitmapString("output#"+ofToString(j),0,0);
        ofPopMatrix();
    }
    ofSetColor(225,255,0);
    {
        ofPushMatrix();
        float xPos = leftMargin+boxSz.x*delayMic1InsertionIndex*1.f/delaySamples.size();
        ofTranslate(xPos, currentTop+600/2);
        ofEllipse(0,0, 50, 50);
        ofTranslate(-40,-40);
        ofDrawBitmapString("Input1",0,0);
        ofPopMatrix();
    }
    {
        ofPushMatrix();
        float xPos = leftMargin+boxSz.x*delayMic2InsertionIndex*1.f/delaySamples.size();
        ofTranslate(xPos, currentTop+600/2);
        ofEllipse(0,0, 50, 50);
        ofTranslate(-40,-40);
        ofDrawBitmapString("Input2",0,0);
        ofPopMatrix();
    }
    for(int i = 0; i < OUT_CHANNEL_COUNT; i++)
    {
        drawSoundrect("Channel["+ofToString(i)+"]", outputBuffers[i],ofVec2f(leftMargin, currentTop),boxSz);
        currentTop+=boxSz.y;
    }

    drawSoundrect("Input1", input1,ofVec2f(leftMargin, currentTop),boxSz);
    drawSoundrect("Input2", input2,ofVec2f(leftMargin, currentTop+=boxSz.y),boxSz);
    
    gui.draw();
}

void ofApp::drawSoundrect(string name, vector <float> samples,ofVec2f pos, ofVec2f sz,ofColor color)
{
    ofPushStyle();
    ofPushMatrix();
    ofTranslate(pos.x, pos.y, 0);
    
    ofSetColor(225);
    ofDrawBitmapString(name, 4, 18);
    
    ofSetLineWidth(1);
    ofRect(0, 0, sz.x, sz.y);
    ofSetColor(color);
    ofSetLineWidth(3);
    
    ofBeginShape();
    for (unsigned int i = 0; i < samples.size(); i++)
    {
        float x =  ofMap(i, 0, samples.size(), 0, sz.x, true);
        ofVertex(x, (sz.y/2) - samples[i]*sz.y*.9);
    }
    ofEndShape(false);
    
    ofPopMatrix();
	ofPopStyle();

}

//--------------------------------------------------------------
void ofApp::keyPressed  (int key)
{
	if( key == 's' )
    {
		soundStream.start();
	}
	
	if( key == 'e' )
    {
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

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button)
{

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button)
{
}


//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button)
{
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h)
{

}


void ofApp::compressor()
{
    
    float scaleAmt = 50;
    for (int i = 0; i < delaySamples.size();i++ )
    {
        
        {
            delaySamples[i] = delaySamples[i]*scaleAmt;
            delaySamples[i] = min(delaySamples[i],.2f);
            delaySamples[i] = max(delaySamples[i],-.2f);
//            delaySamples[i] = delaySamples[i]/scaleAmt;
        }
        
    }
}


void ofApp::bitcrusher()
{
    int stride = (int)(ofRandom(1, 5));

    for (int i = 0; i < delaySamples.size(); )
    {
        for (int j = 1; j < stride &&i+stride<delaySamples.size(); j++)
        {
            delaySamples[(i+j)%delaySamples.size()] = delaySamples[(i)%delaySamples.size()];
        }
        i+=stride;
        stride = (int)(ofRandom(1, 5));
    }
}

void ofApp::bitcrusher(int start, int end)
{
    int stride = (int)(ofRandom(1, 15));
    
    for (int i = start; i < end; )
    {
        for (int j = 1; j < stride &&i+stride<end; j++)
        {
            delaySamples[(i+j)%delaySamples.size()] = delaySamples[(i)%delaySamples.size()];
        }
        i+=stride;
//        stride = (int)(ofRandom(1, 15));
    }
}


//void ofApp::bitcrusher()
//{
//    
//}

void ofApp::fade()
{
    for (int i = 0; i < delaySamples.size(); i++)
    {
        delaySamples[i] *= sampleFadeSlider;
    }
}
void ofApp::fade(int start, int end)
{
    for (int i = start; i < end; i++)
    {
        delaySamples[i%delaySamples.size()] *= sampleFadeSlider;
    }
}

//--------------------------------------------------------------
void ofApp::audioIn(float * input, int bufferSize, int nChannels)
{
    bufferInUse = true;
//    for (int i = 0; i < delaySamples.size(); i++)
//    {
//        delaySamples[i] *= sampleFadeSlider;//.99;
////        delaySamples[i] += delaySamples[i-1]*.001;
////        delaySamples[i] += delaySamples[i+1]*.01;
//    }
    fade();
    float fluctd = 5*sin(ofGetElapsedTimef());
	//lets go through each sample and calculate the root mean square which is a rough way to calculate volume
    //interleaved samples
    bitcrusher(delayMic1InsertionIndex,delayMic1InsertionIndex+bufferSize);
//    compressor();
	for (int i = 0; i < bufferSize; i++)
    {
		input1[i] = input[i*nChannels]*0.5;
		input2[i] = input[i*nChannels+1]*0.5;
        
        int myIndex = (delayMic1InsertionIndex+i)%delaySamples.size();
        //add current left samples into delay buffer
        delaySamples[myIndex] += input[i*nChannels]*2;
//
////        float rndoff = ofRandom(1);
//        int tmp =(int)(delaySamples[myIndex]*fluctd);
////        delaySamples[myIndex] = (((int)(delaySamples[myIndex]*0xefffffff)/10000));
//        delaySamples[myIndex] = (tmp)/(1.f*fluctd);
        delaySamples[myIndex] = delaySamples[myIndex] + input[i*nChannels]*2;
        
        myIndex = (delayMic2InsertionIndex+i)%delaySamples.size();
        //add current left samples into delay buffer
        delaySamples[myIndex] += input[i*nChannels+1]*2;
	}
    bufferInUse = false;
	delayMic1InsertionIndex = (delayMic1InsertionIndex+bufferSize)%delaySamples.size();
    delayMic2InsertionIndex = (delayMic2InsertionIndex+bufferSize)%delaySamples.size();
}

//--------------------------------------------------------------
void ofApp::audioOut(float * output, int bufferSize, int nChannels)
{
//    for(int j = 0; j < OUT_CHANNEL_COUNT; j++)
//    {
//        ofLog(OF_LOG_ERROR, "cur index["+ofToString(j)+"]" + ofToString(outputBufferCurrentIndex[j]));
//    }
    bufferInUse = true;
    for (int i = 0; i < bufferSize; i++)
    {
        for(int j = 0; j < OUT_CHANNEL_COUNT; j++)
        {
            int myIndex = (outputBufferCurrentIndex[j]+i) % delaySamples.size();
            outputBuffers[j][i] = output[i*nChannels+j] = delaySamples[myIndex];
        }
    }
	
    for(int j = 0; j < OUT_CHANNEL_COUNT; j++)
    {
        outputBufferCurrentIndex[j]=(outputBufferCurrentIndex[j]+bufferSize) % delaySamples.size();
    }
    bufferInUse = false;
//    ofLog(OF_LOG_ERROR,"nChannels: " + ofToString(nChannels));
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
