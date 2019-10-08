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

#include "elementattrib.h"


void ElementAttrib::parseElement(TiXmlElement* elem)
{
		if (elem ==0)
			return;
		
		m_attribs.clear();
		
		TiXmlAttribute* pAttrib = elem->FirstAttribute();
		
		while (pAttrib != 0)
		{
			m_attribs[std::string(pAttrib->Name())] = pAttrib;
			pAttrib=pAttrib->Next();
		}
}

void ElementAttrib::getString(std::string attrib, std::string& data, std::string def)
{
		std::map<std::string, TiXmlAttribute*>::iterator it;
		it = m_attribs.find(attrib);
		
		if (it == m_attribs.end())
		{
			data = def;
			return;
		}
		
		data = it->second->Value();
}

void ElementAttrib::getInt(std::string attrib, int& data, int def)
{
		std::map<std::string, TiXmlAttribute*>::iterator it;
		it = m_attribs.find(attrib);
		
		if (it == m_attribs.end())
		{
			data = def;
			return;
		}
		
		int tmp;
		if ( it->second->QueryIntValue(&tmp) == TIXML_SUCCESS)
		{
			data = tmp;
		}
		else
		{
			data = def;
		}
}
	
	
void ElementAttrib::getShort(std::string attrib, short& data, short def)
{
	std::map<std::string, TiXmlAttribute*>::iterator it;
	it = m_attribs.find(attrib);
		
	if (it == m_attribs.end())
	{
		data = def;
		return;
	}
		
	int tmp;
	if ( it->second->QueryIntValue(&tmp) == TIXML_SUCCESS)
	{
		data = tmp;
	}
	else
	{
		data = def;
	}
}

	
void ElementAttrib::getFloat(std::string attrib, float& data, float def )
{
		std::map<std::string, TiXmlAttribute*>::iterator it;
		it = m_attribs.find(attrib);
		
		if (it == m_attribs.end())
		{
			data = def;
			return;
		}
		
		double tmp;
		if ( it->second->QueryDoubleValue(&tmp) == TIXML_SUCCESS)
		{
			data = float(tmp);
		}
		else
		{
			data = def;
		}
}

void ElementAttrib::getBool(std::string attrib, bool& data, bool def)
{
	std::map<std::string, TiXmlAttribute*>::iterator it;
	it = m_attribs.find(attrib);
		
	if (it == m_attribs.end())
	{
		data = def;
		return;
	}
	if (std::string(it->second->Value()) =="true")
	{
		data = true;
	}
	else if (std::string(it->second->Value()) == "false")
	{
		data = false;
	}
	else
	{
		data = def;
	}
}
