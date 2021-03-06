#include "ofApp.h"
#include "Util.h"
/*
* ARROW KEYS = MOVE IN X AND Y
* W and S = MOVE IN Z 
* HUKJ = CAMERA MOVE
* YI = CAMERA ZOOM
* CAMERA MODES:
* 1 = SIDE VIEW 1
* 2 = SIDE VIEW 2
* 3 = ONBOARD FROM SIDE
* 4 = ONBOARD FROM BOTTOM
* key light follows the ship
*/

//--------------------------------------------------------------
void ofApp::setup(){

	background.loadImage("images/star_background.png");

	bWireframe = false;
	bDisplayPoints = false;
	bAltKeyDown = false;
	bCtrlKeyDown = false;
	bRoverLoaded = false;
	bTerrainSelected = true;
	//	ofSetWindowShape(1024, 768);
	cam.setDistance(10);
	cam.setNearClip(.1);
	cam.setFov(65.5);   // approx equivalent to 28mm in 35mm format
	camMode = 1;
	cam.setPosition(cam1);
	ofSetVerticalSync(true);
	//cam.disableMouseInput();
	ofEnableSmoothing();
	ofEnableDepthTest();
	//texture loading
	ofDisableArbTex();
	if (!ofLoadImage(particleTex, "images/dot.png")) {
		cout << "Image file not found" << endl;
		ofExit();
	}

	//load the shader 
#ifdef  TARGET_OPENGLES
	shader.load("shaders_gles/shader");
#else
	shader.load("shaders/shader");
#endif 


	// setup rudimentary lighting 
	//
	initLightingAndMaterials();

	//setup models and octree
	surface.loadModel("geo/moon-houdini.obj");
	surface.setScaleNormalization(false);
	LANDER_SCALE = 0.04;
	lander.init("geo/retro_rocket.obj", LANDER_SCALE, ofVec3f (0, 100, 0));
	lander.setMainExhaustOffset(ofVec3f(0, -9, 0));
	lander.setLeftExhaustOffset(ofVec3f(0.9, 0, 0.9));
	lander.setRightExhaustOffset(ofVec3f(-0.9, 0, 0.9));
	lander.setFrontExhaustOffset(ofVec3f(0, 0, -0.9));
	lander.mainExhaust.radius = 0.9;
	lander.leftExhaust.radius = 0.4;
	lander.rightExhaust.radius = 0.4;
	lander.frontExhaust.radius = 0.4;
	lander.setThrustSound("sounds/rocket.wav");
	surfaceMesh = surface.getMesh(0);
	octreeHead.box = meshBounds(surfaceMesh);
	int numofVerticies = surfaceMesh.getNumVertices();
	for (int i = 0; i < numofVerticies; i++)
		octreeHead.indicies.push_back(i);
	createOctree(&octreeHead);
	impulse = new ImpulseForce();
	lander.sys.addForce(impulse);

	ofEnableLighting();

	// Setup 3 - Light System
	// square shaped
	keyLight.setup();
	keyLight.enable();
	//keyLight.setDirectional();
	keyLight.setPointLight();
	keyLight.setAreaLight(1, 1);
	keyLight.setAmbientColor(ofColor(216, 41, 79));
	keyLight.setDiffuseColor(ofColor(69, 22, 127));
	keyLight.setSpecularColor(ofColor(69, 22, 127));

	//keyLight.rotate(45, ofVec3f(0, 1, 0));
	//keyLight.rotate(-45, ofVec3f(1, 0, 0));
	keyLight.setPosition(cam1);
	keyLight.lookAt(lander.sys.particles[0].position);

	fillLight.setup();
	fillLight.enable();
	fillLight.setSpotlight();
	fillLight.setScale(0.7);
	fillLight.setSpotlightCutOff(50);
	fillLight.setAttenuation(2, 0, 0);
	fillLight.setAmbientColor(ofColor(69, 22, 127));
	fillLight.setDiffuseColor(ofColor(19, 178, 69));
	fillLight.setSpecularColor(ofColor(19, 178, 69));
	fillLight.rotate(-10, ofVec3f(1, 0, 0));
	fillLight.rotate(-45, ofVec3f(0, 1, 0));
	fillLight.setPosition(100, 200, 100);
	fillLight.lookAt(lander.sys.particles[0].position);


	rimLight.setup();
	rimLight.enable();
	//rimLight.setSpotlight();
	rimLight.setScale(.05);
	//rimLight.setSpotlightCutOff(50);
	rimLight.setAttenuation(.2, .1, .1);
	rimLight.setAmbientColor(ofColor(216, 41, 79));
	rimLight.setDiffuseColor(ofColor(216, 41, 79));
	rimLight.setSpecularColor(ofColor(216, 41, 79));
	rimLight.rotate(180, ofVec3f(0, 1, 0));
	rimLight.setPosition(0, 60, -70);
}

//--------------------------------------------------------------
void ofApp::update(){
	lander.update();
	checkCollisions();
	keyLight.lookAt(lander.sys.particles[0].position);
	fillLight.lookAt(lander.sys.particles[0].position);
	cout << std::to_string(ofGetFrameRate()) <<endl;

	//POSITIONING CAMERA BASED ON MODE
	if (camMode < 3) cam.lookAt(lander.sys.particles[0].position);
	else if (camMode == 3) {
		//cam.setPosition(lander.sys.particles[0].position-ofVec3f(lander.boundingBox.max().x()/2)); //TODO: add 1/2 size of lander
		cam.setPosition(lander.sys.particles[0].position + ofVec3f(0, 0, lander.boundingBox.max().z() / 2));
		cam.lookAt(ofVec3f(cam.getPosition().x, cam.getPosition().y, cam.getPosition().z * 2));
	}
	else if (camMode == 4) {
		cam.setPosition(lander.sys.particles[0].position - ofVec3f(0, lander.boundingBox.max().y() / 2)); //TODO: add 1/2 size of lander and watch out when the particle's position is less than 0
		cam.lookAt(ofVec3f(cam.getPosition().x, cam.getPosition().y - 10, cam.getPosition().z));
	}
}

//--------------------------------------------------------------
void ofApp::draw(){

	//	ofBackgroundGradient(ofColor(20), ofColor(0));   // pick your own backgroujnd
	ofBackground(ofColor::black);
	//	cout << ofGetFrameRate() << endl;
	loadVbo();

	//background here


	ofSetColor(255, 255, 255);
	ofDisableDepthTest();

	background.draw(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
	ofEnableDepthTest();




	cam.begin();
	ofPushMatrix();
	if (bWireframe) {                    // wireframe mode  (include axis)
		ofDisableLighting();
		ofSetColor(ofColor::slateGray);
		surface.drawWireframe();
		//if (bTerrainSelected) drawAxis(ofVec3f(0, 0, 0));
	}
	else {
		ofEnableLighting();              // shaded mode
		surface.drawFaces();
		//if (bTerrainSelected) drawAxis(ofVec3f(0, 0, 0));
	}


	if (bDisplayPoints) {                // display points as an option    
		glPointSize(3);
		ofSetColor(ofColor::green);
		surface.drawVertices();
	}
	lander.draw();
	ofPopMatrix();
	glDepthMask(GL_FALSE);
	ofEnableBlendMode(OF_BLENDMODE_ADD);
	ofEnablePointSprites();
	shader.begin();
	particleTex.bind();
	ofSetColor(255, 100, 90);
	vbo.draw(GL_POINTS, 0, (int)lander.numParticles());
	particleTex.unbind();
	shader.end();
	ofDisablePointSprites();
	ofDisableBlendMode();
	ofEnableAlphaBlending();
	glDepthMask(GL_TRUE);
	//keyLight.draw();
	//fillLight.draw();
	//rimLight.draw();
	cam.end();
	string altitudeText;
	altitudeText += "Altitude: " + std::to_string(altitude);
	ofDrawBitmapString(altitudeText, ofPoint(10, 20));
}
// Draw an XYZ axis in RGB at world (0,0,0) for reference.
//
void ofApp::drawAxis(ofVec3f location) {

	ofPushMatrix();
	ofTranslate(location);

	ofSetLineWidth(1.0);

	// X Axis
	ofSetColor(ofColor(255, 0, 0));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(1, 0, 0));


	// Y Axis
	ofSetColor(ofColor(0, 255, 0));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(0, 1, 0));

	// Z Axis
	ofSetColor(ofColor(0, 0, 255));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(0, 0, 1));

	ofPopMatrix();
}


void ofApp::keyPressed(int key) {

	switch (key) {
	case 'C':
	case 'c':
		if (cam.getMouseInputEnabled()) cam.disableMouseInput();
		else cam.enableMouseInput();
		break;
	case 'F':
	case 'f':
		ofToggleFullscreen();
		break;
	case 'r':
		//cam.reset();
		resetRocket();
		break;
	case 's':
		lander.moveBack();
		break;
	case 't':
		setCameraTarget();
		break;
	case 'v':
		togglePointsDisplay();
		break;
	case 'V':
		break;
	case 'w':
		lander.moveForward();
		break;
	case OF_KEY_ALT:
		cam.enableMouseInput();
		bAltKeyDown = true;
		break;
	case OF_KEY_CONTROL:
		bCtrlKeyDown = true;
		break;
	case OF_KEY_SHIFT:
		break;
	case OF_KEY_DEL:
		break;
	case OF_KEY_DOWN:
		lander.moveDown();
		break;
	case OF_KEY_UP:
		lander.moveUp();
		break;
	case OF_KEY_LEFT:
		lander.moveLeft();
		break;
	case OF_KEY_RIGHT:
		lander.moveRight();
		break;
	case 'u':
		if (camMode == 2)
			cam.setPosition(cam.getPosition() + ofVec3f(0, 1, 0));
		break;
	case 'h':
		if (camMode == 2)
			cam.setPosition(cam.getPosition() + ofVec3f(-1, 0, 0));
		break;
	case 'j':
		if (camMode == 2)
			cam.setPosition(cam.getPosition() + ofVec3f(0, -1, 0));
		break;
	case 'k':
		if (camMode == 2)
			cam.setPosition(cam.getPosition() + ofVec3f(1, 0, 0));
		break;
	case 'y':
		if (camMode == 2)
			cam.setPosition(cam.getPosition() + ofVec3f(0, 0, 1));
		break;
	case 'i':
		if (camMode == 2)
			cam.setPosition(cam.getPosition() + ofVec3f(0, 0, -1));
		break;
	case '1':
		cam.setPosition(cam1);
		camMode = 1;
		break;
	case '2':
		cam.setPosition(cam2);
		camMode = 2;
		break;
	case '3':
		camMode = 3;
		break;
	case '4':
		camMode = 4;
		break;
	default:
		break;
	}
}

void ofApp::toggleWireframeMode() {
	bWireframe = !bWireframe;
}

void ofApp::toggleSelectTerrain() {
	bTerrainSelected = !bTerrainSelected;
}

void ofApp::togglePointsDisplay() {
	bDisplayPoints = !bDisplayPoints;
}

void ofApp::keyReleased(int key) {

	switch (key) {

	case OF_KEY_ALT:
		cam.disableMouseInput();
		bAltKeyDown = false;
		break;
	case OF_KEY_CONTROL:
		bCtrlKeyDown = false;
		break;
	case OF_KEY_SHIFT:
		break;
	default:
		break;

	}
	lander.stopThrust();
}



//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {

}


//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}


//--------------------------------------------------------------
// setup basic ambient lighting in GL  (for now, enable just 1 light)
//
void ofApp::initLightingAndMaterials() {

	static float ambient[] =
	{ .5f, .5f, .5, 1.0f };
	static float diffuse[] =
	{ 1.0f, 1.0f, 1.0f, 1.0f };

	static float position[] =
	{ 5.0, 5.0, 5.0, 0.0 };

	static float lmodel_ambient[] =
	{ 1.0f, 1.0f, 1.0f, 1.0f };

	static float lmodel_twoside[] =
	{ GL_TRUE };


	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, position);

	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, position);


	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
	glLightModelfv(GL_LIGHT_MODEL_TWO_SIDE, lmodel_twoside);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	//	glEnable(GL_LIGHT1);
	glShadeModel(GL_SMOOTH);
}

void ofApp::savePicture() {
	ofImage picture;
	picture.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
	picture.save("screenshot.png");
	cout << "picture saved" << endl;
}


bool ofApp::mouseIntersectPlane(ofVec3f planePoint, ofVec3f planeNorm, ofVec3f &point) {
	ofVec2f mouse(mouseX, mouseY);
	ofVec3f rayPoint = cam.screenToWorld(mouse);
	ofVec3f rayDir = rayPoint - cam.getPosition();
	rayDir.normalize();
	return (rayIntersectPlane(rayPoint, rayDir, planePoint, planeNorm, point));
}
// Set the camera to use the selected point as it's new target
//  
void ofApp::setCameraTarget() {

}

// return a Mesh Bounding Box for the entire Mesh
//
Box ofApp::meshBounds(const ofMesh & mesh) {
	int n = mesh.getNumVertices();
	ofVec3f v = mesh.getVertex(0);
	ofVec3f max = v;
	ofVec3f min = v;
	for (int i = 1; i < n; i++) {
		ofVec3f v = mesh.getVertex(i);

		if (v.x > max.x) max.x = v.x;
		else if (v.x < min.x) min.x = v.x;

		if (v.y > max.y) max.y = v.y;
		else if (v.y < min.y) min.y = v.y;

		if (v.z > max.z) max.z = v.z;
		else if (v.z < min.z) min.z = v.z;
	}
	return Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
}

//Creates an octree
//
void ofApp::createOctree(Node *node) {
	vector<Box> children;
	subDivideBox8(node->box, children);
	for (int i = 0; i < children.size(); i++) {
		Node *child = new Node();
		child->box = children[i];
		if (checkPoints(node->indicies, child->indicies, child->box)) {
			node->children.push_back(*child);
		}
		delete child;
	}
	for (int i = 0; i < node->children.size(); i++) {
		Node *child = &node->children[i];
		if (child->indicies.size() > 1) {
			createOctree(child);
		}
	}
}

//Adds indicies to child's indicies list. Returns true if at least one point was added
bool ofApp::checkPoints(vector<int> indicies, vector<int> &childIndicies, Box box) {
	bool hasPoint = false;
	for (int i = 0; i < indicies.size(); i++) {
		ofVec3f vertex = surfaceMesh.getVertex(indicies[i]);
		Vector3 point = Vector3(vertex.x, vertex.y, vertex.z);
		if (box.hasPointWithin(point)) {
			hasPoint = true;
			childIndicies.push_back(indicies[i]);
		}

	}
	return hasPoint;
}

bool ofApp::checkCollisions() {
	vector<Vector3> points;
	lander.getLanderBoxPoints(points);
	float drag = 0.5;

	for (int i = 0; i < points.size(); i++) {
		Vector3 rayPoint = points[i];
		Vector3 rayDir = Vector3(0, -1, 0);
		rayDir.normalize();
		Ray ray = Ray(rayPoint, rayDir);
		vector<int> indicies;
		rayCastSelection(indicies, &octreeHead, ray);
		if (indicies.size() > 0) {
			ofVec3f collisionPoint = surfaceMesh.getVertex(indicies[0]);
			altitude = lander.boundingBox.min().y()-collisionPoint.y;
			float epsilon = lander.sys.particles[0].velocity.length() * (1.0 / ofGetFrameRate());
			ofVec3f vel = lander.sys.particles[0].velocity;
			if (altitude <= epsilon && vel.y < 0) {
				float restitution = 0.2;
				ofVec3f normal = surfaceMesh.getNormal(indicies[0]);
				ofVec3f impulseForce = ((-vel).dot(normal)) * normal;
				impulse->apply(impulseForce*(ofGetFrameRate() * (10 * restitution)));
			}
		}
	}
	return true;
}


//  Subdivide a Box into eight(8) equal size boxes, return them in boxList;
//
void ofApp::subDivideBox8(const Box &box, vector<Box> & boxList) {
	Vector3 min = box.parameters[0];
	Vector3 max = box.parameters[1];
	Vector3 size = max - min;
	Vector3 center = size / 2 + min;
	float xdist = (max.x() - min.x()) / 2;
	float ydist = (max.y() - min.y()) / 2;
	float zdist = (max.z() - min.z()) / 2;
	Vector3 h = Vector3(0, ydist, 0);

	//  generate ground floor
	//
	Box b[8];
	b[0] = Box(min, center);
	b[1] = Box(b[0].min() + Vector3(xdist, 0, 0), b[0].max() + Vector3(xdist, 0, 0));
	b[2] = Box(b[1].min() + Vector3(0, 0, zdist), b[1].max() + Vector3(0, 0, zdist));
	b[3] = Box(b[2].min() + Vector3(-xdist, 0, 0), b[2].max() + Vector3(-xdist, 0, 0));

	boxList.clear();
	for (int i = 0; i < 4; i++)
		boxList.push_back(b[i]);

	// generate second story
	//
	for (int i = 4; i < 8; i++) {
		b[i] = Box(b[i - 4].min() + h, b[i - 4].max() + h);
		boxList.push_back(b[i]);
	}
}

//draw a box from a "Box" class  
//
void ofApp::drawBox(const Box &box) {
	Vector3 min = box.parameters[0];
	Vector3 max = box.parameters[1];
	Vector3 size = max - min;
	Vector3 center = size / 2 + min;
	ofVec3f p = ofVec3f(center.x(), center.y(), center.z());
	float w = size.x();
	float h = size.y();
	float d = size.z();
	ofDrawBox(p, w, h, d);
}

void ofApp::rayCastSelection(vector<int> &indicies, Node *node, Ray ray) {
	if (node->indicies.size() == 1) {
		indicies.push_back(node->indicies[0]);
	}
	else if (node->box.intersect(ray, -10000, 10000)) {
		ofVec3f v;
		for (int i = 0; i < node->children.size(); i++) {
			if (node->children[i].box.intersect(ray, -10000, 10000))
				rayCastSelection(indicies, &node->children[i], ray);
		}
	}
}

void ofApp::resetRocket() {
	lander.setPosition(ofVec3f(0, 15, 0));
	lander.thrustForce->set(ofVec3f(0, 0, 0));
	lander.sys.particles[0].velocity.set(0, 0, 0);
}

void ofApp::loadVbo() {
	if (lander.numParticles() < 1) return;

	vector<ofVec3f> sizes;
	vector<ofVec3f> points;

	//add main exhaust particles
	for (int i = 0; i < lander.mainExhaust.sys->particles.size(); i++) {
		points.push_back(lander.mainExhaust.sys->particles[i].position);
		sizes.push_back(ofVec3f(lander.mainExhaust.sys->particles[i].radius));
	}


	//add left exhaust particles
	for (int i = 0; i < lander.leftExhaust.sys->particles.size(); i++) {
		points.push_back(lander.leftExhaust.sys->particles[i].position);
		sizes.push_back(ofVec3f(lander.leftExhaust.sys->particles[i].radius));
	}

	//add right exhaust particles
	for (int i = 0; i < lander.rightExhaust.sys->particles.size(); i++) {
		points.push_back(lander.rightExhaust.sys->particles[i].position);
		sizes.push_back(ofVec3f(lander.rightExhaust.sys->particles[i].radius));
	}

	//add front exhaust particles
	for (int i = 0; i < lander.frontExhaust.sys->particles.size(); i++) {
		points.push_back(lander.frontExhaust.sys->particles[i].position);
		sizes.push_back(ofVec3f(lander.frontExhaust.sys->particles[i].radius));
	}

	int total = (int)points.size();
	vbo.clear();
	vbo.setVertexData(&points[0], total, GL_STATIC_DRAW);
	vbo.setNormalData(&sizes[0], total, GL_STATIC_DRAW);
}