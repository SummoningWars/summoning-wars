
#include "sumwarsframelistener.h"
#include <iostream>
#include <fstream>
#include <algorithm>

#include "graphicmanager.h"

SumWarsFrameListener::SumWarsFrameListener(Ogre::RenderWindow* win, Ogre::Camera* cam, Ogre::SceneManager *sceneMgr, Document* doc)
: ExampleFrameListener(win, cam, true, true)
{
	m_camera = cam;
	m_doc = doc;
	mCamNode = cam->getParentSceneNode();
	mSceneMgr = sceneMgr;
	mouseVector = new Ogre::Vector3(0,0,0);
	mousePosition = new Ogre::Vector3(0,0,0);

	mContinue = true;

	mMouse->setEventCallback(this);
	mKeyboard->setEventCallback(this);

	m_dist = GraphicManager::g_global_scale*14;
	m_phi = -90;
	m_theta = 45;

	m_middle = false;
}

bool SumWarsFrameListener::frameStarted(const Ogre::FrameEvent &evt)
{
	if(mMouse)
		mMouse->capture();
	if(mKeyboard)
		mKeyboard->capture();
	mSceneMgr->getSceneNode("PlayerNode")->setPosition(m_doc->x,0,m_doc->z);
			//translationvector for Mousepointer
	mouseVector->x = m_doc->m_x_r;
	mouseVector->z = m_doc->m_y_r;

	m_doc->m_time += evt.timeSinceLastFrame;
	if (m_doc->m_time > m_doc->m_max_time)
	{
		m_doc->m_time -= m_doc->m_max_time;
	}

	m_doc->m_object->updateAction(m_doc->m_animation, m_doc->m_time/m_doc->m_max_time);
	m_doc->m_object->update(evt.timeSinceLastFrame*1000);

	static float time=0;
	time += evt.timeSinceLastFrame;

	return mContinue;
}

    // MouseListener
bool SumWarsFrameListener::mouseMoved(const OIS::MouseEvent &e)
{
	if (m_middle || e.state.Z.rel!=0)
	{
		m_dist += e.state.Z.rel*0.01*GraphicManager::g_global_scale;
		m_dist = std::max(m_dist,0.0f);

		m_theta += e.state.Y.rel*0.5;
		m_theta =std::max(std::min(m_theta,90.0f),5.0f);


		m_phi -= e.state.X.rel*0.5;
		if (m_phi > 180)
			m_phi -= 360;

		if (m_phi< -180)
			m_phi += 360;

		float th = m_theta *3.14159 / 180;
		float ph = m_phi *3.14159 / 180;

		//printf("%f %f %f\n",m_dist,th,ph);
		//printf("position %f %f %f\n", m_dist*cos(th)*cos(ph), m_dist*sin(th),  - m_dist*cos(th)*sin(ph));

		m_camera->setPosition(Ogre::Vector3(m_dist*cos(th)*cos(ph), m_dist*sin(th),  - m_dist*cos(th)*sin(ph)));
		m_camera->lookAt(Ogre::Vector3(0,GraphicManager::g_global_scale,0));


	}
	return true;
}

bool SumWarsFrameListener::mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
	if (id == OIS::MB_Middle)
	{
		m_middle = true;
	}

	return true;
}


bool SumWarsFrameListener::mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
	if (id == OIS::MB_Middle)
	{
		m_middle = false;
	}
	return true;
}


    // KeyListener
bool SumWarsFrameListener::keyPressed(const OIS::KeyEvent &e)
{
	std::ofstream file;
	switch (e.key)
	{
		case OIS::KC_ESCAPE:
			mContinue = false;
			file.open("range");
			file << "range "<< (m_doc->max_z-25)/GraphicManager::g_global_scale << "\n";
			file << "extent "<< (m_doc->ext_x)/GraphicManager::g_global_scale<<" "<<(m_doc->ext_z)/GraphicManager::g_global_scale << "\n";
			file.close();
			break;

		case OIS::KC_9:
			m_doc->m_anim = !m_doc->m_anim;
			break;

        case OIS::KC_2:

            break;
        case OIS::KC_0:

            break;
        case OIS::KC_1:


            break;
        default:
            break;

	}
	return true;
}



