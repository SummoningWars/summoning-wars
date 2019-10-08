
#include "document.h"

void createLog()
{
	LogManager::instance().addLog("stdout",new StdOutLog(Log::LOGLEVEL_DEBUG));
}

#include<iostream>
#include "ExampleApplication.h"

#include <fstream>
#include "graphicmanager.h"
#include "sound.h"

using namespace Ogre;

class SumWarsApplication : public ExampleApplication
{
	protected:
		Document* m_doc;
	public:
		SumWarsApplication()
		{
			m_doc = new Document();
			m_doc->x = 0;
			m_doc->z = 0;
			m_doc->max_z=0;
			m_doc->min_z=100000;

			createLog();
		}

		~SumWarsApplication()
		{
			delete m_doc->m_object;
			delete m_doc;
		}

	protected:
		virtual void createCamera(void)
		{
			mCamera = mSceneMgr->createCamera("PlayerCam");
			mCamera->setPosition(Vector3(0, GraphicManager::g_global_scale*10,GraphicManager::g_global_scale*10));
			mCamera->lookAt(Vector3(0,GraphicManager::g_global_scale,0));
			mCamera->setNearClipDistance(GraphicManager::g_global_scale*0.1);
			
			
			std::cout <<"camera position "<< GraphicManager::g_global_scale*10  <<"\n";
		}

		virtual void createViewport(void)
		{
			Viewport *vp = mWindow->addViewport(mCamera);
			vp->setBackgroundColour(ColourValue(0,0,0));
			mCamera->setAspectRatio(Real(vp->getActualWidth())/Real(vp->getActualHeight()));
		}

		bool createScene(void)
		{

			SoundSystem::init();

			std::ifstream fin("mesh");

			if (!fin.is_open())
			{
				std::cout <<"No file named mesh found. Copy it from mesh.sample. \n\n\n";
				return false;
			}

			double maxtime = 5000;
			fin >>m_doc -> m_mesh;
			fin >>m_doc ->m_animation;
			fin >>maxtime;


			Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../../data/renderinfo", "FileSystem", "renderinfo");
			Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../../resources/sound", "FileSystem", "sound");


			Ogre::FileInfoListPtr files;
			Ogre::FileInfoList::iterator it;
			std::string file;

			files = Ogre::ResourceGroupManager::getSingleton().findResourceFileInfo("renderinfo","*.xml");

			for (it = files->begin(); it != files->end(); ++it)
			{
				file = it->archive->getName();
				file += "/";
				file += it->filename;
				GraphicManager::loadRenderInfoData(file.c_str());
			}
			mSceneMgr->setAmbientLight(ColourValue(1,1,1));

			files = Ogre::ResourceGroupManager::getSingleton().findResourceFileInfo("sound","*.xml");
			for (it = files->begin(); it != files->end(); ++it)
			{
				file = it->archive->getName();
				file += "/";
				file += it->filename;

				SoundSystem::loadSoundData(file.c_str());
			}

			Plane plane(Vector3::UNIT_Y, 0);
			MeshManager::getSingleton().createPlane("ground", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 20*GraphicManager::g_global_scale, 30*GraphicManager::g_global_scale, 20, 20, true, 1,5,5,Vector3::UNIT_X);

			SceneNode *playerNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("PlayerNode");
			/*
			Ogre::ParticleSystem* part;
			part = mSceneMgr->createParticleSystem("blind", "Blind");
			playerNode->attachObject(part);
			part->setKeepParticlesInLocalSpace(true);
			*/

			Entity *ground = mSceneMgr->createEntity("GroundEntity", "ground");
			ground->setMaterialName("grass1");
			ground->setCastShadows(false);
			playerNode->attachObject(ground);


			SceneNode *camNode = playerNode->createChildSceneNode("PlayerCamNode");
			camNode->attachObject(mCamera);

			GraphicManager::setSceneManager(mSceneMgr);
			GraphicObject* gobj = GraphicManager::createGraphicObject(m_doc -> m_mesh,"obj",1);
			gobj->getTopNode()->setPosition(GraphicManager::g_global_scale*1,0,0);
			m_doc->m_object = gobj;
			m_doc->m_time=0;
			m_doc->m_max_time=maxtime/1000;
			
			return true;
		}

		void createFrameListener(void)
		{

 //           mFrameListener = new ExampleFrameListener(mWindow, mCamera);
			mFrameListener = new SumWarsFrameListener(mWindow, mCamera, mSceneMgr, m_doc);
			mRoot->addFrameListener(mFrameListener);

			mFrameListener->showDebugOverlay(false);
		}
};

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"

			 INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
			 int main(int argc, char **argv)
#endif
{
    // Create application object
	SumWarsApplication app;

	try {
		app.go();
	} catch( Exception& e ) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
		MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occurred!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
		fprintf(stderr, "An exception has occurred: %s\n",
			e.getFullDescription().c_str());
#endif
	}

	return 0;
}
