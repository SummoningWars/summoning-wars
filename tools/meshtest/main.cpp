
#include<iostream>
#include "ExampleApplication.h"
#include "document.h"
#include <fstream>
#include <sstream>
using namespace Ogre;

class SumWarsApplication : public ExampleApplication
{
	protected:
		Document* m_doc;
		std::istringstream m_arguments;
		int m_nr_arguments;
		
	public:
		SumWarsApplication(int argc=0, char **argv=0)
		{
			m_doc = new Document();
			m_doc->x = 0;
			m_doc->z = 0;
			m_doc->max_z=0;
			m_doc->min_z=100000;
			
			
			m_nr_arguments = argc-1;
			std::stringstream stream;
			for (int i=1; i<argc; i++)
			{
				stream << argv[i] << " ";
			}
			m_arguments.str(stream.str());
		}

		~SumWarsApplication()
		{
			delete m_doc;
		}

	protected:
		virtual void createCamera(void)
		{
			mCamera = mSceneMgr->createCamera("PlayerCam");
			mCamera->setPosition(Vector3(0, 10,10));
			mCamera->lookAt(Vector3(0,1,0));
			mCamera->setNearClipDistance(0.1);
		}

		virtual void createViewport(void)
		{
			Viewport *vp = mWindow->addViewport(mCamera);
			vp->setBackgroundColour(ColourValue(0,0,0));
			mCamera->setAspectRatio(Real(vp->getActualWidth())/Real(vp->getActualHeight()));
		}

		void createScene(void)
		{

			std::cout << "\n\n\n0: activate / deactive automatic animation \
					      \n1: 10\% step \
						  \n2: 1% step \
						  \n4: slow down automatic animation \
						  \n5: speed up automatic animation	\
					      \n9: reset animation \n\n\n";
			
			
			std::istream* input=0; 
			std::ifstream fin;
			if (m_nr_arguments > 0)
			{
				input = &m_arguments;
			}
			else
			{
				fin.open("mesh");

				if (!fin.is_open())
				{
					std::cout << "You must create a file named mesh. Copy it from mesh.sample.\n";
					return;
				}
				else
				{
					input = &fin;
				}
			}

			m_doc-> m_scaling_factor = 1.0;
			
			(*input) >>m_doc -> m_mesh;
			
			if (m_doc -> m_mesh == "-s")
			{
				(*input) >> m_doc-> m_scaling_factor;
				(*input) >>m_doc -> m_mesh;
			}
			(*input) >>m_doc ->m_animation;


			m_doc ->m_anim = false;
			m_doc->speed_factor = 1.0;

			mSceneMgr->setAmbientLight(ColourValue(1,1,1));


			Plane plane(Vector3::UNIT_Y, 0);
			MeshManager::getSingleton().createPlane("ground", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 20, 30, 20, 20, true, 1,5,5,Vector3::UNIT_X);

			Entity *player;
			try
			{
				player = mSceneMgr->createEntity("Player", m_doc -> m_mesh);
			}
			catch(std::exception& e)
			{
				std::cout << "Mesh "<< m_doc->m_mesh << " not found.\n";
			}

			if (m_doc -> m_animation != "#")
			{
				try
				{
					m_doc->mAnimationState = player->getAnimationState(m_doc -> m_animation);
					m_doc->mAnimationState->setLoop(true);
					m_doc->mAnimationState->setEnabled(true);
					m_doc->m_anim = true;
				}
				catch(std::exception& e)
				{
					std::cout << "no animation " <<m_doc -> m_animation << "defined \n";
					return;
				}



				std::string node;
				std::string smesh;
				Entity *submesh;
				while (!(*input).eof())
				{
					node = "";
					smesh = "";
					(*input) >> node;
					(*input) >> smesh;
					std::cout << node << " " << smesh << "\n";
					if (node != "" && smesh != "")
					{
						if (node.c_str()[0] == '#')
							break;

						submesh = mSceneMgr->createEntity(smesh,smesh );
						player->attachObjectToBone(node, submesh);
						submesh->getParentNode()->setInheritScale(false);
						//submesh->getParentNode()->setScale(m_doc-> m_scaling_factor, m_doc-> m_scaling_factor, m_doc-> m_scaling_factor);
					}
				}
			}

			SceneNode *playerNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("PlayerNode");
			playerNode->attachObject(player);
			playerNode->setScale(m_doc-> m_scaling_factor, m_doc-> m_scaling_factor, m_doc-> m_scaling_factor);

			Entity *ground = mSceneMgr->createEntity("GroundEntity", "ground");
			ground->setMaterialName("grass1");
			ground->setCastShadows(false);
			playerNode->attachObject(ground);


			SceneNode *camNode = playerNode->createChildSceneNode("PlayerCamNode");
			camNode->attachObject(mCamera);
		}

		void createFrameListener(void)
		{

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
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    // Create application object
	SumWarsApplication app;
#else
	SumWarsApplication app(argc,argv);
#endif

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
