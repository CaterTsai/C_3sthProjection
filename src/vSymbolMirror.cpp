#include "vSymbolMirror.h"
#include "config.h"

//--------------------------------------------------------------
void vSymbolMirror::setup(int width, int height)
{
	bool result = true;
	_mb.setup();
	result &= initSymbol();
	initLayerMask(width, height);
	result &= load("mirror");

	if (result)
	{
		reset();
	}
	_isSetup = result;
}

//--------------------------------------------------------------
void vSymbolMirror::update(float delta)
{
	if (!_isSetup)
	{
		return;
	}

	_animMirrorAlpha.update(delta);
	_animSymbolAlpha.update(delta);
	updateMirror(delta);
	if (_isStart)
	{
		_mb.update(delta);
		updateSymbol(delta);
		updateToMirror();
		_mirrorContext.beginMask();
		ofSetColor(255);
		_mask.draw(0, 0, cSymbolRect.width, cSymbolRect.height);

		_mirrorContext.endMask();
	}
}

//--------------------------------------------------------------
void vSymbolMirror::draw(ofVec3f pos)
{
	if (!_isSetup)
	{
		return;
	}

	drawMirror(pos);

	if (_isStart)
	{
		drawSymbol(pos);
	}
}

//--------------------------------------------------------------
void vSymbolMirror::debugUpdate(float delta)
{
	_mb.update(delta);
	updateSymbol(delta);
	updateToMirror();
	_mirrorContext.beginMask();
	ofSetColor(255);
	_mask.draw(0, 0, cSymbolRect.width, cSymbolRect.height);

	_mirrorContext.endMask();
}

//--------------------------------------------------------------
void vSymbolMirror::debugDraw()
{	
	ofPushMatrix();
	
	ofScale(0.5, 0.5);
	ofPushStyle();
	ofSetColor(255);
	//_mirror.draw();
	_mirrorContext.draw();
	//_mb.draw();
	ofPopStyle();
	ofPopMatrix();
}

//--------------------------------------------------------------
void vSymbolMirror::reset()
{
	_animMirrorAlpha.reset(0);
	_animSymbolAlpha.reset(0);
	resetMirror();
	resetSymbol();
	
}

//--------------------------------------------------------------
void vSymbolMirror::start()
{
	_isStart = true;
}

//--------------------------------------------------------------
void vSymbolMirror::stop()
{
	_isStart = false;
	_isFinish = false;
}

//--------------------------------------------------------------
void vSymbolMirror::displayMirror()
{
	_animMirrorAlpha.setDuration(config::getInstance()->_symbolChangeT);
	_animMirrorAlpha.animateFromTo(0.0, (config::getInstance()->_symbolMirrorAlpha / 255.0f));
}

//--------------------------------------------------------------
void vSymbolMirror::hideMirror()
{
	_animMirrorAlpha.setDuration(config::getInstance()->_symbolChangeT);
	_animMirrorAlpha.animateTo(0.0);
}

//--------------------------------------------------------------
void vSymbolMirror::displayContent()
{
	_animSymbolAlpha.setDuration(2.0f);
	_animSymbolAlpha.animateFromTo(0.0, 255);
}

//--------------------------------------------------------------
void vSymbolMirror::setIsLooping(bool isLoop)
{
	_isLoop = isLoop;
}

//--------------------------------------------------------------
void vSymbolMirror::initLayerMask(int width, int height)
{
	_mirrorContext.setup(width, height);
	_mirrorContext.newLayer();
}

#pragma region Symbol & Metaball
//--------------------------------------------------------------
void vSymbolMirror::addMetaball(int num)
{
	_mb.add(num);
}

//--------------------------------------------------------------
void vSymbolMirror::removeMetaball(int num)
{
	_mb.remove(num);
}

//--------------------------------------------------------------
void vSymbolMirror::onGetPattern(string & symbol64)
{
	stringstream ss;
	ss << symbol64;
	Poco::Base64Decoder decoder(ss);

	ofBuffer buffer;
	decoder >> buffer;

	ofImage image;
	image.load(buffer);
	string path = ofGetTimestampString("symbol/%m%d.png");
	image.saveImage(path);
	ofLog(OF_LOG_NOTICE, "[onGetPattern]Get new Symbol and save it");
	
	ofRemoveListener(serverReq::getInstance()->newPattern, this, &vSymbolMirror::onGetPattern);
	loadSymbol();
	_isSetup = true;
	reset();
}

//--------------------------------------------------------------
bool vSymbolMirror::initSymbol()
{
	float fullSize = config::getInstance()->_exSymbolUnitSize * (cMetaAiSymbolSize - 1);

	_symbolDisplay.setup(fullSize, 100, config::getInstance()->_exSymbolConnMin, config::getInstance()->_exSymbolConnMax);

	string path = ofGetTimestampString("symbol/%m%d.png");
	ofFile f(path);

	if (f.exists())
	{
		loadSymbol();
		return true;
	}
	else
	{
		ofAddListener(serverReq::getInstance()->newPattern, this, &vSymbolMirror::onGetPattern);
		serverReq::getInstance()->reqPattern();
		return false;
	}
}

//--------------------------------------------------------------
void vSymbolMirror::loadSymbol()
{
	ofDirectory dir("symbol");
	dir.allowExt("png");
	auto size = dir.listDir();

	_symbolList.resize(size);
	for (int i = 0; i < size; i++)
	{
		_symbolList[i].load(dir.getPath(i));
	}
	_symbolDisplay.setSymbol(_symbolList[0]);
	_symbolIndex = 0;
	_translateTimer = config::getInstance()->_symbolChangeT;

}

//--------------------------------------------------------------
void vSymbolMirror::updateSymbol(float delta)
{
	_symbolDisplay.update(delta);
	_translateTimer -= delta;

	if (_translateTimer <= 0.0f && !_isFinish)
	{
		int nextIndex = _symbolIndex + 1;
		if (!_isLoop && nextIndex >= _symbolList.size())
		{
			_isFinish = true;
			ofNotifyEvent(_symbolPlayFinish, this);
		}
		else
		{
			nextIndex = nextIndex % _symbolList.size();
			if (_symbolDisplay.toSymbol(_symbolList[nextIndex], config::getInstance()->_symbolChangeT))
			{
				_translateTimer = config::getInstance()->_symbolChangeT;
				_symbolIndex = nextIndex;
			}
		}
	}
}

//--------------------------------------------------------------
void vSymbolMirror::updateToMirror()
{
	_mirrorContext.beginLayer();
	ofPushStyle();
	ofClear(0);
	ofSetColor(255);
	ofPushMatrix();
	ofTranslate(_mirrorContext.getWidth() * 0.5f, _mirrorContext.getHeight() * 0.55f);
	{
		_symbolDisplay.draw();
	}
	ofPopMatrix();
	_mb.draw();
	ofPopStyle();

	_mirrorContext.endLayer();
}

//--------------------------------------------------------------
void vSymbolMirror::drawSymbol(ofVec3f pos)
{
	ofPushMatrix();
	ofTranslate(pos);
	ofRotateX(90);
	ofPushStyle();
	{
		ofSetColor(255);
		_mirrorContext.draw(_mirrorContext.getWidth() * -0.5f, _mirrorContext.getHeight() * -0.5f);
	}
	ofPopStyle();
	ofPopMatrix();
}

//--------------------------------------------------------------
void vSymbolMirror::resetSymbol()
{
	_symbolDisplay.setSymbol(_symbolList[0]);
	_symbolIndex = 0;
	_translateTimer = config::getInstance()->_symbolChangeT;
}

#pragma endregion

#pragma region Mirror & Mask
//--------------------------------------------------------------
bool vSymbolMirror::load(string name)
{
	if (!_mask.load("image/" + name + "_mask.jpg"))
	{
		ofLog(OF_LOG_ERROR, "[vSymbolMirror::load]load mask faield");
		return false;
	}

	if (!loadMirror(name))
	{
		return false;
	}
	return true;
}

//--------------------------------------------------------------
bool vSymbolMirror::loadMirror(string name)
{
	ofImage mirror;
	if (!mirror.load("image/" + name + ".png"))
	{
		ofLog(OF_LOG_ERROR, "[vSymbolMirror::loadMirror]load mirror image faield");
		return false;
	}

	float centerX = mirror.getWidth() / 2;
	float centerY = mirror.getHeight() / 2;
	for (int y = 0; y < mirror.getHeight(); y += 2) {
		for (int x = 0; x < mirror.getWidth(); x += 2) {
			ofColor c = mirror.getColor(x, y);
			int brightness = c.getBrightness();
			//filter the point which it's alpha > 20
			if (c.a > cImg2MeshAlpahT)
			{
				c.a = 0.0f;
				ofVec3f vertex = ofVec3f((x - centerX) * 4, (y - centerY) * 4, float(brightness) / 255.0 * -50 + 25);
				_mirror.addVertex(vertex);
				_mirror.addColor(c);
			}
		}
	}
	_mirror.setMode(ofPrimitiveMode::OF_PRIMITIVE_POINTS);
	return true;
}

//--------------------------------------------------------------
void vSymbolMirror::updateMirror(float delta)
{
	if (!_animMirrorAlpha.isAnimating())
	{
		return;
	}

	for (int i = 0; i < _mirror.getNumColors(); i++)
	{
		auto color = _mirror.getColor(i);
		color.a = _animMirrorAlpha.getCurrentValue();
		_mirror.setColor(i, color);
	}
}

//--------------------------------------------------------------
void vSymbolMirror::resetMirror()
{
	for (int i = 0; i < _mirror.getNumColors(); i++)
	{
		auto color = _mirror.getColor(i);
		color.a = 0.0f;
		_mirror.setColor(i, color);
	}
}

//--------------------------------------------------------------
void vSymbolMirror::drawMirror(ofVec3f pos)
{
	ofPushMatrix();
	ofTranslate(pos);
	ofRotateX(-90);
	ofPushStyle();
	{
		_mirror.draw();
	}
	ofPopStyle();
	ofPopMatrix();
}
#pragma endregion


