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
