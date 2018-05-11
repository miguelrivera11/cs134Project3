#pragma once

#include "ofMain.h"
#include  "ofxAssimpModelLoader.h"
#include "box.h"
#include "ray.h"
#include "Lander.h"

class Node {
public:
	Box box;
	vector<Node> children;
	vector<int> indicies;
};

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void rayCastSelection(vector<int> &indicies, Node *node, Ray ray);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		//void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		void drawAxis(ofVec3f);
		void initLightingAndMaterials();
		void savePicture();
		void toggleWireframeMode();
		void togglePointsDisplay();
		void toggleSelectTerrain();
		void setCameraTarget();
		bool  doPointSelection();
		void createOctree(Node *node);
		bool checkPoints(vector<int> indicies, vector<int> &childIndicies, Box box);
		void drawBox(const Box &box);
		Box meshBounds(const ofMesh &);
		void subDivideBox8(const Box &b, vector<Box> & boxList);
		void resetRocket();

		void loadVbo();

		bool mouseIntersectPlane(ofVec3f planePoint, ofVec3f planeNorm, ofVec3f &point);

		void drawOctree(Node &node, int numLevels, int level);

		ofEasyCam cam;
		ofxAssimpModelLoader surface;
		ofLight light;
		Box boundingBox;
		vector<Box> level1, level2, level3;

		ofVec3f cam1 = ofVec3f(-200, 50, 50);
		ofVec3f cam2 = ofVec3f(0, 20, 50);
		ofVec3f cam3; // front cam
		ofVec3f cam4; // ground cam
		ofLight keyLight, fillLight, rimLight;
		int camMode;
	
		bool bAltKeyDown;
		bool bCtrlKeyDown;
		bool bWireframe;
		bool bDisplayPoints;
		bool bPointSelected;
		
		bool bRoverLoaded;
		bool bTerrainSelected;
	
		ofVec3f selectedPoint;
		ofVec3f intersectPoint;
		ofMesh surfaceMesh;
		Box landerBox;
		Node octreeHead;

		const float selectionRange = 4.0;
		Lander lander;
		bool checkCollisions();
		float LANDER_SCALE;

		ofTexture particleTex;

		ofVbo vbo;
		ofShader shader;
		float altitude;
		ImpulseForce *impulse;

		ofImage background;
};
