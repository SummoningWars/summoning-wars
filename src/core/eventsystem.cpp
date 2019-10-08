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


#include <OgreRoot.h>
#include <OgreRenderTarget.h>
#include <OgreRenderWindow.h>

#include "eventsystem.h"
#include "region.h"
#include "player.h"
#include "item.h"
#include "itemfactory.h"
#include "dialogue.h"
#include "scriptobject.h"
#include "options.h"
#include "sound.h"
#include "sumwarshelper.h"
#include <algorithm> 
#include <cctype>

#ifdef DEBUG_DATABASE
		std::map<int, std::string> EventSystem::m_code_fragments;
#endif



lua_State * EventSystem::m_lua=0;

Region* EventSystem::m_region =0;

Trigger*  EventSystem::m_trigger=0;

Dialogue*  EventSystem::m_dialogue=0;

Damage* EventSystem::m_damage=0;

CreatureBaseAttrMod* EventSystem::m_base_mod=0;

CreatureDynAttrMod* EventSystem::m_dyn_mod=0;

Item* EventSystem::m_item=0;
bool EventSystem::m_item_in_game = false;

CharConv* EventSystem::m_charconv =0;

Event* EventSystem::m_event =0;

std::map<int, std::string> EventSystem::m_player_varupdates;

std::stack<std::string> EventSystem::m_gettext_domains;

void EventSystem::init()
{
	if (m_lua !=0)
	{
		cleanup();
	}

	m_lua = lua_open();

	luaL_openlibs(m_lua);
	luaopen_debug(m_lua);

	lua_register(m_lua, "getRegion", getRegion);
	lua_register(m_lua, "getObjectValue", getObjectValue);
	lua_register(m_lua, "get", getObjectValue);
	lua_register(m_lua, "setObjectValue", setObjectValue);
	lua_register(m_lua, "set", setObjectValue);
	lua_register(m_lua, "moveObject",moveObject);
	lua_register(m_lua, "objectIsInRegion", objectIsInRegion);
	lua_register(m_lua, "pointIsInArea", pointIsInArea);
	lua_register(m_lua, "unitIsInArea", unitIsInArea);
	lua_register(m_lua, "objectIsInArea", unitIsInArea);
	
	lua_register(m_lua, "setObjectNameRef",setObjectNameRef);
	lua_register(m_lua, "getObjectByNameRef",getObjectByNameRef);
	
	lua_register(m_lua, "createObject", createObject);
	lua_register(m_lua, "createScriptObject", createScriptObject);
	lua_register(m_lua, "deleteObject", deleteObject);
	lua_register(m_lua, "createObjectGroup", createObjectGroup);
	
	lua_register(m_lua, "dropItem", dropItem);
	lua_register(m_lua, "createItem", createItem);
	lua_register(m_lua, "createRandomItem", createRandomItem);
	lua_register(m_lua, "getItemValue", getItemValue);
	lua_register(m_lua, "setItemValue", setItemValue);
	lua_register(m_lua, "addItemMagicMods",addItemMagicMods);
	lua_register(m_lua, "searchPlayerItem", searchPlayerItem);
	lua_register(m_lua, "getPlayerItem", getPlayerItem);
	lua_register(m_lua, "removePlayerItem", removePlayerItem);
	lua_register(m_lua, "insertPlayerItem",insertPlayerItem );
	lua_register(m_lua, "getInventoryPosition",getInventoryPosition );
	lua_register(m_lua, "deleteItem", deleteItem);
	
	
	lua_register(m_lua, "getLocation", getLocation);
	lua_register(m_lua, "addLocation", addLocation);
	lua_register(m_lua, "setLocation", setLocation);
	lua_register(m_lua, "addArea", addArea);
	
	lua_register(m_lua, "startTimer",startTimer);
	lua_register(m_lua, "insertTrigger",insertTrigger);
	lua_register(m_lua, "addTriggerVariable", addTriggerVariable);
	
	lua_register(m_lua, "setDamageValue", setDamageValue);
	lua_register(m_lua, "getDamageValue", getDamageValue);
	lua_register(m_lua, "setBaseModValue", setBaseModValue);
	lua_register(m_lua, "getBaseModValue", getBaseModValue);
	lua_register(m_lua, "setDynModValue", setDynModValue);
	lua_register(m_lua, "getDynModValue", getDynModValue);
	
	lua_register(m_lua, "createProjectile", createProjectile);
	lua_register(m_lua, "createMonsterGroup", createMonsterGroup);
	lua_register(m_lua, "getObjectAt", getObjectAt);
	lua_register(m_lua, "getObjectsInArea", getObjectsInArea);
	lua_register(m_lua, "addUnitCommand", addUnitCommand);
	lua_register(m_lua, "clearUnitCommands",clearUnitCommands); 
	lua_register(m_lua, "setUnitAction", setUnitAction);
	lua_register(m_lua, "setObjectAnimation",setObjectAnimation);
	lua_register(m_lua, "setScriptObjectFlag",setScriptObjectFlag);
	lua_register(m_lua, "getScriptObjectFlag",getScriptObjectFlag);
	lua_register(m_lua, "setObjectFlag",setScriptObjectFlag);
	lua_register(m_lua, "getObjectFlag",getScriptObjectFlag);		
	lua_register(m_lua, "setCutsceneMode", setCutsceneMode);
	lua_register(m_lua, "addCameraPosition", addCameraPosition);
	lua_register(m_lua, "setLight", setLight);
	lua_register(m_lua, "createFloatingText", createFloatingText);
	
	lua_register(m_lua, "speak", speak);
	lua_register(m_lua, "unitSpeak", unitSpeak);
	lua_register(m_lua, "setSpeakerEmotion", setSpeakerEmotion);
	lua_register(m_lua, "setSpeakerPosition", setSpeakerPosition);
	lua_register(m_lua, "setSpeakerAnimation", setSpeakerAnimation);
	lua_register(m_lua, "addQuestion", addQuestion);
	lua_register(m_lua, "addAnswer", addAnswer);
	lua_register(m_lua, "changeTopic", changeTopic);
	lua_register(m_lua, "jumpTopic", jumpTopic);
	lua_register(m_lua, "createDialog", createDialogue);
	lua_register(m_lua, "createDialogue", createDialogue);
	lua_register(m_lua, "setTopicBase",setTopicBase );
	lua_register(m_lua, "addSpeaker", addSpeaker);
	lua_register(m_lua, "addSpeakerInDialogue", addSpeakerInDialog);
	lua_register(m_lua, "addSpeakerInDialog", addSpeakerInDialog);
	lua_register(m_lua, "removeSpeaker", removeSpeaker);
	lua_register(m_lua, "getSpeaker", getSpeaker);
	lua_register(m_lua, "setDialogueActive", setDialogueActive);
	lua_register(m_lua, "setCurrentDialogue", setCurrentDialogue);
	lua_register(m_lua, "executeInDialog",executeInDialog);
	lua_register(m_lua, "executeInDialogue",executeInDialog);
	
	lua_register(m_lua, "setRefName", setRefName);
	lua_register(m_lua, "getRolePlayers", getRolePlayers);
	lua_register(m_lua, "teleportPlayer", teleportPlayer);
	
	lua_register(m_lua, "createEvent", createEvent);
	lua_register(m_lua, "addCondition", addCondition);
	lua_register(m_lua, "addEffect", addEffect);
	lua_register(m_lua, "setCondition", addCondition);
	lua_register(m_lua, "setEffect", addEffect);
	lua_register(m_lua, "timedExecute",timedExecute);
	lua_register(m_lua, "createScriptObjectEvent", createScriptObjectEvent);
	
	lua_register(m_lua, "getRelation",getRelation);
	lua_register(m_lua, "setRelation",setRelation);
	
	lua_register(m_lua, "printQuestMessage",printMessage);
	lua_register(m_lua, "printMessage",printMessage);
	
	lua_register(m_lua, "addMusic",addMusic);
	lua_register(m_lua, "clearMusicList",clearMusicList);
	lua_register(m_lua, "playSound",playSound);
	
	lua_register(m_lua, "getOption", getOption);
	lua_register(m_lua, "setOption", setOption);
	lua_register(m_lua, "writeLog", writeLog);
	lua_register(m_lua, "clearOgreStatistics", clearOgreStatistics);
	lua_register(m_lua, "getOgreStatistics", getOgreStatistics);
	lua_register(m_lua, "reloadData", reloadData);
	
	lua_register(m_lua, "writeString", writeString);
	lua_register(m_lua, "writeNewline", writeNewline);
	lua_register(m_lua, "writeUpdateString", writeUpdateString);

	m_region =0;
	m_trigger =0;
	m_dialogue =0;
	
	pushGettextDomain("sumwars-events");

	doString("private_vars = {}; quests = {private_vars = private_vars} ; playervars = {}; scriptobjectvar = {}; math.randomseed( os.time() );");
}

void  EventSystem::cleanup()
{
	if (m_lua != 0)
	{
		lua_close(m_lua);
	}
	m_lua =0;
}


void EventSystem::doString(const char* instructions)
{
	//DEBUG("instr %s",instructions);
	int err  = luaL_dostring(m_lua, instructions);

	if (err!=0)
	{
		reportErrors(m_lua, err,instructions);
	}
}

void EventSystem::doFile(const char* file)
{
	int err  = luaL_dofile(m_lua, file);

	if (err!=0)
	{
		std::string expl = "error in file of ";
		expl += file;
		reportErrors(m_lua, err,expl.c_str());
	}
}

void EventSystem::clearCodeReference(int& coderef)
{
	if (coderef != LUA_NOREF)
	{
		luaL_unref(EventSystem::getLuaState(), LUA_REGISTRYINDEX, coderef);
		coderef=LUA_NOREF;
	}
}

bool EventSystem::executeCodeReference(int coderef)
{
	if (coderef ==LUA_NOREF)
		return true;
	/*
#ifdef DEBUG_DATABASE
	const char* instr = m_code_fragments[coderef].c_str();
	DEBUG("instr %s",instr);
#endif
	*/	
	
	lua_rawgeti(EventSystem::getLuaState(),LUA_REGISTRYINDEX , coderef);
	int err = lua_pcall(EventSystem::getLuaState(), 0, LUA_MULTRET, 0);
	if (err !=0)
	{	
		
#ifdef DEBUG_DATABASE
		const char* instr = m_code_fragments[coderef].c_str();
		std::cout << instr;
#else
		char* instr =0;
#endif
		EventSystem::reportErrors(EventSystem::getLuaState(), err,instr);
		return false;
	}
	return true;
}

int EventSystem::createCodeReference(const char* code)
{
	int err = luaL_loadstring(EventSystem::getLuaState(),code);
	int ref;
	if (err ==0)
	{
		ref = luaL_ref(EventSystem::getLuaState(),LUA_REGISTRYINDEX);
#ifdef DEBUG_DATABASE
		m_code_fragments[ref] = code;
#endif
		return ref;
	}
	else
	{
		EventSystem::reportErrors(EventSystem::getLuaState(), err,code);
		return LUA_NOREF;
	}
}


void EventSystem::reportErrors(lua_State *L, int status, const char* instr)
{
	if ( status!=0 )
	{
		ERRORMSG("lua error %s \n", lua_tostring(L, -1));
		lua_pop(L, 1); // remove error message
		if (instr != 0)
		{
			std::string cstr = instr;
			std::string line;
			size_t pos=0;
			size_t pos2 = cstr.find('\n',pos);
			int n=1;
			while (pos2 != std::string::npos)
			{
				line = cstr.substr(pos, pos2-pos);
				printf("%3i %s\n",n,line.c_str());
				
				pos = pos2+1;
				pos2 = cstr.find('\n',pos);
				n++;
			}
			
			line = cstr.substr(pos);
			printf("%3i %s\n",n,line.c_str());
		}
	}
}

std::string EventSystem::getReturnValue()
{
	if (lua_gettop(m_lua) >0)
	{
		const char* cret = lua_tostring(m_lua, -1);
		lua_pop(m_lua, 1);
		if (cret != 0)
		{
			return std::string(cret);	
		}
	}
	return "";
}

void EventSystem::setItem(Item* item)
{
	if (!m_item_in_game && m_item != 0)
		delete m_item;
	m_item = item;
}

bool EventSystem::executeEvent(Event* event)
{
	// Lua Code zum Bedingung pruefen ausfuehren
	DEBUGX("execute Event %p",event)
	bool ret = event->checkCondition();
	if (ret ==false)
	{
		return false;
	}
	
	event->doEffect();

	return true;
}

int EventSystem::timedExecute(lua_State *L)
{
	// einzigartiger Name wird durch den counter sichergestellt
	static int counter=1;
	
	int argc = lua_gettop(L);
	if (argc>=2 && lua_isstring(L,1) && lua_isnumber(L,2))
	{
		std::string code = lua_tostring(L,1);
		float time  = lua_tonumber(L, 2);
		
		Region* reg = m_region;
		if (argc>=3 && lua_isstring(L,3))
		{
			reg = World::getWorld()->getRegion(lua_tostring(L,3));
		}
		if (reg ==0)
		{
			return 0;
			DEBUG("region for timedExecute does not exist");
		}
		
		m_event = new Event();
		m_event->setOnce(true);
		m_event->setEffect(code.c_str());
		m_event->setGettextDomain(m_gettext_domains.top());
		std::stringstream stream;
		stream << counter << "_event";
		
		reg ->addEvent(stream.str(),m_event);
		Trigger* trigger = new Trigger(stream.str());
		m_region->insertTimedTrigger(trigger,time);
		
		counter++;
	}
	else
	{
		ERRORMSG("Syntax: timedExecute( string luacode, float time, [string regionname])");
	}
	return 0;
}

Vector EventSystem::getVector(lua_State *L, int index)
{
	if (lua_istable(L,index))
	{
		float x,y;

		int idx = index;
		if (index <0)
			idx --;
		
		lua_pushinteger(L, 1);
		lua_gettable(L, idx);
		x = lua_tonumber(L, -1);
		lua_pop(L, 1);

		lua_pushinteger(L, 2);
		lua_gettable(L, idx);
		y = lua_tonumber(L, -1);
		lua_pop(L, 1);

		return Vector(x,y);
	}
	else if (lua_isstring(L,index))
	{
		LocationName loc = lua_tostring(L, index);
		return m_region->getLocation(loc);
	}
	return Vector(0,0);
}

Shape EventSystem::getArea(lua_State *L, int index)
{
	Shape s;
	s.m_center = Vector(0,0);
	if (lua_isstring(L,index))
	{
		std::string name = lua_tostring(L, index);
		return m_region->getArea(name);
	}
	else if (lua_istable(L,index))
	{
		int idx = index;
		if (index <0)
			idx --;
		
		lua_pushinteger(L, 1);
		lua_gettable(L, idx);
		std::string type = lua_tostring(L, -1);
		lua_pop(L, 1);
		
		if (type =="circle" || type =="rect")
		{
			lua_pushinteger(L, 2);
			lua_gettable(L, idx);
			s.m_center = getVector(L,-1);
			lua_pop(L, 1);
			
			if (type =="circle")
			{
				s.m_type = Shape::CIRCLE;
				lua_pushinteger(L, 3);
				lua_gettable(L, idx);
				s.m_radius = lua_tonumber(L, -1);
				lua_pop(L, 1);
			}
			else
			{
				s.m_type = Shape::RECT;
				lua_pushinteger(L, 3);
				lua_gettable(L, idx);
				s.m_extent = getVector(L,-1);
				lua_pop(L, 1);
			}
		}
		else
		{
			ERRORMSG("getArea: ungueltige Typ fuer Area (muss circle oder rect sein)");
		}
	}
	else
	{
		ERRORMSG("getArea: ungueltige Eingabe fuer Area");
	}
	
	
	return s;
}


void EventSystem::pushVector(lua_State *L, Vector v)
{
	lua_newtable(L);
	lua_pushinteger(L, 1);
	lua_pushnumber(L, v.m_x);
	lua_settable(L, -3);

	lua_pushinteger(L, 2);
	lua_pushnumber(L,v.m_y);
	lua_settable(L, -3);
}

void EventSystem::getTranslatableString(lua_State *L, TranslatableString& t, int index)
{
	t.setTextDomain(m_gettext_domains.top());
	
	if (lua_isstring(L,index))
	{
		std::string text = lua_tostring(L, index);
		t = text;
	}
	else if (lua_istable(L,index))
	{
		// Layout of the table:
		// 0-> text
		// 1-> substring1
		// 2-> substring2...
		int idx = index;
		if (index <0)
			idx --;
		
		lua_pushinteger(L, 0);
		lua_gettable(L, idx);
		std::string text = lua_tostring(L, -1);
		lua_pop(L, 1);
		
		int id = 1;
		std::vector<std::string> substrings;
		while (id <=9)
		{
			lua_pushinteger(L, id);
			lua_gettable(L, idx);
			if (!lua_isstring(L,-1))
				break;
			std::string subtext = lua_tostring(L, -1);
			lua_pop(L, 1);
			
			substrings.push_back(subtext);
			id ++;
		}
		
		t.setText(text,substrings);
	}
	else
	{
		ERRORMSG("ungueltige Eingabe fuer TranslatableString");
	}
}

int EventSystem::getRegion(lua_State *L)
{
	if (m_region != 0)
	{
		lua_pushstring(L,m_region->getName().c_str());
		return 1;
	}
	return 0;
}


int EventSystem::getObjectValue(lua_State *L)
{

	int ret =0;
	int argc = lua_gettop(L);
	if (argc>=2 && lua_isnumber(L,1) && lua_isstring(L,2))
	{
		int id = lua_tointeger(L, 1);
		std::string valname = lua_tostring(L, 2);

		WorldObject* wo =0;
		if (m_region !=0)
		{
			wo = m_region->getObject(id);
		}

		if (wo ==0)
		{
			wo = World::getWorld()->getPlayer(id);
		}
		
		if (wo !=0)
		{
			ret = wo->getValue(valname);
		}
		else
		{
			ERRORMSG("getObjectValue: Object doesnt exist %i",id);
		}
	}
	else
	{
		ERRORMSG("Syntax: getObjectValue( int id, string valname)");
	}

	return ret;
}

int EventSystem::setObjectValue(lua_State *L)
{

	int ret =0;
	int argc = lua_gettop(L);
	if (argc>=2 && lua_isnumber(L,1) && lua_isstring(L,2))
	{
		int id = lua_tointeger(L, 1);
		std::string valname = lua_tostring(L, 2);

		WorldObject* wo =0;
		if (m_region !=0)
		{
			wo = m_region->getObject(id);
		}
		
		
		if (wo ==0)
		{
			wo = World::getWorld()->getPlayer(id);
		}

		if (wo !=0)
		{
			ret = wo->setValue(valname);
		}
		else
		{
			ERRORMSG("setObjectValue: Object doesnt exist");
		}
	}
	else
	{
		ERRORMSG("Syntax: setObjectValue( int id, string valname, value)");
	}

	return 0;
}

int EventSystem::moveObject(lua_State *L)
{
	int argc = lua_gettop(L);
	if (argc>=2 && lua_isnumber(L,1) && (lua_isstring(L,2) || lua_istable(L,2)))
	{
		int id = (int) lua_tonumber(L,1);
		Vector pos = getVector(L,2);
		
		if (m_region ==0)
			return 0;
		
		WorldObject* wo = m_region->getObject(id);
		if (wo ==0)
			return 0;
		
		bool check = true;
		if (argc>=3 && lua_isboolean(L,3))
		{
			check = lua_toboolean(L,3);
		}
		
		if (check)
		{
			wo->getRegion()->getFreePlace (wo->getShape(), wo->getLayer(), pos,wo);
		}
		wo->moveTo(pos);
	}
	else
	{
		ERRORMSG("Syntax: moveObject(int objectid, Vector pos [,bool collision_check])");
	}
	return 0;
}

int EventSystem::objectIsInRegion(lua_State *L)
{
	int argc = lua_gettop(L);
	if (argc>=1 && lua_isnumber(L,1))
	{
		int id = lua_tointeger(L, 1);
		
		WorldObject* wo =0;
		if (m_region !=0)
		{
			wo = m_region->getObject(id);
			lua_pushboolean(EventSystem::getLuaState() , (wo!=0) );
			return 1;
		}
	}
	else
	{
		ERRORMSG("Syntax: objectIsInRegion( int id)");
	}
	return 0;
}


int EventSystem::getDamageValue(lua_State *L)
{
	int ret =0;
	int argc = lua_gettop(L);
	if (argc>=1 && lua_isstring(L,1))
	{
		std::string valname = lua_tostring(L, 1);

		if (m_damage != 0)
		{
			ret = m_damage->getValue(valname);
		}
	}
	else
	{
		ERRORMSG("Syntax: getDamageValue(string valname)");
	}

	return ret;
}

int EventSystem::setDamageValue(lua_State *L)
{
	int argc = lua_gettop(L);
	if (argc>=2 && lua_isstring(L,1))
	{
		std::string valname = lua_tostring(L, 1);

		if (m_damage != 0)
		{
			m_damage->setValue(valname);
		}
	}
	else
	{
		ERRORMSG("Syntax: setDamageValue(string valname, value)");
	}

	return 0;
}

int EventSystem::getBaseModValue(lua_State *L)
{
	int ret =0;
	int argc = lua_gettop(L);
	if (argc>=1 && lua_isstring(L,1))
	{
		std::string valname = lua_tostring(L, 1);

		if (m_base_mod != 0)
		{
			ret = m_base_mod->getValue(valname);
		}
	}
	else
	{
		ERRORMSG("Syntax: getBaseModValue(string valname)");
	}

	return ret;
}

int EventSystem::setBaseModValue(lua_State *L)
{
	int argc = lua_gettop(L);
	if (argc>=2 && lua_isstring(L,1))
	{
		std::string valname = lua_tostring(L, 1);

		if (m_base_mod != 0)
		{
			m_base_mod->setValue(valname);
		}
	}
	else
	{
		ERRORMSG("Syntax: setBaseModValue(string valname, value)");
	}

	return 0;
}

int EventSystem::getDynModValue(lua_State *L)
{
	int ret =0;
	int argc = lua_gettop(L);
	if (argc>=1 && lua_isstring(L,1))
	{
		std::string valname = lua_tostring(L, 1);

		if (m_dyn_mod != 0)
		{
			ret = m_dyn_mod->getValue(valname);
		}
	}
	else
	{
		ERRORMSG("Syntax: getDynModValue(string valname)");
	}

	return ret;
}

int EventSystem::setDynModValue(lua_State *L)
{
	int argc = lua_gettop(L);
	if (argc>=2 && lua_isstring(L,1))
	{
		std::string valname = lua_tostring(L, 1);

		if (m_dyn_mod != 0)
		{
			m_dyn_mod->setValue(valname);
		}
	}
	else
	{
		ERRORMSG("Syntax: setDynModValue(string valname, value)");
	}

	return 0;
}

int EventSystem::createProjectile(lua_State *L)
{
	int argc = lua_gettop(L);
	if (argc >= 2 && lua_isstring(L,1)  && (lua_istable(L,2) || lua_isstring(L,2)))
	{
		if (m_region !=0)
		{
			std::string tname = lua_tostring(L, 1);
			Vector pos = getVector(L,2);

			float speed = 0.0;
			
			// Schaden
			// Projektil erzeugen
			Projectile* pr = ObjectFactory::createProjectile(tname);
			if (pr ==0)
			{
				ERRORMSG("could not create projectile with type %s",tname.c_str());
				return 0;
			}
			
			Damage* tmp = new Damage;
			pr->setDamage(tmp);
			delete tmp;
			
			m_damage = pr->getDamage();

			// Richtung, Geschwindigkeit ermitteln
			if (argc>=3 && (lua_istable(L,3) || lua_isstring(L,3)))
			{
				speed = 10000.0;
				Vector goal = getVector(L,3);
				Vector dir = goal - pos;
				dir.normalize();

				if (argc>=4  && lua_isnumber(L,4))
				{
					speed = lua_tonumber(L, 4);
				}

				// Geschwindigkeit wird in m/ms gemessen
				pr->setSpeed(dir *speed/1000000);

				if (argc>=5 && lua_isnumber(L,5))
				{
					float dist = lua_tonumber(L, 5);
					pos += dir*dist;
				}
			}

			m_region->insertProjectile(pr,pos);
		}
	}
	else
	{
		ERRORMSG("Syntax: createProjectile( string type, {startx, starty}, [{goalx, goaly}] , [speed], [dist])");
	}
	return 0;
}

int EventSystem::unitIsInArea(lua_State *L)
{
	bool ret =false;
	int argc = lua_gettop(L);
	if (argc>=2 && lua_isnumber(L,1) && lua_isstring(L,2) )
	{
		int id = lua_tointeger(L, 1);


		if (m_region !=0)
		{
			WorldObject* wo = m_region->getObject(id);
			if (wo !=0)
			{
				ret = getArea(L,2).intersects(*(wo->getShape()));
			}
			else
			{
				ERRORMSG("unitIsInArea: Unit does not exist");
			}
		}

	}
	else
	{
		ERRORMSG("Syntax: unitIsInArea( int unitid, string areaname)");
	}

	lua_pushboolean(EventSystem::getLuaState() , ret);
	return 1;
}

int EventSystem::pointIsInArea(lua_State *L)
{
	bool ret =false;
	int argc = lua_gettop(L);
	if (argc>=2 && (lua_istable(L,1) || lua_isstring(L,1)) && (lua_istable(L,2) || lua_isstring(L,2)))
	{
		Vector c = getVector(L,1);

		Shape s;
		s.m_center = c;
		s.m_type = Shape::CIRCLE;
		s.m_radius=0;

		if (m_region !=0)
		{
			ret = getArea(L,2).intersects(s);
		}

	}
	else
	{
		ERRORMSG("Syntax: pointIsInArea( {float x, float y}, string areaname)");
	}

	lua_pushboolean(EventSystem::getLuaState() , ret);
	return 1;
}

int EventSystem::setObjectNameRef(lua_State *L)
{
	int argc = lua_gettop(L);
	if (argc>=2 && lua_isnumber(L,1)  && lua_isstring(L,2))
	{
		int id = int(lua_tonumber(L,1));
		std::string name = lua_tostring(L,2);
		
		
		if (m_region !=0)
		{
			m_region->setNamedId(name,id);
		}
	}
	else
	{
		ERRORMSG("Syntax: setObjectNameRef(int id, string name)");
	}
	return 0;
}

int EventSystem::getObjectByNameRef(lua_State *L)
{
	int argc = lua_gettop(L);
	if (argc>=1 &&  lua_isstring(L,1))
	{
		std::string name = lua_tostring(L,1);
		if (m_region !=0)
		{
			int id = m_region->getIdByName(name);
			lua_pushnumber(L, id);
			return 1;
		}
	}
	else
	{
		ERRORMSG("Syntax: getObjectByNameRef(string name)");
	}
	return 0;
}

int EventSystem::createObject(lua_State *L)
{
	int ret =0;
	int argc = lua_gettop(L);
	if (argc>=2 && (lua_istable(L,2) || lua_isstring(L,2)) && lua_isstring(L,1))
	{


		WorldObject::Subtype subtype = lua_tostring(L, 1);
		Vector pos = getVector(L,2);

		if (m_region!=0)
		{
			float angle =0,height=0;
			if (argc>=3 && lua_isnumber(L,3))
			{
				angle = lua_tonumber(L, 3);
				angle *= 3.14159 / 180;
			}
			if (argc>=4 && lua_isnumber(L,4))
			{
				height = lua_tonumber(L, 4);
			}

			ret = m_region->createObject(subtype, pos,angle, height,WorldObject::STATE_AUTO);
		}

	}
	else
	{
		ERRORMSG("Syntax: createObject( string subtype, {float x, float y}, [angle],[height])");
	}

	lua_pushinteger(EventSystem::getLuaState() , ret);
	return 1;
}

int EventSystem::createObjectGroup(lua_State *L)
{
	int argc = lua_gettop(L);
	if (argc>=2 && (lua_istable(L,2) || lua_isstring(L,2)) && lua_isstring(L,1))
	{


		ObjectGroupTemplateName type = lua_tostring(L, 1);
		Vector pos = getVector(L,2);

		if (m_region!=0)
		{
			float angle =0;
			if (argc>=3 && lua_isnumber(L,3))
			{
				angle = lua_tonumber(L, 3);
				angle *= 3.14159 / 180;
			}
			m_region->createObjectGroup(type, pos,angle,"",WorldObject::STATE_AUTO);
		}

	}
	else
	{
		ERRORMSG("Syntax: createObjectGroup( string type, {float x, float y}, [float angle])");
	}

	return 0;
}

int  EventSystem::createScriptObject(lua_State *L)
{
	int ret =0;
	int argc = lua_gettop(L);
	if (argc>=4)
	{
		WorldObject::Subtype subtype = lua_tostring(L, 1);
		ScriptObject* wo = new ScriptObject(0);
		
		std::string render_info = lua_tostring(L, 2);
		wo->setRenderInfo(render_info);
		
		Shape* ps = wo->getShape();
		*ps = getArea(L,3);
		
		short layer = WorldObject::LAYER_NOCOLLISION;
		if (argc>=4 && lua_isstring(L,4))
		{
			std::string lstr = lua_tostring(L,4);
			if (lstr == "base")
			{
				layer = WorldObject::LAYER_BASE;
			}
			else  if (lstr == "air")
			{
				layer = WorldObject::LAYER_AIR;
			}
			else  if (lstr == "dead")
			{
				layer = WorldObject::LAYER_DEAD;
			}
			else  if (lstr == "normal")
			{
				layer = WorldObject::LAYER_BASE | WorldObject::LAYER_AIR;
			}
		}
		
		wo->setLayer(layer);
		wo->setSubtype(subtype);
		m_region->insertObject(wo, wo->getPosition(),0);
		ret = wo->getId();
		
	}
	else
	{
		ERRORMSG("Syntax: createScriptObject( string subtype, string renderinfo, {'circle' | 'rect',{float posx, float posx}, {float extentx, float extentx} | radius}, [layer]");
	}
	lua_pushinteger(EventSystem::getLuaState() , ret);
	return 1;
	
}

int EventSystem::createMonsterGroup(lua_State *L)
{
	int argc = lua_gettop(L);
	if (argc>=2 && (lua_istable(L,2) || lua_isstring(L,2)) && lua_isstring(L,1))
	{


		MonsterGroupName group= lua_tostring(L, 1);
		Vector pos = getVector(L,2);

		float radius = 3;
		if (argc>=3 && lua_isstring(L,3))
		{
			radius = lua_tonumber(L,3);
		}
		
		if (m_region!=0)
		{
			std::list<int> monsters;
			m_region->createMonsterGroup(group, pos,radius,&monsters);
			
			// lua Tabelle mit den Ids ausgeben
			lua_newtable(L);
			
			int i=1;
			std::list<int>::iterator it;
			for (it = monsters.begin(); it != monsters.end(); ++it)
			{
				lua_pushinteger(L, i);
				lua_pushnumber(L, *it);
				lua_settable(L, -3);
				
				i++;
			}
			return 1;
		}
		
	}
	else
	{
		ERRORMSG("Syntax: createMonsterGroup( string monstergroup, {float x, float y}[, float radius])");
	}
	return 0;
}

int EventSystem::deleteObject(lua_State *L)
{
	int argc = lua_gettop(L);
	if (argc>=1 && lua_isnumber(L,1))
	{
		int id = lua_tointeger(L, 1);
		if (m_region !=0)
		{
			WorldObject* wo = m_region->getObject(id);
			if (wo !=0)
			{
				if (wo->getType() == "PLAYER")
				{
					ERRORMSG("deleteObject must not be called on a player");
					return 0;
				}
				
				bool success = m_region->deleteObject(wo);
				if (success)
				{
					delete wo;
				}
			}
			
		}
	}
	else
	{
		ERRORMSG("Syntax: deleteObject( int objectID)");
	}
	return 0;
}

int EventSystem::addUnitCommand(lua_State *L)
{
	int argc = lua_gettop(L);
	if (argc>=2 && lua_isnumber(L,1) && lua_isstring(L,2) )
	{
		if (m_region ==0)
			return 0;

		int id = int (lua_tonumber(L,1));
		std::string actstr = lua_tostring(L,2);

		WorldObject* wo = m_region->getObject(id);
		if (wo !=0)
		{
			if (wo->isCreature())
			{
				Creature* cr = static_cast<Creature*>(wo);

				Action::ActionType act = Action::getActionType(actstr);
				if (act != "noaction")
				{
					Command com;
					com.m_type = act;
					com.m_range = cr->getBaseAttrMod()->m_attack_range;

					if (act == "use")
						com.m_range = 1.0;
					
					if (act == "walk")
						com.m_range =cr->getShape()->m_radius;
					
					if (argc >=3 && lua_isnumber(L,3))
					{
						com.m_goal_object_id = lua_tointeger(L,3);
						com.m_goal = cr->getPosition();
					}
					else if (argc >=3 && (lua_istable(L,3) || lua_isstring(L,3)))
					{
						com.m_goal = getVector(L,3);
						com.m_goal_object_id =0;
					}
					
					if (argc>=4 && lua_isnumber(L,4))
					{
						float range = lua_tonumber(L,4);
						if (range > 0)
							com.m_range = range;
					}
					
					if (argc>=5 && lua_isstring(L,5))
					{
						std::string flags = lua_tostring(L,5);
						char flg =0;
						
						if (flags.find("repeat") != std::string::npos)
						{
							flg |= Command::REPEAT;
							
						}
						if (flags.find("secondary") != std::string::npos)
						{
							flg |= Command::SECONDARY;
						}
						com.m_flags = flg;
					}
					
					float time = 50000;
					if (argc>=6 && lua_isnumber(L,6))
						time = lua_tonumber(L,6);
					cr->insertScriptCommand(com,time);
				}
			}
		}
	}
	else
	{
		ERRORMSG("Syntax: :addUnitCommand(int unitid, string command,  [goal_unitid | {goal_x,goal_y}], [range], [flags], [float time])");
	}
	return 0;

}

int EventSystem::clearUnitCommands(lua_State *L)
{
	int argc = lua_gettop(L);
	if (argc>=1 && lua_isnumber(L,1))
	{
		if (m_region ==0)
			return 0;
		
		int id = int (lua_tonumber(L,1));
		Creature* cr = dynamic_cast<Creature*>(m_region->getObject(id));
		if (cr != 0)
		{
			cr->clearScriptCommands();
		}
	}
	return 0;
}

int  EventSystem::setUnitAction(lua_State *L)
{
	int argc = lua_gettop(L);
	if (argc>=2 && lua_isnumber(L,1) && lua_isstring(L,2) )
	{
		if (m_region ==0)
			return 0;

		int id = int (lua_tonumber(L,1));
		std::string actstr = lua_tostring(L,2);

		WorldObject* wo = m_region->getObject(id);
		if (wo !=0)
		{
			if (wo->isCreature())
			{
				Creature* cr = static_cast<Creature*>(wo);

				Action::ActionType act = Action::getActionType(actstr);
				if (act != "noaction")
				{
					Action action;
					action.m_action_equip = Action::NO_WEAPON;
					action.m_type = act;
					
					if (argc >=3 && lua_isnumber(L,3))
					{
						action.m_goal_object_id = lua_tointeger(L,3);
						action.m_goal = cr->getPosition();
					}
					else if (argc >=3 && (lua_istable(L,3) || lua_isstring(L,3)))
					{
						action.m_goal = getVector(L,3);
						action.m_goal_object_id =0;
					}
					else
					{
						action.m_goal_object_id =0;
						action.m_goal = cr->getPosition();
					}
					
					
					action.m_elapsed_time=0;
					action.m_time = 1000;
					if (argc>=4 && lua_isnumber(L,4))
						action.m_time = lua_tonumber(L,4);
					
					cr->setAction(action);
				}
			}
		}
	}
	else
	{
		ERRORMSG("Syntax: :setUnitAction(int unitid, string command,  [goal_unitid | {goal_x,goal_y}], [float time])");
	}
	return 0;

}

int EventSystem::setObjectAnimation(lua_State *L)
{
	int argc = lua_gettop(L);
	if (argc>=3 && lua_isnumber(L,1) && lua_isstring(L,2) && lua_isnumber(L,3))
	{
		if (m_region ==0)
			return 0;

		int id = int (lua_tonumber(L,1));
		std::string actstr = lua_tostring(L,2);

		
		WorldObject* wo =  m_region->getObject(id);
		if (wo !=0)
		{
			
			float time = lua_tonumber(L,3);
			bool repeat = false;
			if (argc>=4 && lua_isboolean(L,4))
			{
				repeat =lua_toboolean(L,4);
			}
			else if (argc>=4 && lua_isstring(L,4))
			{
				std::string repstr = lua_tostring(L,4);
				repeat = (repstr == "true");
			}
			wo->setAnimation(actstr,time,repeat);
			DEBUGX("action %s perc %f",wo->getActionString().c_str(),wo->getActionPercent());
			
		}
	}
	else
	{
		ERRORMSG("Syntax: setObjectAnimation(int objid, string action, float time, [bool repeat])");
	}
	return 0;
}

int EventSystem::setScriptObjectFlag(lua_State *L)
{
	int argc = lua_gettop(L);
	if (argc>=2 && lua_isnumber(L,1) && lua_isstring(L,2))
	{
		if (m_region ==0)
			return 0;

		int id = int (lua_tonumber(L,1));
		std::string flag = lua_tostring(L,2);

		
		WorldObject* wo =  m_region->getObject(id);
		if (wo !=0)
		{
			bool set = true;
			if (argc>=3 && lua_isboolean(L,3))
			{
				set = lua_toboolean(L,3);
			}
			wo->setFlag(flag,set);
		}
	}
	else
	{
		ERRORMSG("Syntax: setScriptObjectFlag(int objid, string flagname [,bool set])");
	}
	return 0;	
}

int EventSystem::getScriptObjectFlag(lua_State *L)
{
	int argc = lua_gettop(L);
	if (argc>=2 && lua_isnumber(L,1) && lua_isstring(L,2))
	{
		if (m_region ==0)
			return 0;

		int id = int (lua_tonumber(L,1));
		std::string flag = lua_tostring(L,2);

		
		WorldObject* wo = m_region->getObject(id);
		if (wo !=0)
		{
			std::set<std::string> flags;
			wo->getFlags(flags);
			bool set = (flags.count(flag) > 0);
			lua_pushboolean(L,set);
			return 1;
		}
	}
	else
	{
		ERRORMSG("Syntax: setScriptObjectFlag(int objid, string flagname [,bool set])");
	}
	return 0;	
}

int EventSystem::createScriptObjectEvent(lua_State *L)
{
	int argc = lua_gettop(L);
	if (argc>=2 && lua_isnumber(L,1) && lua_isstring(L,2))
	{
		int id = int (lua_tonumber(L,1));
		std::string trigger = lua_tostring(L,2);
		bool once = false;
		if (argc>=2 && lua_isboolean(L,3))
		{
			once = lua_toboolean(L,3);
		}
	
		ScriptObject* wo = dynamic_cast<ScriptObject*> (m_region->getObject(id));
		if (wo !=0)
		{
			m_event = new Event();
			m_event->setOnce(once);
			wo->addEvent(trigger,m_event);
		}
	}
	else
	{
		ERRORMSG("Syntax: createScriptObjectEvent(objectid id, string triggername [,bool once])");
	}
	return 0;
	
}


int EventSystem::getObjectAt(lua_State *L)
{
	int argc = lua_gettop(L);
	if (argc>=1 && (lua_istable(L,1) || lua_isstring(L,1)))
	{
		Vector pos = getVector(L,1);
		if (m_region !=0)
		{
			WorldObject* wo = m_region->getObjectAt(pos);
			DEBUG("pos %f %f wo %p",pos.m_x, pos.m_y,wo);
			if (wo !=0)
			{
				lua_pushnumber(L,wo->getId());
				return 1;
			}
		}
	}
	else
	{
		ERRORMSG("Syntax: getObjectAt({float x, float y})");
	}

	return 0;
}

int EventSystem::getObjectsInArea(lua_State *L)
{
	int argc = lua_gettop(L);
	lua_newtable(L);
	if (argc>=1 && (lua_isstring(L,1) || lua_istable(L,1)))
	{
		if (m_region !=0)
		{
			WorldObjectList obj;
			WorldObjectList::iterator it;
			Shape s = getArea(L,1);

			short layer = WorldObject::LAYER_BASE | WorldObject::LAYER_AIR;
			if (argc>=3 && lua_isstring(L,3))
			{
				std::string lstr = lua_tostring(L,3);
				if (lstr == "base")
				{
					layer = WorldObject::LAYER_BASE;
				}
				else  if (lstr == "air")
				{
					layer = WorldObject::LAYER_AIR;
				}
				else  if (lstr == "dead")
				{
					layer = WorldObject::LAYER_DEAD;
				}
				else  if (lstr == "normal")
				{
					layer = WorldObject::LAYER_BASE | WorldObject::LAYER_AIR;
				}
			}

			short group = WorldObject::GROUP_ALL;
			if (argc>=2 && lua_isstring(L,2))
			{

				std::string gstr = lua_tostring(L,2);
				if (gstr == "unit" || gstr == "creature")
				{
					group = WorldObject::CREATURE;
				}
				else if (gstr == "player")
				{
					group = WorldObject::PLAYER;
				}
				else if (gstr == "monster")
				{
					group = WorldObject::CREATURE_ONLY;
				}
				else if (gstr == "fixed")
				{
					group = WorldObject::FIXED;
				}
			}
			m_region->getObjectsInShape(&s,&obj,layer,group);

			int cnt =1;
			for (it = obj.begin(); it!= obj.end(); ++it)
			{
				lua_pushnumber(L,(*it)->getId());
				lua_rawseti (L, -2, cnt);
				cnt++;
			}
		}
	}
	else
	{
		ERRORMSG("Syntax: getObjectsInArea(string areaname, string objectgroup, string layer)")
	}


	return 1;
}

int EventSystem::dropItem(lua_State *L)
{
	int argc = lua_gettop(L);
	if (argc>=1 && (lua_istable(L,1) || lua_isstring(L,1)))
	{
		Vector pos = getVector(L,1);

		if (m_region!=0 && m_item != 0 && m_item_in_game == false)
		{
			m_region->dropItem(m_item,pos);
			m_item_in_game = true;
		}
	}
	else
	{
		ERRORMSG("Syntax: :dropItem({float x,float y}");
	}
	return 0;
}


int EventSystem::createItem(lua_State *L)
{
	int argc = lua_gettop(L);
	if (argc>=1  && lua_isstring(L,1))
	{
		Item::Subtype subtype = lua_tostring(L, 1);
	

		float magic_power=0;
		if (argc>=2 && lua_isnumber(L,2))
		{
			magic_power= lua_tonumber(L, 2);
		}
		Item::Rarity rarity = Item::NORMAL;
		if (argc>=3 && lua_isnumber(L,3))
		{
			std::string rar = lua_tostring(L, 3);
			if (rar == "magical")
				rarity = Item::MAGICAL;
			if (rar == "rare")
				rarity = Item::RARE;
			if (rar == "unique")
				rarity = Item::UNIQUE;
		}

		Item* item = ItemFactory::createItem(ItemFactory::getBaseType(subtype),subtype,0,magic_power,rarity);
		setItem(item);
		m_item_in_game = false;
	}
	else
	{
		ERRORMSG("Syntax: createItem(string itemtype, [magic_power],[rarity]");
	}
	return 0;
	
}

int EventSystem::createRandomItem(lua_State *L)
{
	int argc = lua_gettop(L);
	if (argc >=3 && lua_istable(L,1) && lua_istable(L,2) && lua_istable(L,3))
	{
		DropSlot ds;
		ds.m_size_probability[4] = 1;
		for (int i=1; i<=4; i++)
		{
			lua_pushinteger(L, i);
			lua_gettable(L, 1);
			ds.m_size_probability[i-1] = lua_tonumber(L, -1);
			ds.m_size_probability[4] -= ds.m_size_probability[i-1];
			lua_pop(L, 1);
		}
		
		// Vektor zum Tabelle laden missbrauchen
		Vector vec;
		vec = getVector(L,2);
		ds. m_min_level = (int) vec.m_x;
		ds. m_max_level = (int) vec.m_y;
		
		vec = getVector(L,3);
		ds.m_min_gold = (int) vec.m_x;
		ds.m_max_gold = (int) vec.m_y;
		
		if (argc>=4 && lua_isnumber(L,4))
		{
			ds.m_magic_probability = lua_tonumber(L,4);
		}
		
		if (argc>=5 && lua_isnumber(L,5))
		{
			ds.m_magic_power = lua_tonumber(L,5);
		}
		
		int nrplayers = World::getWorld()->getPlayers()->size();
		float faktor = 1+0.4*(nrplayers-1);
		
		Item* item = ItemFactory::createItem(ds,faktor);
		setItem(item);
		m_item_in_game = false;

	}
	else
	{
		ERRORMSG("Syntax: createRandomItem({prob_big, prob_medium, pro_small, prob_gold},{min_level, max_level},{min_gold, max_gold}, magic_prob, magic_power)");
	}
	return 0;
}

int EventSystem::getItemValue(lua_State *L)
{
	int argc = lua_gettop(L);
	if (argc>=1  && lua_isstring(L,1))
	{
		std::string valname = lua_tostring(L, 1);
		if (m_item != 0)
		{
			return m_item->getValue(valname);
		}
		else if (valname == "type")
		{
			lua_pushstring(L,"noitem");
			return 1;
		}
		return 0;
	}
	else
	{
		ERRORMSG("Syntax: getItemValue(string valname)");
	}
	return 0;
}

int EventSystem::setItemValue(lua_State *L)
{
	int argc = lua_gettop(L);
	if (argc>=1  && lua_isstring(L,1))
	{
		std::string valname = lua_tostring(L, 1);
		if (m_item != 0)
		{
			m_item->setValue(valname);
		}
		return 0;
	}
	else
	{
		ERRORMSG("Syntax: setItemValue(string valname, value)");
	}
	return 0;
}

int EventSystem::addItemMagicMods(lua_State *L)
{
	int argc = lua_gettop(L);
	if (argc>=2  && lua_isstring(L,1) && lua_isnumber(L,2))
	{
		// map from strings to magic mod enum
		std::map<std::string, int> magic_mods;
		magic_mods["max_health"] = ItemFactory::HEALTH_MOD;
		magic_mods["armor"] = ItemFactory::ARMOR_MOD;
		magic_mods["block"] = ItemFactory::BLOCK_MOD;
		magic_mods["strength"] = ItemFactory::STRENGTH_MOD;
		magic_mods["dexterity"] = ItemFactory::DEXTERITY_MOD;
		magic_mods["willpower"] = ItemFactory::WILLPOWER_MOD;
		magic_mods["magic_power"] = ItemFactory::MAGIC_POWER_MOD;
		magic_mods["resist_phys"] = ItemFactory::RESIST_PHYS_MOD;
		magic_mods["resist_fire"] = ItemFactory::RESIST_FIRE_MOD;
		magic_mods["resist_ice"] = ItemFactory::RESIST_ICE_MOD;
		magic_mods["resist_air"] = ItemFactory::RESIST_AIR_MOD;
		magic_mods["resist_all"] = ItemFactory::RESIST_ALL_MOD;
		magic_mods["phys_dmg"] = ItemFactory::DAMAGE_PHYS_MOD;
		magic_mods["fire_dmg"] = ItemFactory::DAMAGE_FIRE_MOD;
		magic_mods["ice_dmg"] = ItemFactory::DAMAGE_ICE_MOD;
		magic_mods["air_dmg"] = ItemFactory::DAMAGE_AIR_MOD;
		magic_mods["phys_mult"] = ItemFactory::DAMAGE_MULT_PHYS_MOD;
		magic_mods["fire_mult"] = ItemFactory::DAMAGE_MULT_FIRE_MOD;
		magic_mods["ice_mult"] = ItemFactory::DAMAGE_MULT_ICE_MOD;
		magic_mods["air_mult"] = ItemFactory::DAMAGE_MULT_AIR_MOD;
		magic_mods["attack_speed"] = ItemFactory::ATTACK_SPEED_MOD;
		magic_mods["attack"] = ItemFactory::ATTACK_MOD;
		magic_mods["power"] = ItemFactory::POWER_MOD;
		
		std::string modtype = lua_tostring(L, 1);
		float min_enchant = lua_tonumber(L,2);
		float max_enchant = min_enchant;
		if (argc >= 3)
		{
			max_enchant = lua_tonumber(L,3);
		}
		
		if (magic_mods.count(modtype) == 0)
		{
			ERRORMSG("magic modifier type %s unknown",modtype.c_str());
			return 0;
		}
		
		if (m_item != 0)
		{
			// create probability array
			// all probabilities set to 0, except the selected one, that is 1
			float modprobs[NUM_MAGIC_MODS];
			for (int i=0; i<NUM_MAGIC_MODS; i++)
			{
				modprobs[i] = 0;
			}
			
			int selectedmod = magic_mods[modtype];
			modprobs[selectedmod] = 1;
			
			// Get general information on the item type
			ItemBasicData* idata = ItemFactory::getItemBasicData(m_item->m_subtype);
			if (idata == 0)
			{
				ERRORMSG("No Item Data for Item type %s found",m_item->m_subtype.c_str());
			}
			
			// cap enchantment values with items values
			min_enchant = std::min(min_enchant, idata->m_max_enchant);
			max_enchant = std::min(max_enchant, idata->m_max_enchant);
			
			int num_mods = ItemFactory::createMagicMods(m_item,modprobs ,max_enchant, min_enchant, max_enchant, idata->m_enchant_multiplier, false, 1);
			
			lua_pushnumber(L,num_mods);
			return 1;
		}
		return 0;
	}
	else
	{
		ERRORMSG("Syntax: addItemMagicMods(string modtype, int min_enchant, int max_enchant)");
	}
	return 0;
}

int EventSystem::searchPlayerItem(lua_State *L)
{
	int argc = lua_gettop(L);
	if (argc>=2  && lua_isnumber(L,1) && lua_isstring(L,2))
	{
		int id = lua_tointeger(L, 1);
		std::string subtype = lua_tostring(L, 2);
		Player* pl = dynamic_cast<Player*>(World::getWorld()->getPlayer(id));
		if (pl != 0)
		{
			short startpos = 0;
			if (argc>=3  && lua_isnumber(L,3))
			{
				startpos = lua_tointeger(L, 3);
			}
			else if (argc>=3  && lua_isstring(L,3))
			{
				std::string posstr = lua_tostring(L, 3);
				startpos = Equipement::stringToPosition(posstr, pl->isUsingSecondaryEquip ());
			}
			short ret = pl->getEquipement()->findItem(subtype,startpos);
			lua_pushnumber(L,ret);
			return 1;
		}
		return 0;
	}
	else
	{
		ERRORMSG("Syntax: searchPlayerItem(int playerid, string subtype, [int startpos])");
	}
	return 0;
}

int EventSystem::getPlayerItem(lua_State *L)
{
	int argc = lua_gettop(L);
	if (argc>=2  && lua_isnumber(L,1) && (lua_isnumber(L,2) || lua_isstring(L,2)))
	{
		int id = lua_tointeger(L, 1);
		std::string subtype = lua_tostring(L, 2);
		Player* pl = dynamic_cast<Player*>(World::getWorld()->getPlayer(id));
		if (pl != 0)
		{
			short pos =0;
			if (lua_isnumber(L,2))
			{
				pos = (short) lua_tonumber(L,2);
			}
			else
			{
				std::string subtype = lua_tostring(L,2);
				pos = Equipement::stringToPosition(subtype, pl->isUsingSecondaryEquip ());
				if (pos == Equipement::NONE)
				{
					pos = pl->getEquipement()->findItem(subtype);
				}
			}
			
			if (pos == Equipement::NONE)
				return 0;
			
			Item* item = pl->getEquipement()->getItem(pos);
			if (item != 0)
			{
				setItem(item);
				m_item_in_game = true;
			}
			else
			{
				setItem(0);
				m_item_in_game = false;
			}
		}
	}
	else
	{
		ERRORMSG("Syntax: getPlayerItem(int playerid, string subtype | short itempos)");
	}
	return 0;
}

int EventSystem::removePlayerItem(lua_State *L)
{
	int argc = lua_gettop(L);
	if (argc>=2  && lua_isnumber(L,1) && (lua_isnumber(L,2) || lua_isstring(L,2)))
	{
		int id = lua_tointeger(L, 1);
		std::string subtype = lua_tostring(L, 2);
		Player* pl = dynamic_cast<Player*>(World::getWorld()->getPlayer(id));
		if (pl != 0)
		{
			short pos =0;
			if (lua_isnumber(L,2))
			{
				pos = (short) lua_tonumber(L,2);
			}
			else
			{
				std::string subtype = lua_tostring(L,2);
				pos = Equipement::stringToPosition(subtype, pl->isUsingSecondaryEquip ());
				if (pos == Equipement::NONE)
				{
					pos = pl->getEquipement()->findItem(subtype);
				}
			}
			
			if (pos == Equipement::NONE)
				return 0;
			
			Item* item = 0;
			pl->getEquipement()->swapItem(item,pos);
			if (item != 0)
			{
				setItem(item);
				m_item_in_game = false;
			}
			
			if (pos < Equipement::CURSOR_ITEM)
			{
				NetEvent event;
				event.m_id = pl->getId();
				event.m_data = pos;
				event.m_type = NetEvent::PLAYER_ITEM_REMOVE;
				World::getWorld()->insertNetEvent(event);
			}
		}
	}
	else
	{
		ERRORMSG("Syntax: removePlayerItem(int playerid, string subtype | short itempos)");
	}
	return 0;
}

int EventSystem::getInventoryPosition(lua_State *L)
{
	int argc = lua_gettop(L);
	if (argc>=2  && lua_isnumber(L,1) &&  lua_isstring(L,2))
	{
		int id = lua_tointeger(L, 1);
		std::string posstr = lua_tostring(L, 2);
		Player* pl = dynamic_cast<Player*>(World::getWorld()->getPlayer(id));
		if (pl != 0)
		{
			short ret = Equipement::stringToPosition(posstr, pl->isUsingSecondaryEquip ());
			lua_pushnumber(L,ret);
			return 1;
		}
	}
	else
	{
		ERRORMSG("Syntax: getInventoryPosition(int playerid, string position)");
	}
	return 0;
}

int EventSystem::insertPlayerItem(lua_State *L)
{
	int argc = lua_gettop(L);
	if (argc>=1  && lua_isnumber(L,1))
	{
		int id = lua_tointeger(L, 1);
		Player* pl = dynamic_cast<Player*>(World::getWorld()->getPlayer(id));
		if (pl != 0 && m_item_in_game == false)
		{
			bool equip = true;
			if (argc>=2  && lua_isboolean(L,2))
			{
				equip = lua_toboolean(L,2);
			}
			pl->insertItem(m_item,true,equip);
			m_item_in_game = true;
		}
	}
	else
	{
		ERRORMSG("Syntax: insertPlayerItem(int playerid, [bool equip])");
	}
	
	return 0;
}

int EventSystem::deleteItem(lua_State *L)
{
	setItem(0);
	m_item_in_game = false;
	return 0;
}


int EventSystem::createFloatingText(lua_State *L)
{
	int argc = lua_gettop(L);
	if (argc>=1 && (lua_isstring(L,1) || lua_istable(L,1)) && (lua_isstring(L,2) || lua_istable(L,2)))
	{
		TranslatableString text;
		getTranslatableString(L,text,1);
		
		Vector position;
		position = getVector(L,2);
		
		std::string colour="FFFF5555";
		if (argc>=3 && lua_isstring(L,3))
		{
			colour = lua_tostring(L,3);
			// if only RRGGBB is given, add full alpha value
			if (colour.size() == 6)
			{
				colour = std::string("FF") + colour;
			}
		}
		
		FloatingText::Size size = FloatingText::NORMAL;
		if (argc>=4 && lua_isstring(L,4))
		{
			std::string sizestr = lua_tostring(L,4);
			if (sizestr == "small")
				size = FloatingText::SMALL;
			if (sizestr == "big")
				size = FloatingText::BIG;
		}
		
		float time = 1000;
		if (argc>=5 && lua_isnumber(L,5))
		{
			time = lua_tonumber(L,5);
		}
		
		float offset = 0.1;
		if (argc>=6 && lua_isnumber(L,6))
		{
			offset = lua_tonumber(L,6);
		}
		
		m_region->createFloatingText(text,position,size,colour,time,offset);
	}
	else
	{
		ERRORMSG("Syntax: createFloatingText(string text,Vector position, [string colour], [string size], [float time], [float offset] )");
	}
	return 0;
}

int EventSystem::addLocation(lua_State *L)
{
	int argc = lua_gettop(L);
	if (argc>=2 && (lua_istable(L,2) || lua_isstring(L,2)) && lua_isstring(L,1))
	{
		LocationName loc = lua_tostring(L, 1);
		Vector v = getVector(L,2);


		if (m_region !=0)
		{
			m_region->addLocation(loc,v);
		}
	}
	else
	{
		ERRORMSG("Syntax: addLocation(string locname, {x, y})");
	}
	return 0;
}

int EventSystem::setLocation(lua_State *L)
{
	int argc = lua_gettop(L);
	if (argc>=2 && (lua_istable(L,2) || lua_isstring(L,2)) && lua_isstring(L,1))
	{
		LocationName loc = lua_tostring(L, 1);
		Vector v = getVector(L,2);


		if (m_region !=0)
		{
			m_region->setLocation(loc,v);
		}
	}
	else
	{
		ERRORMSG("Syntax: setLocation(string locname, {x, y})");
	}
	return 0;
}

int EventSystem::getLocation(lua_State *L)
{
	int argc = lua_gettop(L);
	if (argc>=1 && lua_isstring(L,1))
	{
		LocationName loc = lua_tostring(L, 1);
		if (m_region !=0)
		{
			pushVector(L,m_region->getLocation(loc));

			return 1;
		}
	}
	else
	{
		ERRORMSG("Syntax: getLocation( string locationname");
	}
	return 0;
}

int EventSystem::addArea(lua_State *L)
{
	int argc = lua_gettop(L);
	if (argc>=2 && lua_isstring(L,1) && (lua_isstring(L,2) || (lua_istable(L,2)) ))
	{
		AreaName area = lua_tostring(L, 1);
		Shape s=getArea(L,2);

		if (m_region !=0)
		{
			m_region->addArea(area,s);
		}
	}
	else
	{
		ERRORMSG("Syntax: addArea(string areaname, 'circle' , {mx, my} ,r) \n \
				addArea(string areaname, 'rect' , {cx, cy} ,{ex, ey}) ");
	}
	return 0;
}

int EventSystem::startTimer(lua_State *L)
{
	int argc = lua_gettop(L);
	if (argc>=2 && lua_isstring(L,1) && lua_isnumber(L,2))
	{
		std::string type = lua_tostring(L, 1);
		float time = lua_tonumber(L, 2);

		if (m_region !=0)
		{
			m_trigger = new Trigger(type);
			m_region->insertTimedTrigger(m_trigger,time);
		}
	}
	else
	{
		ERRORMSG("Syntax: startTimer(triggername, time");
	}
	return 0;
}

int EventSystem::insertTrigger(lua_State *L)
{
	int argc = lua_gettop(L);
	if (argc>=1 && lua_isstring(L,1))
	{
		std::string type = lua_tostring(L, 1);
		Region* reg = m_region;

		if (argc >=2 && lua_isstring(L,2))
		{
			std::string regname = lua_tostring(L, 2);
			reg = World::getWorld()->getRegion(regname);
		}

		if (reg !=0)
		{
			m_trigger = new Trigger(type);
			reg->insertTrigger(m_trigger);
		}
	}
	else
	{
		ERRORMSG("Syntax: insertTrigger(triggername, [regionname]");
	}
	return 0;
}

int EventSystem::addTriggerVariable(lua_State *L)
{
	int argc = lua_gettop(L);
	if (argc>=2  && lua_isstring(L,1))
	{
		if (m_trigger !=0)
		{
			std::string name = lua_tostring(L, 1);

			if (lua_isnumber(L,2))
			{
				float f = lua_tonumber(L, 2);
				m_trigger->addVariable(name,f);
			}
			else if (lua_isstring(L,2))
			{
				std::string s = lua_tostring(L, 2);
				m_trigger->addVariable(name, s);
			}
			else if (lua_istable(L,2))
			{
				Vector v = getVector(L,2);
				m_trigger->addVariable(name,v);
			}

		}
	}
	else
	{
		ERRORMSG("Syntax: addTriggerVariable(string varname, value)");
	}
	return 0;
}

int EventSystem::setCutsceneMode(lua_State *L)
{
	int argc = lua_gettop(L);
	if (argc>=1  && lua_isboolean(L,1))
	{
		bool mode = lua_toboolean(L,1);
		if (m_region!=0)
		{
			m_region->setCutsceneMode(mode);
		}
	}
	else
	{
		ERRORMSG("Syntax: setCutsceneMode(bool mode) ");
	}

	return 0;
}


int EventSystem::addCameraPosition(lua_State *L)
{
	int argc = lua_gettop(L);
	if (argc>=1)
	{
		if (m_region !=0)
		{
			RegionCamera& cam = m_region->getCamera();
			RegionCamera::Position pos;
			if (cam.m_next_positions.empty())
			{
				pos = cam.m_position;
			}
			else
			{
				pos = cam.m_next_positions.back().first;
			}

			float time =0;
			if (lua_isnumber(L,1))
			{
				time = lua_tonumber(L,1);
			}

			if (lua_istable(L,2) || lua_isstring(L,2))
			{
				pos.m_focus = getVector(L,2);
			}

			if (lua_isnumber(L,3))
			{
				pos.m_phi = lua_tonumber(L,3);
			}

			if (lua_isnumber(L,4))
			{
				pos.m_theta = lua_tonumber(L,4);
			}

			if (lua_isnumber(L,5))
			{
				pos.m_distance = lua_tonumber(L,5);
			}

			cam.addPosition(pos,time);
		}
	}
	else
	{
		ERRORMSG("Syntax: setCameraPosition(float time, {float x, float y}, float phi, float theta, float dist");
	}

	return 0;
}


int EventSystem::setLight(lua_State *L)
{
	int argc = lua_gettop(L);
	if (argc>=2 && lua_isstring(L,1) &&lua_istable(L,2))
	{
		std::string type = lua_tostring(L,1);
		
		float value[3];
		for (int i=0; i<3; i++)
		{
			lua_pushinteger(L, i+1);
			lua_gettable(L, 2);
			value[i] = lua_tonumber(L, -1);
			lua_pop(L, 1);
		}
		
		float time =0;
		if (argc>=3 && lua_isnumber(L,3))
		{
			time = lua_tonumber(L,3);
		}
		
		m_region->getLight().setLight(type,value,time);
	}
	else
	{
		ERRORMSG("Syntax: setLight(string lighttype, {red, green,blue} [,time=0])");
	}
	return 0;
}

int EventSystem::speak(lua_State *L)
{
	if (m_dialogue ==0)
		return 0;

	int argc = lua_gettop(L);
	if (argc>=2 && lua_isstring(L,1) && (lua_isstring(L,2) || lua_istable(L,2)))
	{
		std::string refname = lua_tostring(L, 1);
		TranslatableString text;
		getTranslatableString(L,text,2);

		std::string emotion = "";
		if (argc>=3 && lua_isstring(L,3))
		{
			emotion = lua_tostring(L,3);
		}
		
		float time = 0;
		if (argc>=4 && lua_isnumber(L,4))
		{
			time = lua_tonumber(L,4);
		}
		if (time ==0)
		{
			time = (text.size()*50+800) * Options::getInstance()->getTextSpeed();
		}

		m_dialogue->speak(refname,text,emotion,time);

	}
	else
	{
		ERRORMSG("Syntax: speak(string refname, string text [,string emotion [,float time]])");
	}

	return 0;
}

int EventSystem::unitSpeak(lua_State *L)
{
	int argc = lua_gettop(L);
	if (argc>=2 && (lua_isnumber(L,1) || lua_isstring(L,1)) && (lua_isstring(L,2) ||lua_istable(L,1)))
	{
		int id =0;
		if (lua_isnumber(L,1))
		{
			id = lua_tointeger(L, 1);
		}
		else
		{
			if (m_dialogue !=0)
			{
				id = m_dialogue->getSpeaker(lua_tostring(L, 1));
			}
		}
		
		if (id ==0)
			return 0;
		
		CreatureSpeakText text;
		text.m_in_dialogue = false;
		getTranslatableString(L,text.m_text,2);
		
		WorldObject* wo =0;
		if (m_region !=0)
		{
			wo = m_region->getObject(id);
		}
		
		Creature* cr = dynamic_cast<Creature*>(wo);
		
		text.m_emotion = "";
		if (argc>=3 && lua_isstring(L,3))
		{
			text.m_emotion = lua_tostring(L,3);
		}
		
		text.m_time = 0;
		if (argc>=4 && lua_isnumber(L,4))
		{
			text.m_time = lua_tonumber(L,4);
		}
		if (text.m_time ==0)
		{
			text.m_time = (text.m_text.size()*50+800) * Options::getInstance()->getTextSpeed();
		}

		if (cr!=0)
		{
			cr->speakText(text);
		}
		else
		{
			ERRORMSG("getObjectValue: Object doesnt exist %i",id);
		}
	}
	else
	{
		ERRORMSG("Syntax: unitSpeak(( int id, string text [,string emotion [,float time]])");
	}
	return 0;
	

}

int EventSystem::addQuestion(lua_State *L)
{
	if (m_dialogue ==0)
		return 0;

	int argc = lua_gettop(L);
	if (argc>=1 && (lua_isstring(L,1) || lua_istable(L,1)))
	{
		
		TranslatableString text;
		getTranslatableString(L, text,1);
		if (argc>=2 && lua_isstring(L,2))
		{
			std::string player = lua_tostring(L,2);
			m_dialogue->addQuestion(text,player);
		}
		else
		{
			m_dialogue->addQuestion(text);
		}
	}
	else
	{
		ERRORMSG("Syntax: addQuestion(string text, string asked_player='main_player')");
	}

	return 0;
}


int EventSystem::addAnswer(lua_State *L)
{
	if (m_dialogue ==0)
		return 0;

	int argc = lua_gettop(L);
	if (argc>=2 && (lua_isstring(L,1) || lua_istable(L,1)) && lua_isstring(L,2))
	{
		TranslatableString text;
		getTranslatableString(L, text,1);
		std::string topic = lua_tostring(L, 2);

		m_dialogue->addAnswer(text,topic);
	}
	else
	{
		ERRORMSG("Syntax: addAnswer(string text, string topic)")
	}

	return 0;
}

int EventSystem::setSpeakerEmotion(lua_State *L)
{
	if (m_dialogue ==0)
		return 0;
	
	int argc = lua_gettop(L);
	if (argc>=2 && lua_isstring(L,1) && lua_isstring(L,2))
	{
		std::string refname = lua_tostring(L, 1);
		std::string emotion = lua_tostring(L, 2);
		m_dialogue->speak(refname,TranslatableString("#emotion#"),emotion,0);
	}
	else
	{
		ERRORMSG("Syntax: setSpeakerEmotion(string refname, string emotion)");
	}

	return 0;
}

int EventSystem::setSpeakerAnimation(lua_State *L)
{
	if (m_dialogue ==0)
		return 0;
	
	int argc = lua_gettop(L);
	if (argc>=3 && lua_isstring(L,1) && lua_isstring(L,2) && lua_isnumber(L,3))
	{
		std::string refname = lua_tostring(L, 1);
		std::string anim = lua_tostring(L, 2);
		float time = lua_tonumber(L, 3);
		
		std::string txt = "#animation#";
		if (argc>=4 && lua_isboolean(L,4))
		{
			bool repeat = lua_toboolean(L, 4);
			if (repeat)
			{
				DEBUG("repeat");
				txt="#animation_r#";
			}
		}
		
		m_dialogue->speak(refname,TranslatableString(txt),anim,time);
	}
	else
	{
		ERRORMSG("Syntax: setSpeakerAnimation(string refname, string animation, float time, [bool repeat = false])");
	}

	return 0;
}

int EventSystem::setSpeakerPosition(lua_State *L)
{
	if (m_dialogue ==0)
		return 0;
	
	int argc = lua_gettop(L);
	if (argc>=2 && lua_isstring(L,1) && lua_isstring(L,2))
	{
		std::string refname = lua_tostring(L, 1);
		std::string position = lua_tostring(L, 2);
		m_dialogue->speak(refname,TranslatableString("#position#"),position,0);
	}
	else
	{
		ERRORMSG("Syntax: setSpeakerPosition(string refname, string position)");
	}

	return 0;
}

int EventSystem::changeTopic(lua_State *L)
{
	if (m_dialogue ==0)
		return 0;

	int argc = lua_gettop(L);
	if (argc>=1 && lua_isstring(L,1))
	{
		std::string topic = lua_tostring(L, 1);
		m_dialogue->speak("",TranslatableString("#change_topic#"),topic);
	}
	else
	{
		ERRORMSG("Syntax: addQuestion(string text)");
	}

	return 0;
}

int EventSystem::jumpTopic(lua_State *L)
{
	if (m_dialogue ==0)
		return 0;

	int argc = lua_gettop(L);
	if (argc>=1 && lua_isstring(L,1))
	{
		std::string topic = lua_tostring(L, 1);
		m_dialogue->speak("",TranslatableString("#jump_topic#"),topic);
	}
	else
	{
		ERRORMSG("Syntax: addQuestion(string text)");
	}

	return 0;
}

int EventSystem::executeInDialog(lua_State *L)
{
	if (m_dialogue ==0)
		return 0;
	
	int argc = lua_gettop(L);
	if (argc>=1 && lua_isstring(L,1))
	{
		std::string code = lua_tostring(L, 1);
		m_dialogue->speak("",TranslatableString("#execute#"),code);
	}
	else
	{
		ERRORMSG("Syntax: executeInDialog(luacode)");
	}

	return 0;
}

int EventSystem::addSpeakerInDialog(lua_State *L)
{
	if (m_dialogue ==0)
		return 0;

	int argc = lua_gettop(L);
	if (lua_isnil(L,1))
	{
		return 0;
	}
	
	if (argc>=1 && lua_isnumber(L,1))
	{
		
		std::string speaker = lua_tostring(L, 1);

		std::string refname ="";
		if (argc>=2 && lua_isstring(L,2))
		{
			refname = lua_tostring(L,2);
		}
		
		m_dialogue->speak(speaker,TranslatableString("#add_speaker#"),refname);
	}
	else
	{
		ERRORMSG("Syntax: addSpeakerInDialog(int speaker, string refname)");
	}

	return 0;
}

int EventSystem::createDialogue(lua_State *L)
{
	if (m_region ==0)
		return 0;

	m_dialogue = new Dialogue(m_region, "global");
	m_region->insertDialogue(m_dialogue);

	return 0;
}

int EventSystem::addSpeaker(lua_State *L)
{
	if (m_dialogue ==0)
		return 0;

	int argc = lua_gettop(L);
	if (lua_isnil(L,1))
	{
		return 0;
	}
	
	if (argc>=1 && lua_isnumber(L,1))
	{
		
		int speaker = int (lua_tonumber(L, 1));

		std::string refname ="";
		if (argc>=2 && lua_isstring(L,2))
		{
			refname = lua_tostring(L,2);
		}
		
		bool force = true;
		if (argc>=3 && lua_isboolean(L,3))
		{
			force = lua_toboolean(L,3);
		}

		m_dialogue->addSpeaker(speaker,refname,force);
	}
	else
	{
		ERRORMSG("Syntax: addSpeaker(int speaker, string refname, [bool force])");
	}

	return 0;
}

int EventSystem::removeSpeaker(lua_State *L)
{
	if (m_dialogue ==0)
		return 0;

	int argc = lua_gettop(L);
	if ((argc>=1 && lua_isnumber(L,1)) || lua_isstring(L,1))
	{
		
		std::string refname = lua_tostring(L, 1);
		std::string txt = "#remove_speaker#";
		m_dialogue->speak(refname,TranslatableString(txt));
	}
	else
	{
		ERRORMSG("Syntax: removeSpeaker(int unitid)");
	}

	return 0;	
}

int EventSystem::getSpeaker(lua_State *L)
{
	if (m_dialogue ==0)
		return 0;

	int argc = lua_gettop(L);
	if (argc>=1 && lua_isstring(L,1))
	{
		std::string refname = lua_tostring(L,1);
		int id = m_dialogue->getSpeaker(refname);
		lua_pushnumber(L,id);
	}
	else
	{
		ERRORMSG("Syntax: int getSpeaker(string refname)");
		return 0;
	}

	return 1;
}


int EventSystem::setTopicBase(lua_State *L)
{
	if (m_dialogue ==0)
		return 0;

	int argc = lua_gettop(L);
	if (argc>=1 && lua_isstring(L,1))
	{
		std::string base = lua_tostring(L, 1);
		m_dialogue->setTopicBase(base);
	}
	else
	{
		ERRORMSG("Syntax: setTopicBase(string text)");
	}

	return 0;
}

int EventSystem::setRefName(lua_State *L)
{
	int argc = lua_gettop(L);
	if (argc>=2 && lua_isnumber(L,1) && lua_isstring(L,2))
	{
		int id = int (lua_tonumber(L,1));
		std::string refname = lua_tostring(L,2);

		WorldObject* wo = m_region->getObject(id);
		if (wo!=0 && wo->isCreature())
		{
			static_cast<Creature*>(wo)->setRefName(refname);
		}
	}
	else
	{
		ERRORMSG("Syntax: setRefName(int id, string name)");
	}

	return 0;
}

int EventSystem::setDialogueActive(lua_State *L)
{
	int argc = lua_gettop(L);
	if (argc>=1 &&  lua_isboolean(L,1))
	{
		if (m_dialogue ==0)
			return 0;
		
		bool act = lua_toboolean(L,1);
		m_dialogue->setActive(act);
	}
	else
	{
		ERRORMSG("Syntax: setDialogueActive(true | false)");
	}
	return 0;
}

int EventSystem::setCurrentDialogue(lua_State *L)
{
	int argc = lua_gettop(L);
	if (argc>=1 &&  lua_isnumber(L,1))
	{
		int id = int (lua_tonumber(L,1));
		WorldObject* wo = m_region->getObject(id);
		if (wo!=0 && wo->isCreature())
		{
			m_dialogue = static_cast<Creature*>(wo)->getDialogue();
		}
	}
	else
	{
		ERRORMSG("Syntax: setCurrentDialogue(int objectid)");
	}
	return 0;
}

int EventSystem::getRolePlayers(lua_State *L)
{
	int argc = lua_gettop(L);
	if (argc>=1 && lua_isstring(L,1))
	{
		lua_newtable(L);

		std::set< int >& members = World::getWorld()->getParty(Fraction::PLAYER)->getMembers();
		std::set< int >::iterator it;

		WorldObject* wo;
		Player* pl;

		std::string role = lua_tostring(L,1);

		int cnt =1;
		for (it = members.begin(); it != members.end(); ++it)
		{
			wo = World::getWorld()->getPlayer(*it);
			pl = dynamic_cast<Player*>(wo);
			if (pl !=0 && pl->checkRole(role))
			{
				lua_pushnumber(L,(*it));
				lua_rawseti (L, -2, cnt);
				cnt++;
			}
		}


	}
	else
	{
		ERRORMSG("Syntax: getRolePlayers(string role)");
		return 0;
	}

	return 1;
}

int EventSystem::teleportPlayer(lua_State *L)
{
	int argc = lua_gettop(L);
	if (argc>=3 && lua_isnumber(L,1) && lua_isstring(L,2) && lua_isstring(L,3))
	{
		int id = lua_tointeger(L, 1);
		std::string regname = lua_tostring(L, 2);
		std::string locname = lua_tostring(L, 3);
		
		RegionLocation regloc;
		regloc.first = regname;
		regloc.second = locname;
		WorldObject* wo =World::getWorld()->getPlayer(id);
		if (wo !=0 && wo->getRegion() !=0)
		{
			if (wo->getRegion()->getIdString() == regname)
			{
				// Spieler ist schon in der Richtigen Region
				Vector pos = wo->getRegion()->getLocation(locname);
				wo->getRegion()->getFreePlace (wo->getShape(), wo->getLayer(), pos);
				wo->moveTo(pos);
				lua_pushboolean(L,false);
				return 1;
			}
			else
			{
				wo->getRegion()->insertPlayerTeleport(wo->getId(),regloc);
				lua_pushboolean(L,true);
				return 1;
			}
		}
	}
	else
	{
		ERRORMSG("Syntax: teleportPlayer(int playerid, string regionname, string locationname)");
	}
	return 0;
}

int EventSystem::createEvent(lua_State *L)
{
	int argc = lua_gettop(L);
	if (argc>=1 && lua_isstring(L,1))
	{
		std::string trigger = lua_tostring(L,1);
		bool once = false;
		if (argc>=2 && lua_isboolean(L,2))
		{
			once = lua_toboolean(L,2);
		}
		
		Region* reg = m_region;
		if (argc>=3 && lua_isstring(L,3))
		{
			reg = World::getWorld()->getRegion(lua_tostring(L,3));
		}
		if (reg ==0)
		{
			return 0;
			DEBUG("region for createEvent does not exist");
		}
		
		m_event = new Event();
		m_event->setOnce(once);
		reg ->addEvent(trigger,m_event);
	}
	else
	{
		ERRORMSG("Syntax: createEvent(string triggername [,bool once [,string regionname]])");
	}
	return 0;
}

int EventSystem::addCondition(lua_State *L)
{
	int argc = lua_gettop(L);
	if (argc>=1 && lua_isstring(L,1))
	{
		if (m_event !=0)
		{
			std::string cond = lua_tostring(L,1);
			m_event->setCondition(cond.c_str());
		}
	}
	else
	{
		ERRORMSG("Syntax: addCondition(string luacode)");		
	}
	return 0;
}

int EventSystem::addEffect(lua_State *L)
{
	int argc = lua_gettop(L);
	if (argc>=1 && lua_isstring(L,1))
	{
		if (m_event !=0)
		{
			std::string effect = lua_tostring(L,1);
			m_event->setEffect(effect.c_str());
		}
	}
	else
	{
		ERRORMSG("Syntax: addEffect(string luacode)");		
	}
	return 0;
}

int EventSystem::getRelation(lua_State *L)
{
	int argc = lua_gettop(L);
	if (argc>=1 && (lua_isstring(L,1) || lua_isnumber(L,1)))
	{
		Fraction::Id frac1= Fraction::PLAYER;
		Fraction::Id frac2= Fraction::PLAYER;
		 if (lua_isnumber(L,1))
		{
			int id = lua_tonumber(L,1);
			WorldObject* wo = m_region->getObject(id);
			if (wo != 0)
			{
				frac1 = wo->getFraction();
			}
		}
		else if (lua_isstring(L,1))
		{
			std::string f1 = lua_tostring(L,1);
			frac1 = World::getWorld()->getFractionId(f1);
		}
		
		
		if (lua_isnumber(L,2))
		{
			int id = lua_tonumber(L,2);
			WorldObject* wo = m_region->getObject(id);
			if (wo != 0)
			{
				frac2 = wo->getFraction();
			}
		}
		else if (lua_isstring(L,2))
		{
			std::string f2 = lua_tostring(L,2);
			frac2 = World::getWorld()->getFractionId(f2);
		}
		
		Fraction::Relation rel = World::getWorld()->getRelation(frac1,frac2);
		std::string result="neutral";
		if (rel == Fraction::ALLIED)
			result="allied";
		if (rel == Fraction::HOSTILE)
			result="hostile";
		
		lua_pushstring(L,result.c_str());
		return 1;
	}
	else
	{
		ERRORMSG("Syntax: getRelation(string fraction1, string fraction2)");		
	}
	return 0;
}

int EventSystem::setRelation(lua_State *L)
{
	int argc = lua_gettop(L);
	if (argc>=3 && lua_isstring(L,1) && lua_isstring(L,2) && lua_isstring(L,3))
	{
		std::string frac1 = lua_tostring(L,1);
		std::string frac2 = lua_tostring(L,2);
		std::string relation = lua_tostring(L,3);
		
		Fraction::Relation rel = Fraction::NEUTRAL;
		if (relation == "allied" || relation =="ALLIED")
			rel = Fraction::ALLIED;
		if (relation == "hostile"  || relation == "HOSTILE")
			rel = Fraction::HOSTILE;
		
		World::getWorld()->setRelation(frac1,frac2,rel);
	}
	else
	{
		ERRORMSG("Syntax: setRelation(string fraction1, string fraction2, 'neutral' | 'allied' | 'hostile')");		
	}
	return 0;
}

int EventSystem::printMessage(lua_State *L)
{
	int argc = lua_gettop(L);
	if (argc>=1 && (lua_isstring(L,1) || lua_istable(L,1)))
	{
		TranslatableString msg;
		getTranslatableString(L,msg,1);
		World::getWorld()->handleMessage(msg,NOSLOT);
	}
	else
	{
		ERRORMSG("Syntax: printMessage(string message)");		
	}
	return 0;
}

int EventSystem::addMusic(lua_State *L)
{
	int argc = lua_gettop(L);
	if (argc>=1 && lua_isstring(L,1))
	{
		if (m_region != 0)
		{
			std::string track = lua_tostring(L,1);
			m_region->addMusicTrack(track);
		}
	}
	else
	{
		ERRORMSG("Syntax: addMusic(string oggfile)");		
	}
	return 0;
}


int EventSystem::clearMusicList(lua_State *L)
{
	if (m_region != 0)
	{
			m_region->clearMusicTracks();
	}
	return 0;
}

int EventSystem::playSound(lua_State *L)
{
	int argc = lua_gettop(L);
	if (argc>=1 && lua_isstring(L,1))
	{
		SoundName sname = lua_tostring(L,1);
		float volume = 1.0;
		if (argc>=2 && lua_isnumber(L,2))
		{
			volume = lua_tonumber(L,2);
		}
		
		Vector position;
		Vector* ppos =0;
		if (argc>=3)
		{
			position = getVector(L,3);
			ppos = &position;
		}
		
		SoundSystem::playAmbientSound(sname, volume, ppos);
	}
	else
	{
		ERRORMSG("Syntax: playSound(string soundname, float volume=1.0, Vector position=(playerpos))");		
	}
	return 0;
}

int EventSystem::getOption(lua_State *L)
{
	int argc = lua_gettop(L);
	if (argc>=1 && lua_isstring(L,1))
	{
		std::string valname = lua_tostring(L,1);
		return Options::getInstance()->getValue(valname);
	}
	else
	{
		ERRORMSG("Syntax: getOption(string optionname)");		
	}
	return 0;
}

int EventSystem::setOption(lua_State *L)
{
	int argc = lua_gettop(L);
	if (argc>=2 && lua_isstring(L,1))
	{
		std::string valname = lua_tostring(L,1);
		Options::getInstance()->setValue(valname);
	}
	else
	{
		ERRORMSG("Syntax: setOption(string optionname, value)");		
	}
	return 0;
}

int EventSystem::writeLog(lua_State *L)
{
	int argc = lua_gettop(L);
	if (argc>=2 && lua_isstring(L,1) && lua_isstring(L,2))
	{
		std::string loglevel = lua_tostring(L,1);
		std::string text = lua_tostring(L,2);
		
		Log::MessageLogLevel level = Log::MSG_INFO;
		if (loglevel == "debug" || loglevel == "DEBUG")
			level = Log::MSG_DEBUG;
		if (loglevel == "warning" || loglevel == "WARNING")
			level = Log::MSG_WARNING;
		if (loglevel == "error" || loglevel == "ERROR")
			level = Log::MSG_ERROR;
		
		LOGGER(level, text.c_str());
	}
	else
	{
		ERRORMSG("Syntax: writeLog(string loglevel, string text)");
	}
	return 0;
}

int EventSystem::clearOgreStatistics(lua_State *L)
{
	Ogre::Root::getSingleton().getAutoCreatedWindow()->resetStatistics();
	return 0;
}

int EventSystem::getOgreStatistics(lua_State *L)
{
	lua_newtable(L);
	
	const Ogre::RenderTarget::FrameStats& stats = Ogre::Root::getSingleton().getAutoCreatedWindow()->getStatistics();
	
	Ogre::LogManager::getSingleton().getLog(SumwarsHelper::userPath() + "/BenchLog.log")->logMessage("------------------------------");
	Ogre::LogManager::getSingleton().getLog(SumwarsHelper::userPath() + "/BenchLog.log")->logMessage("avarage FPS:     " + Ogre::StringConverter::toString(stats.avgFPS));
	Ogre::LogManager::getSingleton().getLog(SumwarsHelper::userPath() + "/BenchLog.log")->logMessage("worst FPS:       " + Ogre::StringConverter::toString(stats.worstFPS));
	Ogre::LogManager::getSingleton().getLog(SumwarsHelper::userPath() + "/BenchLog.log")->logMessage("worst FrameTime: " + Ogre::StringConverter::toString(stats.worstFrameTime));
	Ogre::LogManager::getSingleton().getLog(SumwarsHelper::userPath() + "/BenchLog.log")->logMessage("Triangles:       " + Ogre::StringConverter::toString(stats.triangleCount));
	Ogre::LogManager::getSingleton().getLog(SumwarsHelper::userPath() + "/BenchLog.log")->logMessage("Batches:         " + Ogre::StringConverter::toString(stats.batchCount));
	
	lua_pushstring(L, "avgFPS");
	lua_pushnumber(L, stats.avgFPS);
	lua_settable(L, -3);
	
	lua_pushstring(L, "worstFPS");
	lua_pushnumber(L, stats.worstFPS);
	lua_settable(L, -3);
	
	lua_pushstring(L, "worstFrameTime");
	lua_pushnumber(L, stats.worstFrameTime);
	lua_settable(L, -3);
	
	lua_pushstring(L, "triangleCount");
	lua_pushnumber(L, stats.triangleCount);
	lua_settable(L, -3);
	
	lua_pushstring(L, "batchCount");
	lua_pushnumber(L, stats.batchCount);
	lua_settable(L, -3);
	
	return 1;
}

int  EventSystem::reloadData(lua_State *L)
{
	int argc = lua_gettop(L);
	if (argc>=1 && lua_isstring(L,1))
	{
		std::string dataname = lua_tostring(L,1);
		// std::transform(dataname.begin(), dataname.end(), dataname.begin(), std::tolower);
		int bitmask = 0;
		if (dataname == "playerclasses")
			bitmask = World::DATA_PLAYERCLASSES;
		if (dataname == "monsters")
			bitmask = World::DATA_MONSTERS;
		if (dataname == "objects")
			bitmask = World::DATA_OBJECTS;
		if (dataname == "projectiles" || dataname == "missiles")
			bitmask = World::DATA_PROJECTILES;
		if (dataname == "items")
			bitmask = World::DATA_ITEMS;
		if (dataname == "abilities")
			bitmask = World::DATA_ABILITIES;
		if (dataname == "luacode")
			bitmask = World::DATA_LUACODE;
		if (dataname == "events")
			bitmask = World::DATA_EVENTS;
		if (dataname == "sounds")
			bitmask = World::DATA_SOUND;
		if (dataname == "music")
			bitmask = World::DATA_MUSIC;
		if (dataname == "renderinfo")
			bitmask = World::DATA_RENDERINFO;
		if (dataname == "models")
			bitmask = World::DATA_MODELS;
		if (dataname == "gui")
			bitmask = World::DATA_GUI;
		if (dataname == "particlesystems")
			bitmask = World::DATA_PARTICLESYSTEMS;
		if (dataname == "images")
			bitmask = World::DATA_IMAGES;
		
		World::getWorld()->requestDataReload(bitmask);
			
	}
	else
	{
		ERRORMSG("Syntax: reloadData(string dataname)");		
	}
	return 0;
}

int EventSystem::writeString(lua_State *L)
{
	std::string s;
	s = lua_tostring (L,1);
	m_charconv->toBuffer(static_cast<char>(1));
	m_charconv->toBuffer(s);
	return 0;
}

int EventSystem::writeNewline(lua_State *L)
{
	m_charconv->printNewline();
	return 0;
}

void EventSystem::writeSavegame(CharConv* savegame)
{
	m_charconv = savegame;
	if (m_lua != 0)
	{
		EventSystem::doString("writeSavegame();");
		m_charconv->toBuffer(static_cast<char>(0));
	}
}

void EventSystem::readSavegame(CharConv* savegame, int playerid, bool local_player)
{
	std::stringstream stream;
	stream << "addPlayer(" <<playerid <<");";
	doString(stream.str().c_str());

	stream.str("");
	stream << "playervars["<<playerid<<"].";
	std::string prefix = stream.str();

	std::string instr,instr_pr;
	std::string tablename;
	DEBUGX("prefix %s",prefix.c_str());

	char c=1;
	while (c==1)
	{
		c=0;
		savegame->fromBuffer(c);
		if (c==0)
			break;

		savegame->fromBuffer(instr);
		if (local_player)
		{
			doString(instr.c_str());

			tablename = instr.substr(0,instr.find_first_of ('='));
			DEBUGX("table name %s",tablename.c_str());
			stream.str("");
			stream << "quests."<<tablename <<" = "<<tablename;
			DEBUGX("instr %s",stream.str().c_str());
			doString(stream.str().c_str());
		}

		if (World::getWorld() !=0 && World::getWorld()->isServer())
		{
			instr_pr = prefix;
			instr_pr += instr;
			doString(instr_pr.c_str());
		}
	}

	if (!local_player)
	{
		stream.str("");
		stream << "initPlayerVars("<<playerid<<");";
		doString(stream.str().c_str());
	}
}

void EventSystem::clearPlayerVarString(int id)
{
	m_player_varupdates.erase(id);
}

std::string EventSystem::getPlayerVarString(int id)
{
	if (m_player_varupdates.count(id) ==0)
	{
		return "";
	}
	return m_player_varupdates[id];
}

int EventSystem::writeUpdateString(lua_State *L)
{
	int argc = lua_gettop(L);
	if (argc <2)
		return 0;

	int id = (int) lua_tonumber(L,1);
	std::string instr =  lua_tostring(L,2);

	m_player_varupdates[id] += instr;
	return 0;
}



