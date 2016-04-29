#define CLIENT_DESCRIPTION "Quaternion"

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#include <Ogre.h>
#include <OIS/OIS.h>

using namespace Ogre;

int recentKeyDown = 0;
int keyDown = 0;

enum KEY_DOWN
{
	UP = 180,
	DOWN = 0,
	LEFT = -90,
	RIGHT = 90
};

class InputController : public FrameListener, 
  public OIS::KeyListener
{

public:
	InputController(Root* root, OIS::Keyboard *keyboard) : mRoot(root), mKeyboard(keyboard)
  {
    mProfessorNode = mRoot->getSceneManager("main")->getSceneNode("Professor");
	mProfessorEntity = mRoot->getSceneManager("main")->getEntity("Professor");
  
	mAnimationState = mProfessorEntity->getAnimationState("Idle");
	mAnimationState->setEnabled(true);
	mAnimationState->setLoop(true);
    

    keyboard->setEventCallback(this);
   
  }


  bool frameStarted(const FrameEvent &evt)
  {
	  
	  mAnimationState->addTime(evt.timeSinceLastFrame);
      mKeyboard->capture();
	  if (keyDown == KEY_DOWN::UP)
	  {
		  moveProfessor(evt, KEY_DOWN::UP);
	  }
	  else if (keyDown == KEY_DOWN::DOWN)
	  {
		  moveProfessor(evt, KEY_DOWN::DOWN);
	  }
	  else if (keyDown == KEY_DOWN::LEFT)
	  {
		  moveProfessor(evt, KEY_DOWN::LEFT);
	  }
	  else if (keyDown == KEY_DOWN::RIGHT)
	  {
		  moveProfessor(evt, KEY_DOWN::RIGHT);
	  }
	  	  
      return mContinue;
  }

  void moveProfessor(const FrameEvent &evt, int degree)
  {
	  static float ProfessorSpeed = 80.0f;

	  mProfessorNode->setOrientation(Quaternion(Degree(degree), Vector3::UNIT_Y));
	  mProfessorNode->translate(0.0f, 0.0f, ProfessorSpeed * evt.timeSinceLastFrame, Node::TS_LOCAL);
  }

  // Key Linstener Interface Implementation

  bool keyPressed(const OIS::KeyEvent &evt)
  {	  
	  
	  switch (evt.key) {
	  case OIS::KC_UP:
		  recentKeyDown = keyDown;
		  keyDown = KEY_DOWN::UP;
		  break;
	  case OIS::KC_DOWN:
		  recentKeyDown = keyDown;
		  keyDown = KEY_DOWN::DOWN;
		  break;
	  case OIS::KC_LEFT:
		  recentKeyDown = keyDown;
		  keyDown = KEY_DOWN::LEFT;
		  break;
	  case OIS::KC_RIGHT:
		  recentKeyDown = keyDown;
		  keyDown = KEY_DOWN::RIGHT;
		  break;
	  case OIS::KC_ESCAPE: 
		  mContinue = false; 
		  break;
	  }
	  
	  return true;
  }

  bool keyReleased( const OIS::KeyEvent &evt )
  {
    return true;
  }

private:
  bool mContinue;
  Ogre::Root* mRoot;
  OIS::Keyboard* mKeyboard;
  OIS::Mouse* mMouse;
  Camera* mCamera;
  SceneNode* mProfessorNode;
  Ogre::Entity *mProfessorEntity;
  Ogre::AnimationState* mAnimationState;
};


class LectureApp {

  Root* mRoot;
  RenderWindow* mWindow;
  SceneManager* mSceneMgr;
  Camera* mCamera;
  Viewport* mViewport;
  OIS::Keyboard* mKeyboard;

  OIS::InputManager *mInputManager;


public:

  LectureApp() {}

  ~LectureApp() {}

  void go(void)
  {
    // OGRE의 메인 루트 오브젝트 생성
#if !defined(_DEBUG)
    mRoot = new Root("plugins.cfg", "ogre.cfg", "ogre.log");
#else
    mRoot = new Root("plugins_d.cfg", "ogre.cfg", "ogre.log");
#endif


    // 초기 시작의 컨피규레이션 설정 - ogre.cfg 이용
    if (!mRoot->restoreConfig()) {
      if (!mRoot->showConfigDialog()) return;
    }

    mWindow = mRoot->initialise(true, CLIENT_DESCRIPTION " : Copyleft by Dae-Hyun Lee 2010");

    mSceneMgr = mRoot->createSceneManager(ST_GENERIC, "main");
    mCamera = mSceneMgr->createCamera("main");


    mCamera->setPosition(0.0f, 200.0f, 1000.0f);
    mCamera->lookAt(0.0f, 100.0f, 0.0f);

    mViewport = mWindow->addViewport(mCamera);
    mViewport->setBackgroundColour(ColourValue(0.0f,0.0f,0.5f));
    mCamera->setAspectRatio(Real(mViewport->getActualWidth()) / Real(mViewport->getActualHeight()));

    ResourceGroupManager::getSingleton().addResourceLocation("resource.zip", "Zip");
    ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

    mSceneMgr->setAmbientLight(ColourValue(1.0f, 1.0f, 1.0f));

    // 좌표축 표시
    Ogre::Entity* mAxesEntity = mSceneMgr->createEntity("Axes", "axes.mesh");
    mSceneMgr->getRootSceneNode()->createChildSceneNode("AxesNode",Ogre::Vector3(0,0,0))->attachObject(mAxesEntity);
    mSceneMgr->getSceneNode("AxesNode")->setScale(5, 5, 5);

    _drawGridPlane();


    Entity* entity1 = mSceneMgr->createEntity("Professor", "DustinBody.mesh");
    SceneNode* node1 = mSceneMgr->getRootSceneNode()->createChildSceneNode("Professor", Vector3(0.0f, 0.0f, 0.0f));
	node1->yaw(Degree(180));
	node1->attachObject(entity1);

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
   

    InputController* inputController = new InputController(mRoot, mKeyboard);
    mRoot->addFrameListener(inputController);

    mRoot->startRendering();

    mInputManager->destroyInputObject(mKeyboard);

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

