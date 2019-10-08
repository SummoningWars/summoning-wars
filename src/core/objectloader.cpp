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



#include "scene.h"
#include "player.h"
#include "fixedobject.h"
#include "monster.h"
#include "objectfactory.h"
#include "objectloader.h"
#include "worldloader.h"
#include "graphicmanager.h"
#include "projectile.h"

std::string ObjectLoader::m_filename;

bool ObjectLoader::loadMonsterData(const char* pFilename)
{
	TiXmlDocument doc(pFilename);
	bool loadOkay = doc.LoadFile();

	if (loadOkay)
	{
		m_filename = pFilename;
		loadMonster(&doc);
		return true;
	}
	else
	{
		ERRORMSG("Failed to load file %s", pFilename);
		return false;
	}
}

bool  ObjectLoader::loadMonster(TiXmlNode* node, bool silent_replace)
{
	TiXmlNode* child;
	if (node->Type()==TiXmlNode::TINYXML_ELEMENT && !strcmp(node->Value(), "Monster"))
	{
		ElementAttrib attr;
		attr.parseElement(node->ToElement());
		
		MonsterBasicData* data = new MonsterBasicData;
		
		// Typinformationen auslesen
		std::string type,subtype, race, fraction,layer;
		data->m_type = "MONSTER";
		attr.getString("subtype",subtype);
		attr.getString("race",race);
		attr.getString("fraction",fraction,"MONSTER");
		attr.getString("layer",layer,"NORMAL");
		std::string name;
		attr.getString("name",name,subtype);
		data->m_name = TranslatableString(name, EventSystem::GetGettextDomain());
		data->m_subtype = subtype;
		
		DEBUGX("monster: %s %s %s %s",type.c_str(), subtype.c_str(), race.c_str(), fraction.c_str());
		
		data->m_fraction_name = fraction;
	
		data->m_race = race;
		
		if (layer == "NORMAL")
			data->m_layer = WorldObject::LAYER_BASE | WorldObject::LAYER_AIR;
		else if (layer == "BASE")
			data->m_layer = WorldObject::LAYER_BASE;
		else if (layer == "AIR")
			data->m_layer = WorldObject::LAYER_AIR;
		
		// Datenfelder nullen
		data->m_base_attr.m_immunity =0;
		data->m_base_attr.m_special_flags =0;
		
		
		// Schleife ueber die Elemente von Monster
		for ( child = node->FirstChild(); child != 0; child = child->NextSibling())
		{
			if (child->Type()==TiXmlNode::TINYXML_ELEMENT)
			{
				attr.parseElement(child->ToElement());
				
				if (!strcmp(child->Value(), "Mesh"))
				{
					std::string mesh;
					attr.getString("file",mesh);
					
					DEBUGX("mesh %s %s",subtype.c_str(), mesh.c_str());
					GraphicManager::registerGraphicMapping(subtype, mesh,silent_replace);
				}
				else if (!strcmp(child->Value(), "RenderInfo"))
				{
					std::string mesh;
					attr.getString("name",mesh);
					
					DEBUGX("mesh %s %s",subtype.c_str(), mesh.c_str());
					GraphicManager::registerGraphicMapping(subtype, mesh,silent_replace);
				}
				else if (!strcmp(child->Value(), "EmotionSet"))
				{
					attr.getString("name",data->m_emotion_set);
				}
				else if (!strcmp(child->Value(), "Dropslots"))
				{
					int nr =0;
					for (TiXmlNode* child2 = child->FirstChild(); child2 != 0; child2 = child2->NextSibling())
					{
						attr.parseElement(child2->ToElement());
						attr.getFloat("pbig",data->m_drop_slots[nr].m_size_probability[Item::BIG],0);
						attr.getFloat("pmedium",data->m_drop_slots[nr].m_size_probability[Item::MEDIUM],0);
						attr.getFloat("psmall",data->m_drop_slots[nr].m_size_probability[Item::SMALL],0);
						attr.getFloat("pgold",data->m_drop_slots[nr].m_size_probability[Item::GOLD],0);
						data->m_drop_slots[nr].m_size_probability[4] =1;
						for (int i=0; i<4; i++)
							data->m_drop_slots[nr].m_size_probability[4] -= data->m_drop_slots[nr].m_size_probability[i];
						
						attr.getInt("min_level",data->m_drop_slots[nr].m_min_level,0);
						attr.getInt("max_level",data->m_drop_slots[nr].m_max_level,0);
						attr.getInt("min_gold",data->m_drop_slots[nr].m_min_gold,1);
						attr.getInt("max_gold",data->m_drop_slots[nr].m_max_gold,1);
						attr.getFloat("magic_prob",data->m_drop_slots[nr].m_magic_probability,0);
						attr.getFloat("magic_power",data->m_drop_slots[nr].m_magic_power,0);
						
						DEBUGX("Dropslot %i %i %f %f", data->m_drop_slots[nr].m_min_level,data->m_drop_slots[nr].m_max_level,  data->m_drop_slots[nr].m_magic_probability, data->m_drop_slots[nr].m_magic_power);
						nr ++;
					}
					
				}
				else if (!strcmp(child->Value(), "BasicAttributes"))
				{
					
					loadCreatureBaseAttr(child,data->m_base_attr);
				
				}
				else if (!strcmp(child->Value(), "Geometry"))
				{
					attr.getFloat("radius",data->m_radius);
				}
				else if (!strcmp(child->Value(), "AI"))
				{
					attr.getFloat("ai_sight_range",data-> m_ai_vars.m_sight_range,12);
					attr.getFloat("ai_shoot_range",data-> m_ai_vars.m_shoot_range,data-> m_ai_vars.m_sight_range);
					attr.getFloat("ai_action_range",data-> m_ai_vars.m_action_range,data-> m_ai_vars.m_sight_range);
					attr.getFloat("ai_randaction_prob",data-> m_ai_vars.m_randaction_prob,0);
					attr.getFloat("ai_chase_range",data-> m_ai_vars.m_chase_distance,20);
					attr.getFloat("ai_warn_range",data-> m_ai_vars.m_warn_radius,4);
					
				}
				else if (child->Type()!=TiXmlNode::TINYXML_COMMENT)
				{
					WARNING("%s : unexpected element of <Monster>: %s",m_filename.c_str(),child->Value());
				}
				
			}
		}
		
		ObjectFactory::registerMonster(subtype,data,silent_replace);
		
	}
	else if (node->Type()==TiXmlNode::TINYXML_ELEMENT && !strcmp(node->Value(), "MonsterGroup"))
	{
		ElementAttrib attr;
		attr.parseElement(node->ToElement());
		
		MonsterGroup* data = new MonsterGroup;
		
		std::string name;
		attr.getString("name",name);
		for ( child = node->FirstChild(); child != 0; child = child->NextSibling())
		{
			attr.parseElement(child->ToElement());
			if (!strcmp(child->Value(), "SubGroup"))
			{
				std::string subtype;
				int number;
				float prob;
				attr.getString("subtype",subtype);
				attr.getInt("number",number,1);
				attr.getFloat("prob",prob,1);
				
				data-> addMonsters(subtype,number,prob);
			}
			else if (child->Type()!=TiXmlNode::TINYXML_COMMENT)
			{
				WARNING("%s : unexpected element of <MonsterGroup>: %s",m_filename.c_str(),child->Value());
			}
		}
		
		ObjectFactory::registerMonsterGroup(name,data);
	}
	else if (!strcmp(node->Value(), "EmotionSet"))
	{
		loadEmotionSet(node);
	}
	else
	{
		// rekursiv durchmustern
		for ( child = node->FirstChild(); child != 0; child = child->NextSibling())
		{
			loadMonster(child);
		}
	}
	
	return true;
}

bool  ObjectLoader::loadCreatureBaseAttr(TiXmlNode* node, CreatureBaseAttr& basattr)
{
	ElementAttrib attr;
	attr.parseElement(node->ToElement());
	
	// Basisattribute setzen
	attr.getFloat("experience",basattr.m_max_experience,0);
	short level;
	attr.getShort("level",level,1);
	basattr.m_level = level;
	attr.getFloat("max_health",basattr.m_max_health,1);
	attr.getShort("armor",basattr.m_armor,0);
	attr.getShort("block",basattr.m_block,0);
	attr.getShort("attack",basattr.m_attack,1);
	attr.getShort("power",basattr.m_power,1);
	attr.getShort("strength",basattr.m_strength,1);
	attr.getShort("dexterity",basattr.m_dexterity,1);
	attr.getShort("magic_power",basattr.m_magic_power,1);
	attr.getShort("willpower",basattr.m_willpower,1);
	attr.getShort("resistances_fire",basattr.m_resistances[Damage::FIRE],0);
	attr.getShort("resistances_ice",basattr.m_resistances[Damage::ICE],0);
	attr.getShort("resistances_air",basattr.m_resistances[Damage::AIR],0);
	attr.getShort("resistances_physical",basattr.m_resistances[Damage::PHYSICAL],0);
	attr.getShort("resistances_cap_fire",basattr.m_resistances_cap[Damage::FIRE],50);
	attr.getShort("resistances_cap_ice",basattr.m_resistances_cap[Damage::ICE],50);
	attr.getShort("resistances_cap_air",basattr.m_resistances_cap[Damage::AIR],50);
	attr.getShort("resistances_cap_physical",basattr.m_resistances_cap[Damage::PHYSICAL],50);
	attr.getShort("walk_speed",basattr.m_walk_speed,2000);	
	attr.getShort("attack_speed",basattr.m_attack_speed,1500);	
	attr.getFloat("step_length",basattr.m_step_length,1);
	attr.getFloat("attack_range",basattr.m_attack_range,1);
	
	if (basattr.m_resistances[Damage::FIRE] > basattr.m_resistances_cap[Damage::FIRE] 
		   || basattr.m_resistances[Damage::ICE] > basattr.m_resistances_cap[Damage::ICE]
			|| basattr.m_resistances[Damage::AIR] > basattr.m_resistances_cap[Damage::AIR]
			|| basattr.m_resistances[Damage::PHYSICAL] > basattr.m_resistances_cap[Damage::PHYSICAL])
	{
		WARNING("%s : resistance exceeds resistance cap", m_filename.c_str());
		basattr.m_resistances_cap[Damage::FIRE] = MathHelper::Max(basattr.m_resistances_cap[Damage::FIRE], basattr.m_resistances[Damage::FIRE]);
		basattr.m_resistances_cap[Damage::ICE] = MathHelper::Max(basattr.m_resistances_cap[Damage::ICE], basattr.m_resistances[Damage::ICE]);
		basattr.m_resistances_cap[Damage::AIR] = MathHelper::Max(basattr.m_resistances_cap[Damage::AIR], basattr.m_resistances[Damage::AIR]);
		basattr.m_resistances_cap[Damage::PHYSICAL] = MathHelper::Max(basattr.m_resistances_cap[Damage::PHYSICAL], basattr.m_resistances[Damage::PHYSICAL]);
		
	}

					
	// Schleife ueber die Elemente von BasicAttributes
	TiXmlNode* child = node;
	for (TiXmlNode* child2 = child->FirstChild(); child2 != 0; child2 = child2->NextSibling())
	{
		attr.parseElement(child2->ToElement());
						
		if (!strcmp(child2->Value(), "Ability"))
		{
			std::string anim,act;
			Action::ActionType action;
			attr.getString("type",act);
			basattr.m_abilities[act].m_timer = 0;
			attr.getFloat("time",basattr.m_abilities[act].m_time);
			attr.getInt("timer_nr",basattr.m_abilities[act].m_timer_nr,-1);
			attr.getFloat("timer_value",basattr.m_abilities[act].m_timer, -1);
 			attr.getFloat("rating",basattr.m_abilities[act].m_rating,-1);
			attr.getFloat("all_target_rating",basattr.m_abilities[act].m_all_target_rating);
			attr.getFloat("all_target_rating_radius",basattr.m_abilities[act].m_all_target_rating_range,-1);
			attr.getFloat("random_rating",basattr.m_abilities[act].m_random_rating,0.01);
		}
		else if (!strcmp(child2->Value(), "Immunity"))
		{
			std::string type;
			attr.getString("type",type);
							
							// string -> enum
			if (type == "blind")
				basattr.m_immunity |= 1<<Damage::BLIND;
			else if (type ==  "poisoned")
				basattr.m_immunity |= 1<<Damage::POISONED;
			else if (type ==  "berserk")
				basattr.m_immunity |= 1<<Damage::BERSERK;
			else if (type ==  "confused")
				basattr.m_immunity |= 1<<Damage::CONFUSED;
			else if (type ==  "mute")
				basattr.m_immunity |= 1<<Damage::MUTE;
			else if (type ==  "paralyzed")
				basattr.m_immunity |= 1<<Damage::PARALYZED;
			else if (type ==  "frozen")
				basattr.m_immunity |= 1<<Damage::FROZEN;
			else if (type ==  "burning")
				basattr.m_immunity |= 1<<Damage::BURNING;
							
		}
		else if (!strcmp(child2->Value(), "SpecialFlag"))
		{
			std::string type;
			attr.getString("type",type);
							
							// string -> enum
			/*
			if (type== "noflags")
				basattr.m_special_flags |= Damage::NOFLAGS;
			else if (type ==  "unblockable")
				basattr.m_special_flags |= Damage::UNBLOCKABLE;
			else if (type ==  "ignore_armor")
				basattr.m_special_flags |= Damage::IGNORE_ARMOR;
			else if (type == "extra_human_dmg")
				basattr.m_special_flags |= Damage::EXTRA_HUMAN_DMG;
			else if (type == "extra_demon_dmg")
				basattr.m_special_flags |= Damage::EXTRA_DEMON_DMG;
			else if (type ==  "extra_undead_dmg")
				basattr.m_special_flags |= Damage::EXTRA_UNDEAD_DMG;
			else if (type == "extra_dwarf_dmg")
				basattr.m_special_flags |= Damage::EXTRA_DWARF_DMG;
			else if (type == "extra_drake_dmg")
				basattr.m_special_flags |= Damage::EXTRA_DRAKE_DMG;
			else if (type == "extra_fairy_dmg")
				basattr.m_special_flags |= Damage::EXTRA_FAIRY_DMG;
			else if (type == "extra_goblin_dmg")
				basattr.m_special_flags |= Damage::EXTRA_GOBLIN_DMG;
			else if (type == "extra_animal_dmg")
				basattr.m_special_flags |= Damage::EXTRA_ANIMAL_DMG;
			else if (type == "extra_summoned_dmg")
				basattr.m_special_flags |= Damage::EXTRA_SUMMONED_DMG;
			*/
		}
		else if (child2->Type()!=TiXmlNode::TINYXML_COMMENT)
		{
			WARNING("%s : unexpected element of <BasicAttributes>: %s",m_filename.c_str(),child2->Value());
		}
	}
	
	return true;
}

bool ObjectLoader::loadPlayerData(const char* pFilename)
{
	TiXmlDocument doc(pFilename);
	bool loadOkay = doc.LoadFile();

	if (loadOkay)
	{
		m_filename = pFilename;
		loadPlayer(&doc);
		return true;
	}
	else
	{
		ERRORMSG("Failed to load file %s", pFilename);
		return false;
	}
}

bool ObjectLoader::loadPlayer(TiXmlNode* node)
{
	TiXmlNode* child;
	if (node->Type()==TiXmlNode::TINYXML_ELEMENT && !strcmp(node->Value(), "PlayerClass"))
	{
		ElementAttrib attr;
		attr.parseElement(node->ToElement());
		
		PlayerBasicData* data = new PlayerBasicData;
		attr.getString("type",data->m_subtype);
		attr.getString("name",data->m_name,data->m_subtype);
		for ( child = node->FirstChild(); child != 0; child = child->NextSibling())
		{
			attr.parseElement(child->ToElement());
			if (!strcmp(child->Value(), "BasicAttributes"))
			{
				loadCreatureBaseAttr(child, data->m_base_attr);
			}
			else if (!strcmp(child->Value(), "BasicAbility"))
			{
				attr.getString("type",data->m_base_ability);
			}
			else if (!strcmp(child->Value(), "ItemRequirementTag"))
			{
				std::string name;
				attr.getString("name",name);
				data->m_item_req_tags.push_back(name);
			}
			else if (!strcmp(child->Value(), "Look"))
			{
				PlayerLook look;
				std::string gender;
				look.m_gender = MALE;
				attr.getString("name",look.m_name);
				attr.getString("render_info",look.m_render_info);
				attr.getString("gender",gender);
				if (gender == "female" || gender == "FEMALE")
					look.m_gender = FEMALE;
				attr.getString("emotionset",look.m_emotion_set);
				attr.getString("hair",look.m_hair);
				attr.getString("item_suffix",look.m_item_suffix);
				ObjectFactory::registerPlayerLook(data->m_subtype,look);
			}
			else if (!strcmp(child->Value(), "SkilltreeTabs"))
			{
				attr.getString("tab1",data->m_tabnames[0]);
				attr.getString("tab2",data->m_tabnames[1]);
				attr.getString("tab3",data->m_tabnames[2]);
			}
			else if (!strcmp(child->Value(), "Item"))
			{
				GameObject::Subtype type;
				attr.getString("type",type);
				data->m_start_items.push_back(type);
			}
			else if (!strcmp(child->Value(), "LearnableAbility"))
			{
				int id = data->m_learnable_abilities.size();
				LearnableAbility& ablt = data->m_learnable_abilities[id];
				ablt.m_id = id;
				attr.getString("type",ablt.m_type);
				attr.getInt("skilltree_tab",ablt.m_skilltree_tab);
				attr.getFloat("skilltree_posx",ablt.m_skilltree_position.m_x);
				attr.getFloat("skilltree_posy",ablt.m_skilltree_position.m_y);
				attr.getShort("required_level",ablt.m_req_level);
				attr.getString("image",ablt.m_image);
				
				for (TiXmlNode* child2 = child->FirstChild(); child2 != 0; child2 = child2->NextSibling())
				{
					attr.parseElement(child2->ToElement());
					if (!strcmp(child2->Value(), "RequiredAbility"))
					{
						Action::ActionType atype;
						attr.getString("type",atype);
						ablt.m_req_abilities.push_back(atype);
					}
					else if (child2->Type()!=TiXmlNode::TINYXML_COMMENT)
					{
						WARNING("%s : unexpected element of <LearnableAbility>: %s",m_filename.c_str(),child2->Value());
					}
					
				}
			}
			else if (child->Type()!=TiXmlNode::TINYXML_COMMENT)
			{
				WARNING("%s : unexpected element of <PlayerClass>: %s",child->Value());
			}
		}
		ObjectFactory::registerPlayer(data->m_subtype,data);
		
	}
	else
	{
		// rekursiv durchmustern
		for ( child = node->FirstChild(); child != 0; child = child->NextSibling())
		{
			loadPlayer(child);
		}
	}
	
	return true;
}

bool ObjectLoader::loadObjectData(const char* pFilename)
{
	TiXmlDocument doc(pFilename);
	bool loadOkay = doc.LoadFile();

	if (loadOkay)
	{
		m_filename = pFilename;

		loadObject(&doc);
		return true;
	}
	else
	{
		ERRORMSG("Failed to load file %s", pFilename);
		return false;
	}
}

bool ObjectLoader::loadObject(TiXmlNode* node, bool silent_replace)
{
	
	TiXmlNode* child;
	if (node->Type()==TiXmlNode::TINYXML_ELEMENT && (!strcmp(node->Value(), "Object") 
		   || !strcmp(node->Value(), "ScriptObject")
		   || !strcmp(node->Value(), "Treasure")))
	{
		ElementAttrib attr;
		attr.parseElement(node->ToElement());
		
		FixedObjectData* data;
		ScriptObjectData* sdata =0;
		TreasureBasicData* tdata =0;
		
		if (!strcmp(node->Value(), "ScriptObject"))
		{
			sdata = new ScriptObjectData;
			sdata->m_interaction_flags = WorldObject::USABLE | WorldObject::EXACT_MOUSE_PICKING;
			data = &(sdata->m_fixed_data);
		}
		else if (!strcmp(node->Value(), "Treasure"))
		{
			tdata = new TreasureBasicData;
			data = &(tdata->m_fixed_data);
		}
		else
		{
			data = new FixedObjectData;
		}
		
		GameObject::Subtype subtype;
		attr.getString("subtype",subtype);
		std::string layer;
		attr.getString("layer",layer, "NORMAL");
		
		if (layer == "NORMAL")
			data->m_layer = WorldObject::LAYER_BASE | WorldObject::LAYER_AIR;
		else if (layer == "BASE")
			data->m_layer = WorldObject::LAYER_BASE;
		else if (layer == "AIR")
			data->m_layer = WorldObject::LAYER_AIR;
		else if (layer == "NOCOLLISION")
			data->m_layer = WorldObject::LAYER_NOCOLLISION;
		
		// Schleife ueber die Elemente von Object
		for ( child = node->FirstChild(); child != 0; child = child->NextSibling())
		{
			if (child->Type()==TiXmlNode::TINYXML_ELEMENT && !strcmp(child->Value(), "Mesh"))
			{
				attr.parseElement(child->ToElement());
				std::string mesh;
				attr.getString("file",mesh);
				
				GraphicManager::registerGraphicMapping(subtype, mesh, silent_replace);
				if (sdata != 0)
				{
					sdata->m_render_info = mesh;
				}
			}
			else if (child->Type()==TiXmlNode::TINYXML_ELEMENT && !strcmp(child->Value(), "RenderInfo"))
			{
				
				std::string mesh;
				attr.parseElement(child->ToElement());
				attr.getString("name",mesh);
				GraphicManager::registerGraphicMapping(subtype, mesh, silent_replace);
				if (sdata != 0)
				{
					sdata->m_render_info = mesh;
				}
			}
			else if (child->Type()==TiXmlNode::TINYXML_ELEMENT && !strcmp(child->Value(), "Geometry"))
			{
				attr.parseElement(child->ToElement());
				std::string shape;
				attr.getString("shape",shape,"CIRCLE");
				if (shape == "RECT")
				{
					data->m_shape.m_type = Shape::RECT;
					attr.getFloat("extent_x",data->m_shape.m_extent.m_x,0);
					attr.getFloat("extent_y",data->m_shape.m_extent.m_y,0);
				}
				else
				{
					data->m_shape.m_type = Shape::CIRCLE;
					attr.getFloat("radius",data->m_shape.m_radius,0);
				}
			}
			else if (child->Type()==TiXmlNode::TINYXML_ELEMENT && !strcmp(child->Value(), "Event") && sdata != 0)
			{
				TriggerType type;
				Event* ev = new Event();
				WorldLoader::loadEvent(child, ev,type);
				sdata->m_events.insert(std::make_pair(type,ev));
			}
			else if (child->Type()==TiXmlNode::TINYXML_ELEMENT && !strcmp(child->Value(), "Flags") && sdata != 0)
			{
				std::string flag;
				attr.parseElement(child->ToElement());
				attr.getString("usable",flag,"");
				if (flag == "true")
					sdata->m_interaction_flags |= WorldObject::USABLE;
				else if (flag == "false")
					sdata->m_interaction_flags &= ~WorldObject::USABLE;
				
				attr.getString("exact_picking",flag,"");
				if (flag == "true")
					sdata->m_interaction_flags |= WorldObject::EXACT_MOUSE_PICKING;
				else if (flag == "false")
					sdata->m_interaction_flags &= ~WorldObject::EXACT_MOUSE_PICKING;
				
				attr.getString("collision_detect",flag,"");
				if (flag == "true")
					sdata->m_interaction_flags |= WorldObject::COLLISION_DETECTION;
				else if (flag == "false")
					sdata->m_interaction_flags &= ~WorldObject::COLLISION_DETECTION;
				
			}
			else if (child->Type()==TiXmlNode::TINYXML_ELEMENT && !strcmp(child->Value(), "Dropslots") && tdata != 0)
			{
				int nr =0;
				for (TiXmlNode* child2 = child->FirstChild(); child2 != 0; child2 = child2->NextSibling())
				{
					attr.parseElement(child2->ToElement());
					attr.getFloat("pbig",tdata->m_drop_slots[nr].m_size_probability[Item::BIG],0);
					attr.getFloat("pmedium",tdata->m_drop_slots[nr].m_size_probability[Item::MEDIUM],0);
					attr.getFloat("psmall",tdata->m_drop_slots[nr].m_size_probability[Item::SMALL],0);
					attr.getFloat("pgold",tdata->m_drop_slots[nr].m_size_probability[Item::GOLD],0);
					tdata->m_drop_slots[nr].m_size_probability[4] =1;
					for (int i=0; i<4; i++)
						tdata->m_drop_slots[nr].m_size_probability[4] -= tdata->m_drop_slots[nr].m_size_probability[i];
						
					attr.getInt("min_level",tdata->m_drop_slots[nr].m_min_level,0);
					attr.getInt("max_level",tdata->m_drop_slots[nr].m_max_level,0);
					attr.getInt("min_gold",tdata->m_drop_slots[nr].m_min_gold,1);
					attr.getInt("max_gold",tdata->m_drop_slots[nr].m_max_gold,1);
					attr.getFloat("magic_prob",tdata->m_drop_slots[nr].m_magic_probability,0);
					attr.getFloat("magic_power",tdata->m_drop_slots[nr].m_magic_power,0);
						
					DEBUGX("Dropslot %i %i %f %f", tdata->m_drop_slots[nr].m_min_level,tdata->m_drop_slots[nr].m_max_level,  tdata->m_drop_slots[nr].m_magic_probability, tdata->m_drop_slots[nr].m_magic_power);
					nr ++;
				}
					
			}
			else if (child->Type()!=TiXmlNode::TINYXML_COMMENT)
			{
				WARNING("%s : unexpected element of <Object>: %s",m_filename.c_str(),child->Value());
			}
			
		}
		
		if (sdata !=0)
		{
			ObjectFactory::registerScriptObject(subtype,sdata);
			
		}
		else if (tdata !=0)
		{
			tdata->m_subtype = subtype;
			ObjectFactory::registerTreasure(subtype,tdata);
		}
		else
		{
			ObjectFactory::registerFixedObject(subtype,data, silent_replace);
		}
	}
	else
	{
		// rekursiv durchmustern
		for ( child = node->FirstChild(); child != 0; child = child->NextSibling())
		{
			loadObject(child);
		}
	}	
	return true;
}

bool ObjectLoader::loadProjectileData(const char* pFilename)
{
	TiXmlDocument doc(pFilename);
	bool loadOkay = doc.LoadFile();

	if (loadOkay)
	{
		m_filename = pFilename;
		loadProjectile(&doc);
		return true;
	}
	else
	{
		ERRORMSG("Failed to load file %s", pFilename);
		return false;
	}
}


bool ObjectLoader::loadProjectile(TiXmlNode* node)
{
	TiXmlNode* child;
	if (node->Type()==TiXmlNode::TINYXML_ELEMENT && !strcmp(node->Value(), "Projectile"))
	{
		ElementAttrib attr;
		attr.parseElement(node->ToElement());
		
		ProjectileBasicData* data = new ProjectileBasicData;
		data->m_flags =0;
		
		attr.getString("subtype",data->m_subtype);
		attr.getFloat("radius",data->m_radius,0.1);
		attr.getFloat("lifetime",data->m_lifetime);
		attr.getInt("counter",data->m_counter);
		attr.getString("new_projectile_type",data->m_new_projectile_type);
		attr.getString("implementation",data->m_implementation);
		attr.getFloat("critical_percent",data->m_crit_percent,1.0);
		
		DEBUGX("type %s counter %i data %p",data->m_subtype.c_str(), data->m_counter,data);
		
		// Schleife ueber die Elemente von Object
		for ( child = node->FirstChild(); child != 0; child = child->NextSibling())
		{
			if (child->Type()==TiXmlNode::TINYXML_ELEMENT)
			{
				attr.parseElement(child->ToElement());
				if (!strcmp(child->Value(), "Mesh"))
				{
					std::string mesh;
					attr.getString("file",mesh);
					
					DEBUGX("mesh %s %s",data->m_subtype.c_str(), mesh.c_str());
					GraphicManager::registerGraphicMapping(data->m_subtype, mesh);
				}
				else if (!strcmp(child->Value(), "RenderInfo"))
				{
					std::string mesh;
					attr.getString("name",mesh);
					
					DEBUGX("mesh %s %s",data->m_subtype.c_str(), mesh.c_str());
					GraphicManager::registerGraphicMapping(data->m_subtype, mesh);
				}
				else if (!strcmp(child->Value(), "Flag"))
				{
					std::string flag;
					attr.getString("name",flag);
					if (flag == "explodes")
					{
						data->m_flags |= Projectile::EXPLODES;
					}
					else if (flag == "multi_explodes")
					{
						data->m_flags |= Projectile::MULTI_EXPLODES;
					}
					else if (flag == "piercing")
					{
						data->m_flags |= Projectile::PIERCING;
								
					}
					else if (flag == "bouncing")
					{
						data->m_flags |= Projectile::BOUNCING;
					}
					else if (flag == "prob_bouncing")
					{
						data->m_flags |= Projectile::PROB_BOUNCING;
					}
				}
				else if (!strcmp(child->Value(), "Effect"))
				{
					Action::loadHybridImplementation(child,data->m_effect);
				}
			}
		}
	
		ObjectFactory::registerProjectile(data->m_subtype,data);
	}
	else
	{
		// rekursiv durchmustern
		for ( child = node->FirstChild(); child != 0; child = child->NextSibling())
		{
			loadProjectile(child);
		}
	}	
	return true;
}

bool ObjectLoader::loadEmotionSet(TiXmlNode* node)
{
	EmotionSet* es = new EmotionSet;
	TiXmlNode* child;
	
	std::string name;
	
	ElementAttrib attr;
	attr.parseElement(node->ToElement());
	
	attr.getString("name",name);
	attr.getString("default_image",es->m_default_image);
	
	for ( child = node->FirstChild(); child != 0; child = child->NextSibling())
	{
		if (child->Type()==TiXmlNode::TINYXML_ELEMENT && !strcmp(child->Value(), "Emotion"))
		{
			ElementAttrib attr;
			attr.parseElement(child->ToElement());
			
			std::string emotion;
			std::string image;
			attr.getString("name",emotion);
			attr.getString("image",image);
			
			es->m_emotion_images[emotion] = image;
			DEBUGX("Emotion set %s emotion %s image %s",name.c_str(), emotion.c_str(), image.c_str());
		}
	}
	
	ObjectFactory::registerEmotionSet(name,es);
	return true;
}
