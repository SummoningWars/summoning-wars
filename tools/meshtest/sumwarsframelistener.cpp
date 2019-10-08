
#include "sumwarsframelistener.h"
#include <iostream>
#include <fstream>
#include <algorithm>

SumWarsFrameListener::SumWarsFrameListener(Ogre::RenderWindow* win, Ogre::Camera* cam, Ogre::SceneManager *sceneMgr, Document* doc): ExampleFrameListener(win, cam, true, true)
{
	m_camera = cam;
	m_doc = doc;
	mCamNode = cam->getParentSceneNode();
	mSceneMgr = sceneMgr;
	mouseVector = new Vector3(0,0,0);
	mousePosition = new Vector3(0,0,0);

	mContinue = true;

	mMouse->setEventCallback(this);
	mKeyboard->setEventCallback(this);

	m_dist = 14;
	m_phi = -90;
	m_theta = 45;

	m_middle = false;
}

bool SumWarsFrameListener::frameStarted(const FrameEvent &evt)
{
	if(mMouse)
		mMouse->capture();
	if(mKeyboard)
		mKeyboard->capture();
	mSceneMgr->getSceneNode("PlayerNode")->setPosition(m_doc->x,0,m_doc->z);
			//translationvector for Mousepointer
	mouseVector->x = m_doc->m_x_r;
	mouseVector->z = m_doc->m_y_r;

	if (m_doc->m_anim )
	{
		m_doc->mAnimationState->addTime(evt.timeSinceLastFrame * m_doc->speed_factor );
	}
	
	static int cnt =0;;
	if (m_doc->m_anim || cnt < 3)
	{
		Ogre::Entity* ent = mSceneMgr->getEntity("Player");
		const Ogre::AxisAlignedBox& box = ent->getWorldBoundingBox();
		Ogre::Vector3 min = box.getMinimum();
		Ogre::Vector3 max = box.getMaximum();
		//std::cout << "min "<<min.x << " " << min.y << " " << min.z << "\n";
		if (max.x >m_doc->max_z)
		{
			m_doc->max_z = max.x;
		}
		
		if (max.x <m_doc->min_z)
		{
			m_doc->min_z = max.x;
		}
		cnt++;
		if (cnt ==3)
		{
			m_doc->ext_z= std::max(max.z,-min.z);
			m_doc->ext_x= std::max(max.x,-min.x);
			
		}
	}

	return mContinue;
}

    // MouseListener
bool SumWarsFrameListener::mouseMoved(const OIS::MouseEvent &e)
{
	if (m_middle || e.state.Z.rel!=0)
	{
		m_dist += e.state.Z.rel*0.01;
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

		m_camera->setPosition(Ogre::Vector3(m_dist*cos(th)*cos(ph), m_dist*sin(th),  - m_dist*cos(th)*sin(ph)));
		m_camera->lookAt(Ogre::Vector3(0,1,0));


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
			file << "range "<< (m_doc->max_z-25) << "\n";
			file << "extent "<< (m_doc->ext_x)<<" "<<(m_doc->ext_z) << "\n";
			file.close();
			break;

		case OIS::KC_0:
			m_doc->m_anim = !m_doc->m_anim;
			break;

		case OIS::KC_1:
			m_doc->mAnimationState->addTime(m_doc->mAnimationState->getLength ()*0.1);
			printf("animation %f \n",m_doc->mAnimationState->getTimePosition()/ m_doc->mAnimationState->getLength ());
			break;

        case OIS::KC_2:
			m_doc->mAnimationState->addTime(m_doc->mAnimationState->getLength ()*0.01);
			printf("animation %f \n",m_doc->mAnimationState->getTimePosition()/ m_doc->mAnimationState->getLength ());
            break;
			
        case OIS::KC_9:
			m_doc->mAnimationState->setTimePosition(0.0);
            break;
		
		case OIS::KC_5:
			m_doc->speed_factor*=0.8;

            break;
			
		case OIS::KC_6:
			m_doc->speed_factor/=0.8;

			break;
			
        default:
            break;

	}
	return true;
}



