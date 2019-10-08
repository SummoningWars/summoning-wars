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

#ifndef HYBRIDIMPLEMENTATION_H
#define HYBRIDIMPLEMENTATION_H


extern "C"
{
	
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include "eventsystem.h"

/**
 * \var struct HybridImplementation
 * \brief Struktur fuer die Beschreibung einer Implementation ind C++ und/oder Lua
 */
struct HybridImplementation
{
	/**
	 * \var std::string m_cpp_impl
	 * \brief Kennstrings anhand denen ein oder mehrere Stuecke Quellcode selektiert werden
	 */
	std::list<std::string> m_cpp_impl;
	
	/**
	 * \var int m_lua_impl
	 * \brief Handle fuer Lua-Code
	 */
	int m_lua_impl;
	
	HybridImplementation()
	{
		m_lua_impl = LUA_NOREF;
	}
	
	/**
	 * \brief Destructor
	 */
	~HybridImplementation()
	{
		EventSystem::clearCodeReference(m_lua_impl);
	}
};

#endif

