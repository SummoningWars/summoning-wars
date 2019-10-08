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

#include "graphicobjectrenderinfo.h"

#include "graphicmanager.h"


void MovableObjectInfo::writeToXML(TiXmlNode* node)
{
	TiXmlElement* elem = node->ToElement();
	if (elem == 0) 
		return;
	
	// This should be done somewhat more elegant x(
	if (m_type == MovableObjectInfo::ENTITY)
	{
		elem->SetValue("Entity");
	}
	else if (m_type == MovableObjectInfo::PARTICLE_SYSTEM)
	{
		elem->SetValue("ParticleSystem");
	}	
	else if (m_type == MovableObjectInfo::BILLBOARD_SET)
	{
		elem->SetValue("BillboardSet");
	}	
	else if (m_type == MovableObjectInfo::BILLBOARD_CHAIN)
	{
		elem->SetValue("BillboardChain");
	}	
	else if (m_type == MovableObjectInfo::SUBOBJECT)
	{
		elem->SetValue("Subobject");
	}	
	else if (m_type == MovableObjectInfo::SOUNDOBJECT)
	{
		elem->SetValue("Soundobject");
	}	
	
	if (m_objectname != "mainmesh" && m_objectname != "")
	{
		elem->SetAttribute("objectname", m_objectname.c_str());
	}
	else
	{
		elem->RemoveAttribute("objectname");
	}
	
	elem->SetAttribute("source", m_source.c_str());
	
	
	if (m_bone != "")
	{
		elem->SetAttribute("bone", m_bone.c_str());
	}
	else
	{
		elem->RemoveAttribute("bone");
	}
	
	if (m_scale != 1.0)
	{
		elem->SetDoubleAttribute("scale", m_scale);
	}
	else
	{
		elem->RemoveAttribute("scale");
	}
	
	if (m_position[0]!=0 || m_position[1]!=0 || m_position[2]!=0)
	{
		std::stringstream stream;
		stream << m_position[0] << " " <<  m_position[1] << " " << m_position[2];
		elem->SetAttribute("position", stream.str().c_str());
	}
	else
	{
		elem->RemoveAttribute("position");
	}
	
	if (m_rotation[0]!=0 || m_rotation[1]!=0 || m_rotation[2]!=0)
	{
		std::stringstream stream;
		stream << m_rotation[0] << " " <<  m_rotation[1] << " " << m_rotation[2];
		elem->SetAttribute("rotation", stream.str().c_str());
	}
	else
	{
		elem->RemoveAttribute("rotation");
	}
	
	if (m_highlightable != true)
	{
		elem->SetAttribute("highlightable", "false");
	}
	else
	{
		elem->RemoveAttribute("highlightable");
	}

}

GraphicRenderInfo::GraphicRenderInfo(std::string parent)
	:m_objects(),m_action_infos(), m_action_references()
{
	m_parent = parent;
	m_parent_ptr =0;
	m_inherit_mask = INHERIT_ALL;
}

GraphicRenderInfo::~GraphicRenderInfo()
{
	std::multimap<std::string, ActionRenderInfo*>::iterator it;
	for (it =m_action_infos.begin(); it != m_action_infos.end(); ++it)
	{
		delete	it->second;
	}
}

void GraphicRenderInfo::clear()
{
	std::multimap<std::string, ActionRenderInfo*>::iterator it;
	for (it =m_action_infos.begin(); it != m_action_infos.end(); ++it)
	{
		delete	it->second;
	}
	
	m_action_references.clear();
	m_action_infos.clear();
	m_objects.clear();
	m_parent_ptr = 0;
}

MovableObjectInfo* GraphicRenderInfo::getObject(std::string objectname)
{
	for (std::list<MovableObjectInfo>::iterator it = m_objects.begin(); it != m_objects.end(); ++it)
	{
		if (it->m_objectname == objectname)
			return &(*it);
	}
	return 0;
}

void GraphicRenderInfo::removeObject(std::string objectname)
{
	for (std::list<MovableObjectInfo>::iterator it = m_objects.begin(); it != m_objects.end(); ++it)
	{
		if (it->m_objectname == objectname)
		{
			m_objects.erase(it);
			return;
		}
	}
}

ActionRenderInfo* GraphicRenderInfo::getActionRenderInfo(std::string action,int random_action_nr)
{
	ActionRenderInfo* info = getInheritedActionRenderInfo(action, random_action_nr);
	
	// Falls keine Informationen gefunden: Aktion reduzieren (Anhaenge mit # weglassen)
	size_t pos = action.find_last_of('#');
	while (info == 0 && pos != std::string::npos)
	{
		action.erase(pos);
		DEBUGX("reduced action %s",action.c_str());
		
		info = getInheritedActionRenderInfo(action,random_action_nr);
		pos = action.find_last_of('#');
	}
	return info;
}

ActionRenderInfo* GraphicRenderInfo::getOwnActionRenderInfo(std::string action,int random_action_nr)
{
	std::multimap<std::string, ActionRenderInfo*>::iterator it,jt;
	
	it = m_action_infos.lower_bound(action);
	jt = m_action_infos.upper_bound(action);

	// keine Daten vorhanden
	if ( m_action_infos.count(action) ==0)
	{
		// pruefen, ob eine Referenz existiert
		std::map<std::string, std::string>::iterator rt;
		rt = m_action_references.find(action);
		if (rt != m_action_references.end() && rt->second != action)
		{
			return getInheritedActionRenderInfo(rt->second, random_action_nr);
		}
		else
		{
			return 0;
		}
	}

	// unter allen gleichmaessig einen auswuerfeln
	ActionRenderInfo* info;

	int count =0;
	std::multimap<std::string, ActionRenderInfo*>::iterator nt = it;
	while (nt != jt)
	{
		count ++;
		++nt;
	}
	
	int nr = random_action_nr % count;
	for (int i=0; i<nr; i++)
	{
		++it;
	}
	info = it->second;

	return info;	
}

ActionRenderInfo* GraphicRenderInfo::getInheritedActionRenderInfo(std::string action,int  random_action_nr)
{
	DEBUGX("get info for %s (%p)",action.c_str(),this);
	ActionRenderInfo*  info = getOwnActionRenderInfo(action, random_action_nr);
	if (info ==0 && m_parent !="")
	{
		if (m_parent_ptr == 0)
		{
			m_parent_ptr = GraphicManager::getRenderInfo(m_parent);
			DEBUGX("parent render info %s %p",m_parent.c_str(), m_parent_ptr);
		}
		
		if (m_parent_ptr != 0)
		{
			info = m_parent_ptr->getInheritedActionRenderInfo(action, random_action_nr);
		}
	}
	return info;
}

GraphicRenderInfo* GraphicRenderInfo::getParentInfo()
{
	if (m_parent =="")
		return 0;
	
	if (m_parent_ptr == 0)
	{
		m_parent_ptr = GraphicManager::getRenderInfo(m_parent);
	}
	return m_parent_ptr;
	
}

bool GraphicRenderInfo::checkActionInheritMask(ActionRenderpart::Type arpart)
{
	if (m_inherit_mask == INHERIT_ALL)
		return true;
	
	unsigned int mask = 0;
	if (arpart == ActionRenderpart::ANIMATION)
		mask = INHERIT_ANIMATION;
	if (arpart == ActionRenderpart::ROTATION)
		mask = INHERIT_ROTATION;
	if (arpart == ActionRenderpart::MOVEMENT)
		mask = INHERIT_MOVEMENT;
	if (arpart == ActionRenderpart::SCALE)
		mask = INHERIT_SCALE;
	if (arpart == ActionRenderpart::SOUND)
		mask = INHERIT_SOUND;
	
	return checkInheritMask(mask);
}


void GraphicRenderInfo::writeToXML(TiXmlNode* node)
{
	// make a map of all Subobjects by Name
	// this also has the character of a todo list
	// all updated structures are deleted from the list
	std::map<std::string, MovableObjectInfo*> objects;
	for (std::list<MovableObjectInfo>::iterator it = m_objects.begin(); it != m_objects.end(); ++it)
	{
		std::string objectname = it->m_objectname;
		if (objectname == "")
		{
			objectname = "mainmesh";
		}
		objects[objectname] = &(*it);
	}
	
	std::list<TiXmlElement*> delete_list;
	
	// loop over existing subobjects
	TiXmlElement* child;
	// point where to insert additional subobjects
	TiXmlElement* insert_point = 0; 
	for ( child = node->FirstChildElement(); child != 0; child = child->NextSiblingElement())
	{
		if ((!strcmp(child->Value(), "Entity")) || (!strcmp(child->Value(), "ParticleSystem")) || (!strcmp(child->Value(), "BillboardSet")) || (!strcmp(child->Value(), "BillboardChain")) || (!strcmp(child->Value(), "Subobject")) || (!strcmp(child->Value(), "Soundobject")))
		{
			// check if the Objekt is still present in the current map
			std::string objectname = "mainmesh";
			const char* attr = child->Attribute("objectname");
			if (attr != 0)
				objectname = attr;
			if (objects.count(objectname) > 0)
			{
				// exists, update
				objects[objectname]->writeToXML(child);
				objects.erase(objectname);
				insert_point = child;
			}
			else
			{
				// mark for deletion
				delete_list.push_back(child);
			}
		}
	}
	
	// now delete the obsolete child nodes
	for (std::list<TiXmlElement*>::iterator it = delete_list.begin(); it != delete_list.end(); ++it)
	{
		node->RemoveChild(*it);
	}
	
	// insert new Childs for all newly added subobjects
	TiXmlElement inserter("dummy");	// dummy node just for being compied by tinyxml x(
	TiXmlElement* newobject;
	for (std::map<std::string, MovableObjectInfo*>::iterator it =  objects.begin(); it != objects.end(); ++it)
	{
		// normally, insert after the last subobject
		// special cases for empty node and no other subobject found so far
		if (insert_point == 0)
		{
			TiXmlElement* first = node->FirstChildElement();
			if (first == 0)
			{
				newobject = node->InsertEndChild(inserter)->ToElement();
			}
			else
			{
				newobject = node->InsertBeforeChild(first,inserter)->ToElement();
			}
		}
		else
		{
			newobject = node->InsertAfterChild(insert_point, inserter)->ToElement();
		}
		
		// write the subobject
		it->second->writeToXML(newobject);
		insert_point = newobject;
	}
}