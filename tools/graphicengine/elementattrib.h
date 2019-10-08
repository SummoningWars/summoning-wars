#ifndef ELEMENTATTRIB_H
#define ELEMENTATTRIB_H

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
	
	private:
		/**
		 * \var std::map<std::string, TiXmlAttribute*> m_attribs
		 * \brief Map zum zwischenspeichern der Attribute
		 */
		std::map<std::string, TiXmlAttribute*> m_attribs;
};

#endif
