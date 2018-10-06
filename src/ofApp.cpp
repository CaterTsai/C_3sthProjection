#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {

	setupViewer();
	_multiCam.setup(0, 0, 1920, 180);
	_multiCam.updateParent(_viewCam.getCam());

	//_kinectMgr.setup();
	//_cam.setVFlip(true);
	ofSetSmoothLighting(true);

	ofBackground(50);
	_timer = ofGetElapsedTimef();
}

//--------------------------------------------------------------
void ofApp::update() {
	float delta = ofGetElapsedTimef() - _timer;
	_timer += delta;

	
	updateViewer(delta);
	_multiCam.update(delta);
	_multiCam.updateParent(_viewCam.getCam());
	
	flowField::getInstance()->update(delta);
	ofSetWindowTitle(ofToString(ofGetFrameRate()));
}

//--------------------------------------------------------------
void ofApp::draw() {
	
	ofSetDepthTest(true);
	for (int i = 0; i < _multiCam.getCamNum(); i++)
	{
		_multiCam.begin(i);
		{
			drawViewer();
		}
		_multiCam.end();
	}

	debugDraw();
	_multiCam.draw();
	ofSetDepthTest(false);
}

//--------------------------------------------------------------
void ofApp::debugDraw()
{
	_cam.begin();
	_viewArms.draw(_armsPos);
	_viewThreeBody.draw(_threeBodyPos);
	_viewSymbol.draw(_symbolPos);
	_viewParticle.draw(_particlePos);
	_viewCam.drawCamera();
	_multiCam.drawCam();
	_cam.end();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {

	switch (key)
	{
	case 's':
	{
		_viewCam.start();
		_viewArms.start();
		break;
	}
	case 't':
	{
		_viewParticle.start(100);
		break;
	}
	}
}

//--------------------------------------------------------------
void ofApp::onViewerChange(eViewState & nowState)
{
	switch (nowState)
	{
	case eViewArms:
	{
		break;
	}
	case eArmsToThreeBody:
	{
		break;
	}
	case eViewThreeBody:
	{
		_viewSymbol.displayMirror();
		_viewThreeBody.start();

		_viewArms.stop();
		_viewArms.setStage(false);
		break;
	}
	case eViewThreeBodyAndSymbol:
	{
		_viewSymbol.start();
		break;
	}
	case eViewSymbol:
	{
		_viewThreeBody.stop();
		break;
	}
	case eSymbolToArms:
	{
		break;
	}
	}
}

//--------------------------------------------------------------
void ofApp::setupViewer()
{
	_viewArms.setup();
	_viewThreeBody.setup();
	_viewSymbol.setup(cMetaballRect.getWidth(), cMetaballRect.getHeight());

	_armsPos.set(0);
	_threeBodyPos.set(cThreeBodyPos);
	_symbolPos.set(cSymbolPos);
	_particlePos.set(cParticlePos);

	_viewArms.setStage(true);

	ofAddListener(_viewCam._onViewStateChange, this, &ofApp::onViewerChange);
}

//--------------------------------------------------------------
void ofApp::updateViewer(float delta)
{
	_viewArms.update(delta);
	_viewThreeBody.update(delta);
	_viewSymbol.update(delta);
	_viewParticle.update(delta);
	_viewCam.update(delta);
}

//--------------------------------------------------------------
void ofApp::drawViewer()
{	
	ofPushStyle();
	glPointSize(2.0f);
	_viewArms.draw(_armsPos);
	_viewThreeBody.draw(_threeBodyPos);
	_viewSymbol.draw(_symbolPos);
	ofPopStyle();
}
