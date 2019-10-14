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

#include "graphicobject.h"
#include "graphicmanager.h"
#include "random.h"

GraphicObject::GraphicObject(Type type, GraphicRenderInfo* render_info, std::string name, int id)
	: m_attached_objects(), m_subobjects(), m_dependencies()
{
	m_id = id;
	m_type = type;
	m_render_info = render_info;
	m_name = name;
	
	m_action.m_current_action ="";
	m_action.m_current_percent=1.0;
	
	m_top_node = GraphicManager::getSceneManager()->getRootSceneNode()->createChildSceneNode();
	
	m_render_info_valid = true;
	
	m_highlight = false;
	m_exact_animations = false; // as default ?

	initContent();
}

void GraphicObject::initContent()
{
	if (m_render_info != 0)
	{
		addObjectsFromRenderInfo(m_render_info);
	}
	else
	{
		// special case: No Renderinfo, GraphicObject is a single mesh
		MovableObjectInfo mainmesh;
		mainmesh.m_source = m_type;
		mainmesh.m_type = MovableObjectInfo::ENTITY;
		addMovableObject(mainmesh);
	}
}

GraphicObject::~GraphicObject()
{
	/*
	DEBUG("dependencies");
	std::map<std::string, ObjectDependency >::iterator it;
	std::set<std::string>::iterator jt;
	
	for (it = m_dependencies.begin(); it !=  m_dependencies.end(); ++it)
	{
		DEBUG("* %s",it->first.c_str());
		for (jt = it->second.m_children.begin(); jt != it->second.m_children.end(); ++jt)
		{
			DEBUG("  %s",jt->c_str());
		}
	}
	*/
	DEBUGX("destroying %s",m_name.c_str());
	while (! m_dependencies.empty())
	{
		removeMovableObject(m_dependencies.begin()->first);
	}
	m_top_node->getCreator()->destroySceneNode(m_top_node->getName());
}

void GraphicObject::invalidateRenderInfo()
{
	m_render_info_valid = false;
	m_render_info = 0;
}

void GraphicObject::clearContent()
{
	while (! m_dependencies.empty())
	{
		removeMovableObject(m_dependencies.begin()->first);
	}
	m_attached_states.clear();
	m_action.m_arinfo = 0;
	m_action.m_active_parts.clear();
	m_action.m_current_action = "";
	m_action.m_current_percent = 1.0;
	
}

void GraphicObject::ensureRenderInfoValid()
{
	if (!m_render_info_valid)
	{
		clearContent();
		m_render_info = GraphicManager::getRenderInfo(m_type);
		
		initContent();
		m_render_info_valid = true;
	}
}

void GraphicObject::addObjectsFromRenderInfo(GraphicRenderInfo* info)
{
	// add objects from Renderinfo
	std::list<MovableObjectInfo>& objs = info->getObjects();
	std::list<MovableObjectInfo>::iterator it;
		
	for (it = objs.begin(); it != objs.end(); ++it)
	{
		addMovableObject(*it);
	}
	
	// If inherited Objects are active, add objects from parent Renderinfo recursive
	if (info->checkInheritMask(GraphicRenderInfo::INHERIT_OBJECTS))
	{
		GraphicRenderInfo* pinfo = info->getParentInfo();
		if (pinfo != 0 )
		{
			addObjectsFromRenderInfo(pinfo);
		}
	}
}

Ogre::MovableObject* GraphicObject::getMovableObject(std::string name)
{
	if (name == "")
		name ="mainmesh";
	
	if (m_attached_objects.count(name) == 0)
		return 0;
	
	return m_attached_objects[name].m_object;
}

Ogre::MovableObject* GraphicObject::getHighlightObject(std::string name)
{
	if (name == "")
		name ="mainmesh";
	
	if (m_attached_objects.count(name) == 0)
		return 0;
	
	return m_attached_objects[name].m_highlight_entity;
}

Ogre::Entity* GraphicObject::getEntity(std::string name)
{
	return dynamic_cast<Ogre::Entity*>(getMovableObject(name));
}

Ogre::Node* GraphicObject::getParentNode(std::string name)
{
	if (name =="")
		name = "mainmesh";
	
	Ogre::MovableObject* obj = getMovableObject(name);
	Ogre::Node* node=0;
	if (obj != 0)
	{
		node = obj->getParentNode();
	}
	else
	{
			
		std::map<std::string, AttachedGraphicObject >::iterator it;
		it = m_subobjects.find(name);
			
		if (it != m_subobjects.end())
		{
			if (it->second.m_tagpoint !=0)
			{
				node = it->second.m_tagpoint;
			}
			else
			{
				node = it->second.m_object->getTopNode();
			}
		}
	}
	return node;
}

void GraphicObject::setQueryMask(unsigned int mask)
{
	DEBUGX("setting mask %i to %s",mask,m_name.c_str());
	// Set mask for all entities
	std::map<std::string, AttachedMovableObject>::iterator it;
	Ogre::Entity* ent;
	for (it = m_attached_objects.begin(); it != m_attached_objects.end(); ++it)
	{
		ent = dynamic_cast<Ogre::Entity*>(it->second.m_object);
		if (ent != 0)
		{
			ent->setQueryFlags(mask);
		}
	}
	
	// Set mask for all Sub-GraphicObjects
	std::map<std::string, AttachedGraphicObject  >::iterator gt;
	GraphicObject* obj;
	for (gt = m_subobjects.begin(); gt != m_subobjects.end(); ++gt)
	{
		obj = gt->second.m_object;
		obj->setQueryMask(mask);
	}
}

void GraphicObject::addMovableObject(MovableObjectInfo& object)
{
	DEBUGX("adding object %s type %i",object.m_objectname.c_str(), object.m_type);
	
	// Determine meshname and bone 
	std::string bone;
	std::string mesh ="";
	int pos = object.m_bone.find(':');
	if (pos == -1)
	{
		bone = object.m_bone;
		if (bone != "")
		{
			mesh = "mainmesh";
		}
	}
	else
	{
		mesh = object.m_bone.substr(0,pos);
		bone = object.m_bone.substr(pos+1);
	}
	
	Ogre::Node* node=0;
	
	if (object.m_type == MovableObjectInfo::SUBOBJECT)
	{
		// insert Sub-Graphicobject
		if (m_subobjects.count(object.m_objectname) >0)
		{
			WARNING("graphicobject %s: subobject %s already exists",m_name.c_str(), object.m_objectname.c_str());
			return;
		}
		
		GraphicObject* obj = GraphicManager::createGraphicObject(object.m_source, object.m_objectname, m_id);
		// Detach the SceneTree of the new object from the global Scenetree
		node = obj->getTopNode();
		node->getParent()->removeChild(node);
		
		AttachedGraphicObject attchobj;
		attchobj.m_object = obj;
		attchobj.m_entity =0;
	
		obj->setExactAnimations(m_exact_animations);
		
		Ogre::TagPoint* tag =0;
		if (object.m_bone == "")
		{
			// attach to Topnode
			getTopNode()->addChild(node);
			
			node->setInheritScale(true);
			attchobj.m_tag_trackpoint =0;
		}
		else
		{
			// attach to a Bone
			Ogre::Entity* ent = getEntity(mesh);
			if (ent !=0)
			{
				Ogre::SkeletonInstance* skel= ent->getSkeleton();
				if (skel == 0)
				{
					ERRORMSG("could not attach %s to %s (entity has no skeleton)",object.m_objectname.c_str(),mesh.c_str());
					return;
				}

				Ogre::Bone* boneptr = skel->getBone(bone);
				if (boneptr == 0)
				{
					ERRORMSG("could not attach %s to %s (skeleton has no bone %s",object.m_objectname.c_str(),mesh.c_str(),bone.c_str());
					return;
				}
				
				tag = skel->createTagPointOnBone (boneptr);
				attchobj.m_entity = ent;
				tag->setInheritScale(false);
				tag->setInheritParentEntityScale(true);
				attchobj.m_tag_trackpoint = ent->getParentSceneNode()->createChildSceneNode();
				attchobj.m_tag_trackpoint->addChild(node);
			}
			else
			{
				ERRORMSG("could not attach %s to %s (entity does not exist)",object.m_objectname.c_str(),mesh.c_str());
				return;
			}
		}
		attchobj.m_tagpoint = tag;
		m_subobjects[object.m_objectname] = attchobj;
		DEBUGX("adding subobject %p with name %s", m_subobjects[object.m_objectname].m_object,  object.m_objectname.c_str());
	}
	else if (object.m_type == MovableObjectInfo::SOUNDOBJECT)
	{
		static int counter;
		std::stringstream sname;
		sname << m_name << ":" << counter << ":"<<object.m_objectname;
		std::string name = sname.str();
		counter++;
		
		std::map <std::string, SoundObject>::iterator it = m_soundobjects.find (object.m_objectname.c_str ());
		if (it != m_soundobjects.end ())
		{
			SW_DEBUG ("WARNING:  %s: subobject %s already exists", m_name.c_str(), object.m_objectname.c_str());
		}
		if (m_soundobjects.count(object.m_objectname) >0)
		{
			WARNING("soundobject %s: subobject %s already exists",m_name.c_str(), object.m_objectname.c_str());
			return;
		}
		
		//SoundObject* obj = SoundSystem::createSoundObject(name);
		//m_soundobjects[object.m_objectname] = obj;

		m_soundobjects[object.m_objectname] = SoundObject (name);
		//m_soundobjects[object.m_objectname] = name;
		SW_DEBUG ("setting soundobject with name [%s] to: (%s)", object.m_objectname.c_str (), name.c_str ());
	}
	else 
	{
		// create Ogre::MovableObject 
		if (m_attached_objects.count(object.m_objectname) >0)
		{
			WARNING("graphicobject %s: subobject %s already exists",m_name.c_str(), object.m_objectname.c_str());
			return;
		}
		
		std::ostringstream ostr;
		ostr << m_name << ":"<<object.m_objectname;
		
		Ogre::MovableObject* obj;
		obj = GraphicManager::createMovableObject(object,ostr.str());
		
		
		if (obj == 0)
		{
			return;
		}
		
		// attach ID to an Entity
		// This is used to determine the Object if the Entity is found by a OGRE query
		// 
		if (obj->getMovableType() == "Entity")
		{
			obj->setUserAny(Ogre::Any(m_id));
		}
		Ogre::TagPoint* tag =0;
		Ogre::Entity* ent =0;
		
		// attach object to new Node
		Ogre::SceneNode* snode = GraphicManager::getSceneManager()->getRootSceneNode()->createChildSceneNode();
		snode->getParent()->removeChild(snode);
		snode->setInheritScale(true);
		snode->attachObject(obj);
		node = snode;
		
		if (object.m_bone == "")
		{
			// attach to Topnode
			getTopNode()->addChild(node);
			DEBUGX("node %p parent %p",node,m_top_node );
			m_attached_objects[object.m_objectname].m_tag_trackpoint = 0;
		}
		else
		{
			// attach to Bone
			ent = getEntity(mesh);
			if (ent !=0)
			{
				// chase Tagpoint
				Ogre::SkeletonInstance* skel= ent->getSkeleton();
				if (skel == 0)
				{
					ERRORMSG("could not attach %s to %s (entity has no skeleton)",object.m_objectname.c_str(),mesh.c_str());
					return;
				}

				Ogre::Bone* boneptr = skel->getBone(bone);
				if (boneptr == 0)
				{
					ERRORMSG("could not attach %s to %s (skeleton has no bone %s",object.m_objectname.c_str(),mesh.c_str(),bone.c_str());
					return;
				}
				
				tag = skel->createTagPointOnBone(boneptr);
				tag->setInheritScale(false);
				tag->setInheritParentEntityScale(true);
				m_attached_objects[object.m_objectname].m_tag_trackpoint = ent->getParentSceneNode()->createChildSceneNode();
				m_attached_objects[object.m_objectname].m_tag_trackpoint->addChild(node);
			}
			else
			{
				ERRORMSG("could not attach %s to %s (entity does not exist)",object.m_objectname.c_str(),mesh.c_str());
				return;
			}
		}
		
		// Insert into list of attached Objects
		m_attached_objects[object.m_objectname].m_object_info = object;
		m_attached_objects[object.m_objectname].m_object = obj;
		m_attached_objects[object.m_objectname].m_tagpoint = tag;
		m_attached_objects[object.m_objectname].m_entity = ent;
		m_attached_objects[object.m_objectname].m_highlight_entity = 0;
		
		DEBUGX("adding movable object with name %s (%s)",object.m_objectname.c_str(),obj->getName().c_str());
	}
	
	// set initial position and Rotation
	if (node != 0)
	{
		// FIXME: This factor is used to temporarily scale down meshes until they are exported with reduced size
		double factor = 1;
		/*
		if (object.m_bone == "" && object.m_type != MovableObjectInfo::SUBOBJECT)
		{
			factor = GraphicManager::g_global_scale/50;
		}
		*/
		node->setPosition(object.m_position*GraphicManager::g_global_scale);
		node->setOrientation(1,0,0,0);
		node->pitch(Ogre::Degree(object.m_rotation[0]));
		node->yaw(Ogre::Degree(object.m_rotation[1]));
		node->roll(Ogre::Degree(object.m_rotation[2]));
		node->setScale(Ogre::Vector3(object.m_scale*factor, object.m_scale*factor, object.m_scale*factor));
		
		DEBUGX("object %s scale %f",object.m_objectname.c_str(), node->_getDerivedScale().x);
		DEBUGX("Object position %f %f %f",node->getPosition()[0], node->getPosition()[1], node->getPosition()[2]);
	}
	
	// set dependencies
	ObjectDependency& dep = m_dependencies[object.m_objectname];
	dep.m_parent = mesh;
	if (mesh != "")
	{
		m_dependencies[mesh].m_children.insert(object.m_objectname);
	}
	
}

void GraphicObject::removeMovableObject(std::string name)
{
	DEBUGX("removing object %s",name.c_str());
	// Remove all dependant subobjects
	while (! m_dependencies[name].m_children.empty())
	{
		DEBUGX("recursion");
		removeMovableObject(*(m_dependencies[name].m_children.begin()));
	}
	
	Ogre::MovableObject* obj = getMovableObject(name);
	if (obj != 0)
	{
		// remove Movable Object
		Ogre::Node* node = obj->getParentNode();
		Ogre::SceneNode* snode = dynamic_cast<Ogre::SceneNode*>(node);
		Ogre::TagPoint* tag = dynamic_cast<Ogre::TagPoint*>(node);
		GraphicManager::detachMovableObject(obj);
		
		GraphicManager::destroyMovableObject(obj);
		
		// remove Highlight Entity
		obj = getHighlightObject(name);
		if (obj != 0)
		{
			GraphicManager::detachMovableObject(obj);
		
			GraphicManager::destroyMovableObject(obj);
		}
		
		snode->getCreator()->destroySceneNode(snode->getName());
		std::map<std::string, AttachedMovableObject>::iterator it;
		it = m_attached_objects.find(name);
		
		tag = it->second.m_tagpoint;	
		if (tag !=0 && it->second.m_entity!=0)
		{
			DEBUGX("tag %p parent %p",tag, it->second.m_entity);
			it->second.m_entity->getSkeleton()->freeTagPoint(tag);
		}
		
		if (it->second.m_tag_trackpoint != 0)
		{
			it->second.m_tag_trackpoint->getCreator()->destroySceneNode(it->second.m_tag_trackpoint->getName());
		}
		
		if (it != m_attached_objects.end())
		{
			m_attached_objects.erase(it);
		}
	}
	else if (m_soundobjects.find(name) != m_soundobjects.end())
	{
		//SoundSystem::deleteSoundObject(m_soundobjects[name]);
		// should be deleted automatically by the sound manager
		m_soundobjects.erase(name);
		DEBUGX("removing Soundobject %s",name.c_str());
	}
	else
	{
		// remove Graphicobject
		std::map<std::string, AttachedGraphicObject >::iterator jt;
		jt = m_subobjects.find(name);
		if (jt != m_subobjects.end())
		{
			Ogre::TagPoint* tag;
			GraphicObject* obj;
	
			tag = jt->second.m_tagpoint;
			obj = jt->second.m_object;
			
			if (tag !=0 && jt->second.m_entity!=0)
			{
				DEBUGX("tag %p parent %p",tag, jt->second.m_entity);
				jt->second.m_entity->getSkeleton()->freeTagPoint(tag);
			}
			GraphicManager::destroyGraphicObject(obj);
			
			if (jt->second.m_tag_trackpoint != 0)
			{
				jt->second.m_tag_trackpoint->getCreator()->destroySceneNode(jt->second.m_tag_trackpoint->getName());
			}
			
			m_subobjects.erase(jt);
		}
	}
	
	// inform parent to remove dependency to this object
	if (m_dependencies[name].m_parent != "")
	{
		DEBUGX("removing dependency %s -> %s",m_dependencies[name].m_parent.c_str(), name.c_str());
		m_dependencies[m_dependencies[name].m_parent].m_children.erase(name);
	}
	m_dependencies.erase(name);
	
}

void GraphicObject::initAttachedAction(AttachedAction& attchaction, std::string action)
{
	if (m_own_random_number)
	{
		m_random_action_number = Random::randrangei(1,1000000);
	}
	
	attchaction.m_current_action = action;
	attchaction.m_current_percent = -0.00001; // this ensures that 0.0 is already a *step forward*
	
	// Find info how to render this action
	attchaction.m_arinfo = m_render_info->getOwnActionRenderInfo(action,m_random_action_number);
	attchaction.m_inherited = false;
	
	if (attchaction.m_arinfo == 0)
	{
		attchaction.m_arinfo = m_render_info->getActionRenderInfo(action,m_random_action_number);
		attchaction.m_inherited = true;
	}
	
	if (attchaction.m_arinfo !=0)
	{
		attchaction.m_time = attchaction.m_arinfo->m_time;
		DEBUGX("action %s  time %f",action.c_str(), attchaction.m_time);
	}
}

void GraphicObject::updateAction(std::string action, float percent, int random_action_nr)
{
	ensureRenderInfoValid();
	
	if (m_render_info ==0)	// static mesh
		return;
	
	if (random_action_nr != 0)
	{
		m_random_action_number = random_action_nr;
		m_own_random_number = false;
	}
	else
	{
		m_own_random_number = true;
	}
	
	// update own Action
	DEBUGX("update %s action %s %f",m_name.c_str(),action.c_str(),percent);
	updateAttachedAction(m_action,action,percent);
	
	// update Sub-Graphicobjects recursively
	std::map<std::string, AttachedGraphicObject  >::iterator gt;
	GraphicObject* obj;
	for (gt = m_subobjects.begin(); gt != m_subobjects.end(); ++gt)
	{
		obj = gt->second.m_object;
		obj->updateAction(action,percent, m_random_action_number);
	}
}

void GraphicObject::updateAttachedAction(AttachedAction& attchaction, std::string action, float percent)
{
	if (action != attchaction.m_current_action || percent < attchaction.m_current_percent)
	{
		// Name of action changed or action percent *jumped backwards*
		// so precious action was apparently completed
		
		// complete previous action
		if (attchaction.m_current_percent<1.0)
		{
			updateAttachedAction(attchaction, attchaction.m_current_action, 1.0);
		}
		
		// remove Renderparts of this action
		while (!attchaction.m_active_parts.empty())
		{
			removeActiveRenderPart(attchaction.m_active_parts.front());
			attchaction.m_active_parts.pop_front();
		}
		
		// init new action
		initAttachedAction(attchaction,action);
	}
	
	DEBUGX("update action %s %f -> %f in %s", action.c_str(), attchaction.m_current_percent, percent,m_name.c_str());
	
	
	ActionRenderInfo* arinfo;
	arinfo = attchaction.m_arinfo;
	
	if (arinfo !=0)
	{
		// search for newly activated renderparts and new Objects
		// loop over all renderparts
		std::list< ActionRenderpart >::iterator it;
		for (it = arinfo->m_renderparts.begin(); it != arinfo->m_renderparts.end(); ++it)
		{
			DEBUGX("render part %s",it->m_animation.c_str());
			// check if part was inactive on last update and is active now
			if (attchaction.m_current_percent < it->m_start_time && percent >= it->m_start_time)
			{
				if (attchaction.m_inherited == false || m_render_info->checkActionInheritMask(it->m_type))
				{
					DEBUGX("action %s: adding part %s",action.c_str(), it->m_animation.c_str());
				
					addActiveRenderPart(&(*it));
					// Detach and Visibility have only an instant effect
					if (it ->m_type != ActionRenderpart::DETACH && it ->m_type != ActionRenderpart::VISIBILITY)
					{
						attchaction.m_active_parts.push_back(&(*it));
					}
				}
			}
		}
		
		
		// search for new graphic objects
		if (attchaction.m_inherited == false || m_render_info->checkInheritMask(GraphicRenderInfo::INHERIT_OBJECTS))
		{
			std::list< std::pair<float,MovableObjectInfo> >:: iterator mt;
			for (mt = arinfo->m_new_objects.begin(); mt != arinfo->m_new_objects.end(); ++mt)
			{
				if (attchaction.m_current_percent < mt->first && percent >= mt->first)
				{
					DEBUGX("adding object %s",mt->second.m_objectname.c_str());
					addMovableObject(mt->second);
				}
			}
		}
		
		// update active renderparts
		std::list<ActionRenderpart*>::iterator jt;
		float relpercent;
		for (jt = attchaction.m_active_parts.begin(); jt !=attchaction.m_active_parts.end(); )
		{
			if (percent >= (*jt)->m_end_time)
			{
				removeActiveRenderPart(*jt);
				jt = attchaction.m_active_parts.erase(jt);
			}
			else
			{
				relpercent = (percent - (*jt)->m_start_time) / ( (*jt)->m_end_time -(*jt)->m_start_time);
				updateRenderPart(*jt, relpercent);
				++jt;
			}
		}
		
	}
	else if (action!= "")
	{
		DEBUGX("no action render information for %s",action.c_str());
	}
	
	attchaction.m_current_percent = percent;
}

void GraphicObject::updateState(std::string state, bool active)
{
	ensureRenderInfoValid();
	
	if (m_render_info ==0)	// static mesh
		return;

	std::string actionname = state;
	std::string prefix;
	std::map<std::string,  AttachedState>::iterator it;
	it = m_attached_states.find(state);

	if (active)
	{
		prefix = "activate:";
		prefix += actionname;
		actionname = prefix;
		
		if (it == m_attached_states.end() || it->second.m_type == AttachedState::DEACTIVATE)
		{
			DEBUGX("activated state %s",state.c_str());
			// state is not set so far
			AttachedState& astate  = m_attached_states[state];
			astate.m_type = AttachedState::ACTIVATE;
			updateAttachedAction(astate.m_attached_action, actionname, 0.0);
		}
	}
	else
	{
		// Status is inactive
		if (it == m_attached_states.end())	// status is already inactive -> do nothing
			return;
		
		if (it->second.m_type != AttachedState::DEACTIVATE)
		{
			
			DEBUGX("deactivated state %s",state.c_str());
			// state should be deactivated
			// but before deleting it internally, deactivate animation is shown
			prefix = "deactivate:";
			prefix += actionname;
			actionname = prefix;
			
			updateAttachedAction(it->second.m_attached_action, actionname, 0.0);
			it->second.m_type = AttachedState::DEACTIVATE;
		}
	}
	
	// set state for Subobjects recursively
	std::map<std::string, AttachedGraphicObject  >::iterator gt;
	GraphicObject* obj;
	for (gt = m_subobjects.begin(); gt != m_subobjects.end(); ++gt)
	{
		obj = gt->second.m_object;
		obj->updateState(state,active);
	}
}

void GraphicObject::updateAllStates(std::set<std::string>& states)
{
	ensureRenderInfoValid();

	
	std::set<std::string>::iterator it = states.begin();
	std::map<std::string,  AttachedState>::iterator jt = m_attached_states.begin(),jtold;
	
	while (it != states.end() || jt != m_attached_states.end())
	{
		if (it == states.end() || (jt != m_attached_states.end() && *it > jt->first))
		{
			// deactivate state
			updateState(jt->first,false);
			++jt;
			
		}
		else if (jt == m_attached_states.end() || (it != states.end() && *it < jt->first))
		{
			// activate state
			updateState(*it,true);
			++it;
		}
		else if (*it == jt->first)
		{
			if (jt->second.m_type == AttachedState::DEACTIVATE)
			{
				updateState(*it,true);
			}
			// state is correct, do nothing
			++it;
			++jt;
		}
	}
}

bool GraphicObject::updateSubobject(MovableObjectInfo& object)
{
	std::map<std::string, AttachedGraphicObject >::iterator it;
	it = m_subobjects.find(object.m_objectname);
	
	bool ret = false;
	if (it == m_subobjects.end() && object.m_source != "")
	{
		// object does not exist -> create
		addMovableObject(object);
		ret = true;
	}
	else if (it != m_subobjects.end() && object.m_source=="")
	{
		// objects exists, but should not exist -> delete
		removeMovableObject(it->first);
		ret = true;
	}
	else if (it != m_subobjects.end () && it->second.m_object->getType() != object.m_source)
	{
		// Objects exists, but type is incorrect -> delete and recreate
		removeMovableObject(it->first);
		addMovableObject(object);
		ret = true;
	}
	
	return ret;
}

void GraphicObject::update(float time)
{
	ensureRenderInfoValid();

	
	if (m_render_info ==0)
		return;
	
	// loop over all States
	std::map<std::string,  AttachedState>::iterator it;
	float abstime;
	std::string act;
	AttachedAction* attch;
	for (it = m_attached_states.begin();it != m_attached_states.end();)
	{
		// calculate local percent for action generated from state
		attch = &(it->second.m_attached_action);
		abstime =attch->m_time * attch->m_current_percent;
		abstime += time;
		
		act = attch->m_current_action;
		
		if (abstime > attch->m_time)
		{
			// state action complete
			if (it->second.m_type == AttachedState::DEACTIVATE)
			{
				// deactivate complete -> delete
				DEBUGX("deleting attached action %s",it->first.c_str());
				updateAttachedAction(*attch,act,1.0);
				m_attached_states.erase(it++);
				continue;
			}
			else
			{
				abstime = 0;
				if (it->second.m_type == AttachedState::ACTIVATE)
				{
					// activate complete
					it->second.m_type = AttachedState::ACTIVE;
					act = it->first;
					
					std::string prefix = "active:";
					prefix += act;
					act = prefix;
				}
			}
		}
		DEBUGX("time %f  abstime %f percent %f  type %i",time,abstime,abstime/attch->m_time,it->second.m_type) ;
		
		updateAttachedAction(*attch,act,abstime/attch->m_time);
		++it;
	}
	
	// update positions of Graphicobjects attached to Bones
	std::map<std::string, AttachedGraphicObject  >::iterator gt;
	Ogre::TagPoint* tag;
	GraphicObject* obj;
	for (gt = m_subobjects.begin(); gt != m_subobjects.end(); ++gt)
	{
		tag = gt->second.m_tagpoint;
		obj = gt->second.m_object;
		if (tag != 0)
		{
			// Verfolgen eines Tagpoint
			const Ogre::Vector3& pos = tag->_getDerivedPosition();
			const Ogre::Quaternion& dir = tag ->_getDerivedOrientation();
			const Ogre::Vector3& scal = tag->_getDerivedScale();
			gt->second.m_tag_trackpoint->setPosition(pos);
			gt->second.m_tag_trackpoint->setOrientation(dir);
			gt->second.m_tag_trackpoint->setScale(scal);
			
			DEBUGX("attch position %s   %f %f %f",obj->getName().c_str(), pos.x, pos.y,pos.z);
		}
		
		// recursive update
		obj->update(time);
	}

	// update positions of Graphicobjects attached to Bones
	std::map<std::string, AttachedMovableObject>::iterator mt;
	Ogre::MovableObject* mobj;
	for (mt = m_attached_objects.begin(); mt != m_attached_objects.end(); ++mt)
	{
		tag = mt->second.m_tagpoint;
		mobj = mt->second.m_object;
		if (tag != 0)
		{
			const Ogre::Vector3& pos = tag->_getDerivedPosition();
			const Ogre::Quaternion& dir = tag ->_getDerivedOrientation();
			const Ogre::Vector3& scal = tag->_getDerivedScale();
			mt->second.m_tag_trackpoint->setPosition(pos);
			mt->second.m_tag_trackpoint->setOrientation(dir);
			mt->second.m_tag_trackpoint->setScale(scal);
		}
	}
	
	// update Soundobjects
	Ogre::Vector3 opos = getTopNode()->_getDerivedPosition();
	Vector pos(opos.x/GraphicManager::g_global_scale, opos.z / GraphicManager::g_global_scale);

	//std::map<std::string, SoundObject* >::iterator st;
	for (std::map<std::string, SoundObject>::iterator st = m_soundobjects.begin(); st != m_soundobjects.end(); ++st)
	//for (std::map<std::string, std::string>::iterator st = m_soundobjects.begin(); st != m_soundobjects.end(); ++st)
	{
		// Note: sounds should be updated automatically by the sound manager.
		// But they need to be configured correctly as relative or absolute play in order to allow correct updating
		// when the listener (player) moves and sounds are supposed to move with various actors.

		// TODO: XXX: this is a location where it would make sense to update the position of a 3d sound.
		// requires to connect a sound adapter instead of a sound name.

		st->second.setPosition (pos);
		//st->second->setPosition(pos);
		//st->second->update();

		DEBUGX ("setting sound [%s] position to: %f, %f", st->first.c_str (), pos.m_x, pos.m_y);
	}
}



void GraphicObject::addActiveRenderPart(ActionRenderpart* part)
{
	
	if (part->m_type == ActionRenderpart::DETACH)
	{
		removeMovableObject(part->m_objectname);
	}
	else if (part->m_type == ActionRenderpart::VISIBILITY)
	{
		bool visible = (part->m_start_val > 0);
		
		// set entites (normale and highlight) to  required visibility
		Ogre::MovableObject* mobj;
		mobj = GraphicObject::getMovableObject(part->m_objectname);
		if (mobj != 0)
		{
			mobj->setVisible (visible);
		}
		
		mobj = GraphicObject::getHighlightObject(part->m_objectname);
		if (mobj != 0)
		{
			mobj->setVisible (visible);
		}
		
		// change visibility for all
		if (part->m_objectname == "all")
		{
			setVisibility(visible);
		}
		
		// change visibility for subobjects recursively
		std::map<std::string, AttachedGraphicObject >::iterator it;
		it = m_subobjects.find(part->m_objectname);
			
		if (it != m_subobjects.end())
		{
			it->second.m_object->setVisibility(visible);
		}
	}
	else if (part->m_type == ActionRenderpart::SOUND)
	{
		//std::map<std::string, SoundObject* >::iterator it;
		std::map<std::string, SoundObject>::iterator it;
		//std::map<std::string, std::string>::iterator it;
		it = m_soundobjects.find(part->m_objectname);
		if (it != m_soundobjects.end())
		{
			//it->second->setSound(part->m_animation);
			it->second.setSoundData (part->m_animation);
			//it->second = part->m_animation;

			// TODO:XXX: This is where a play command should be triggered.
			it->second.play ();

			SW_DEBUG ("setting sound object [%s] to sound [%s]", it->first.c_str (), part->m_animation.c_str ());
		}
	}
}

void GraphicObject::removeActiveRenderPart(ActionRenderpart* part)
{
	DEBUGX("removing part %s",part->m_animation.c_str());
	if (part->m_type == ActionRenderpart::ANIMATION)
	{
		// deactivate animation
		Ogre::Entity* ent = getEntity(part->m_objectname);
		if (ent != 0)
		{
			Ogre::AnimationState* anim;
			Ogre::AnimationStateSet* anim_set;
			
			anim_set = ent->getAllAnimationStates();
			
			if (anim_set!= 0 && anim_set->hasAnimationState(part->m_animation))
			{
				anim = ent->getAnimationState(part->m_animation);
				if (anim != 0)
				{
					anim->setEnabled(false);
				}
			}
		}
	}
	else if (part->m_type == ActionRenderpart::SCALE)
	{
		// apply the final scale
		Ogre::MovableObject* obj = getMovableObject(part->m_objectname);
		if (obj != 0)
		{
			Ogre::Node* node = obj->getParentNode();
			Ogre::SceneNode* snode = dynamic_cast<Ogre::SceneNode*>(node);
			if (snode != 0)
			{
				snode->setScale(Ogre::Vector3(part->m_end_val,part->m_end_val,part->m_end_val));
			}
		}
	}
	else if (part->m_type == ActionRenderpart::MOVEMENT || part->m_type == ActionRenderpart::ROTATION)
	{
		// move to final position
		Ogre::Node* node = getParentNode(part->m_objectname);
		if (node != 0)
		{
			Ogre::Vector3 pos = part->m_end_vec ;
		
			if (part->m_type == ActionRenderpart::MOVEMENT)
			{
				node->setPosition(pos*GraphicManager::g_global_scale);
			}
			if (part->m_type == ActionRenderpart::ROTATION)
			{
				node->setOrientation(1,0,0,0);
				node->pitch(Ogre::Degree(pos[0]));
				node->yaw(Ogre::Degree(pos[1]));
				node->roll(Ogre::Degree(pos[2]));
			}
		}
	}
}

void GraphicObject::updateRenderPart(ActionRenderpart* part,float  relpercent)
{
	DEBUGX("updating part %s to %f",part->m_animation.c_str(),relpercent);
	if (part->m_type == ActionRenderpart::ANIMATION)
	{
		// update animation state
		Ogre::Entity* ent = getEntity(part->m_objectname);
		Ogre::Entity* highlight_ent = static_cast<Ogre::Entity*>(getHighlightObject(part->m_objectname));
		
		if (ent != 0)
		{
			DEBUGX("anim %s perc %f",part->m_animation.c_str(), relpercent);
			Ogre::AnimationState* anim;
			Ogre::AnimationStateSet* anim_set;
			
			anim_set = ent->getAllAnimationStates();
			
			if (anim_set != 0 && anim_set->hasAnimationState(part->m_animation))
			{
				
				anim = ent->getAnimationState(part->m_animation);
				if (anim != 0)
				{
					anim->setEnabled(true);
					anim->setTimePosition(relpercent*anim->getLength());
					if (m_exact_animations)
					{
						ent->_updateAnimation();
					}
				}
			}
			
			// update highlight entity as well
			if (highlight_ent != 0)
			{
				Ogre::AnimationStateSet* h_anim_set;
				h_anim_set = highlight_ent->getAllAnimationStates();
				anim_set->copyMatchingState(h_anim_set);
			}
		}
		
	}
	else if (part->m_type == ActionRenderpart::SCALE)
	{
		// update scale value
		Ogre::Node* node = getParentNode(part->m_objectname);
		
		if (node != 0)
		{
			float scal = part->m_start_val + relpercent*(part->m_end_val - part->m_start_val);
			node->setScale(Ogre::Vector3(scal,scal,scal));
			DEBUGX("Scale Node %p %f",node,scal);
		}
	}
	else if (part->m_type == ActionRenderpart::MOVEMENT || part->m_type == ActionRenderpart::ROTATION)
	{
		// update position
		Ogre::Node* node = getParentNode(part->m_objectname);
		if (node != 0)
		{
			Ogre::Vector3 pos = part->m_start_vec + relpercent*(part->m_end_vec - part->m_start_vec);
			DEBUGX("start %f %f %f   end %f %f %f",part->m_start_vec[0],part->m_start_vec[1],part->m_start_vec[2],part->m_end_vec[0],part->m_end_vec[1],part->m_end_vec[2]);
			DEBUGX("Movement %f %f %f relpercent %f",pos[0], pos[1],pos[2],relpercent);
			if (part->m_type == ActionRenderpart::MOVEMENT)
			{
				node->setPosition(pos*GraphicManager::g_global_scale);
			}
			if (part->m_type == ActionRenderpart::ROTATION)
			{
				node->setOrientation(1,0,0,0);
				node->pitch(Ogre::Degree(pos[0]));
				node->yaw(Ogre::Degree(pos[1]));
				node->roll(Ogre::Degree(pos[2]));
			}
		}
	}
}

void GraphicObject::setHighlight(bool highlight, std::string material)
{
	// update subobjects recursively
	std::map<std::string, AttachedGraphicObject >::iterator it;
	for (it = m_subobjects.begin(); it != m_subobjects.end(); ++it)
	{
		it->second.m_object->setHighlight(highlight,material);	
	}
	
	if (highlight == m_highlight)
		return;
	
	if (highlight && !m_highlight)
	{
		// activate highlight
		std::map<std::string, AttachedMovableObject>::iterator at;
		for (at = m_attached_objects.begin(); at != m_attached_objects.end(); ++at)
		{
			if (at->second.m_object->getMovableType() == "Entity" && at->second.m_object_info.m_highlightable)
			{
				// create a copy of the entity that is rendered for highlighting
				std::string name = at->first;
				name += "_highlight";
				
				at->second.m_highlight_entity = static_cast<Ogre::Entity*>(GraphicManager::createMovableObject(at->second.m_object_info,name));
				
				at->second.m_highlight_entity->setMaterialName(material);
				at->second.m_highlight_entity->setRenderQueueGroup(RENDER_QUEUE_HIGHLIGHT_OBJECTS); 
				
				Ogre::SceneNode* node = at->second.m_object->getParentSceneNode();
				node->attachObject(at->second.m_highlight_entity);
				
				at->second.m_highlight_entity->setVisible(at->second.m_object->getVisible());
			}
		}
	}
	else if (!highlight && m_highlight)
	{
		// deactivate highlighting
		std::map<std::string, AttachedMovableObject>::iterator at;
		for (at = m_attached_objects.begin(); at != m_attached_objects.end(); ++at)
		{
			if (at->second.m_highlight_entity != 0)
			{
				// delete the highlighting entity
				GraphicManager::detachMovableObject(at->second.m_highlight_entity);
		
				GraphicManager::destroyMovableObject(at->second.m_highlight_entity);
				
				at->second.m_highlight_entity =0;
			}
		}
	}
	
	
	m_highlight = highlight;
}


void GraphicObject::setVisibility(bool visible)
{
	m_top_node->setVisible(visible);
	
	std::map<std::string, AttachedGraphicObject >::iterator it;
	for (it = m_subobjects.begin(); it != m_subobjects.end(); ++it)
	{
		it->second.m_object->setVisibility(visible);
	}
}

void GraphicObject::setExactAnimations(bool exact_animations)
{
	m_exact_animations = exact_animations;
	
	std::map<std::string, AttachedGraphicObject >::iterator it;
	for (it = m_subobjects.begin(); it != m_subobjects.end(); ++it)
	{
		it->second.m_object->setExactAnimations(exact_animations);
	}
}

