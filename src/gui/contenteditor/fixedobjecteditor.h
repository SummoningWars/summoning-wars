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

#ifndef FIXEDOBJECT_EDITOR_TAB_H
#define FIXEDOBJECT_EDITOR_TAB_H

#include "contenteditortab.h"
#include "worldobject.h"
#include "objectloader.h"

/**
 * \brief Editor class for FixedObject XML
 */
class FixedObjectEditor : public ContentEditorTab
{
	public:
		/**
		 * \brief Initialises the panel
		 * \param parent parent CEGUI window
		 */
		virtual void init(CEGUI::Window* parent);
		
		/**
		 * \brief Updates the content of the tab
		 * Function is called once each tick.
		 */
		virtual void update();
		
	private:
		
		/**
		 * \brief is called if the list of all FixedObjects has been modified
		 */
		void updateAllFixedObjectList();
		
		/**
		 * \param evt CEGUI event arguments
		 * \brief Called upon changing any element FixedObject tab
		 */
		bool onFixedObjectModified(const CEGUI::EventArgs& evt);
		
		/**
		 * \param evt CEGUI event arguments
		 * \brief Called upon submitting XML for FixedObject
		 */
		bool onFixedObjectXMLModified(const CEGUI::EventArgs& evt);
		
		/**
		 * \param evt CEGUI event arguments
		 * \brief Called upon clicking the autodetect size button
		 */
		bool onFixedObjectAutodetectSize(const CEGUI::EventArgs& evt);
		
		/**
		 * \param evt CEGUI event arguments
		 * \brief Called upon clicking create Fixed Object Button
		 */
		bool onFixedObjectCreate(const CEGUI::EventArgs& evt);
		
		/**
		 * \param evt CEGUI event arguments
		 * \brief Called upon clicking the Copy Data from existing object Button
		 */
		bool onCopyData(const CEGUI::EventArgs& evt);
		
		
		/**
		 * \param evt CEGUI event arguments
		 * \brief Called upon clicking the delete all objects Button
		 */
		bool onDelAllObjects(const CEGUI::EventArgs& evt);
		
		/**
		 * \param evt CEGUI event arguments
		 * \brief Called upon clicking the get player position Button
		 */
		bool onGetPlayerPosition(const CEGUI::EventArgs& evt);
		
		/**
		 * \brief updates the content of the FixedObject XML editor
		 */
		void updateFixedObjectXML();
		
		/**
		 * \brief updates the content of the FixedObject XML editor
		 */
		void updateFixedObjectEditor();
		
		
		
		/**
		 * \brief marks if the fixed object data has been modified
		 */
		bool m_modified_fixed_object;
		
		/**
		 * \brief marks that the XML of the fixed object data has been modified
		 */
		bool m_modified_fixed_object_xml;
		
		/**
		 * \brief marks whether the list of all fixed objects is dirty
		 */
		bool m_modified_fixed_object_list;
		
		/**
		 * \brief FixedObject currently edited
		 */
		FixedObjectData m_edited_fixed_object;
		
		/**
		 * \brief XML representation of the  FixedObject edited with this window
		 */
		TiXmlDocument m_fixed_object_xml; 
		
		/**
		 * \brief List of all created objects
		 * The first integer is the region ID, the second is the object ID
		 */
		std::list< std::pair<int,int> > m_created_objects;
};


#endif