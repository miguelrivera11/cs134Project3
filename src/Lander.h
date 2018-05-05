
#include "ParticleEmitter.h"
#include  "ofxAssimpModelLoader.h"
#include "box.h"

class Lander {
public:
	void init(string landerModelFile, float scale, ofVec3f pos);
	ParticleSystem sys;
	ParticleEmitter mainExhaust;
	ParticleEmitter leftExhaust;
	ParticleEmitter rightExhaust;
	ParticleEmitter frontExhaust;
	ofxAssimpModelLoader landerModel;
	void update();
	void draw();
	void setPosition(ofVec3f pos) {sys.particles[0].position = pos; }
	void setThrustMagnitude(float thrust) {thrustAmount = thrust; }
	void moveLeft();
	void moveRight();
	void getLanderBoxPoints(std::vector<Vector3>& points);
	void getLanderBoxPointsofVec3f(std::vector<ofVec3f>& points);
	void moveUp();
	void moveDown();
	void moveForward();
	void moveBack();
	void stopThrust();
	void setMainExhaustOffset(ofVec3f offset) { mainExhaustOffset = offset;}
	void setLeftExhaustOffset(ofVec3f offset) { leftExhaustOffset = offset; }
	void setRightExhaustOffset(ofVec3f offset) { rightExhaustOffset = offset; }
	void setFrontExhaustOffset(ofVec3f offset) { frontExhaustOffset = offset; }
	void setThrustSound(string soundFile) { hasSound = true; thrustSound.load(soundFile);};
	long numParticles();
	ofVec3f position;
	ThrusterForce *thrustForce;
	float thrustAmount;
	ofVec3f mainExhaustOffset;
	ofVec3f leftExhaustOffset;
	ofVec3f rightExhaustOffset;
	ofVec3f frontExhaustOffset;
	bool thrusting;
	Box boundingBox;
	float modelScale;
	ofSoundPlayer thrustSound;
	bool hasSound = false;
};