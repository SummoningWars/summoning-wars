/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


// Utility for CEGUI cross-version compatibility
#include "ceguiutility.h"

// needed to be able to create the CEGUI renderer interface
#ifdef CEGUI_07
#include "CEGUI/RendererModules/Ogre/CEGUIOgreRenderer.h"
#include "CEGUI/RendererModules/Ogre/CEGUIOgreTexture.h"
#include "CEGUI/RendererModules/Ogre/CEGUIOgreResourceProvider.h"
#else
#include "CEGUI/RendererModules/Ogre/Renderer.h"
#include "CEGUI/RendererModules/Ogre/Texture.h"
#include "CEGUI/RendererModules/Ogre/ResourceProvider.h"
#endif

#include "scene.h"

#include "minimapwindow.h"

#include "OgreResource.h"

MinimapWindow::MinimapWindow (Document* doc, const std::string& ceguiSkinName)
	: Window (doc)
	, m_ceguiSkinName (ceguiSkinName)
	, m_reloadIconsOnNextUpdate (true)
{
	m_region_id = -1;
	DEBUGX("setup main menu");

	// Create GUI Items.
	CEGUI::Window* minimapWnd = CEGUIUtility::loadLayoutFromFile ("minimapwindow.layout");
	if (!minimapWnd)
	{
		SW_DEBUG ("WARNING: Failed to load [%s]", "minimapwindow.layout");
	}

	CEGUI::Window* minimapWnd_holder = CEGUIUtility::loadLayoutFromFile ("minimapwindow_holder.layout");
	if (!minimapWnd_holder)
	{
		SW_DEBUG ("WARNING: Failed to load [%s]", "minimapwindow_holder.layout");
	}

	SW_DEBUG ("Placing layout into holder");
	minimapWnd->setVisible (true);
	minimapWnd_holder->setVisible (true);

	CEGUI::Window* wndHolder = CEGUIUtility::getWindowForLoadedLayoutEx (minimapWnd_holder, "MinimapWindow_Holder");
	CEGUI::Window* wndHeld = CEGUIUtility::getWindowForLoadedLayoutEx (minimapWnd, "MinimapWindow");
	if (wndHolder && wndHeld)
	{
		CEGUIUtility::addChildWidget (wndHolder, wndHeld);
	}
	else
	{
		if (!wndHolder) SW_DEBUG ("ERROR: Unable to get the window holder for inventory.");
		if (!wndHeld) SW_DEBUG ("ERROR: Unable to get the window for inventory.");
	}
	m_window = minimapWnd_holder;

	/*
	Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().create("RttMat", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	Ogre::Technique *technique = material->createTechnique();
	technique->createPass();
	material->getTechnique(0)->getPass(0)->setLightingEnabled(false);
	material->getTechnique(0)->getPass(0)->createTextureUnitState("minimap_tex");
	
	Ogre::Overlay* overlay = Ogre::OverlayManager::getSingleton().create("minimapOverlay");
	Ogre::OverlayContainer* container = (Ogre::OverlayContainer*) Ogre::OverlayManager::getSingleton().createOverlayElement("Panel", "minimapContainer");
	overlay->add2D(container);
	container->setMaterialName("RttMat");
	container->setDimensions(0.8, 0.8);
	//textArea->setMetricsMode(GMM_PIXELS);
	container->setPosition(0.1, 0.1);
	overlay->show();
	*/
}


/**
 * \fn virtual void reloadIconsOnNextUpdate ()
 * \brief Makes sure that at the next update, minimap icons are reloaded.
 */
void MinimapWindow::reloadIconsOnNextUpdate ()
{
	m_reloadIconsOnNextUpdate = true;
}



void MinimapWindow::update()
{
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	CEGUI::FrameWindow* minimap = (CEGUI::FrameWindow*) CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "MinimapWindow");

	Player* player = m_document->getLocalPlayer();
	if (player ==0)
		return;
	
	Region* region = player->getRegion();
	if (region ==0)
		return;
	
	const WorldObjectMap& players = region->getPlayers();
	WorldObjectMap::const_iterator it;
	WorldObject* pl;

	// Window counter
	static int ncount =0;
	
	CEGUI::Window* label;
	std::ostringstream stream;
	
	int cnt =0;
	float relx, rely;
	float alpha;
	for (it = players.begin(); it != players.end(); ++it)
	{
		pl = it->second;
		stream.str("");
		stream << "PlayerMinimapImage"<<cnt;
		
		if (cnt >= ncount)
		{
			label = win_mgr.createWindow (CEGUIUtility::getWidgetWithSkin (m_ceguiSkinName, "StaticImage"), stream.str());
			CEGUIUtility::addChildWidget (minimap, label);

			label->setProperty("BackgroundEnabled", "true");
			
			if (label->isPropertyPresent ("BackgroundColours"))
			{
				label->setProperty("BackgroundColours", "tl:00000000 tr:00000000 bl:00000000 br:00000000"); 
			}
			else if (label->isPropertyPresent ("BackgroundColour"))
			{
				label->setProperty("BackgroundColour", "00000000");
			}

			// Not yet sure whether it looks nicer with a frame or without it...
			// If a frame is used, a larger image size needs to be specified.

#if 1
			std::string propertyValue ("true");
#else
			std::string propertyValue ("false");
#endif

			label->setProperty("FrameEnabled", propertyValue);
			CEGUIUtility::setWidgetSizeRel (label, 0.038f, 0.038f);

			label->setMousePassThroughEnabled(true);
			Player* playerPtr = static_cast<Player*> (pl);
			if (playerPtr)
			{
				std::string playerScreenImage = playerPtr->getEmotionImage("normal");
				label->setProperty ("Image", playerScreenImage.c_str ()); 
			}

			label->setInheritsAlpha (false);
			label->setAlwaysOnTop(true);
			
			SW_DEBUG("creating Window!");
			
			ncount ++;
		}
		else
		{
			stream.str ("");
			stream << "MinimapWindow/PlayerMinimapImage";
			stream << cnt;
			label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, stream.str());
		}

		if (m_reloadIconsOnNextUpdate)
		{
			stream.str ("");
			stream << "MinimapWindow/PlayerMinimapImage";
			stream << cnt;

			if (CEGUIUtility::isWindowPresent (stream.str ().c_str ()))
			{
				label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, stream.str ().c_str ());
				Player* playerPtr = static_cast<Player*> (pl);
				if (playerPtr)
				{
					std::string playerScreenImage = playerPtr->getEmotionImage ("normal");
					label->setProperty ("Image", playerScreenImage.c_str ()); 
				}
			}
			m_reloadIconsOnNextUpdate = false;
		}

		alpha = 0.5;
		if (pl == player)
		{
			alpha = 1.0;
		}
		if (label->getAlpha() != alpha)
		{
			label->setAlpha(alpha);
		}
		
		if (region->getDimX()>region->getDimY())
		{
			relx =pl->getShape()->m_center.m_x/ region->getDimX()/4.0f - 0.015;
			rely =(pl->getShape()->m_center.m_y +2*(region->getDimX()-region->getDimY()))/ region->getDimX()/4.0f - 0.015;
		}
		else
		{
			relx =(pl->getShape()->m_center.m_x +2*(region->getDimY()-region->getDimX()))/ region->getDimY()/4 - 0.015;
			rely =pl->getShape()->m_center.m_y/ region->getDimY()/4 - 0.015;
		}
		
		DEBUGX("relx %f  rely %f",relx,rely);
		
		label->setPosition(CEGUI::UVector2(cegui_reldim(relx), cegui_reldim(rely)));
		label->setVisible(true);
		cnt++;
	}
	
	// Hide the rest of the labels.
	for (; cnt <ncount; cnt++)
	{
		stream.str("");
		stream << "MinimapWindow/PlayerMinimapImage";
		stream << cnt;
			
		label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, stream.str());
		label->setVisible(false);
	}
	
	// Check to see whether the label displaying the region name needs to be updated.
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "MinimapWindow/RegionNameLabel");
	
	std::string regionName (region->getName ());
	DEBUGX ("Got region name as: [%s]", regionName.c_str ());

	CEGUI::String actualRegionName = (CEGUI::utf8*) dgettext("sumwars",regionName.c_str());
	DEBUGX ("Got actual region name as: [%s]", actualRegionName.c_str ());

	if (actualRegionName != label->getText())
	{
		DEBUGX ("Region name update; old vs new [%s] - [%s]", label->getText().c_str (), actualRegionName.c_str ());
		label->setText (actualRegionName);
	}
	
}

