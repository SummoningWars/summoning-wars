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

#include <OgreSceneNode.h>
#include <OgreMesh.h>
#include <OgreEntity.h>

#include "contenteditortab.h"

// Utility for CEGUI cross-version compatibility
#include "ceguiutility.h"

void ContentEditorTab::setWindowText(std::string windowname, std::string text)
{
	CEGUI::utf8* str = (CEGUI::utf8*) (text.c_str());
	//CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
  CEGUI::Window* win = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, windowname);
	
	if (win != 0)
	{
		if (win->getText() != str)
		{
			win->setText(str);
		}
	}
}

std::string ContentEditorTab::getWindowText(std::string windowname, std::string def)
{
	//CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	CEGUI::Window* win = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, windowname);
	
	if (win != 0)
	{
		return win->getText().c_str();
	}
	return def;
}

void ContentEditorTab::setSpinnerValue(std::string windowname, double value)
{
	//CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	CEGUI::Spinner* spinner =  static_cast<CEGUI::Spinner*>(CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, windowname));
	
	if (spinner != 0)
	{
		if (spinner->getCurrentValue() != value)
		{
			spinner->setCurrentValue(value);
		}
	}
}

double ContentEditorTab::getSpinnerValue(std::string windowname, double def)
{
	//CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
  CEGUI::Spinner* spinner =  static_cast<CEGUI::Spinner*>(CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, windowname));
	
	if (spinner != 0)
	{
		return spinner->getCurrentValue();
	}
	return def;
}

void ContentEditorTab::setCheckboxSelected(std::string windowname, bool state)
{
	//CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	CEGUIUtility::ToggleButton* box = static_cast<CEGUIUtility::ToggleButton*>(CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, windowname));
	
	if (box != 0)
	{
		if (box->isSelected() != state)
		{
			box->setSelected(state);
		}
	}
}

bool ContentEditorTab::getCheckboxSelected(std::string windowname)
{
	//CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
  CEGUIUtility::ToggleButton* box = static_cast<CEGUIUtility::ToggleButton*>(CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, windowname));
	
	if (box != 0)
	{	
		return  box->isSelected();
	}
	return false;
}

std::string ContentEditorTab::getComboboxSelection(std::string windowname, std::string def)
{
	//CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
  CEGUI::Combobox* box = static_cast<CEGUI::Combobox*>(CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, windowname));
	
	if (box != 0)
	{
		CEGUI::ListboxItem* item = box->getSelectedItem();
		if (item != 0)
		{
			return item->getText().c_str();
		}
	}
	return def;
}

void ContentEditorTab::setComboboxSelection(std::string windowname, std::string selection)
{
	//CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	CEGUI::Combobox* box = static_cast<CEGUI::Combobox*>(CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, windowname));
	
	if (box != 0)
	{
		box->setText(selection.c_str());
		CEGUI::ListboxItem* sel = box->findItemWithText(CEGUI::String(selection.c_str()),0);
		if (sel != 0)
		{
			int id = sel->getID();
			box->setSelection(id,id);
		}
	}
}

void ContentEditorTab::setMultiLineEditboxCursor(std::string windowname, int row, int col)
{
	//CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	CEGUI::MultiLineEditbox* editor = static_cast<CEGUI::MultiLineEditbox*>(CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, windowname));
	
	if (editor != 0)
	{
		int r =1, c = 1;	
		size_t pos = 0;	// cursor position found
		const CEGUI::String& text = editor->getText();
		while (pos < text.size())
		{
			// second condition ensures, that cursor is placed on the end,
			// if the row err_row is shorter than err_col for some reason
			if ((r == row && c == col)
				|| (r > row))
			{
				break;
			}
			
			if (text.compare(pos,1,"\n") == 0)
			{
				r++;
				col = 1;
			}
			else
			{
				c++;
			}
			pos++;
		}
		
    editor->setCaretIndex(pos);
		editor->ensureCaretIsVisible();
		editor->activate();
	}
}


void ContentEditorTab::getNodeBounds(const Ogre::SceneNode* node, Ogre::Vector3& minimum, Ogre::Vector3& maximum, int level)
{
	const Ogre::AxisAlignedBox& boundingbox = node->_getWorldAABB();
	
	// current node boundss
	Ogre::Vector3 bbox_min = boundingbox.getMinimum();
	Ogre::Vector3 bbox_max = boundingbox.getMaximum();
	
	std::string indent = "";
	for (int i=0; i<level; i++)
		indent += "  ";
	
	
	// SubNodes
	Ogre::SceneNode::ConstChildNodeIterator child_it = node->getChildIterator();
	while (child_it.hasMoreElements())
	{
		const Ogre::SceneNode* subnode = dynamic_cast<Ogre::SceneNode*>(child_it.getNext());
		if (subnode != 0)
		{
			getNodeBounds(subnode,minimum,maximum,level+1);
		}
	}
	
	// Entities
	indent += "  ";
	Ogre::SceneNode::ConstObjectIterator attch_obj_it = node->getAttachedObjectIterator();
	while (attch_obj_it.hasMoreElements())
	{
		const Ogre::Entity* ent = dynamic_cast<Ogre::Entity*>(attch_obj_it.getNext());
		if (ent != 0)
		{
			const Ogre::AxisAlignedBox  & box = ent->getMesh()->getBounds();
			Ogre::Vector3 box_min = box.getMinimum();
			Ogre::Vector3 box_max = box.getMaximum();
			
			box_min = ent->getParentSceneNode()->_getFullTransform() * box_min;
			box_max = ent->getParentSceneNode()->_getFullTransform() * box_max;
			/*
			 *			std::cout << indent << "Entity: " << ent->getName() << "\n";
			 *			std::cout << indent << box_min[0] << " " << box_min[1] << " " <<  box_min[2] << "\n";
			 *			std::cout << indent << box_max[0] << " " << box_max[1] << " " <<  box_max[2] << "\n";
			 */
			for (int i=0; i<3; i++)
			{
				minimum[i] = MathHelper::Min(minimum[i], box_min[i]);
				maximum[i] = MathHelper::Max(maximum[i], box_max[i]);
			}
		}
	}
	/*
	 *	std::cout << indent << "Node: " << node->getName() << "\n";
	 *	std::cout << indent << minimum[0] << " " << minimum[1] << " " <<  minimum[2] << "\n";
	 *	std::cout << indent << maximum[0] << " " << maximum[1] << " " <<  maximum[2] << "\n";
	 */
}