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

#ifndef __SUMWARS_CORE_TEMPLATELOADER_H__
#define __SUMWARS_CORE_TEMPLATELOADER_H__


#include <map>
#include <list>
#include "region.h"
#include "objectfactory.h"

#include <tinyxml.h>
#include <string>
#include <cstring>



/**
 * \class TemplateLoader
 * \brief Laedt verschiedene Templates aus einer XML-Datei
 */
class TemplateLoader
{
	
	public:
		/**
	 * \fn static bool loadObjectTemplateData(const char* pFilename)
		 * \brief Liest Daten zu Objekt Templates aus der angegebenen Datei
		 * \parampFilename Name der XML Datei
		 */
		static bool loadObjectTemplateData(const char* pFilename);
		
		/**
		 * \fn static bool loadObjectTemplate(TiXmlNode* node)
		 * \brief Liest die Daten zu einem Objekt Template aus dem Knoten
		 * \param node XML Knoten
		 */
		static bool loadObjectTemplate(TiXmlNode* node);
		
		/**
		 * \fn static bool loadObjectGroupData(const char* pFilename)
		 * \brief Liest Daten zu Objektgruppen Templates aus der angegebenen Datei
		 * \parampFilename Name der XML Datei
		 */
		static bool loadObjectGroupData(const char* pFilename);

		
		/**
		 * \fn static bool loadObjectGroup(TiXmlNode* node)
		 * \brief Liest die Daten zu einem Objektgruppen Template aus dem Knoten
		 * \param node XML Knoten
		 */
		static bool loadObjectGroup(TiXmlNode* node);
	
	private:
		/**
		 * \brief Name der Datei die gerade gelesen wird
		 */
		static std::string m_filename;
		

};


#endif // __SUMWARS_CORE_TEMPLATELOADER_H__

