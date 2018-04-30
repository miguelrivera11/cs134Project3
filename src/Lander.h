
#include "ParticleEmitter.h"
#include  "ofxAssimpModelLoader.h"

class Lander {
public:
	void init(string landerModelFile, ofVec3f pos);
	ParticleSystem sys;
	ParticleEmitter exhaust;
	ofxAssimpModelLoader landerModel;
	void update();
	void draw();
	void setPosition(ofVec3f pos) {sys.particles[0].position = pos; }
	void setThrustMagnitude(float thrust) {thrustAmount = thrust; }
	void moveLeft();
	void moveRight();
	void moveUp();
	void moveDown();
	void stopThrust();
	ofVec3f position;
	ThrusterForce *thrustForce;
	float thrustAmount;
	bool thrusting;
};