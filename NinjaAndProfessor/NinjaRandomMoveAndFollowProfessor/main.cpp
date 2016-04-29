#define CLIENT_DESCRIPTION "Quaternion"

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#include <Ogre.h>
#include <OIS/OIS.h>

using namespace Ogre;

enum PlayerRunState{
	PLAYER_RUNNING,
	PLAYER_STOP,
};


static float ProfessorSpeed = 0.0f;
static int PlayerRunState = PLAYER_STOP;


class InputController : public FrameListener, 
  public OIS::KeyListener, 
  public OIS::MouseListener
{

public:
	InputController(Root* root, OIS::Keyboard *keyboard, OIS::Mouse *mouse) : mRoot(root), mKeyboard(keyboard), mMouse(mouse)
  {
    mProfessorNode = mRoot->getSceneManager("main")->getSceneNode("Professor");
	mEmptyNode = mRoot->getSceneManager("main")->getSceneNode("Empty");
	mProfessorEntity = mRoot->getSceneManager("main")->getEntity("Professor");
    mCamera = mRoot->getSceneManager("main")->getCamera("main");
	mCameraMoveVector = Ogre::Vector3::ZERO;
	mAnimationState = mProfessorEntity->getAnimationState("Idle");
	mAnimationState->setEnabled(true);
	mAnimationState->setLoop(true);
    mContinue = true;

    keyboard->setEventCallback(this);
    mouse->setEventCallback(this);
  }


  bool frameStarted(const FrameEvent &evt)
  {
	  mAnimationState->addTime(evt.timeSinceLastFrame);
      mKeyboard->capture();
      mMouse->capture();
	  
	  mCamera->moveRelative(mCameraMoveVector);

	  if (PlayerRunState == PLAYER_RUNNING)
	  {
	    mProfessorNode->translate(0.0f, 0.0f, ProfessorSpeed * evt.timeSinceLastFrame,Node::TS_LOCAL);
	  }
	  else if (PlayerRunState == PLAYER_STOP)
	  {
		  mProfessorEntity = mRoot->getSceneManager("main")->getEntity("Professor");
		  mAnimationState = mProfessorEntity->getAnimationState("Idle");
	  }	  
	  //mCamera->setPosition(mEmptyNode->_getDerivedPosition().x, 300.0f, mEmptyNode->_getDerivedPosition().z);
	  //mCamera->lookAt(mProfessorNode->getPosition().x, 200.0f, mProfessorNode->getPosition().z);
	  
      return mContinue;
  }

  // Key Linstener Interface Implementation

  bool keyPressed(const OIS::KeyEvent &evt)
  {
	  
	  switch (evt.key) {
	  case OIS::KC_W: mCameraMoveVector.y += 1; break;
	  case OIS::KC_S: mCameraMoveVector.y -= 1; break;
	  case OIS::KC_A: mCameraMoveVector.x -= 1; break;
	  case OIS::KC_D: mCameraMoveVector.x += 1; break;
	  case OIS::KC_Q: mCameraMoveVector.z -= 1; break;
	  case OIS::KC_E: mCameraMoveVector.z += 1; break;
	  case OIS::KC_ESCAPE: mContinue = false; break;
	  }
	  
	  return true;


  }

  bool keyReleased( const OIS::KeyEvent &evt )
  {
	  switch (evt.key) {
	  case OIS::KC_W: mCameraMoveVector.y += 1; break;
	  case OIS::KC_S: mCameraMoveVector.y -= 1; break;
	  case OIS::KC_A: mCameraMoveVector.x -= 1; break;
	  case OIS::KC_D: mCameraMoveVector.x += 1; break;
	  case OIS::KC_Q: mCameraMoveVector.z -= 1; break;
	  case OIS::KC_E: mCameraMoveVector.z += 1; break;
	  case OIS::KC_ESCAPE: mContinue = false; break;
	  }
    return true;
  }


  // Mouse Listener Interface Implementation

  bool mouseMoved( const OIS::MouseEvent &evt )
  {
	mProfessorNode->yaw(Degree(-evt.state.X.rel));
	
    return true;
  }

  bool mousePressed( const OIS::MouseEvent &evt, OIS::MouseButtonID id )
  {
	  if (id == OIS::MB_Right)
	  {
		  ProfessorSpeed -= 10.0f;
		  if (ProfessorSpeed <= 0)
		  {
			  ProfessorSpeed = 0.0f;
		  }
		  if (ProfessorSpeed <= 0)
		  {
			  PlayerRunState = PLAYER_STOP;			  
		  }
	  }
	  if (id == OIS::MB_Left)
	  {
		  
		  ProfessorSpeed += 10.0f;
		  
		  if (ProfessorSpeed > 0)
		  {
			  PlayerRunState = PLAYER_RUNNING;
			  mAnimationState = mProfessorEntity->getAnimationState("Walk");
			  mAnimationState->setEnabled(true);
			  mAnimationState->setLoop(true);
		  }
	  }	  
    return true;
  }

  bool mouseReleased( const OIS::MouseEvent &evt, OIS::MouseButtonID id )
  {
    return true;
  }


private:
  bool mContinue;
  Ogre::Root* mRoot;
  OIS::Keyboard* mKeyboard;
  OIS::Mouse* mMouse;
  Camera* mCamera;
  SceneNode* mProfessorNode, *mEmptyNode;
  Ogre::Entity *mProfessorEntity;
  Ogre::AnimationState* mAnimationState;
  Ogre::Vector3 mCameraMoveVector;
};

class NinjaController : public FrameListener
{

public:
	NinjaController(Root* root)
	{
		mNinjaNode = root->getSceneManager("main")->getSceneNode("Ninja");
		mNinjaEntity = root->getSceneManager("main")->getEntity("Ninja");
		mProfessorNode = root->getSceneManager("main")->getSceneNode("Professor");
		mProfessorEntity = root->getSceneManager("main")->getEntity("Professor");

		mWalkSpeed = 80.0f;
		mDirection = Vector3::ZERO;
		mRotating = false;

		mWalkList.push_back(Vector3(150.0f, 0.0f, 200.0f));
		mWalkList.push_back(Vector3(-450.0f, 0.0f, 200.0f));
		mWalkList.push_back(Vector3(50.0f, 0.0f, -200.0f));
		mWalkList.push_back(Vector3(-350.0f, 0.0f, -200.0f));
		mWalkList.push_back(Vector3(-150.0f, 0.0f, 300.0f));
		mWalkList.push_back(Vector3(-250.0f, 0.0f, 500.0f));
		mWalkList.push_back(Vector3(550.0f, 0.0f, -400.0f));
		mWalkList.push_back(Vector3(-350.0f, 0.0f, -100.0f));
	}

	bool frameStarted(const FrameEvent &evt)
	{
		moveNinjaToProfessor(evt);
		
		if (mDirection == Vector3::ZERO)
		{
			if (nextLocation())
			{
				mAnimationState = mNinjaEntity->getAnimationState("Walk");
				mAnimationState->setLoop(true);
				mAnimationState->setEnabled(true);
			}
		}
		else if (mRotating)
		{
			static const float ROTATION_TIME = 0.3f;
			mRotatingTime = (mRotatingTime > ROTATION_TIME) ? ROTATION_TIME : mRotatingTime;
			Quaternion delta = Quaternion::Slerp(mRotatingTime / ROTATION_TIME, mSrcQuat, mDestQuat, true);
			mNinjaNode->setOrientation(delta);
			if (mRotatingTime >= ROTATION_TIME)
			{
				mRotatingTime = 0.0f;
				mRotating = false;
				mNinjaNode->setOrientation(mDestQuat);
			}
			else
				mRotatingTime += evt.timeSinceLastFrame;
		}
		
		else
		{
			Real move = mWalkSpeed * evt.timeSinceLastFrame; // 이동량 계산
			mDistance -= move; // 남은 거리 계산
			if (mDistance <= 0.0f)
			{ // 목표 지점에 다 왔으면…
				mNinjaNode->setPosition(mDestination); // 목표 지점에 캐릭터를 위치
				mDirection = Vector3::ZERO; // 정지 상태로 들어간다.
				if (!nextLocation())
				{
					mAnimationState->setEnabled(false);
					mAnimationState = mNinjaEntity->getAnimationState("Idle");
					mAnimationState->setLoop(true);
					mAnimationState->setEnabled(true);
				}
			}
			else
			{
				mNinjaNode->translate(mDirection * move);
				
			}
		}

		mAnimationState->addTime(evt.timeSinceLastFrame);

		return true;
	}

	bool nextLocation(void)
	{
		if (mWalkList.empty())  // 더 이상 목표 지점이 없으면 false 리턴
			return false;

		mDestination = mWalkList.front(); // 큐의 가장 앞에서 꺼내기
		mWalkList.push_back(mWalkList.front());
		mWalkList.pop_front(); // 가장 앞 포인트를 제거
		mDirection = mDestination - mNinjaNode->getPosition(); // 방향 계산
		mDistance = mDirection.normalise(); // 거리 계산


		mSrcQuat = mNinjaNode->getOrientation();
		mDestQuat = Vector3(-Vector3::UNIT_Z).getRotationTo(mDirection);
		mRotating = true;
		mRotatingTime = 0.0f;

		return true;
	}

	void moveNinjaToProfessor(const FrameEvent &evt)
	{		
		mDirectionToProfessor = mProfessorNode->getPosition() - mNinjaNode->getPosition();
		mDistanceToProfessor = mDirectionToProfessor.normalise();
		if (mDistanceToProfessor < 300.0f)
		{
			mSrcQuat = mNinjaNode->getOrientation();
			mDestQuat = Vector3(Vector3::NEGATIVE_UNIT_Z).getRotationTo(mDirectionToProfessor);
			mNinjaNode->setOrientation(mDestQuat*0.1);
			if (mDistanceToProfessor > 50.0f)
			{
				mNinjaNode->translate(0.0f, 0.0f, -1 * mWalkSpeed * evt.timeSinceLastFrame, Node::TS_LOCAL);
			}
			mDirection = Vector3::ZERO;
		}		
	}

private:
	SceneNode *mNinjaNode, *mProfessorNode;
	Ogre::Entity *mNinjaEntity, *mProfessorEntity;
	Ogre::AnimationState* mAnimationState;

	std::deque<Vector3> mWalkList;
	Real mWalkSpeed;
	Vector3 mDirection, mDirectionToProfessor;
	Real mDistance, mDistanceToProfessor;
	Vector3 mDestination;


	bool mRotating;
	float mRotatingTime;
	Quaternion mSrcQuat, mDestQuat;
};


class LectureApp {

  Root* mRoot;
  RenderWindow* mWindow;
  SceneManager* mSceneMgr;
  Camera* mCamera;
  Viewport* mViewport;
  OIS::Keyboard* mKeyboard;
  OIS::Mouse* mMouse;

  OIS::InputManager *mInputManager;


public:

  LectureApp() {}

  ~LectureApp() {}

  void go(void)
  {

#if !defined(_DEBUG)
    mRoot = new Root("plugins.cfg", "ogre.cfg", "ogre.log");
#else
    mRoot = new Root("plugins_d.cfg", "ogre.cfg", "ogre.log");
#endif


    if (!mRoot->restoreConfig()) {
      if (!mRoot->showConfigDialog()) return;
    }

    mWindow = mRoot->initialise(true, CLIENT_DESCRIPTION " : Copyleft by Dae-Hyun Lee 2010");

    mSceneMgr = mRoot->createSceneManager(ST_GENERIC, "main");
    mCamera = mSceneMgr->createCamera("main");


    mCamera->setPosition(0.0f, 300.0f, 1500.0f);
    mCamera->lookAt(0.0f, 0.0f, 0.0f);

    mViewport = mWindow->addViewport(mCamera);
    mViewport->setBackgroundColour(ColourValue(0.0f,0.0f,0.5f));
    mCamera->setAspectRatio(Real(mViewport->getActualWidth()) / Real(mViewport->getActualHeight()));

    ResourceGroupManager::getSingleton().addResourceLocation("resource.zip", "Zip");
    ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

    mSceneMgr->setAmbientLight(ColourValue(1.0f, 1.0f, 1.0f));

	Ogre::Plane plane(Ogre::Vector3::UNIT_Y, 0);
	Ogre::MeshManager::getSingleton().createPlane(
		"ground",
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		plane,
		15000, 15000, 20, 20,
		true,
		1, 50, 50,
		Ogre::Vector3::UNIT_Z);
	Ogre::Entity* groundEntity = mSceneMgr->createEntity("ground");
	mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(groundEntity);
	groundEntity->setCastShadows(false);
	groundEntity->setMaterialName("Examples/Rockwall");

	mSceneMgr->setSkyDome(true, "Examples/CloudySky", 5, 8);


    Ogre::Entity* mAxesEntity = mSceneMgr->createEntity("Axes", "axes.mesh");
    mSceneMgr->getRootSceneNode()->createChildSceneNode("AxesNode",Ogre::Vector3(0,0,0))->attachObject(mAxesEntity);
    mSceneMgr->getSceneNode("AxesNode")->setScale(5, 5, 5);

    _drawGridPlane();


    Entity* entity1 = mSceneMgr->createEntity("Professor", "DustinBody.mesh");
    SceneNode* node1 = mSceneMgr->getRootSceneNode()->createChildSceneNode("Professor", Vector3(0.0f, 0.0f, 0.0f));
	node1->yaw(Degree(180));
	node1->attachObject(entity1);

	Entity* emptyentity = mSceneMgr->createEntity("Empty", "Empty.Mesh");
	SceneNode* emptyNode = node1->createChildSceneNode("Empty", Vector3(0.0f, 300.0f, -1000.0f));
	emptyNode->attachObject(emptyentity);

	Entity* entity2 = mSceneMgr->createEntity("Ninja", "ninja.mesh");
	SceneNode* node3 = mSceneMgr->getRootSceneNode()->createChildSceneNode("Ninja", Vector3(300.0f, 0.0f, 300.0f));
	node3->attachObject(entity2);

    size_t windowHnd = 0;
    std::ostringstream windowHndStr;
    OIS::ParamList pl;
    mWindow->getCustomAttribute("WINDOW", &windowHnd);
    windowHndStr << windowHnd;
    pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));
    pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_FOREGROUND" )));
    pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_NONEXCLUSIVE")));
    pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_FOREGROUND")));
    pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_NONEXCLUSIVE")));
    mInputManager = OIS::InputManager::createInputSystem(pl);


    mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject(OIS::OISKeyboard, true));
    mMouse = static_cast<OIS::Mouse*>( mInputManager->createInputObject(OIS::OISMouse, true));

    InputController* inputController = new InputController(mRoot, mKeyboard, mMouse);
    mRoot->addFrameListener(inputController);

	NinjaController* ninjaController = new NinjaController(mRoot);
	mRoot->addFrameListener(ninjaController);

    mRoot->startRendering();

    mInputManager->destroyInputObject(mKeyboard);
    mInputManager->destroyInputObject(mMouse);
    OIS::InputManager::destroyInputSystem(mInputManager);

    delete inputController;

    delete mRoot;
  }

private:
  void _drawGridPlane(void)
  {
    Ogre::ManualObject* gridPlane =  mSceneMgr->createManualObject("GridPlane"); 
    Ogre::SceneNode* gridPlaneNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("GridPlaneNode"); 

    Ogre::MaterialPtr gridPlaneMaterial = Ogre::MaterialManager::getSingleton().create("GridPlanMaterial","General"); 
    gridPlaneMaterial->setReceiveShadows(false); 
    gridPlaneMaterial->getTechnique(0)->setLightingEnabled(true); 
    gridPlaneMaterial->getTechnique(0)->getPass(0)->setDiffuse(1,1,1,0); 
    gridPlaneMaterial->getTechnique(0)->getPass(0)->setAmbient(1,1,1); 
    gridPlaneMaterial->getTechnique(0)->getPass(0)->setSelfIllumination(1,1,1); 

    gridPlane->begin("GridPlaneMaterial", Ogre::RenderOperation::OT_LINE_LIST); 
    for(int i=0; i<21; i++)
    {
      gridPlane->position(-500.0f, 0.0f, 500.0f-i*50);
      gridPlane->position(500.0f, 0.0f, 500.0f-i*50);

      gridPlane->position(-500.f+i*50, 0.f, 500.0f);
      gridPlane->position(-500.f+i*50, 0.f, -500.f);
    }

    gridPlane->end(); 

    gridPlaneNode->attachObject(gridPlane);
  }
};


#ifdef __cplusplus
extern "C" {
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
  INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
  int main(int argc, char *argv[])
#endif
  {
    LectureApp app;

    try {

      app.go();

    } catch( Ogre::Exception& e ) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
      MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
      std::cerr << "An exception has occured: " <<
        e.getFullDescription().c_str() << std::endl;
#endif
    }

    return 0;
  }

#ifdef __cplusplus
}
#endif

