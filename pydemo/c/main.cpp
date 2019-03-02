#include "ogre.h"
#include "OgreApplicationContext.h"
#include "OgreTrays.h"
#include "OgreRTShaderSystem.h"
#include "OgreCameraMan.h"

#include "OgreRecastDefinitions.h"
#include "OgreRecast.h"
#include "OgreDetourCrowd.h"
#include "Character.h"
#include "OgreDetourTileCache.h"
#include "Obstacle.h"
#include "AnimateableCharacter.h"

#include <iostream>
using namespace Ogre;
using namespace OgreBites;


class pyApplication : public OgreBites::ApplicationContext, public OgreBites::InputListener
{
public:
	bool debugFlag;
	SceneManager * mScnMgr;
	Camera* cam;
	CameraMan* caman;
	std::vector<Ogre::Entity*> mNavMeshEnts;
	OgreRecast* recast;
	OgreDetourTileCache* tilecache;
	OgreDetourCrowd* crowd;
	std::vector<Character*> chs;


	pyApplication(bool debug=true) {
		ApplicationContext("ogrecrowdpydemo");
		//The switch of draw all the debug line or print debug info
		debugFlag = debug;
		manualCtrlFlag = false;
	}

	virtual ~pyApplication() {
		for (vector<Character*>::iterator it = chs.begin();it != chs.end();it++)
			delete (*it);

		delete crowd;
		delete tilecache;
		delete recast;
		delete caman;

	}

	void setup() {
		ApplicationContext::setup();
		addInputListener(this);
		//create scn mgr
		Root * rt = getRoot();
		mScnMgr = rt->createSceneManager();
		RTShader::ShaderGenerator* shrgen = RTShader::ShaderGenerator::getSingletonPtr();
		shrgen->addSceneManager(mScnMgr);
		//create light
		mScnMgr->setAmbientLight(ColourValue(0.5f, 0.5f, 0.5f));
		Light *lt = mScnMgr->createLight("main_light");
		SceneNode *ltNode = mScnMgr->getRootSceneNode()->createChildSceneNode();
		ltNode->setPosition(20, 80, 50);
		ltNode->attachObject(lt);
		//create cam
		cam = mScnMgr->createCamera("main_cam");
		cam->setNearClipDistance(0.1);
		cam->setAutoAspectRatio(true);
		SceneNode *camNode= mScnMgr->getRootSceneNode()->createChildSceneNode();
		camNode->setPosition(-46.3106, 62.3307, 40.7579);
		camNode->lookAt(Vector3(0, 0, 0), Ogre::Node::TS_WORLD);
		camNode->attachObject(cam);
		//and create camera manual
		caman = new CameraMan(camNode);
		caman->setStyle(CS_ORBIT);
		caman->setYawPitchDist(Radian(0), Radian(0.3), 15);
		addInputListener(caman);
		//view port
		getRenderWindow()->addViewport(cam);

		//==========create entity
		Ogre::Entity* mapEn = mScnMgr->createEntity("dungen", "dungeon.mesh");
		Ogre::SceneNode* mapNode = mScnMgr->getRootSceneNode()->createChildSceneNode("MapNode");
		mapNode->attachObject(mapEn);

		//create navmesh
		mNavMeshEnts.push_back(mapEn);
		recast = new OgreRecast(mScnMgr);
		tilecache = new OgreDetourTileCache(recast);
		if (tilecache->TileCacheBuild(mNavMeshEnts)) {
			if (debugFlag)
				tilecache->drawNavMesh();
		}else{
			Ogre::LogManager::getSingletonPtr()->logMessage("ERROR: could not generate useable navmesh from mesh using detourTileCache.");
			return;
		}
		crowd = new OgreDetourCrowd(recast);
		//create charactor
		if (crowd->getNbAgents() >= crowd->getMaxNbAgents()) {
			Ogre::LogManager::getSingletonPtr()->logMessage("Error: Cannot create crowd agent for new character. Limit of " + Ogre::StringConverter::toString(crowd->getMaxNbAgents()) + " reached", Ogre::LML_CRITICAL);
			throw new Ogre::Exception(1, "Cannot create crowd agent for new character. Limit of " + Ogre::StringConverter::toString(crowd->getMaxNbAgents()) + " reached", "create ch");
		}
		Ogre::Vector3 randPos = recast->getRandomNavMeshPoint();
		//Character* ch = new AnimateableCharacter("role",mScnMgr,crowd,debugFlag,randPos);
		Character* ch = new AnimateableCharacter("role","robot.mesh",Vector3(1,0,0),0.9,mScnMgr, crowd, debugFlag, randPos);
		
		chs.push_back(ch);

		//Ogre::Entity* robotEn = mScnMgr->createEntity("robot", "robot.mesh");
		//robotNode = mScnMgr->getRootSceneNode()->createChildSceneNode("robotNode");
		//Ogre::SceneNode* robotEntityNode = robotNode->createChildSceneNode("robotEntityNode");
		//robotEntityNode->setScale(Vector3(0.03, 0.03, 0.03));
		//robotEntityNode->attachObject(robotEn);


	}

	bool keyPressed(const KeyboardEvent &evt) {
		if (evt.keysym.sym == int('t'))
			LtPressed = true;
		if (evt.keysym.sym == int('c')) {
			manualCtrlFlag = !manualCtrlFlag;
			if (debugFlag)
				std::cout << "manualCtrlFlag: " << manualCtrlFlag << std::endl;
			if (manualCtrlFlag) {
				chs[0]->setDetourTileCache(tilecache);
				//delete an agent
				//chs[0]->setAgentControlled(false);
				if(debugFlag)
					recast->m_pRecastMOPath->setVisible(false);
				bool moving = chs[0]->isMoving();
				if (moving)
					chs[0]->stop();
			}
			else {
				//create a new agent
				//chs[0]->setAgentControlled(true);
				startVec3 = chs[0]->getNode()->getPosition();
				chs[0]->setPosition(startVec3);
				chs[0]->updateDestination(endVec3);
				if (debugFlag) {
					recast->m_pRecastMOPath->setVisible(true);
					drawPath(startVec3, endVec3);
				}
					
			}
				
		}
		
		if (evt.keysym.sym == int('-')&&manualCtrlFlag)
			chs[0]->getNode()->yaw(Radian(0.02));
		if (evt.keysym.sym == int('=') && manualCtrlFlag)
			chs[0]->getNode()->yaw(Radian(-0.02));


		if (evt.keysym.sym == SDLK_UP&&ctrlDireciton != Vector3(0, 0, 1)) {
			ctrlDireciton = Vector3(0, 0, 1);
			//chs[0]->setAgentControlled(true);
			chs[0]->moveForward();
		}
			
		if (evt.keysym.sym == SDLK_DOWN&&ctrlDireciton != Vector3(0, 0, -1)) {
			ctrlDireciton = Vector3(0, 0, -1);
			chs[0]->getNode()->yaw(Radian(Ogre::Math::PI));
			chs[0]->moveForward();
		}

		if (evt.keysym.sym == SDLK_LEFT&&ctrlDireciton != Vector3(1, 0, 0)) {
			ctrlDireciton = Vector3(1, 0, 0);
			chs[0]->getNode()->yaw(Radian(Ogre::Math::PI/2));
			chs[0]->moveForward();
		}

		if (evt.keysym.sym == SDLK_RIGHT&&ctrlDireciton != Vector3(-1, 0, 0)) {
			ctrlDireciton = Vector3(-1, 0, 0);
			chs[0]->getNode()->yaw(-Radian(Ogre::Math::PI / 2));
			chs[0]->moveForward();
		}



		return InputListener::keyPressed(evt);
	}
	bool keyReleased(const KeyboardEvent &evt) {
		if (evt.keysym.sym == int('t'))
			LtPressed = false;
		if (evt.keysym.sym == SDLK_UP && ctrlDireciton == Vector3(0, 0, 1)) {
			ctrlDireciton = Vector3(0, 0, 0);
			//chs[0]->setAgentControlled(false);
			chs[0]->stop();
		}
			
		if (evt.keysym.sym == SDLK_DOWN && ctrlDireciton == Vector3(0, 0, -1)) {
			ctrlDireciton = Vector3(0, 0, 0);
			chs[0]->stop();
		}

		if (evt.keysym.sym == SDLK_LEFT && ctrlDireciton == Vector3(1, 0, 0)) {
			ctrlDireciton = Vector3(0, 0, 0);
			chs[0]->stop();
		}
		if (evt.keysym.sym == SDLK_RIGHT && ctrlDireciton == Vector3(-1, 0, 0)) {
			ctrlDireciton = Vector3(0, 0, 0);
			chs[0]->stop();
		}

		return InputListener::keyReleased(evt);
	}
	bool mouseMoved(const MouseMotionEvent &evt) {
		//std::cout << evt.x << " " << evt.y << std::endl;
		mouseX = evt.x;
		mouseY = evt.y;


		return InputListener::mouseMoved(evt);
	}
	bool mousePressed(const MouseButtonEvent &arg) {

		Vector3 vec3;
		if (arg.button == BUTTON_LEFT && LtPressed) {
			queryRay(vec3, NULL);
			startVec3 = vec3;
			for (vector<Character*>::iterator it = chs.begin();it != chs.end();it++)
				(*it)->setPosition(vec3);
			if (debugFlag)
				std::cout <<"start pos: "<< vec3 << std::endl;
		}

		if (arg.button == BUTTON_RIGHT && LtPressed) {
			queryRay(vec3, NULL);
			endVec3 = vec3;
			for (vector<Character*>::iterator it = chs.begin();it != chs.end();it++)
				(*it)->updateDestination(vec3, false);
			if (debugFlag) {
				drawPath(startVec3, endVec3);
				std::cout << "end pos: " << vec3 << std::endl;
			}
				

		}
		




		return InputListener::mousePressed(arg);
	}

	bool frameRenderingQueued(const FrameEvent& evt) {

		tilecache->handleUpdate(evt.timeSinceLastEvent);
		crowd->updateTick(evt.timeSinceLastEvent);
		for (std::vector<Character*>::iterator it = chs.begin();it != chs.end();it++)
			(*it)->update(evt.timeSinceLastEvent);
		return ApplicationContext::frameRenderingQueued(evt);
	}


protected:
	//SceneNode* robotNode;
	bool LtPressed = false;
	int mouseX = 0;
	int mouseY = 0;
	Vector3 startVec3;
	Vector3 endVec3;
	bool manualCtrlFlag=false;
	Vector3 ctrlDireciton = Vector3(0, 0, 0);

	bool queryRay(Vector3 &HitPoint, MovableObject **rayHitObject) {
		Ray mray = cam->getCameraToViewportRay((float)mouseX/getRenderWindow()->getWidth(),(float)mouseY/getRenderWindow()->getHeight());
		Ogre::MovableObject *hitObject;
		if (rayQueryPointInScene(mray, HitPoint, &hitObject)) {

			if (Ogre::StringUtil::startsWith(hitObject->getName(), "recastmowalk", true)) {
				// Compensate for the fact that the ray-queried navmesh is drawn a little above the ground
				HitPoint.y = HitPoint.y - recast->getNavmeshOffsetFromGround();
			}
			else {
				// Queried point was not on navmesh, find nearest point on the navmesh (if not possible returns exact point)
				recast->findNearestPointOnNavmesh(HitPoint, HitPoint);
			}

			// Pass pointer to hit movable
			if (rayHitObject)
				*rayHitObject = hitObject;
			return true;
		}
		return false;
	}


	bool rayQueryPointInScene(Ogre::Ray ray, Ogre::Vector3 &result, Ogre::MovableObject **foundMovable)
	{
		// TODO: destroy queries using scenemgr::destroyRayQuery or reuse one query object by storing it in a member variable
		RaySceneQuery* mRayScnQuery = mScnMgr->createRayQuery(Ogre::Ray());

		mRayScnQuery->setRay(ray);
		Ogre::RaySceneQueryResult& query_result = mRayScnQuery->execute();

		// at this point we have raycast to a series of different objects bounding boxes.
		// we need to test these different objects to see which is the first polygon hit.
		// there are some minor optimizations (distance based) that mean we wont have to
		// check all of the objects most of the time, but the worst case scenario is that
		// we need to test every triangle of every object.
		Ogre::Real closest_distance = -1.0f;
		Ogre::Vector3 closest_result;
		Ogre::MovableObject *closest_movable=NULL;
		for (size_t qr_idx = 0; qr_idx < query_result.size(); qr_idx++)
		{
			// Debug:
			//Ogre::LogManager::getSingletonPtr()->logMessage(query_result[qr_idx].movable->getName());
			//Ogre::LogManager::getSingletonPtr()->logMessage(query_result[qr_idx].movable->getMovableType());


			// stop checking if we have found a raycast hit that is closer
			// than all remaining entities
			if ((closest_distance >= 0.0f) &&
				(closest_distance < query_result[qr_idx].distance))
			{
				break;
			}

			// only check this result if its a hit against an entity
			if ((query_result[qr_idx].movable != NULL) &&
				((query_result[qr_idx].movable->getMovableType().compare("Entity") == 0)
					|| query_result[qr_idx].movable->getMovableType().compare("ManualObject") == 0))
			{
				// mesh data to retrieve
				size_t vertex_count;
				size_t index_count;
				Ogre::Vector3 *vertices;
				unsigned long *indices;

				// get the mesh information
				if (query_result[qr_idx].movable->getMovableType().compare("Entity") == 0) {
					// For entities
					// get the entity to check
					Ogre::Entity *pentity = static_cast<Ogre::Entity*>(query_result[qr_idx].movable);

					InputGeom::getMeshInformation(pentity->getMesh(), vertex_count, vertices, index_count, indices,
						pentity->getParentNode()->_getDerivedPosition(),
						pentity->getParentNode()->_getDerivedOrientation(),
						pentity->getParentNode()->_getDerivedScale());
				}
				else {
					// For manualObjects
					// get the entity to check
					Ogre::ManualObject *pmanual = static_cast<Ogre::ManualObject*>(query_result[qr_idx].movable);

					InputGeom::getManualMeshInformation(pmanual, vertex_count, vertices, index_count, indices,
						pmanual->getParentNode()->_getDerivedPosition(),
						pmanual->getParentNode()->_getDerivedOrientation(),
						pmanual->getParentNode()->_getDerivedScale());
				}

				// test for hitting individual triangles on the mesh
				bool new_closest_found = false;
				for (int i = 0; i < static_cast<int>(index_count); i += 3)
				{
					// check for a hit against this triangle
					std::pair<bool, Ogre::Real> hit = Ogre::Math::intersects(ray, vertices[indices[i]],
						vertices[indices[i + 1]], vertices[indices[i + 2]], true, false);

					// if it was a hit check if its the closest
					if (hit.first)
					{
						if ((closest_distance < 0.0f) ||
							(hit.second < closest_distance))
						{
							// this is the closest so far, save it off
							closest_distance = hit.second;
							new_closest_found = true;
						}
					}
				}


				// free the verticies and indicies memory
				delete[] vertices;
				delete[] indices;

				// if we found a new closest raycast for this object, update the
				// closest_result before moving on to the next object.
				if (new_closest_found)
				{
					closest_result = ray.getPoint(closest_distance);
					if (query_result[qr_idx].movable != NULL)
						closest_movable = query_result[qr_idx].movable;
				}
			}
		}

		// return the result
		if (closest_distance >= 0.0f)
		{
			// raycast success
			result = closest_result;
			*foundMovable = closest_movable;
			return (true);
		}
		else
		{
			// raycast failed
			return (false);
		}

	}

	void drawPath(Vector3 start, Vector3 end) {
		if (recast->FindPath(start, end, 1, 1) >= 0)
			recast->CreateRecastPathLine(1);
		else
			std::cout << ">>>ERROR>>>fail to find path" << std::endl;
	}
};

int main(int argc, char **argv)
{

	try
	{
		//pyApplication::loadConfig("OgreRecast.cfg");

		pyApplication app;
		app.initApp();
		app.getRoot()->startRendering();
		app.closeApp();
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error occurred during execution: " << e.what() << '\n';
		return 1;
	}

	return 0;
}