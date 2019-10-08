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

#include "action.h"
#include "elementattrib.h"

#include "eventsystem.h"
#include "projectile.h"

std::map<Action::ActionType,Action::ActionInfo> Action::m_action_info;

void Action::init()
{

}

void Action::cleanup()
{
	std::map<Action::ActionType,Action::ActionInfo>::iterator it;
	for (it = m_action_info.begin(); it != m_action_info.end(); ++it)
	{
		if (it->second.m_effect.m_lua_impl != LUA_NOREF)
			EventSystem::clearCodeReference(it->second.m_effect.m_lua_impl);
		
		if (it->second.m_damage.m_lua_impl != LUA_NOREF)
			EventSystem::clearCodeReference(it->second.m_damage.m_lua_impl);
		
		if (it->second.m_base_mod.m_lua_impl != LUA_NOREF)
			EventSystem::clearCodeReference(it->second.m_base_mod.m_lua_impl);
		
		if (it->second.m_dyn_mod.m_lua_impl != LUA_NOREF)
			EventSystem::clearCodeReference(it->second.m_dyn_mod.m_lua_impl);

	}
	
	m_action_info.clear();
}


bool Action::loadAbilityData(const char* pFilename)
{
	DEBUGX("loading file: %s",pFilename);
	TiXmlDocument doc(pFilename);
	bool loadOkay = doc.LoadFile();

	if (loadOkay)
	{
		loadAbilities(&doc);
		return true;
	}
	else
	{
		DEBUG("Failed to load file %s", pFilename);
		return false;
	}
}

void Action::loadAbilities(TiXmlNode* node)
{
	TiXmlNode* child;
	if (node->Type()==TiXmlNode::TINYXML_ELEMENT && !strcmp(node->Value(), "Ability"))
	{
		loadAbility(node);
	}
	else
	{
		for ( child = node->FirstChild(); child != 0; child = child->NextSibling())
		{
			loadAbilities(child);
		}
	}
}

void Action::loadAbility(TiXmlNode* node)
{
	ElementAttrib attr;
	attr.parseElement(node->ToElement());
	
	ActionType type;
	attr.getString("type", type);
	
	if (m_action_info.count(type) != 0)
	{
		WARNING("Ability info for %s already exists",type.c_str());
		return;
	}

	
	Action::ActionInfo* a;
	a = &(Action::m_action_info[type]);
	
	
	a->m_projectile_type="";
	attr.getString("name",a->m_name);
	attr.getString("description",a->m_description);
	attr.getString("base_ability",a->m_base_action,"noaction");
	attr.getFloat("time",a->m_standard_time,1);
	attr.getInt("timer_nr",a->m_timer_nr,0);
	attr.getFloat("timer_value",a->m_timer,0);
	attr.getFloat("critical_percent",a->m_critical_perc,0);
	attr.getFloat("radius",a->m_radius,0);
	std::string target;
	attr.getString("target_type",target,"none");
	if (target =="none")
		a->m_target_type = NONE;
	else if (target =="melee")
		a->m_target_type = MELEE;	
	else if (target =="ranged")
		a->m_target_type = RANGED;	
	else if (target =="self")
		a->m_target_type = SELF;
	else if (target =="circle")
		a->m_target_type = CIRCLE;		
	else if (target =="party")
		a->m_target_type = PARTY;	
	else if (target =="party_all")
		a->m_target_type = PARTY_MULTI;	
	else if (target =="passive")
		a->m_target_type = PASSIVE;	
	else
	{
		ERRORMSG("invalid target type for ability %s",type.c_str());
	}
	DEBUGX("ability %s %s %f %i %f %f %s %s",type.c_str(), a->m_name.c_str(), a->m_standard_time, a->m_timer_nr, a->m_timer, a->m_critical_perc, target.c_str(), a->m_base_action.c_str());
		
	TiXmlNode* child;
	TiXmlNode* child2;
	a->m_flags =0;
	for ( child = node->FirstChild(); child != 0; child = child->NextSibling())
	{
		if (child->Type()==TiXmlNode::TINYXML_ELEMENT) 
		{
			attr.parseElement(child->ToElement());
			
			if (!strcmp(child->Value(), "Flag"))
			{
				attr.parseElement(child->ToElement());
				std::string flag;
				attr.getString("name",flag);
				if (flag == "mute_affected")
				{
					a->m_flags |= MUTE_AFFECTED;
				}
				DEBUGX("  flag: %s",flag.c_str());
			}
			else if (!strcmp(child->Value(), "Effect"))
			{
				loadHybridImplementation(child, a->m_effect);
			}
			else if (!strcmp(child->Value(), "Damage"))
			{
				loadHybridImplementation(child, a->m_damage);
			}
			else if (!strcmp(child->Value(), "BaseAttributesMod"))
			{
				loadHybridImplementation(child, a->m_base_mod);
			}
			else if (!strcmp(child->Value(), "DynAttributesMod"))
			{
				loadHybridImplementation(child, a->m_dyn_mod);
			}
			else if (!strcmp(child->Value(), "Projectile"))
			{
				attr.parseElement(child->ToElement());
				attr.getString("type", a->m_projectile_type,"");
				attr.getInt("counter", a->m_projectile_counter);
				attr.getFloat("speed", a->m_projectile_speed);
				attr.getFloat("radius", a->m_projectile_radius);
				a->m_projectile_flags =0;
				for ( child2 = child->FirstChild(); child2 != 0; child2 = child2->NextSibling())
				{
					if (child2->Type()==TiXmlNode::TINYXML_ELEMENT) 
					{
						attr.parseElement(child2->ToElement());
						if (!strcmp(child2->Value(), "Flag"))
						{
							std::string flag;
							attr.getString("name",flag);
							if (flag == "explodes")
							{
								a->m_projectile_flags |= Projectile::EXPLODES;
							}
							else if (flag == "multi_explodes")
							{
								a->m_projectile_flags |= Projectile::MULTI_EXPLODES;
							}
							else if (flag == "piercing")
							{
								a->m_projectile_flags |= Projectile::PIERCING;
								
							}
							else if (flag == "bouncing")
							{
								a->m_projectile_flags |= Projectile::BOUNCING;
							}
							else if (flag == "prob_bouncing")
							{
								a->m_projectile_flags |= Projectile::PROB_BOUNCING;
							}
							DEBUGX(" %s flags %i",a->m_projectile_type.c_str(), a->m_projectile_flags);
						}
					}
				}
			}
			else if (child->Type()!=TiXmlNode::TINYXML_COMMENT)
			{
				DEBUG("unexpected element of <Ability>: %s",child->Value());
			}
		}
	}
}

std::string Action::getName(ActionType type)
{
	ActionInfo* aci = getActionInfo(type);
	if (aci == 0)
	{
		WARNING("unknown action: %s",type.c_str());
		return "";
	}
	return dgettext("sumwars",aci->m_name.c_str()) ;

}


std::string Action::getDescription(ActionType type)
{
	ActionInfo* aci = getActionInfo(type);
	if (aci == 0)
	{
		WARNING("unknown action: %s",type.c_str());
		return "";
	}
	return  dgettext("sumwars",aci->m_description.c_str());
}

Action::ActionType Action::getActionType(std::string name)
{
	std::map<ActionType, ActionInfo>::iterator it;
	it = m_action_info.find(name);
	if (it != m_action_info.end())
	{
		return name;
	}
	return "noaction";
}

void Action::toString(CharConv* cv)
{

	cv->toBuffer(m_type);
	cv->toBuffer(m_time);
	cv->toBuffer((char) m_action_equip);
	cv->toBuffer(m_elapsed_time);


}

void Action::fromString(CharConv* cv)
{
	
	char ctmp;
	cv->fromBuffer(m_type);
	cv->fromBuffer(m_time);
	cv->fromBuffer(ctmp);
	m_action_equip = (ActionEquip) ctmp;
	cv->fromBuffer(m_elapsed_time);
}

bool Action::loadHybridImplementation(TiXmlNode* node, HybridImplementation& impl)
{
	TiXmlNode* child;
	TiXmlText* text;
	ElementAttrib attr;
	for ( child = node->FirstChild(); child != 0; child = child->NextSibling())
	{
		if (child->Type()==TiXmlNode::TINYXML_ELEMENT) 
		{
			attr.parseElement(child->ToElement());
			
			if (!strcmp(child->Value(), "CppImplementation"))
			{
				std::string type;
				ElementAttrib attr;
				attr.parseElement(child->ToElement());
				attr.getString("type",type);
				
				impl.m_cpp_impl.push_back(type);
			}
			else if (!strcmp(child->Value(), "LuaImplementation"))
			{
				text = child->FirstChild()->ToText();
				if (text != 0)
				{
					impl.m_lua_impl = EventSystem::createCodeReference(text->Value());
				}
			}
		}
	}
	return true;
}

void Action::toxml()
{
	TiXmlDocument doc;
	
	std::map<ActionType,ActionInfo>::iterator it;
	TiXmlElement * elem;
	TiXmlElement * elem2;
	for (it = m_action_info.begin(); it != m_action_info.end(); ++it)
	{
		elem = new TiXmlElement("Ability");
		elem->SetAttribute("type",it->first.c_str());
		elem->SetAttribute("name",it->second.m_name.c_str());
		elem->SetAttribute("description",it->second.m_description.c_str());
		elem->SetDoubleAttribute("time",it->second.m_standard_time);
		elem->SetDoubleAttribute("timer_nr",it->second.m_timer_nr);
		elem->SetDoubleAttribute("timer_value",it->second.m_timer);
		elem->SetAttribute("base_ability",it->second.m_base_action.c_str());
		elem->SetDoubleAttribute("critical_percent",it->second.m_critical_perc);
		if (it->second.m_target_type == MELEE)			
			elem->SetAttribute("target_type","melee");
		else if (it->second.m_target_type == RANGED)
			elem->SetAttribute("target_type","ranged");
		else if (it->second.m_target_type == CIRCLE)
			elem->SetAttribute("target_type","circle");
		else if (it->second.m_target_type == PARTY)
			elem->SetAttribute("target_type","party");
		else if (it->second.m_target_type == PASSIVE)
			elem->SetAttribute("target_type","passive");
		else if (it->second.m_target_type == PARTY_MULTI)
			elem->SetAttribute("target_type","party_all");
		if (it->second.m_flags & MUTE_AFFECTED)
		{
			elem2 = new TiXmlElement("Flag");
			elem2->SetAttribute("name","mute_affected");
			elem->LinkEndChild(elem2);
		}
		doc.LinkEndChild(elem);
	}
	doc.SaveFile("../data/abilities/base_abilities.xml");
}


