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

#ifndef XMLUTIL_H
#define XMLUTIL_H

#include <tinyxml.h>
#include <string>

/**
 * \brief Helper class containing XMl utility functions
 */
class XMLUtil
{
	public:
		/**
		 * \brief Sets a string attribute for an XML Element
		 * \param name name of the attribute
		 * \param value value of the attribute
		 * \param def default value
		 * If the value is equal to the default value, the attribute is not written but deleted instead.
		 */
		static void setAttribute(TiXmlElement* elem, std::string name, std::string value, std::string def="")
		{
			if (value != def)
			{
				elem->SetAttribute(name.c_str(), value.c_str());
			}
			else
			{
				elem->RemoveAttribute(name.c_str());
			}
		}
		
		/**
		 * \brief Sets an int attribute for an XML Element
		 * \param name name of the attribute
		 * \param value value of the attribute
		 * \param def default value
		 * If the value is equal to the default value, the attribute is not written but deleted instead.
		 */
		static void setAttribute(TiXmlElement* elem, std::string name, int value, int def=0)
		{
			if (value != def)
			{
				elem->SetAttribute(name.c_str(), value);
			}
			else
			{
				elem->RemoveAttribute(name.c_str());
			}
		}
		
		/**
		 * \brief Sets a double attribute for an XML Element
		 * \param name name of the attribute
		 * \param value value of the attribute
		 * \param def default value
		 * If the value is equal to the default value, the attribute is not written but deleted instead.
		 */
		static void setDoubleAttribute(TiXmlElement* elem, std::string name, double value, double def=0)
		{
			if (fabs(value - def) > 1e-9*def && fabs(value - def) > 1e-9)
			{
				elem->SetDoubleAttribute(name.c_str(), value);
			}
			else
			{
				elem->RemoveAttribute(name.c_str());
			}
		}
		
		static TiXmlElement* findElementAfter(TiXmlElement* parent, TiXmlElement* elem, std::string name)
		{
			if (elem)
			{
				return elem->NextSiblingElement(name.c_str());
			}
			else
			{
				return parent->FirstChildElement(name.c_str());
			}
		}
		
		static TiXmlElement* insertNodeAfter(TiXmlElement* parent, TiXmlElement* after, TiXmlElement* insert)
		{
			TiXmlElement* newobject;
			if (after == 0)
			{
				TiXmlElement* first = parent->FirstChildElement();
				if (first == 0)
				{
					newobject = parent->InsertEndChild(*insert)->ToElement();
				}
				else
				{
					newobject = parent->InsertBeforeChild(first,*insert)->ToElement();
				}
			}
			else
			{
				newobject = parent->InsertAfterChild(after, *insert)->ToElement();
			}
			return newobject;
		}
};


#endif