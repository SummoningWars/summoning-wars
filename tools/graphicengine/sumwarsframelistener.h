#ifndef SUMWARSFRAMELISTENER_H
#define SUMWARSFRAMELISTENER_H

#include<iostream>
#include "ExampleFrameListener.h"
#include "document.h"


class SumWarsFrameListener : public ExampleFrameListener, public OIS::MouseListener, public OIS::KeyListener
{
	private:
		/**
		 *\var Document* m_doc
		 * \brief Das Dokument
		 */
		Document* m_doc;
		Ogre::Vector3* mouseVector;
		const Ogre::Vector3* mousePosition;
		Ogre::Entity* ent;
		Ogre::Entity* went;
		Ogre::Real timeslide;


	public:
		/**
	 	 * \fn SumWarsFrameListener(RenderWindow* win, Camera* cam, SceneManager *sceneMgr, Document* doc)
		 * \brief Der Konstruktor
		 * \param doc Zeiger auf das Dokument
		 */
		SumWarsFrameListener(Ogre::RenderWindow* win, Ogre::Camera* cam, Ogre::SceneManager *sceneMgr, Document* doc);

		bool frameStarted(const Ogre::FrameEvent &evt);

		bool mouseMoved(const OIS::MouseEvent &e);

		bool mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id);

		bool keyReleased(const OIS::KeyEvent &e)
		{
			return true;
		}

		bool mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id);
		
		bool keyPressed(const OIS::KeyEvent &e);

	protected:
		Ogre::SceneManager *mSceneMgr;   // The current SceneManager
		Ogre::SceneNode *mCamNode;   // The SceneNode the camera is currently attached to

		bool mContinue;        // Whether to continue rendering or not
		
		bool m_middle;
		
		float m_dist;
		float m_theta;
		float m_phi;
		
		Ogre::Camera* m_camera;
		
};

#endif

