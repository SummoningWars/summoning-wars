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

#include "quest.h"
#include <libintl.h>

Quest::Quest(TranslatableString name, std::string table_name)
{
	m_name = name;
	m_table_name = table_name;
	
	m_initialisation = LUA_NOREF;
	m_description = LUA_NOREF;
	
}

Quest::~Quest()
{
	if (m_initialisation != LUA_NOREF)
	{
		luaL_unref(EventSystem::getLuaState(), LUA_REGISTRYINDEX, m_initialisation);
	}
	
	if (m_description != LUA_NOREF)
	{
		luaL_unref(EventSystem::getLuaState(), LUA_REGISTRYINDEX,m_description);
	}
}

void Quest::setInit(const char* init)
{
	EventSystem::clearCodeReference(m_initialisation);
	
	m_initialisation = EventSystem::createCodeReference(init);
}


void Quest::setDescription(const char* descr)
{
	EventSystem::clearCodeReference(m_description);
	
	m_description = EventSystem::createCodeReference(descr);
	if (m_description != LUA_NOREF)
	{
		m_description_code = descr;
	}
}

void Quest::init()
{
	std::string luainit = "if (";
	luainit += m_table_name;
	luainit += "==nil ) then \n";
	luainit += m_table_name;
	luainit += " = {}; \n";
	luainit += m_table_name;
	luainit += "_tmp = {}; \n";
	luainit += m_table_name;
	luainit += ".started = false; \n";
	luainit += m_table_name;
	luainit += ".finished = false; \n";
	luainit += m_table_name;
	luainit += ".failed = false; \n";
	luainit += "quests.";
	luainit += m_table_name;
	luainit += " = ";
	luainit += m_table_name;
	luainit += "\n end;";
	DEBUGX("lua init %s",luainit.c_str());
	EventSystem::doString(luainit.c_str());
	
	EventSystem::executeCodeReference(m_initialisation);
}

Quest::State Quest::getState()
{
	std::stringstream stream;
	stream << "if "<< m_table_name <<".failed then return " << FAILED << "\n ";
	stream << "else if "<< m_table_name <<".finished then return " << FINISHED << "\n ";
	stream << "else if "<< m_table_name <<".started then return " << STARTED << "\n ";
	stream << "else return " << NOT_STARTED << " end end end\n ";
	
	
	EventSystem::doString((char*) stream.str().c_str());
	std::string ret = EventSystem::getReturnValue();
	stream.str(ret);
	int r;
	stream >> r;
	return (State) r;
}

std::string Quest::getDescription()
{
	EventSystem::pushGettextDomain(m_gettext_domain);
	EventSystem::executeCodeReference(m_description);
	
	TranslatableString descr;
	descr.setTextDomain(EventSystem::GetGettextDomain());
	EventSystem::getTranslatableString(EventSystem::getLuaState(), descr, -1);
	
	EventSystem::getReturnValue();
	EventSystem::popGettextDomain();
	
	return descr.getTranslation();
}

TranslatableString Quest::getName()
{
	return m_name;
}

void Quest::toString(CharConv* cv)
{
	m_name.toString(cv);
	cv->toBuffer(m_table_name);
	cv->toBuffer(m_description_code);
}

void Quest::fromString(CharConv* cv)
{
	// Name und Tabellenname werden extern gelesen
	cv->fromBuffer(m_description_code);
	setDescription(m_description_code.c_str());
}

