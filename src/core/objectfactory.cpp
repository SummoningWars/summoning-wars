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

#include "objectfactory.h"

#include "fixedobject.h"
#include "monster.h"
#include "world.h"
#include "player.h"
#include "projectile.h"
#include "scriptobject.h"
#include "templateloader.h"
#include "treasure.h"
#include "waypoint.h"


// globale Daten
std::map<GameObject::Subtype, MonsterBasicData*> ObjectFactory::m_monster_data;

std::map<GameObject::Subtype, FixedObjectData*> ObjectFactory::m_fixed_object_data;

std::map<ObjectTemplateType, ObjectTemplate*> ObjectFactory::m_object_templates;

std::map<ObjectGroupName, ObjectGroup*> ObjectFactory::m_object_groups;

std::map< MonsterGroupName, MonsterGroup*>  ObjectFactory::m_monster_groups;

std::map<GameObject::Subtype, ScriptObjectData*> ObjectFactory::m_script_object_data;

std::map<GameObject::Subtype, GameObject::Type> ObjectFactory::m_object_types;

std::map<GameObject::Subtype, PlayerBasicData*> ObjectFactory::m_player_data;

std::map<GameObject::Subtype, ProjectileBasicData*> ObjectFactory::m_projectile_data;

std::map<std::string, EmotionSet*> ObjectFactory::m_emotion_sets;

std::multimap< GameObject::Subtype, PlayerLook> ObjectFactory::m_player_look;

std::map<GameObject::Subtype, TreasureBasicData*> ObjectFactory::m_treasure_data;

std::map<ObjectGroupTemplateName, ObjectGroupTemplate*> ObjectFactory::m_object_group_templates;

GameObject::Subtype ObjectTemplate::getObject(EnvironmentName env)
{
	// Daten aus der Map suchen
	std::map<EnvironmentName, WorldObjectTypeList >::iterator it;
	it = m_env_objects.find(env);
	
	// If the name contains some # symbols, the last # and all subsequent characters are removed
	// abc#def#ghi -> abc#def -> abc -> nothing was found
	while (it == m_env_objects.end())
	{
		size_t pos = env.find_last_of('#');
		if (pos != std::string::npos)
		{
			env.erase(pos);
			it = m_env_objects.find(env);
		}
		else
		{
			break;
		}
	}
	
	// try default information
	if (it == m_env_objects.end())
	{
		it = m_env_objects.find(m_default_environment);
	}
	
	if (it == m_env_objects.end() || it->second.empty())
	{
		// nothing found
		return "";
	}
	
	if (it->second.empty())
	{
		return "notype";
	}
	
	WorldObjectTypeList::iterator jt;
	int r = Random::randi(it->second.size());
	
	jt = it->second.begin();
	for (int i=0; i<r; ++i)
	{
		++jt;
	}

	return *jt;

}

ObjectGroupName ObjectGroupTemplate::getObjectGroup(EnvironmentName env)
{
	// Daten aus der Map suchen
	std::map<EnvironmentName, std::list<ObjectGroupName > >::iterator it;
	it = m_env_object_groups.find(env);
	
	// If the name contains some # symbols, the last # and all subsequent characters are removed
	// abc#def#ghi -> abc#def -> abc -> nothing was found
	while (it == m_env_object_groups.end())
	{
		size_t pos = env.find_last_of('#');
		if (pos != std::string::npos)
		{
			env.erase(pos);
			it = m_env_object_groups.find(env);
		}
		else
		{
			break;
		}
	}
	
	// try default information
	if (it == m_env_object_groups.end())
	{
		it = m_env_object_groups.find(m_default_environment);
	}
	
	if (it == m_env_object_groups.end() || it->second.empty())
	{
		// nothing found
		return "";
	}
	
	if (it->second.empty())
	{
		return "notype";
	}
	
	WorldObjectTypeList::iterator jt;
	int r = Random::randi(it->second.size());
	
	jt = it->second.begin();
	for (int i=0; i<r; ++i)
	{
		++jt;
	}

	return *jt;

}


void ObjectGroup::addObject(ObjectTemplateType objtype, Vector pos, float angle, float probability )
{
	GroupObject gobj;
	gobj.m_type = objtype;
	gobj.m_center = pos;
	gobj.m_angle = angle;
	gobj.m_probability = probability;
	m_objects.push_back(gobj);
}


GameObject::Subtype ObjectFactory::getObjectType(ObjectTemplateType generictype, EnvironmentName env)
{
	// Namen die nicht mit $ anfangen sind normale Typen
	if (generictype[0] != '$')
	{
		DEBUGX("simple subtype %s",generictype.c_str());
		return generictype;
	}
	
	// Suchen in der Datenbank
	std::map<ObjectTemplateType, ObjectTemplate*>::iterator it;
	it = m_object_templates.find(generictype);
	if (it == m_object_templates.end())
	{
		return generictype.substr(1);
	}
	else
	{
		return it->second->getObject(env);
	}
}

ObjectGroupName ObjectFactory::getObjectGroupType(ObjectGroupTemplateName generictype, EnvironmentName env)
{
	// Namen die nicht mit $ anfangen sind normale Typen
	if (generictype[0] != '$')
	{
		DEBUGX("simple subtype %s",generictype.c_str());
		return generictype;
	}
	
	// Suchen in der Datenbank
	std::map<ObjectGroupTemplateName, ObjectGroupTemplate*>::iterator it;
	it = m_object_group_templates.find(generictype);
	if (it == m_object_group_templates.end())
	{
		return generictype.substr(1);
	}
	else
	{
		return it->second->getObjectGroup(env);
	}
}

Shape ObjectFactory::getObjectGroupShape(ObjectGroupTemplateName generictype)
{
	Shape dummy;
	dummy.m_center = Vector(-1,-1);
	
	// Namen die nicht mit $ anfangen sind normale Typen
	if (generictype[0] != '$')
	{
		ObjectGroup* templ = getObjectGroup(generictype);
		if (templ != 0)
		{
			return *(templ->getShape());
		}
		return dummy;
	}
	
	// Suchen in der Datenbank
	std::map<ObjectGroupTemplateName, ObjectGroupTemplate*>::iterator it;
	it = m_object_group_templates.find(generictype);
	if (it == m_object_group_templates.end())
	{
		return getObjectGroupShape(generictype.substr(1));
	}
	else
	{
		return it->second->m_shape;
	}
}

WorldObject* ObjectFactory::createObject(GameObject::Type type, GameObject::Subtype subtype, int id)
{
	// Zeiger auf erzeugtes Objekt
	WorldObject* ret=0;

	

	if (type =="PLAYER")
	{
		ret = new Player(id,subtype);
	}
	else if (type =="MONSTER" || type =="NPC")
	{
		DEBUGX("requested subtype: %s",subtype.c_str());
		MonsterBasicData* mdata;
		std::map<GameObject::Subtype, MonsterBasicData*>::iterator i;

		i = m_monster_data.find(subtype);
		if (i== m_monster_data.end())
		{
			ERRORMSG("subtype not found: %s",subtype.c_str());
			return 0;
		}
		mdata = i->second;
		ret = new Monster( id,*mdata);
		DEBUGX("Monster created");
	}
	else if (type =="FIXED_OBJECT")
	{
		if (subtype == "waypoint")
		{
			return new Waypoint(id);
		}
		
		DEBUGX("requested subtype: %s",subtype.c_str());
		FixedObjectData* data;
		std::map<GameObject::Subtype, FixedObjectData*>::iterator i;

		i = m_fixed_object_data.find(subtype);
		if (i== m_fixed_object_data.end())
		{
			ERRORMSG("subtype not found: %s",subtype.c_str());
			return 0;
		}
		data = i->second;
		
		Shape* sp;
		DEBUGX("create fixed object: %s",subtype.c_str());
		ret = new FixedObject(id,subtype);
		ret->setState(WorldObject::STATE_STATIC,false);
		
		sp=ret->getShape();		
		memcpy(sp,&(data->m_shape),sizeof(Shape));
		sp->m_angle =0;
	
		ret->setLayer(data->m_layer);
	}
	else if (type =="SCRIPTOBJECT")
	{
		ScriptObjectData* data;
		std::map<GameObject::Subtype, ScriptObjectData*>::iterator i;

		i = m_script_object_data.find(subtype);
		if (i== m_script_object_data.end())
		{
			ERRORMSG("subtype not found: %s",subtype.c_str());
			return 0;
		}
		data = i->second;
		
		ret = new ScriptObject(id);
		
		Shape* sp;
		sp=ret->getShape();		
		memcpy(sp,&(data->m_fixed_data.m_shape),sizeof(Shape));
		sp->m_angle =0;
		
		ret->setSubtype(subtype);
		ret->setLayer(data->m_fixed_data.m_layer);
		ScriptObject* so = static_cast<ScriptObject*>(ret);
		so->setRenderInfo(data->m_render_info);
		so->setInteractionFlags(data->m_interaction_flags);
		
		std::multimap<TriggerType, Event*>::iterator et;
		Event* ev;
		for (et = data->m_events.begin(); et != data->m_events.end(); ++et)
		{
			ev = new Event(*(et->second));
			so->addEvent(et->first,ev);
		}
		
	}
	else if (type =="TREASURE")
	{
		TreasureBasicData* data;
		std::map<GameObject::Subtype, TreasureBasicData*>::iterator i;

		i = m_treasure_data.find(subtype);
		if (i== m_treasure_data.end())
		{
			ERRORMSG("subtype not found: %s",subtype.c_str());
			return 0;
		}
		data = i->second;
		
		ret = new Treasure(id,*data);
		ret->setSubtype(subtype);
	}
	
	return ret;
}

Projectile* ObjectFactory::createProjectile(GameObject::Subtype subtype, int id)
{
	std::map<GameObject::Subtype, ProjectileBasicData*>::iterator it;
	it = m_projectile_data.find(subtype);
	
	if (it == m_projectile_data.end())
	{
		ERRORMSG("subtype for projectile not found: %s",subtype.c_str());
		return 0;
	}
	
	Projectile* pr = new Projectile(*(it->second),id);
	return pr;
}

TranslatableString ObjectFactory::getObjectName(GameObject::Subtype subtype)
{
	std::map<GameObject::Subtype, MonsterBasicData*>::iterator i;

	i = m_monster_data.find(subtype);
	if (i== m_monster_data.end())
	{
		return TranslatableString(subtype);
	}
	return i->second->m_name;
}

ObjectGroup* ObjectFactory::getObjectGroup(ObjectGroupName name)
{
	std::map<ObjectGroupName, ObjectGroup*>::iterator it;
	it = m_object_groups.find(name);
	
	if (it == m_object_groups.end())
	{
		return 0;
	}
	
	return it->second;
}

MonsterGroup* ObjectFactory::getMonsterGroup(MonsterGroupName name)
{
	std::map< MonsterGroupName,MonsterGroup*>::iterator it;
	it = m_monster_groups.find(name);
	
	if (it == m_monster_groups.end())
	{
		return 0;
	}
	
	return it->second;
}

GameObject::Type ObjectFactory::getObjectBaseType(GameObject::Subtype subtype)
{
	std::map<GameObject::Subtype, GameObject::Type>::iterator it;
	it = m_object_types.find(subtype);
	if (it != m_object_types.end())
		return it->second;
	
	return "NONE";
}

PlayerBasicData* ObjectFactory::getPlayerData(GameObject::Subtype subtype)
{
	std::map<GameObject::Subtype, PlayerBasicData*>::iterator it;
	it = m_player_data.find(subtype);
	
	if (it == m_player_data.end())
	{
		return 0;
	}
	
	return it->second;
}

ProjectileBasicData* ObjectFactory::getProjectileData(GameObject::Subtype subtype)
{
	std::map<GameObject::Subtype, ProjectileBasicData*>::iterator it;
	it = m_projectile_data.find(subtype);
	
	if (it == m_projectile_data.end())
	{
		return 0;
	}
	
	return it->second;
}
		
void ObjectFactory::registerMonster(GameObject::Subtype subtype, MonsterBasicData* data, bool silent_replace)
{
	if (m_monster_data.count(subtype) != 0)
	{
		if (!silent_replace)
		{
			WARNING("Duplicate Monster type %s. Replacing data.",subtype.c_str());
		}
		m_object_types.erase(subtype);
		delete m_monster_data[subtype];
		m_monster_data.erase(subtype);
	}
	m_object_types.insert(std::make_pair(subtype, "MONSTER"));
	
	DEBUGX("registered monster for subtype %s",subtype.c_str());
	m_monster_data.insert(std::make_pair(subtype,data));
}

void ObjectFactory::registerFixedObject(GameObject::Subtype subtype, FixedObjectData* data, bool silent_replace)
{
	if (m_fixed_object_data.count(subtype) != 0)
	{
		if (!silent_replace)
		{
			WARNING("Duplicate fixed object type %s. Replacing data.",subtype.c_str());
		}
		m_object_types.erase(subtype);
		delete m_fixed_object_data[subtype];
		m_fixed_object_data.erase(subtype);
	}
	m_object_types.insert(std::make_pair(subtype, "FIXED_OBJECT"));
	m_fixed_object_data.insert(std::make_pair(subtype,data));
}

void ObjectFactory::registerScriptObject(GameObject::Subtype subtype, ScriptObjectData* data)
{
	if (m_script_object_data.count(subtype) != 0)
	{
		WARNING("Duplicate scriptobject type %s. Replacing data.",subtype.c_str());
		m_object_types.erase(subtype);
		delete m_script_object_data[subtype];
		m_script_object_data.erase(subtype);
	}
	m_object_types.insert(std::make_pair(subtype, "SCRIPTOBJECT"));
	m_script_object_data.insert(std::make_pair(subtype,data));
}

void ObjectFactory::registerProjectile(GameObject::Subtype subtype, ProjectileBasicData* data)
{
	if (m_projectile_data.count(subtype) != 0)
	{
		WARNING("Duplicate missile type %s. Replacing data.",subtype.c_str());
		m_object_types.erase(subtype);
		delete m_projectile_data[subtype];
		m_projectile_data.erase(subtype);
	}
	m_object_types.insert(std::make_pair(subtype, "PROJECTILE"));
	m_projectile_data.insert(std::make_pair(subtype,data));
}

void ObjectFactory::registerTreasure(GameObject::Subtype subtype, TreasureBasicData* data)
{
	if (m_treasure_data.count(subtype) != 0)
	{
		WARNING("Duplicate treasure type %s. Replacing data.",subtype.c_str());
		m_object_types.erase(subtype);
		delete m_treasure_data[subtype];
		m_treasure_data.erase(subtype);
	}
	m_object_types.insert(std::make_pair(subtype, "TREASURE"));
	m_treasure_data.insert(std::make_pair(subtype,data));
}

void ObjectFactory::registerPlayer(GameObject::Subtype subtype, PlayerBasicData* data)
{
	if (m_player_data.count(subtype) != 0)
	{
		WARNING("Duplicate player type %s. Replacing data.",subtype.c_str());
		m_object_types.erase(subtype);
		delete m_player_data[subtype];
		m_player_data.erase(subtype);
	}
	
	m_object_types.insert(std::make_pair(subtype, "PLAYER"));
	
	DEBUGX("registered playerclass for subtype %s",subtype.c_str());
	m_player_data.insert(std::make_pair(subtype,data));
}

void ObjectFactory::registerObjectTemplate(ObjectTemplateType type, ObjectTemplate* templ)
{
	if (m_object_templates.count(type) != 0)
	{
		WARNING("Duplicate template type %s. Replacing data.",type.c_str());
		delete m_object_templates[type];
		m_object_templates.erase(type);
	}
	
	
	m_object_templates[type] = templ;
}

void ObjectFactory::registerObjectGroup(ObjectGroupName name, ObjectGroup* data)
{
	
	if (m_object_groups.count(name)>0)
	{
		ERRORMSG("Object group template with name %s already exists",name.c_str());
	}
	else
	{
		m_object_groups[name] = data;
	}
}

void ObjectFactory::registerObjectGroupTemplate(ObjectGroupTemplateName type, ObjectGroupTemplate* templ)
{
	if (m_object_group_templates.count(type)>0)
	{
		ERRORMSG("Object template with name %s already exists",type.c_str());
	}
	else
	{
		m_object_group_templates[type] = templ;
	}
}

void ObjectFactory::registerMonsterGroup(MonsterGroupName name, MonsterGroup* data)
{
	if (m_monster_groups.count(name)>0)
	{
		ERRORMSG("Monster group with name %s already exists",name.c_str());
	}
	else
	{
		m_monster_groups[name] = data;
	}
}

void ObjectFactory::registerEmotionSet(std::string name, EmotionSet* set)
{
	if (m_emotion_sets.count(name)>0)
	{
		ERRORMSG("Emotionset with name %s already exists",name.c_str());
	}
		else
	{
		m_emotion_sets[name] = set;
	}
}

void ObjectFactory::registerPlayerLook(GameObject::Subtype subtype, PlayerLook look)
{
	m_player_look.insert(std::make_pair(subtype, look));
}



EmotionSet* ObjectFactory::getEmotionSet(std::string name)
{
	std::map<std::string, EmotionSet*>::iterator it;
	it = m_emotion_sets.find(name);
	if (it == m_emotion_sets.end())
		return 0;
  
	return it->second;
}

void ObjectFactory::getPlayerLooks(GameObject::Subtype subtype, std::list<  PlayerLook> &looks)
{
	std::multimap< GameObject::Subtype,  PlayerLook >::iterator it,jt;
	it = m_player_look.lower_bound(subtype);
	jt = m_player_look.upper_bound(subtype);

	while (it != jt)
	{
		looks.push_back(it->second);
		++it;
	}
}

PlayerLook* ObjectFactory::getPlayerLook(GameObject::Subtype subtype, std::string name)
{
	std::multimap< GameObject::Subtype,  PlayerLook >::iterator it,jt;
	it = m_player_look.lower_bound(subtype);
	jt = m_player_look.upper_bound(subtype);

	while (it != jt)
	{
		if (it->second.m_name == name)
			return &(it->second);
		++it;
	}
	return 0;
}

FixedObjectData* ObjectFactory::getFixedObjectData(GameObject::Subtype subtype)
{
	//FixedObjectData* data; // 2011.10.23: found as unused.
	std::map<GameObject::Subtype, FixedObjectData*>::iterator i;
	
	i = m_fixed_object_data.find(subtype);
	if (i== m_fixed_object_data.end())
	{
		ERRORMSG("subtype not found: %s",subtype.c_str());
		return 0;
	}
	return i->second;
}

MonsterBasicData* ObjectFactory::getMonsterData(GameObject::Subtype subtype)
{
	//MonsterBasicData* data; // 2011.10.23: found as unused.
	std::map<GameObject::Subtype, MonsterBasicData*>::iterator i;
	
	i = m_monster_data.find(subtype);
	if (i== m_monster_data.end())
	{
		ERRORMSG("subtype not found: %s",subtype.c_str());
		return 0;
	}
	return i->second;
}

void ObjectFactory::init()
{

	
	
}

void ObjectFactory::cleanup(int bitmask)
{
	cleanupObjectData(bitmask);
	if (bitmask & World::DATA_OBJECTS)
	{
		std::map<ObjectTemplateType, ObjectTemplate*>::iterator it3;
		for (it3 = m_object_templates.begin(); it3 != m_object_templates.end(); ++it3)
		{
			delete it3->second;
		} 
		m_object_templates.clear();

		std::map<ObjectGroupName, ObjectGroup*>::iterator it4;
		for (it4 = m_object_groups.begin(); it4!= m_object_groups.end(); ++it4)
		{
			delete it4->second;
		} 
		m_object_groups.clear();
		
		std::map<ObjectGroupTemplateName, ObjectGroupTemplate*>::iterator it13;
		for (it13 = m_object_group_templates.begin(); it13 != m_object_group_templates.end(); ++it13)
		{
			delete it13->second;
		}
		m_object_group_templates.clear();
	}

	if (bitmask & World::DATA_PLAYERCLASSES)
	{
		std::map<GameObject::Subtype, PlayerBasicData*>::iterator it6;
		for (it6= m_player_data.begin(); it6 != m_player_data.end(); ++it6)
		{
			delete it6->second;
		}
		m_player_data.clear();
	}
}

void ObjectFactory::cleanupObjectData(int bitmask)
{
	if (bitmask & World::DATA_MONSTERS)
	{
		std::map<GameObject::Subtype, MonsterBasicData*>::iterator it1;
		for (it1 = m_monster_data.begin(); it1 != m_monster_data.end(); ++it1)
		{
			delete it1->second;
		}
		m_monster_data.clear();
		
		std::map< MonsterGroupName, MonsterGroup*>::iterator it5;
		for (it5 = m_monster_groups.begin(); it5!=m_monster_groups.end(); ++it5)
		{
			delete it5->second;
		}
		m_monster_groups.clear();
		
		std::map<std::string, EmotionSet*>::iterator it8;
		for (it8 = m_emotion_sets.begin(); it8 != m_emotion_sets.end(); ++it8)
		{
			delete it8->second;
		}
		m_emotion_sets.clear();
	}
	
	if (bitmask & World::DATA_OBJECTS)
	{
		std::map<GameObject::Subtype, FixedObjectData*>::iterator it2;
		for (it2 = m_fixed_object_data.begin(); it2 != m_fixed_object_data.end(); ++it2)
		{
			delete it2->second;
		} 
		m_fixed_object_data.clear();
		
		std::map<GameObject::Subtype, ScriptObjectData*>::iterator it9;
		for (it9 = m_script_object_data.begin(); it9 !=m_script_object_data.end(); ++it9)
		{
			delete it9->second;
		}
		m_script_object_data.clear();
		
		std::map<GameObject::Subtype, TreasureBasicData*>::iterator it10;
		for (it10 = m_treasure_data.begin(); it10 != m_treasure_data.end(); ++it10)
		{
			delete it10->second;
		}
		m_treasure_data.clear();
	}
	
	
	if (bitmask & World::DATA_PROJECTILES)
	{
		std::map<GameObject::Subtype, ProjectileBasicData*>::iterator it7;
		for (it7= m_projectile_data.begin(); it7 != m_projectile_data.end(); ++it7)
		{
			delete it7->second;
		} 
		m_projectile_data.clear();
	}
	
	// Zuordnung Klassenname -> Spieler wieder eintragen
	std::map<GameObject::Subtype, PlayerBasicData*>::iterator it;
	for (it = m_player_data.begin(); it != m_player_data.end(); ++it)
	{
		m_object_types.insert(std::make_pair(it->first, "PLAYER"));
	}
}



