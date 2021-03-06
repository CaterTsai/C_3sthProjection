#pragma once

#include "constParameter.h"

class threeBody
{
#pragma region CLASS stBody
private:
	class stBody
	{
	public:
		stBody()
			:_mass(cDefaultMass)
		{}

		float dist(stBody p)
		{
			return _pos.distance(p._pos);
		}

		void update(ofVec3f& acc, ofVec3f& att, ofVec3f& flee)
		{
			_vel += acc.limit(0.1) + att.limit(0.5) + att.limit(0.1);
			_pos += _vel.limit(50.0f);
		}

		void draw()
		{
			ofSetColor(_color);
			ofDrawSphere(_pos, cBodyBallSize);
		}

		void backup()
		{
			_velBackup = _vel;
			_posBackup = _pos;
		}

		void recover()
		{
			_vel = _velBackup;
			_pos = _posBackup;
		}
	public:
		int _mass;
		ofVec3f _vel, _pos;
		ofVec3f _velBackup, _posBackup;
		ofColor _color;

	};
#pragma endregion
	
public:
	threeBody()
		:_isSetup(false)
		,_timer(cTBUpdateTime)
		, _meshTimer(cTBAddMeshTime)
	{}
	void setup(int startStep = 0);
	void update(float delta);
	void draw();
	void reset();


public:
	void onAlphaChange(int& alpha);

private:
	void oneStep();
	void updateMesh();
	ofVec3f getAttFouce(stBody& body, int totalMass);
	void fleeBetween(stBody& b1, stBody& b2, float dist, ofVec3f& flee1, ofVec3f& flee2);
	ofVec3f getAccForce(stBody& b1, ofVec3f diff1, ofVec3f diff2, float f1, float f2, float d1, float d2);
	
private:
	bool _isSetup;
	float _timer, _meshTimer;
	stBody _bodyA, _bodyB, _bodyC;
	ofVboMesh _mesh; //the mesh of three body movement

};