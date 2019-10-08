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

#include "worldloader.h"
#include "world.h"
#include "fraction.h"

#include <iostream>

std::string WorldLoader::m_filename;

void WorldLoader::loadEvent( TiXmlNode* node, Event *ev, TriggerType &type)
{
	ElementAttrib attrib;
	
	TiXmlNode* child;
	TiXmlText* text;
	attrib.parseElement(node->ToElement());
	attrib.getString("trigger", type);
	
	std::string once;
	attrib.getString("once", once,"false");
	ev->setOnce(once=="true");

	for ( child = node->FirstChild(); child != 0; child = child->NextSibling())
	{
		if (!strcmp(child->Value(), "Condition"))
		{
			text =0;
			if (child->FirstChild()!=0)
				text = child->FirstChild()->ToText();
			if (text != 0)
			{
				ev->setCondition(text->Value());
			}
		}
		else if (!strcmp(child->Value(), "Effect"))
		{
			text =0;
			if (child->FirstChild()!=0)
				text = child->FirstChild()->ToText();
			if (text != 0)
			{
				ev->setEffect(text->Value());
			}
		}
		else if (child->Type()!=TiXmlNode::TINYXML_COMMENT)
		{
			WARNING("%s : unexpected element of <Event>: %s",m_filename.c_str(),child->Value());
		}
	}
}


bool WorldLoader::loadRegionData(const char* pFilename)
{
	TiXmlDocument doc(pFilename);
	bool loadOkay = doc.LoadFile();

	if (loadOkay)
	{
		m_filename = pFilename;

		loadRegions(&doc);
		return true;
	}
	else
	{
		ERRORMSG("Failed to load file %s", pFilename);
		return false;
	}
}

bool WorldLoader::loadRegions(TiXmlNode* node)
{
	TiXmlNode* child;
	RegionData* rdata=0;
	if (node->Type()==TiXmlNode::TINYXML_ELEMENT && !strcmp(node->Value(), "Region"))
	{
		ElementAttrib attr;
		attr.parseElement(node->ToElement());
		
		rdata = new RegionData;
		
		int id;
		attr.getInt("id",id,-1);
		rdata->m_id = id;
		attr.getString("name",rdata->m_name);
		DEBUGX("Region %s %i %p",rdata->m_name.c_str(), id, rdata);
		World::getWorld()->registerRegionData(rdata, rdata->m_id);
		loadRegion(node,rdata);
		
	}
	else if (!strcmp(node->Value(), "Fraction"))
	{
		loadFraction(node);
	}
	else
	{
		if (node->Type()==TiXmlNode::TINYXML_ELEMENT && !strcmp(node->Value(), "World"))
		{
			ElementAttrib attr;
			attr.parseElement(node->ToElement());
			
			attr.getString("start_region",World::getWorld()->getPlayerStartLocation().first,World::getWorld()->getPlayerStartLocation().first);
			
			attr.getString("start_location",World::getWorld()->getPlayerStartLocation().second,World::getWorld()->getPlayerStartLocation().second);
			
		}
		
		for ( child = node->FirstChild(); child != 0; child = child->NextSibling())
		{
			loadRegions(child);
		}
	}
	return true;
}

bool WorldLoader::loadRegion(TiXmlNode* node, RegionData* rdata)
{
	
	TiXmlNode* child;
	TiXmlNode* child2;
	for ( child = node->FirstChild(); child != 0; child = child->NextSibling())
	{
		if (child->Type()==TiXmlNode::TINYXML_ELEMENT)
		{
			ElementAttrib attr;
			attr.parseElement(child->ToElement());
			
			if (!strcmp(child->Value(), "Settings"))
			{
				int dimx,dimy;
				attr.getInt("dimx",dimx,16);
				attr.getInt("dimy",dimy,16);
				rdata->m_dimx = dimx;
				rdata->m_dimy = dimy;
				attr.getString("region_template",rdata->m_region_template);
				attr.getString("ground",rdata->m_ground_material);
				attr.getFloat("area_percent",rdata->m_area_percent,0.5f);
				attr.getFloat("complexity",rdata->m_complexity,0.5f);
				attr.getInt("granularity",rdata->m_granularity,8);
			}
			else if (!strcmp(child->Value(), "Music"))
			{
				for ( child2 = child->FirstChild(); child2 != 0; child2 = child2->NextSibling())
				{
					
					if (!strcmp(child2->Value(), "Track"))
					{
						ElementAttrib attr;
						attr.parseElement(child2->ToElement());
						
						std::string music;
						attr.getString("source",music);
						if (music != "")
						{
							rdata->m_music_tracks.push_back(music);
						}
					}
					else if (child2->Type()!=TiXmlNode::TINYXML_COMMENT)
					{
						WARNING("%s : unexpected element of <Music>: %s",m_filename.c_str(),child2->Value());
					}
				}
			}
			else if (!strcmp(child->Value(), "ExitDirections"))
			{
				std::string boolstr;
				attr.getString("north",boolstr,"false");
				rdata->m_exit_directions[NORTH] = (boolstr =="true");
				attr.getString("south",boolstr,"false");
				rdata->m_exit_directions[SOUTH] = (boolstr =="true");
				attr.getString("west",boolstr,"false");
				rdata->m_exit_directions[WEST] = (boolstr =="true");
				attr.getString("east",boolstr,"false");
				rdata->m_exit_directions[EAST] = (boolstr =="true");
					
			}
			else if (!strcmp(child->Value(), "Exits"))
			{
				for ( child2 = child->FirstChild(); child2 != 0; child2 = child2->NextSibling())
				{
					
					if (!strcmp(child2->Value(), "Exit"))
					{
						RegionExit exit;
						attr.parseElement(child2->ToElement());
						attr.getString("name",exit.m_exit_name);
						attr.getString("destination_region",exit.m_destination_region);
						attr.getString("destination_location",exit.m_destination_location);
						
						TiXmlNode* child3;
						for ( child3 = child2->FirstChild(); child3 != 0; child3 = child3->NextSibling())
						{
							
							if (!strcmp(child3->Value(), "Shape"))
							{
								attr.parseElement(child3->ToElement());
								std::string shape;
								attr.getString("shape",shape,"CIRCLE");
								if (shape == "RECT")
								{
									exit.m_shape.m_type = Shape::RECT;
									attr.getFloat("extent_x",exit.m_shape.m_extent.m_x,0);
									attr.getFloat("extent_y",exit.m_shape.m_extent.m_y,0);
								}
								else
								{
									exit.m_shape.m_type = Shape::CIRCLE;
									attr.getFloat("radius",exit.m_shape.m_radius,0);
								}
							}
							else if (child3->Type()!=TiXmlNode::TINYXML_COMMENT)
							{
								WARNING("%s : unexpected element of <Exit>: %s",m_filename.c_str(),child3->Value());
							}
						}
						rdata->addExit(exit);
					}
					else if (child2->Type()!=TiXmlNode::TINYXML_COMMENT)
					{
						WARNING("%s : unexpected element of <Exits>: %s",m_filename.c_str(),child2->Value());
					}
				}
			}
			else if (!strcmp(child->Value(), "Environments"))
			{
				for ( child2 = child->FirstChild(); child2 != 0; child2 = child2->NextSibling())
				{
					if (!strcmp(child2->Value(), "Environment"))
					{
						std::string envname;
						float height;
						attr.parseElement(child2->ToElement());
						attr.getString("name",envname);
						attr.getFloat("height",height);
						rdata->addEnvironment(height,envname);
					}
					else if (child2->Type()!=TiXmlNode::TINYXML_COMMENT)
					{
						WARNING("%s : unexpected element of <Environments>: %s",m_filename.c_str(),child2->Value());
					}
				}
			}
			else if (!strcmp(child->Value(), "ReviveLocation"))
			{
				attr.getString("location",rdata->m_revive_location);
			}
			else if (!strcmp(child->Value(), "WorldPosition"))
			{
				WaypointInfo winfo;
				winfo.m_id = rdata->m_id;
				winfo.m_name = rdata->m_name;
				attr.getFloat("world_x",winfo.m_world_coord.m_x);
				attr.getFloat("world_y",winfo.m_world_coord.m_y);
				
				bool waypoint;
				attr.getBool("waypoint",waypoint,false);
				rdata->m_has_waypoint = waypoint;
				
				World::getWorld()->addWaypoint(winfo.m_id,winfo);
			}
			else if (!strcmp(child->Value(), "Event"))
			{
				TriggerType type;
				Event* ev = new Event();
				loadEvent(child, ev,type);
				
				if (type == "")
				{
					WARNING("%s : Detected <Event> Tag without trigger",m_filename.c_str());
				}
				
				World::getWorld()->addEvent(rdata->m_name,type,ev);
			}
			else if (!strcmp(child->Value(), "NamedObjectGroups"))
			{
				for ( child2 = child->FirstChild(); child2 != 0; child2 = child2->NextSibling())
				{
					if ((!strcmp(child2->Value(), "NameObjectGroup")) || (!strcmp(child2->Value(), "NamedObjectGroup")))
					{
						attr.parseElement(child2->ToElement());
						std::string group_name, name;
						int prio;
						float angle;
						attr.getString("name",name);
						attr.getString("object_group",group_name);
						if (name == "")
						{
							WARNING("%s : <NamedObjectGroup> tag without name found (type is %s)",m_filename.c_str(),group_name.c_str());
						}
						
						attr.getFloat("angle",angle,0);
						angle *= 3.14159/180;
						attr.getInt("priority",prio);
						
						DEBUGX("named object group %s %s %i",group_name.c_str(), name.c_str(), prio);
						
						rdata->addNamedObjectGroup(group_name,name,angle, prio);
					}
					else if (child2->Type()!=TiXmlNode::TINYXML_COMMENT)
					{
						WARNING("%s : unexpected element of <NamedObjectGroups>: %s",m_filename.c_str(),child2->Value());
					}
				}
			}
			else if (!strcmp(child->Value(), "ObjectGroups"))
			{
				for ( child2 = child->FirstChild(); child2 != 0; child2 = child2->NextSibling())
				{
					if (!strcmp(child2->Value(), "ObjectGroup"))
					{
						attr.parseElement(child2->ToElement());
						std::string group_name;
						int prio,number;
						float prob;
						std::string deco;
						attr.getString("object_group",group_name);
						attr.getInt("priority",prio,0);	
						attr.getInt("number",number,1);	
						attr.getFloat("probability",prob,1.0f);	
						attr.getString("decoration",deco,"true");	
						
						DEBUGX("object group %s  number %i prio %i",group_name.c_str(), number, prio);
						
						rdata->addObjectGroup(group_name,prio, number, prob,(deco=="true"));
					}
					else if (child2->Type()!=TiXmlNode::TINYXML_COMMENT)
					{
						WARNING("%s : unexpected element of <ObjectGroups>: %s",m_filename.c_str(),child2->Value());
					}
				}
			}
			else if (!strcmp(child->Value(), "SpawnGroups"))
			{
				for ( child2 = child->FirstChild(); child2 != 0; child2 = child2->NextSibling())
				{
					if (!strcmp(child2->Value(), "SpawnGroup"))
					{
						attr.parseElement(child2->ToElement());
						std::string group_name;
						int number;
						attr.getInt("number",number,1);
						attr.getString("group_name",group_name);
						
						rdata->addSpawnGroup(group_name,number);
					}
					else if (child2->Type()!=TiXmlNode::TINYXML_COMMENT)
					{
						WARNING("%s : unexpected element of <SpawnGroup>: %s",m_filename.c_str(),child2->Value());
					}
				}
			}
			else if (!strcmp(child->Value(), "AmbientLight"))
			{
				attr.getFloat("red",rdata->m_ambient_light[0],0.2f);
				attr.getFloat("green",rdata->m_ambient_light[1],0.2f);
				attr.getFloat("blue",rdata->m_ambient_light[2],0.2f);
			}
			else if (!strcmp(child->Value(), "HeroLight"))
			{
				attr.getFloat("red",rdata->m_hero_light[0],1.0f);
				attr.getFloat("green",rdata->m_hero_light[1],1.0f);
				attr.getFloat("blue",rdata->m_hero_light[2],1.0f);
			}
			else if (!strcmp(child->Value(), "DirectionalLight"))
			{
				attr.getFloat("red",rdata->m_directional_light[0],0.3f);
				attr.getFloat("green",rdata->m_directional_light[1],0.3f);
				attr.getFloat("blue",rdata->m_directional_light[2],0.3f);
			}
			else if (child->Type()!=TiXmlNode::TINYXML_COMMENT)
			{
				WARNING("%s : unexpected element of <Region>: %s",m_filename.c_str(),child->Value());
			}
		}
	}
	return true;
}

bool  WorldLoader::loadNPCData(const char* pFilename)
{
	TiXmlDocument doc(pFilename);
	bool loadOkay = doc.LoadFile();

	if (loadOkay)
	{
		m_filename = pFilename;

		loadNPC(&doc);
		return true;
	}
	else
	{
		ERRORMSG("Failed to load file %s", pFilename);
		return false;
	}
}

void  WorldLoader::loadNPC( TiXmlNode* node)
{
	TiXmlNode* child;
	
	if (node->Type()==TiXmlNode::TINYXML_ELEMENT && !strcmp(node->Value(), "NPC"))
	{
		ElementAttrib attr;
		attr.parseElement(node->ToElement());
		
		std::string refname;
		std::string topic;
		std::string start_op;
		Event* ev;
		TriggerType dummy;
		NPCTrade::TradeObject tradeobj;
		attr.getString("refname",refname,"");
		
		if (refname == "")
		{
			WARNING("%s: <NPC> tag without refname attribute found",m_filename.c_str());
			return;
		}
		
		for ( child = node->FirstChild(); child != 0; child = child->NextSibling())
		{
			if (child->Type()==TiXmlNode::TINYXML_ELEMENT && !strcmp(child->Value(), "Topic"))
			{
				attr.parseElement(child->ToElement());
				attr.getString("name",topic,"topic");
				attr.getString("start_option",start_op);
				
				DEBUGX("found Topic %s for %s",topic.c_str(),refname.c_str());
				
				ev = new Event;
				ev->setGettextDomain(EventSystem::GetGettextDomain());
				
				// Topic kann mit der selben Funktion geladen werden wie Events
				loadEvent(child,ev,dummy);
				
				Dialogue::getTopicList(refname).addTopic(topic,ev);
				if (start_op != "" || topic == "start_dialogue")
				{
					Dialogue::getTopicList(refname).addStartTopic(TranslatableString(start_op,EventSystem::GetGettextDomain()), topic);
				}
			}
			else if (child->Type()==TiXmlNode::TINYXML_ELEMENT && !strcmp(child->Value(), "Trade"))
			{
				attr.parseElement(child->ToElement());
				float refreshtime,price_factor, buy_factor;
				attr.getFloat("refresh_time", refreshtime,3600000.0f);
				attr.getFloat("price_factor",price_factor,1.5f);
				attr.getFloat("pay_factor",buy_factor,1.0f);
				
				DEBUGX("trade conditions %f %f",refreshtime, price_factor);
				NPCTrade& tradeinfo = Dialogue::getNPCTrade(refname);
				
				tradeinfo.m_cost_multiplier = price_factor;
				tradeinfo.m_pay_multiplier = buy_factor;
				tradeinfo.m_refresh_time = refreshtime;
				
				// TradeObject einlesen
				for (TiXmlNode* child2 = child->FirstChild(); child2 != 0; child2 = child2->NextSibling())
				{
					attr.parseElement(child2->ToElement());
					
					attr.getString("subtype", tradeobj.m_subtype);
					attr.getInt("number", tradeobj.m_number,1);
					attr.getInt("number_magical",tradeobj.m_number_magical,0);
					attr.getFloat("min_enchant", tradeobj.m_min_enchant,0);
					attr.getFloat("max_enchant", tradeobj.m_max_enchant,0);
					attr.getFloat("probability", tradeobj.m_probability,1);
					
					tradeinfo.m_trade_objects.push_back(tradeobj);
					DEBUGX("new trade Object for %s : %s %i %i %f %f",refname.c_str(),tradeobj.m_subtype.c_str(), tradeobj.m_number, tradeobj.m_number_magical, tradeobj.m_min_enchant, tradeobj.m_max_enchant);
				}
			}
			else if (child->Type()!=TiXmlNode::TINYXML_COMMENT)
			{
				WARNING("%s : unexpected element of <NPC>: %s",m_filename.c_str(),child->Value());
			}
		}
	}
	else
	{
		for ( child = node->FirstChild(); child != 0; child = child->NextSibling())
		{
			loadNPC(child);
		}
	}
}

bool WorldLoader::loadQuestsData(const char* pFilename)
{
	TiXmlDocument doc(pFilename);
	bool loadOkay = doc.LoadFile();

	DEBUGX("loading quest file %s",pFilename);
	if (loadOkay)
	{
		m_filename = pFilename;

		loadQuests(&doc);
		return true;
	}
	else
	{
		ERRORMSG("Failed to load file %s", pFilename);
		return false;
	}
}

void WorldLoader::loadQuests(TiXmlNode* node)
{
	TiXmlNode* child;
	Quest* qu;
	if (node->Type()==TiXmlNode::TINYXML_ELEMENT && !strcmp(node->Value(), "Quest"))
	{
		ElementAttrib attr;
		attr.parseElement(node->ToElement());
		
		std::string tabname, sname;
		attr.getString("table_name",tabname);
		attr.getString("name",sname);
	
		std::string domain = EventSystem::GetGettextDomain();
		attr.getString("domain",domain);
		TranslatableString name;
		name= sname;
		name.setTextDomain(domain);
		
		EventSystem::pushGettextDomain(domain);
		
		qu = new Quest(name,tabname);
		qu->setGettextDomain(domain);
		
		loadQuest(node,qu);
		qu->init();
		World::getWorld()->addQuest(tabname,qu);
		
		EventSystem::popGettextDomain();
	}
	else
	{
		for ( child = node->FirstChild(); child != 0; child = child->NextSibling())
		{
			loadQuests(child);
		}
	}
}


void WorldLoader::loadQuest(TiXmlNode* node, Quest* quest)
{
	TiXmlNode* child;
	TiXmlText* text;
	for ( child = node->FirstChild(); child != 0; child = child->NextSibling())
	{
		if (child->Type()==TiXmlNode::TINYXML_ELEMENT)
		{
			if ((!strcmp(child->Value(), "Init")))
			{
				if (child->FirstChild()!=0)
				{
					text = child->FirstChild()->ToText();
					quest->setInit(text->Value());
				}
				
			}
			else if (!strcmp(child->Value(), "Description"))
			{
				text = child->FirstChild()->ToText();
				quest->setDescription(text->Value());
			}
			else if (!strcmp(child->Value(), "NPC"))
			{
				loadNPC(child);
			}
			else if (!strcmp(child->Value(), "Region"))
			{
				TriggerType type;
				std::string rname;
				Event* ev;
				TiXmlNode* child2;
				
				ElementAttrib attr;
				attr.parseElement(child->ToElement());
				attr.getString("name",rname);
				DEBUGX("event for region %s",rname.c_str());
				
				for ( child2 = child->FirstChild(); child2 != 0; child2 = child2->NextSibling())
				{
					if (child2->Type()==TiXmlNode::TINYXML_ELEMENT && !strcmp(child2->Value(), "Event"))
					{
						ev = new Event;
						ev->setGettextDomain(EventSystem::GetGettextDomain());
						loadEvent(child2, ev,type);
						if (type == "")
						{
							WARNING("%s : Detected <Event> Tag without trigger",m_filename.c_str());
						}
						
						World::getWorld()->addEvent(rname,type,ev);
					}
					else if (child2->Type()!=TiXmlNode::TINYXML_COMMENT)
					{
						WARNING("%s : unexpected element of <Region>: %s",m_filename.c_str(),child2->Value());
					}
				}
				
			}
			else if (!strcmp(child->Value(), "Fraction"))
			{
				loadFraction(child);
			}
			else if (child->Type()!=TiXmlNode::TINYXML_COMMENT)
			{
				WARNING("%s : unexpected element of <Quest>: %s",m_filename.c_str(),child->Value());
			}
		}
	}
}

void WorldLoader::loadFraction( TiXmlNode* node)
{
	ElementAttrib attr;
	attr.parseElement(node->ToElement());
	
	std::string type;
	attr.getString("type",type);
	
	if (type == "")
		return;
	World::getWorld()->createFraction(type);
	
	TiXmlNode* child;
	for ( child = node->FirstChild(); child != 0; child = child->NextSibling())
	{
		if (child->Type()==TiXmlNode::TINYXML_ELEMENT)
		{
			if ((!strcmp(child->Value(), "Relation")))
			{
				std::string fraction,relation;
				Fraction::Relation rel = Fraction::NEUTRAL;
				
				attr.parseElement(child->ToElement());
				attr.getString("fraction",fraction);
				attr.getString("relation",relation);
				
				if (relation == "ALLIED"  || relation == "allied")
					rel = Fraction::ALLIED;
				
				if (relation == "HOSTILE"  || relation == "hostile")
					rel = Fraction::HOSTILE;
				
				World::getWorld()->setRelation(type,fraction,rel);
			}
		}
	}
}

