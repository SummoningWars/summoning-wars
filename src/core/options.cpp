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

#include "options.h"

#include "sound.h"
#include "music.h"
#include "gettext.h"

#include "OISKeyboard.h"

#include <tinyxml.h>
#include "elementattrib.h"

extern "C"
{
	
	#include "lua.h"
	#include "lualib.h"
	#include "lauxlib.h"
}

#include "eventsystem.h"


#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include "Windows.h"
#endif

Options::Options()
{
	init();
}

void Options::init()
{
	// Fixed shortkeys
	m_shortkey_map[OIS::KC_ESCAPE] =  CLOSE_ALL;
	m_shortkey_map[OIS::KC_RETURN] =  SHOW_CHATBOX_NO_TOGGLE;
	m_shortkey_map[OIS::KC_TAB] =  SWAP_RIGHT_ACTION;
	m_shortkey_map[OIS::KC_F10] =  SHOW_OPTIONS;

	m_shortkey_map[OIS::KC_1] =  USE_POTION;
	m_shortkey_map[OIS::KC_2] =  (ShortkeyDestination) (USE_POTION+1);
	m_shortkey_map[OIS::KC_3] =  (ShortkeyDestination) (USE_POTION+2);
	m_shortkey_map[OIS::KC_4] =  (ShortkeyDestination) (USE_POTION+3);
	m_shortkey_map[OIS::KC_5] =  (ShortkeyDestination) (USE_POTION+4);
	m_shortkey_map[OIS::KC_6] =  (ShortkeyDestination) (USE_POTION+5);
	m_shortkey_map[OIS::KC_7] =  (ShortkeyDestination) (USE_POTION+6);
	m_shortkey_map[OIS::KC_8] =  (ShortkeyDestination) (USE_POTION+7);
	m_shortkey_map[OIS::KC_9] =  (ShortkeyDestination) (USE_POTION+8);
	m_shortkey_map[OIS::KC_0] =  (ShortkeyDestination) (USE_POTION+9);

	// special keys that may not be used as shortkeys
	m_special_keys.insert(OIS::KC_ESCAPE);
	m_special_keys.insert(OIS::KC_LSHIFT);
	m_special_keys.insert(OIS::KC_TAB);
	m_special_keys.insert(OIS::KC_RETURN);
	m_special_keys.insert(OIS::KC_BACK);
	m_special_keys.insert(OIS::KC_UP);
	m_special_keys.insert(OIS::KC_DOWN);
	m_special_keys.insert(OIS::KC_LEFT);
	m_special_keys.insert(OIS::KC_RIGHT);
	m_special_keys.insert(OIS::KC_CAPITAL);
	m_special_keys.insert(OIS::KC_0);
	m_special_keys.insert(OIS::KC_1);
	m_special_keys.insert(OIS::KC_2);
	m_special_keys.insert(OIS::KC_3);
	m_special_keys.insert(OIS::KC_4);
	m_special_keys.insert(OIS::KC_5);
	m_special_keys.insert(OIS::KC_6);
	m_special_keys.insert(OIS::KC_7);
	m_special_keys.insert(OIS::KC_8);
	m_special_keys.insert(OIS::KC_9);

	m_difficulty = NORMAL;
	m_text_speed = 1.0;
	m_enemy_highlight_color = "red";
	m_show_console_window = true;
	m_shadow_mode = SM_NONE;
}

Options* Options::getInstance()
{
	static Options m_instance;
	return &m_instance;
}

void Options::setToDefaultOptions()
{
	init();

	// shortkeys that may be changed
	m_shortkey_map[OIS::KC_I] = SHOW_INVENTORY ;
	m_shortkey_map[OIS::KC_C] =  SHOW_CHARINFO;
	m_shortkey_map[OIS::KC_T] =  SHOW_SKILLTREE;
	m_shortkey_map[OIS::KC_P] =  SHOW_PARTYMENU;
	m_shortkey_map[OIS::KC_M] =  SHOW_MINIMAP;
	m_shortkey_map[OIS::KC_B] =  SHOW_CHATBOX;
	m_shortkey_map[OIS::KC_Q] =  SHOW_QUESTINFO;
	m_shortkey_map[OIS::KC_W] =  SWAP_EQUIP;
	m_shortkey_map[OIS::KC_LMENU] =  SHOW_ITEMLABELS;


	setSoundVolume(1.0);
	setMusicVolume(1.0);

	setMaxNumberPlayers(8);
	setPort(5331);
	setServerHost("127.0.0.1");
}

bool Options::readFromFile(const std::string& filename)
{
	TiXmlDocument doc(filename.c_str());
	bool loadOkay = doc.LoadFile();

	ElementAttrib attr;

	if (loadOkay)
	{
		TiXmlNode* child;
		TiXmlNode* child2;
		TiXmlNode* root;

		root = doc.FirstChild();
		root = root->NextSibling();
		if (root->Type() == TiXmlNode::TINYXML_ELEMENT && !strcmp(root->Value(), "Options"))
		{
			for ( child = root->FirstChild(); child != 0; child = child->NextSibling())
			{
				if (child->Type()==TiXmlNode::TINYXML_ELEMENT)
				{
					attr.parseElement(child->ToElement());
					std::string env,defstr,objname;
					if (!strcmp(child->Value(), "Shortkeys"))
					{
						for ( child2 = child->FirstChild(); child2 != 0; child2 = child2->NextSibling())
						{
							if (child2->Type()==TiXmlNode::TINYXML_ELEMENT && !strcmp(child2->Value(), "Shortkey"))
							{
								attr.parseElement(child2->ToElement());

								int key, target;
								attr.getInt("key",key);
								attr.getInt("target",target);
								if (key != 0 && target != 0)
								{
									setShortkey(key, (ShortkeyDestination) target);
								}
							}
						}
					}
					else if (!strcmp(child->Value(), "Music"))
					{
						float volume;
						attr.getFloat("volume", volume, 1.0f);
						setMusicVolume(volume);
					}
					else if (!strcmp(child->Value(), "Sound"))
					{
						float volume;
						attr.getFloat("volume", volume, 1.0f);
						setSoundVolume(volume);
					}
					else if (!strcmp(child->Value(), "Language"))
					{
						std::string locale;
						attr.getString("locale",locale);
						setLocale(locale);
					}
					else if (!strcmp(child->Value(), "Gameplay"))
					{
						int diff;
						attr.getInt("difficulty",diff);
						setDifficulty( static_cast<Difficulty>(diff));
						float text_speed;
						attr.getFloat("text_speed",text_speed);
						setTextSpeed( text_speed);

						int showConsoleWindow;
						attr.getInt ("showConsoleWindow", showConsoleWindow, 1);
						setShowConsoleWindow (showConsoleWindow);
					}
					else if (!strcmp(child->Value(), "Network"))
					{
						std::string host;
						int port;
						int max_players;
						attr.getString("host",host,"127.0.0.1");
						attr.getInt("port",port,5331);
						attr.getInt("max_players",max_players,8);

						setPort(port);
						setServerHost(host);
						setMaxNumberPlayers(max_players);
					}
					else if (!strcmp(child->Value(), "Graphic"))
					{
						std::string color = getEnemyHighlightColor();
						attr.getString("ehl_color", color);
						setEnemyHighlightColor(color);
						
						int grabMouse;
						attr.getInt("grabMouseWhenWindowed", grabMouse);
						setGrabMouseInWindowedMode(grabMouse);

						int shadowMode;
						attr.getInt ("shadowMode", shadowMode);
						if (shadowMode > SM_NONE && shadowMode < SM_COUNT)
						{
							setShadowMode (static_cast<ShadowMode>(shadowMode));
						}
						// otherwise it's an invalid value.

						// Other graphic options read from different files (E.g. ogre.cfg)

					}
					else if (!strcmp(child->Value(), "Debug"))
					{
						for (const TiXmlAttribute* attr = child->ToElement()->FirstAttribute(); attr != 0; attr = attr->Next())
						{
							m_debug_options[attr->Name()] = attr->Value();
						}
					}
					else if (child->Type()!=TiXmlNode::TINYXML_COMMENT)
					{
						WARNING("unexpected element in options.xml: %s",child->Value());
					}
				}
			}
		}  // if root == <Options>
		else
		{
			setToDefaultOptions();
			return false;
		}
	}  // if (loadOkay)
	else
	{
		setToDefaultOptions();
		return false;
	}
	return true;
}

bool Options::writeToFile(const std::string& filename)
{
	TiXmlDocument doc;
	TiXmlDeclaration * decl = new TiXmlDeclaration( "1.0", "UTF-8", "" );
	doc.LinkEndChild( decl );

	TiXmlElement* root;
	root = new TiXmlElement("Options");
	doc.LinkEndChild(root);

	TiXmlElement * element;
	element = new TiXmlElement( "Shortkeys" );
	root->LinkEndChild(element);

	TiXmlElement * subele;
	ShortkeyMap::iterator it;
	for (it = m_shortkey_map.begin(); it != m_shortkey_map.end(); ++it)
	{
		subele = new TiXmlElement("Shortkey");
		subele->SetAttribute("key",it->first);
		subele->SetAttribute("target",it->second);
		element->LinkEndChild(subele);
	}

	element = new TiXmlElement( "Gameplay" );
	root->LinkEndChild(element);
	element->SetAttribute("difficulty",getDifficulty());
	element->SetDoubleAttribute("text_speed",getTextSpeed());
	element->SetAttribute ("showConsoleWindow", getShowConsoleWindow ());

	element = new TiXmlElement( "Music" );
	root->LinkEndChild(element);
	element->SetDoubleAttribute("volume",getMusicVolume());

	element = new TiXmlElement( "Sound" );
	element->SetDoubleAttribute("volume",getSoundVolume());
	root->LinkEndChild(element);

	element = new TiXmlElement("Graphic");
	root->LinkEndChild(element);
	element->SetAttribute("ehl_color", getEnemyHighlightColor().c_str());
	element->SetAttribute("grabMouseWhenWindowed", getGrabMouseInWindowedMode());
	element->SetAttribute("display_mode", getUsedDisplayMode());
	element->SetAttribute ("shadowMode", getShadowMode ());
	
	element = new TiXmlElement( "Language" );
	root->LinkEndChild(element);
	element->SetAttribute("locale",getLocale().c_str());

	element = new TiXmlElement( "Network" );
	root->LinkEndChild(element);
	element->SetAttribute("host",getServerHost().c_str());
	element->SetAttribute("port",getPort());
	element->SetAttribute("max_players",getMaxNumberPlayers());
	
	if (!m_debug_options.empty())
	{
		element = new TiXmlElement( "Debug" );
		root->LinkEndChild(element);
		std::map<std::string, std::string>::iterator it;
		for(it = m_debug_options.begin(); it != m_debug_options.end(); ++it)
		{
			element->SetAttribute(it->first.c_str(), it->second.c_str());
		}
	}

	doc.SaveFile( filename.c_str());
	return true;
}

KeyCode Options::getMappedKey(ShortkeyDestination sd)
{
	std::map<KeyCode, ShortkeyDestination>::iterator it;
	for (it=m_shortkey_map.begin(); it!= m_shortkey_map.end();++it)
	{
		if (it->second == sd)
		{
			return it->first;
		}
	}

	return 0;
}

ShortkeyDestination Options::getMappedDestination(KeyCode key)
{
	std::map<KeyCode, ShortkeyDestination>::iterator it;
	it = m_shortkey_map.find(key);

	if (it != m_shortkey_map.end())
		return it->second;

	return NO_KEY;
}

bool Options:: setShortkey(KeyCode key,ShortkeyDestination dest)
{
	if (isSpecialKey(key))
		return false;

	// key that was mapped to the action dest so far
	KeyCode oldkey = getMappedKey(dest);
	// delete mapping
	if (oldkey != 0)
	{
		m_shortkey_map.erase(oldkey);
	}

	// create new mapping
	m_shortkey_map[key]=dest;
	return true;
}

void Options::setSoundVolume(float vol)
{
	SoundSystem::setSoundVolume(vol);
}

float Options::getSoundVolume()
{
	return SoundSystem::getSoundVolume();
}

void Options::setMusicVolume(float vol)
{
	MusicManager::instance().setMusicVolume(vol);
}

float Options::getMusicVolume()
{
	return MusicManager::instance().getMusicVolume();
}

std::string Options::getLocale()
{
	const char* locale = Gettext::getLocale();

	std::string locstr = "";
	if (locale != 0)
	{
		locstr = locale;
	}
	if (locstr == "")
		locstr = "#default#";

	return locstr;
}

void Options::setLocale(const std::string& locale)
{
	if (locale!= "#default#" && locale != "")
	{
		Gettext::setLocale(locale.c_str());
	}
}

int Options::getValue(std::string valname)
{
	if (valname =="server_host")
	{
		lua_pushstring(EventSystem::getLuaState() , m_server_host.c_str() );
		return 1;
	}
	else if (valname =="port")
	{
		lua_pushnumber(EventSystem::getLuaState() , m_port );
		return 1;
	}
	else if (valname =="difficulty")
	{
		std::string values[4] = {"easy","normal","hard","insane"};
		lua_pushstring(EventSystem::getLuaState() , values[m_difficulty- EASY].c_str() );
		return 1;
	}
	else if (valname =="text_speed")
	{
		lua_pushnumber(EventSystem::getLuaState() , m_text_speed );
		return 1;
	}
	else
	{
		if (m_debug_options.count(valname))
		{
			lua_pushstring(EventSystem::getLuaState() , m_debug_options[valname].c_str() );
			return 1;
		}
	}
	return 0;
}

bool Options::setValue(std::string valname)
{
	if (valname =="server_host")
	{
		m_server_host = lua_tostring(EventSystem::getLuaState() ,-1);
		lua_pop(EventSystem::getLuaState(), 1);
		return true;
	}
	else if (valname =="port")
	{
		m_port = lua_tonumber(EventSystem::getLuaState() ,-1);
		lua_pop(EventSystem::getLuaState(), 1);
		return true;
	}
	else if (valname =="difficulty")
	{
		std::string diffstr = lua_tostring(EventSystem::getLuaState() ,-1);
		if (diffstr == "easy") m_difficulty = EASY;
		if (diffstr == "normal") m_difficulty = NORMAL;
		if (diffstr == "hard") m_difficulty = HARD;
		if (diffstr == "insane") m_difficulty = INSANE;
		
		lua_pop(EventSystem::getLuaState(), 1);
		return true;
	}
	else if (valname =="text_speed")
	{
		m_text_speed = lua_tonumber(EventSystem::getLuaState() ,-1);
		lua_pop(EventSystem::getLuaState(), 1);
		return true;
	}
	else
	{
		std::string value = lua_tostring(EventSystem::getLuaState() ,-1);
		m_debug_options[valname] = value;
	}

	return false;
}
