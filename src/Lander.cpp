#include "Lander.h"

void Lander::init(string landerModelFile, ofVec3f pos) {
	landerModel.loadModel(landerModelFile);
	landerModel.setScaleNormalization(false);
	landerModel.setScale(1, 1, 1);
	landerModel.setPosition(pos.x, pos.y, pos.z);
	Particle particle;
	particle.lifespan = -1;
	particle.position.set(pos.x, pos.y, pos.z);
	particle.radius = 0.2;
	sys.add(particle);
	thrustForce = new ThrusterForce();
	GravityForce *gforce = new GravityForce(ofVec3f(0, -1.26, 0));
	sys.addForce(thrustForce);
	sys.addForce(gforce);
	exhaust.setEmitterType(DiscEmitter);
	exhaust.setGroupSize(200);
	exhaust.setRate(100);
	exhaust.setLifespan(1.5);
	exhaust.setVelocity(ofVec3f(0, 1.2, 0));
	exhaust.sys->addForce(gforce);
	thrustAmount = 0.1;
}

void Lander::draw() {
	landerModel.drawFaces();
	exhaust.draw();
}

void Lander::update() {
	sys.update();
	ofVec3f currentPos = sys.particles[0].position;
	landerModel.setPosition(currentPos.x, currentPos.y, currentPos.z);
	exhaust.setPosition(ofVec3f(currentPos.x, currentPos.y, currentPos.z));
	exhaust.update();
}

void Lander::stopThrust() {
	exhaust.stop();
}

void Lander::moveDown() {
	exhaust.start();
	thrustForce->add(ofVec3f(0, -thrustAmount, 0));
}

void Lander::moveUp() {
	exhaust.start();
	thrustForce->add(ofVec3f(0, thrustAmount, 0));
}

void Lander::moveLeft() {
	exhaust.start();
	thrustForce->add(ofVec3f(-thrustAmount, 0, 0));
}

void Lander::moveRight() {
	exhaust.start();
	thrustForce->add(ofVec3f(thrustAmount, 0, 0));
}