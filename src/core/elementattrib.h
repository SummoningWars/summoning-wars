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

#ifndef __SUMWARS_CORE_ELEMENTATTRIB_H__
#define __SUMWARS_CORE_ELEMENTATTRIB_H__

#include <tinyxml.h>
#include <string>
#include <map>

/**
 * \class ElementAttrib
 * \brief Helferklasse zum zwischenspeichert der Attribute eines Elements
 */
class ElementAttrib
{
	public:
		/**
		 * \fn void parseElement(TiXmlElement* elem)
		 * \brief Parst die Attribute des Elements
		 * \param elem XML Element
		 */
		void parseElement(TiXmlElement* elem);
	
		/**
		 * \fn void getString(std::string attrib, std::string& data, std::string def ="")
		 * \brief Gibt ein Attribut als String aus.
		 * \param attrib Name des Attributs
		 * \param data Ausgabeparameter fuer die Daten
		 * \param def String, der ausgegeben wird, wenn das Attribut nicht existiert
		 */
		void getString(std::string attrib, std::string& data, std::string def ="");
	
		/**
		 * \fn void getInt(std::string attrib, int& data, int def =0)
		 * \brief Gibt ein Attribut als Integer aus.
		 * \param attrib Name des Attributs
		 * \param data Ausgabeparameter fuer die Daten
		 * \param def Zahl, die ausgegeben wird, wenn das Attribut nicht existiert oder sich nicht zu int konvertieren laesst
		 */
		void getInt(std::string attrib, int& data, int def =0);
		
		/**
		 * \fn void getShort(std::string attrib, short& data, short def =0)
		 * \brief Gibt ein Attribut als Short aus.
		 * \param attrib Name des Attributs
		 * \param data Ausgabeparameter fuer die Daten
		 * \param def Zahl, die ausgegeben wird, wenn das Attribut nicht existiert oder sich nicht zu int konvertieren laesst
		 */
		void getShort(std::string attrib, short& data, short def =0);
	
	
		/**
		 * \fn void getInt(std::string attrib, int& data, int def =0)
		 * \brief Gibt ein Attribut als Float aus.
		 * \param attrib Name des Attributs
		 * \param data Ausgabeparameter fuer die Daten
		 * \param def Zahl, die ausgegeben wird, wenn das Attribut nicht existiert oder sich nicht zu float konvertieren laesst
		 */
		void getFloat(std::string attrib, float& data, float def =0);
		
		/**
		 * \fn void getBool(std::string attrib, bool& data, bool def =false)
		 * \brief Gibt ein Attribut als bool aus.
		 * \param attrib Name des Attributs
		 * \param data Ausgabeparameter fuer die Daten
		 * \param def Resultat das ausgegeben wird, wenn das Attribut nicht existiert oder sich nicht zu bool konvertieren laesst
		 */
		void getBool(std::string attrib, bool& data, bool def =false);
		
	private:
		/**
		 * \var std::map<std::string, TiXmlAttribute*> m_attribs
		 * \brief Map zum zwischenspeichern der Attribute
		 */
		std::map<std::string, TiXmlAttribute*> m_attribs;
};

#endif // __SUMWARS_CORE_ELEMENTATTRIB_H__
