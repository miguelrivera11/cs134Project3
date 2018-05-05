#include "Lander.h"

void Lander::init(string landerModelFile, float scale, ofVec3f pos) {
	if (!landerModel.loadModel(landerModelFile))
		cout << "did not load" <<endl;
	landerModel.setScaleNormalization(false);
	modelScale = scale;
	landerModel.setScale(scale, scale, scale);
	landerModel.setPosition(pos.x, pos.y, pos.z);
	ofVec3f min = (landerModel.getSceneMin() * modelScale) + landerModel.getPosition();
	ofVec3f max = (landerModel.getSceneMax() * modelScale) + landerModel.getPosition();
	boundingBox = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
	Particle particle;
	particle.lifespan = -1;
	particle.position.set(pos.x, pos.y, pos.z);
	particle.radius = 0.2;
	sys.add(particle);
	thrustForce = new ThrusterForce();
	GravityForce *gforce = new GravityForce(ofVec3f(0, -1.26, 0));
	sys.addForce(thrustForce);
	sys.addForce(gforce);
	mainExhaust.setEmitterType(DiscEmitter);
	mainExhaust.setGroupSize(200);
	mainExhaust.setRate(100);
	mainExhaust.setLifespan(1);
	mainExhaust.setVelocity(ofVec3f(0, 3, 0));
	mainExhaust.sys->addForce(gforce);

	leftExhaust.setEmitterType(DiscEmitter);
	leftExhaust.setGroupSize(200);
	leftExhaust.setRate(100);
	leftExhaust.setLifespan(1);
	leftExhaust.setVelocity(ofVec3f(0, 3, 0));
	leftExhaust.sys->addForce(gforce);

	rightExhaust.setEmitterType(DiscEmitter);
	rightExhaust.setGroupSize(200);
	rightExhaust.setRate(100);
	rightExhaust.setLifespan(1);
	rightExhaust.setVelocity(ofVec3f(0, 3, 0));
	rightExhaust.sys->addForce(gforce);

	frontExhaust.setEmitterType(DiscEmitter);
	frontExhaust.setGroupSize(200);
	frontExhaust.setRate(100);
	frontExhaust.setLifespan(1);
	frontExhaust.setVelocity(ofVec3f(0, 3, 0));
	frontExhaust.sys->addForce(gforce);
	thrustAmount = 0.1;
}

void Lander::draw() {
	landerModel.drawFaces();
}

void Lander::update() {
	sys.update();

	ofVec3f min = (landerModel.getSceneMin() * modelScale) + landerModel.getPosition();
	ofVec3f max = (landerModel.getSceneMax() * modelScale) + landerModel.getPosition();
	boundingBox = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));

	ofVec3f currentPos = sys.particles[0].position;
	landerModel.setPosition(currentPos.x, currentPos.y, currentPos.z);
	mainExhaust.setPosition(currentPos + mainExhaustOffset);
	mainExhaust.update();
	
	//update secondary exhaust positions
	vector<ofVec3f> secondaryExhaustPositions;
	getLanderBoxPointsofVec3f(secondaryExhaustPositions);
	leftExhaust.setPosition(secondaryExhaustPositions[0] + leftExhaustOffset);
	rightExhaust.setPosition(secondaryExhaustPositions[1] + rightExhaustOffset);
	frontExhaust.setPosition(secondaryExhaustPositions[2] + frontExhaustOffset);
	leftExhaust.update();
	rightExhaust.update();
	frontExhaust.update();
}

void Lander::stopThrust() {
	mainExhaust.stop();
	leftExhaust.stop();
	rightExhaust.stop();
	frontExhaust.stop();
	if (hasSound)
		thrustSound.stop();
}

void Lander::moveDown() {
	if (hasSound && !thrustSound.isPlaying())
		thrustSound.play();
	thrustForce->add(ofVec3f(0, -thrustAmount, 0));
}

void Lander::moveUp() {
	if (hasSound && !thrustSound.isPlaying())
		thrustSound.play();
	mainExhaust.start();
	leftExhaust.start();
	rightExhaust.start();
	frontExhaust.start();
	thrustForce->add(ofVec3f(0, thrustAmount, 0));
}

void Lander::moveLeft() {
	if (hasSound && !thrustSound.isPlaying())
		thrustSound.play();
	rightExhaust.start();
	thrustForce->add(ofVec3f(-thrustAmount, 0, 0));
}

void Lander::moveRight() {
	if (hasSound && !thrustSound.isPlaying())
		thrustSound.play();
	leftExhaust.start();
	thrustForce->add(ofVec3f(thrustAmount, 0, 0));
}

void Lander::moveForward() {
	if (hasSound && !thrustSound.isPlaying())
		thrustSound.play();
	leftExhaust.start();
	rightExhaust.start();
	thrustForce->add(ofVec3f(0, 0,thrustAmount));
}

void Lander::moveBack() {
	if (hasSound && !thrustSound.isPlaying())
		thrustSound.play();
	frontExhaust.start();
	thrustForce->add(ofVec3f(0,0,-thrustAmount));
}

void Lander::getLanderBoxPoints(std::vector<Vector3> &points)
{
	Vector3 min = boundingBox.min();
	Vector3 max = boundingBox.max();
	Vector3 point1 = min;
	Vector3 point2 = Vector3(max.x(), min.y(), min.z());
	Vector3 point3 = Vector3((max.x() - min.x()) / 2 + min.x(), min.y(), max.z());
	points.push_back(point1);
	points.push_back(point2);
	points.push_back(point3);
}

void Lander::getLanderBoxPointsofVec3f(std::vector<ofVec3f> &points)
{
	Vector3 min = boundingBox.min();
	Vector3 max = boundingBox.max();
	ofVec3f point1 = ofVec3f(min.x(), min.y(), min.z());
	ofVec3f point2 = ofVec3f(max.x(), min.y(), min.z());
	ofVec3f point3 = ofVec3f((max.x() - min.x()) / 2 + min.x(), min.y(), max.z());
	points.push_back(point1);
	points.push_back(point2);
	points.push_back(point3);
}

long Lander::numParticles() {
	return mainExhaust.sys->particles.size() + leftExhaust.sys->particles.size() + rightExhaust.sys->particles.size() + frontExhaust.sys->particles.size();
}