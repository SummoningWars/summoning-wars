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

#include "templateloader.h"

/*
#include "warrior.h"
#include "mage.h"
#include "archer.h"
#include "priest.h"
#include "fixedobject.h"
#include "monster.h"
*/

#include <iostream>
#include "objectfactory.h"
#include "elementattrib.h"

std::string TemplateLoader::m_filename;

bool TemplateLoader::loadObjectTemplateData(const char* pFilename)
{
	TiXmlDocument doc(pFilename);
	bool loadOkay = doc.LoadFile();

	if (loadOkay)
	{
		m_filename = pFilename;

		loadObjectTemplate(&doc);
		return true;
	}
	else
	{
		ERRORMSG("Failed to load file %s", pFilename);
		return false;
	}
}

bool TemplateLoader::loadObjectTemplate(TiXmlNode* node)
{
	TiXmlNode* child, *child2;
	if (node->Type()==TiXmlNode::TINYXML_ELEMENT && !strcmp(node->Value(), "ObjectTemplate"))
	{
		ElementAttrib attr;
		attr.parseElement(node->ToElement());
		
		ObjectTemplate* templ = new ObjectTemplate;
		attr.getString("type",templ->m_type);
		
		std::string name;
		attr.getString("name",name);
		
		for ( child = node->FirstChild(); child != 0; child = child->NextSibling())
		{
			if (child->Type()==TiXmlNode::TINYXML_ELEMENT)
			{
				attr.parseElement(child->ToElement());
				std::string env,defstr,objname;
				if (!strcmp(child->Value(), "Environment"))
				{
					attr.getString("name",env);
					attr.getString("default",defstr,"false");
					
					templ->addEnvironment(env);
					if (defstr == "true")
					{
						templ->m_default_environment = env;
						DEBUGX("default environment for %s is %s",name.c_str(), env.c_str());
					}
					
					for ( child2 = child->FirstChild(); child2 != 0; child2 = child2->NextSibling())
					{
						if (child2->Type()==TiXmlNode::TINYXML_ELEMENT)
						{
							if (!strcmp(child2->Value(), "Object"))
							{
								attr.parseElement(child2->ToElement());
								attr.getString("name",objname);
								
								templ->addObject (env,objname);
								DEBUGX("added object %s to generic object %s for environment %s",objname.c_str(), name.c_str(), env.c_str());
								
							}
							else if (child2->Type()!=TiXmlNode::TINYXML_COMMENT)
							{
								WARNING("%s : unexpected element of <Environment>: %s",child->Value());
							}
						}
					}
				}
				else if (child->Type()!=TiXmlNode::TINYXML_COMMENT)
				{
					WARNING("%s : unexpected element of <ObjectTemplate>: %s",m_filename.c_str(),child->Value());
				}
				
			}
		}
		DEBUGX("found object template %s",name.c_str());
		
		ObjectFactory::registerObjectTemplate(name,templ);
	}
	else if (node->Type()==TiXmlNode::TINYXML_ELEMENT && !strcmp(node->Value(), "ObjectGroupTemplate"))
	{
		ElementAttrib attr;
		attr.parseElement(node->ToElement());
		
		ObjectGroupTemplate* templ = new ObjectGroupTemplate;
		
		std::string name;
		attr.getString("name",name);
		
		for ( child = node->FirstChild(); child != 0; child = child->NextSibling())
		{
			if (child->Type()==TiXmlNode::TINYXML_ELEMENT)
			{
				attr.parseElement(child->ToElement());
				std::string env,defstr,objname;
				if (!strcmp(child->Value(), "Environment"))
				{
					attr.getString("name",env);
					attr.getString("default",defstr,"false");
					
					templ->addEnvironment(env);
					if (defstr == "true")
					{
						templ->m_default_environment = env;
						DEBUGX("default environment for %s is %s",name.c_str(), env.c_str());
					}
					
					for ( child2 = child->FirstChild(); child2 != 0; child2 = child2->NextSibling())
					{
						if (child2->Type()==TiXmlNode::TINYXML_ELEMENT)
						{
							if (!strcmp(child2->Value(), "ObjectGroup"))
							{
								attr.parseElement(child2->ToElement());
								attr.getString("name",objname);
								
								templ->addObjectGroup (env,objname);
								DEBUGX("added object group %s to generic object group %s for environment %s",objname.c_str(), name.c_str(), env.c_str());
								
							}
							else if (child2->Type()!=TiXmlNode::TINYXML_COMMENT)
							{
								WARNING("%s : unexpected element of <Environment>: %s",m_filename.c_str(),child->Value());
							}
						}
					}
				}
				else if (!strcmp(child->Value(), "Shape"))
				{
					std::string shape;
					attr.getString("type",shape,"CIRCLE");
					if (shape == "RECT")
					{
						templ->m_shape.m_type = Shape::RECT;
						attr.getFloat("extent_x",templ->m_shape.m_extent.m_x,0);
						attr.getFloat("extent_y",templ->m_shape.m_extent.m_y,0);
					}
					else
					{
						templ->m_shape.m_type = Shape::CIRCLE;
						attr.getFloat("radius",templ->m_shape.m_radius,0);
					}
					float angle;
					attr.getFloat("angle",angle,0);
					templ->m_shape.m_angle = angle*3.14159/180;
				}
				else if (child->Type()!=TiXmlNode::TINYXML_COMMENT)
				{
					DEBUGX("unexpected element of <ObjectTemplate>: %s",m_filename.c_str(),child->Value());
				}
				
			}
		}
		DEBUGX("found object group template %s",name.c_str());
		
		ObjectFactory::registerObjectGroupTemplate(name,templ);
	}
	else
	{
	// rekursiv durchmustern
		for ( child = node->FirstChild(); child != 0; child = child->NextSibling())
		{
			loadObjectTemplate(child);
		}
	}

	return true;
}

bool TemplateLoader::loadObjectGroupData(const char* pFilename)
{
	TiXmlDocument doc(pFilename);
	bool loadOkay = doc.LoadFile();

	if (loadOkay)
	{
		m_filename = pFilename;

		loadObjectGroup(&doc);
		return true;
	}
	else
	{
		ERRORMSG("Failed to load file %s", pFilename);
		return false;
	}
}


bool TemplateLoader::loadObjectGroup(TiXmlNode* node)
{
	TiXmlNode* child, *child2;
	if (node->Type()==TiXmlNode::TINYXML_ELEMENT && !strcmp(node->Value(), "ObjectGroup"))
	{
		ElementAttrib attr;
		attr.parseElement(node->ToElement());
		
		std::string name;
		attr.getString("name",name);
		
		ObjectGroup* templ = new ObjectGroup;
		
		for ( child = node->FirstChild(); child != 0; child = child->NextSibling())
		{
			if (child->Type()==TiXmlNode::TINYXML_ELEMENT)
			{
				attr.parseElement(child->ToElement());
				std::string env,defstr,objname;
				
				if (!strcmp(child->Value(), "Shape"))
				{
					std::string shape;
					attr.getString("type",shape,"CIRCLE");
					if (shape == "RECT")
					{
						templ->getShape()->m_type = Shape::RECT;
						attr.getFloat("extent_x",templ->getShape()->m_extent.m_x,0);
						attr.getFloat("extent_y",templ->getShape()->m_extent.m_y,0);
					}
					else
					{
						templ->getShape()->m_type = Shape::CIRCLE;
						attr.getFloat("radius",templ->getShape()->m_radius,0);
					}
					float angle;
					attr.getFloat("angle",angle,0);
					templ->getShape()->m_angle = angle*3.14159/180;
				}
				else if (!strcmp(child->Value(), "WayPoint"))
				{
					Vector pos;
					attr.getFloat("pos_x",pos.m_x);
					attr.getFloat("pos_y",pos.m_y);
					templ->addWaypoint (pos);
					DEBUGX("waypoint in %s at %f %f",name.c_str(), pos.m_x,pos.m_y);
				}
				else if (!strcmp(child->Value(), "ObjectContent"))
				{
					for ( child2 = child->FirstChild(); child2 != 0; child2 = child2->NextSibling())
					{
						if (child2->Type()==TiXmlNode::TINYXML_ELEMENT)
						{
							if (!strcmp(child2->Value(), "Object"))
							{
								ObjectGroup::GroupObject obj;
								
								attr.parseElement(child2->ToElement());
								std::string prob_angle;
								
								attr.getFloat("center_x",obj.m_center.m_x);
								attr.getFloat("center_y",obj.m_center.m_y);
								attr.getString("subtype",obj.m_type);
								attr.getString("name",obj.m_name);
								attr.getFloat("angle",obj.m_angle,0.0);
								attr.getFloat("height",obj.m_height,0.0);
								attr.getFloat("probability",obj.m_probability,1.0);
								attr.getString("prob_angle",prob_angle);
								obj.m_prob_angle = (prob_angle == "true");
								
								obj.m_angle *= 3.14159 / 180.0;
								if (obj.m_height!=0)
								{
									DEBUGX("object %s height %f",name.c_str(), obj.m_height);
								}
								
								DEBUGX("object for %s: %s at %f %f angle %f prob %f",name.c_str(),obj.m_type.c_str(), obj.m_center.m_x, obj.m_center.m_y, obj.m_angle, obj.m_probability);
								
								templ->addObject (obj);
							}
							else if (child2->Type()!=TiXmlNode::TINYXML_COMMENT)
							{
								WARNING("%s : unexpected element of <ObjectContent>: %s",m_filename.c_str(),child->Value());
							}
							
						}
					}
				}
				else if (!strcmp(child->Value(), "Locations"))
				{
					for ( child2 = child->FirstChild(); child2 != 0; child2 = child2->NextSibling())
					{
						if (child2->Type()==TiXmlNode::TINYXML_ELEMENT)
						{
							if (!strcmp(child2->Value(), "Location"))
							{
								std::string lname;
								Vector pos;
								
								attr.parseElement(child2->ToElement());
								
								attr.getFloat("pos_x",pos.m_x);
								attr.getFloat("pos_y",pos.m_y);
								attr.getString("name",lname);
								
								templ->addLocation(lname,pos);
								DEBUGX("location for %s: %s at %f %f",name.c_str(),lname.c_str(), pos.m_x, pos.m_y);
							}
							else if (child2->Type()!=TiXmlNode::TINYXML_COMMENT)
							{
								WARNING("%s : unexpected element of <Locations>: %s",m_filename.c_str(),child->Value());
							}
						}
					}
				}
				else if (!strcmp(child->Value(), "Areas"))
				{
					for ( child2 = child->FirstChild(); child2 != 0; child2 = child2->NextSibling())
					{
						if (child2->Type()==TiXmlNode::TINYXML_ELEMENT)
						{
							if (!strcmp(child2->Value(), "Area"))
							{
								std::string lname;
								Shape shape;
								
								attr.parseElement(child2->ToElement());
								std::string stype;
								
								attr.getFloat("center_x",shape.m_center.m_x);
								attr.getFloat("center_y",shape.m_center.m_y);
								attr.getString("type",stype,"CIRCLE");
								if (stype == "RECT")
								{
									shape.m_type = Shape::RECT;
									attr.getFloat("extent_x",shape.m_extent.m_x,0);
									attr.getFloat("extent_y",shape.m_extent.m_y,0);
								}
								else
								{
									shape.m_type = Shape::CIRCLE;
									attr.getFloat("radius",shape.m_radius,0);
								}
								attr.getFloat("angle",shape.m_angle);
								shape.m_angle *= 3.14159 / 180.0;
								attr.getString("name",lname);
								
								templ->addArea(lname,shape);
								
								DEBUGX("area for %s: %s",name.c_str(),lname.c_str());
							}
							else if (child2->Type()!=TiXmlNode::TINYXML_COMMENT)
							{
								WARNING("%s : unexpected element of <Areas>: %s",m_filename.c_str(),child->Value());
							}
						}
					}
				}
				else if (child->Type()!=TiXmlNode::TINYXML_COMMENT)
				{
					WARNING("%s : unexpected element of <ObjectGroup>: %s",m_filename.c_str(),child->Value());
				}
			}
		}
		
		ObjectFactory::registerObjectGroup(name,templ);
	}
	else
	{
		// rekursiv durchmustern
		for ( child = node->FirstChild(); child != 0; child = child->NextSibling())
		{
			loadObjectGroup(child);
		}
	}

	return true;
}
