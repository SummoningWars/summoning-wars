#include "tinyxml.h"
#include <string>
#include <map>
#include <list>
#include <stack>
#include <iostream>
#include <dirent.h>
#include <fstream>
#include <sys/stat.h>
#include <set>

using namespace std;

class ElementAttrib
{
	public:
	
	std::map<std::string, TiXmlAttribute*> m_attribs;
	
	
	void parseElement(TiXmlElement* elem, std::list<std::string> &elem_strings)
	{
		//std::cout << "parseElement" << std::endl;
		if (elem == 0)
			return;
		
		m_attribs.clear();
		
		TiXmlAttribute* pAttrib = elem->FirstAttribute();
		
		while (pAttrib != 0)
		{
			//std::cout << "[std::string(pAttrib->Name())] " << std::string(pAttrib->Name()) << " | [pAttrib] " << pAttrib << std::endl;
			m_attribs[std::string(pAttrib->Name())] = pAttrib;
			pAttrib=pAttrib->Next();
		}
		
		elem_strings.clear();
		std::string temp_string = "";
		
		if( elem->GetText() )
		{
			const char* text = elem->GetText();
			
			int i=0;
			while( text[i] )
			{
				if( text[i]=='_' && text[i+1]=='(' && (text[i+2]=='"' || text[i+2]==39))
				{
					i=i+3;
					//std::cout << "BEGIN_GETTEXT";
				
					while( text[i] )
					{
						if( (text[i]=='"' || text[i]==39) && (text[i+1]==')' || text[i+1]==',') )
						{
							i=i+2;
							break;
						}
						else
						{
							//std::cout << text[i++];
							temp_string = temp_string + text[i++];
						}
					}
					
					//std::cout << temp_string;
					elem_strings.push_back(temp_string);
					temp_string = "";
					//std::cout << "END_GETTEXT";
				}
				//std::cout << text[i];
				i++;
			}
		}
	}
	
	void getString(std::string attrib, std::string& data, std::string def = "")
	{
		//std::cout << "getString" << std::endl;
		std::map<std::string, TiXmlAttribute*>::iterator it;
		it = m_attribs.find(attrib);
		
		if (it == m_attribs.end())
		{
			data = def;
			return;
		}
		
		//std::cout << "[attrib] " << attrib << " | [it->second->Value()] " << it->second->Value() << std::endl;
		data = it->second->Value();
	}
	
	
	/*void getInt(std::string attrib, int& data, int def =0)
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
	
	
	void getFloat(std::string attrib, float& data, float def =0)
	{
		map<string, TiXmlAttribute*>::iterator it;
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
	}*/
	
};

// Strings die uebersetzt werden
/*std::set<std::string> tr_strings;

void addString(std::string& str,ofstream& stream, char*filename)
{
	if (tr_strings.count(str) > 0)
		return;
	
	tr_strings.insert(str);
	if (str != "")
	{
		//std::cout <<"    "<<str << "\n";
		stream << "#: "<<filename<<"\n";
		stream << "msgid \""<<str<<"\"\n";
		stream << "msgstr \"\"\n\n";
	}
}*/

void parseXML( TiXmlNode* node, char* filename, std::map<std::string, std::string> &strings, std::stack<std::string> &domains,
	std::map<std::string, std::set<std::string> >& foundstrings)
{
	std::list<std::string> elem_strings;
	std::list<std::string> str;
	std::string str_tmp;
	std::string domain;
	TiXmlNode* child;
	
	TiXmlElement* elem = node->ToElement();
	if (elem !=0)
	{
		ElementAttrib attrib;
		
		//std::cout << node->Value() << std::endl;
		
		if (!strcmp(node->Value(), "Item") ||
					!strcmp(node->Value(), "Monster") ||
					!strcmp(node->Value(), "Quest") ||
					!strcmp(node->Value(), "Region") ||
					!strcmp(node->Value(), "PlayerClass"))
		{
			attrib.parseElement(elem, elem_strings);
			attrib.getString("name",str_tmp);
			str.push_back(str_tmp);
		}
		
		if (!strcmp(node->Value(), "Ability"))
		{
			attrib.parseElement(elem, elem_strings);
			attrib.getString("name",str_tmp);
			str.push_back(str_tmp);
			attrib.getString("description",str_tmp);
			str.push_back(str_tmp);
		}
		
		if (!strcmp(node->Value(), "NPC"))
		{
			attrib.parseElement(elem, elem_strings);
			attrib.getString("refname",str_tmp);
			str.push_back(str_tmp);
		}
		
		if (!strcmp(node->Value(), "Topic"))
		{
			attrib.parseElement(elem, elem_strings);
			attrib.getString("start_option",str_tmp);
			str.push_back(str_tmp);
		}
		
		if (!strcmp(node->Value(), "Effect") ||
					!strcmp(node->Value(), "Description"))
		{
			attrib.parseElement(elem, elem_strings);
		}
		
		
		// check if domain is declared as an attribute (no if, so the check is done always, indenpendent from element-name)
		attrib.getString("domain",domain);
		//std::cout << "DECLARED DOMAIN " << domain << std::endl;
		if (domain != "")
		{
			domains.push(domain);
			//std::cout << "PUSH DOMAIN " << domain << std::endl;
		}
		
		// add strings to map
		for (std::list<std::string>::iterator it = str.begin(); it != str.end(); ++it)
		{
			if ((*it) != "")
			{
				size_t pos=0;
				while ((pos = (*it).find("\"",pos)) != std::string::npos)
				{
					(*it) = (*it).replace(pos,1,"\\\"");
					std::cout << pos << (*it) << "\n";
					pos +=2;
				}
				pos=0;
				while ((pos = (*it).find("\\\\\"",pos)) != std::string::npos)
				{
					(*it) = (*it).replace(pos,3,"\\\"");
					std::cout << pos << (*it) << "\n";
					pos +=2;
				}
				if (foundstrings[domains.top()].count((*it)) == 0)
				{
					strings[domains.top()] += std::string("#: ") + filename + "\n" + "msgid \"" + (*it) + "\"\n" + "msgstr \"\"\n\n";
					foundstrings[domains.top()].insert((*it));
				}
			}
		}
		for (std::list<std::string>::iterator it = elem_strings.begin(); it != elem_strings.end(); ++it)
		{
			size_t pos=0;
			while ((pos = (*it).find("\"",pos)) != std::string::npos)
			{
				(*it) = (*it).replace(pos,1,"\\\"");
				std::cout << pos << *it << "\n";
				pos += 2;
			}
			pos=0;
			while ((pos = (*it).find("\\\\\"",pos)) != std::string::npos)
			{
				(*it) = (*it).replace(pos,3,"\\\"");
				std::cout << pos << *it << "\n";
				pos += 2;
			}
			if (foundstrings[domains.top()].count(*it) == 0)
			{
				strings[domains.top()] += std::string("#: ") + filename + "\n" + "msgid \"" + *it + "\"\n" + "msgstr \"\"\n\n";
				foundstrings[domains.top()].insert(*it);
			}
		}
	}
	
	for ( child = node->FirstChild(); child != 0; child = child->NextSibling())
	{
		parseXML(child, filename, strings, domains, foundstrings);
	}
	if (domain != "")
		{
			//std::cout << "POP DOMAIN " << domain << endl;
			domains.pop();
		}
}


void parseDirectory( char* path, std::map<std::string, std::string> &strings, std::stack<std::string> &domains, 
		     std::map<std::string, std::set<std::string> >& foundstrings )
{

	char apath[1024];
	DIR *dir;
	struct dirent *entry;
	struct stat attribut;
	//int result;

	if( ( dir = opendir( path ) ) != NULL ) 
	{
		while( ( entry = readdir( dir ) ) != NULL ) 
		{
			memset( apath, '\0', sizeof( apath ) );
			strcpy( apath, path );
			strcat( apath, "/" );
			strcat( apath, (char*)entry->d_name );

			if( entry->d_name[0]!='.' )	
			{
				//printf( "%s\n", apath );
			}
			else
			{
				continue;
			}
			
			int ok = stat( apath, &attribut );
			if( ok == -1 )
			{
				printf( "stat(%s) failed\n", apath );
				continue;
			}
			
			if( ( attribut.st_mode & S_IFDIR ) )
			{
				parseDirectory( apath, strings, domains,foundstrings );
			}
			
			if (attribut.st_mode & S_IFREG )
			{
				TiXmlDocument doc(apath);
				bool loadOkay = doc.LoadFile();
				if (loadOkay)
				{
					parseXML(&doc, (char*)entry->d_name, strings, domains, foundstrings);
				}
			}
		}
	}
	else
		printf("opendir(%s) failed\n", path);
	closedir(dir);
}


int main(int argc, char* argv[])
{
	std::map<std::string, std::string> strings;
	std::stack<std::string> domains;
	std::map<std::string, std::set<std::string> > foundstrings;
	// if no domain is defined, the standard domainname is sumwars
	domains.push("sumwars");
	
	if (argc > 1)
	{
		parseDirectory(argv[1], strings, domains, foundstrings);
	}
	else
	{
		parseDirectory(".", strings, domains, foundstrings);
	}
	
	
	for (std::map<std::string, std::string>::iterator it = strings.begin(); it != strings.end(); ++it)
    {
		//path = <domainname>.pot
		std::string path = "";
		path = it->first + ".pot";
		
		std::cout << "write " << path << endl;
		
		std::ofstream stream(path.c_str());
		stream << "# SOME DESCRIPTIVE TITLE\n\
# Copyright (C) YEAR THE PACKAGE'S COPYRIGHT HOLDER\n\
# This file is distributed under the same license as the PACKAGE package\n\
# FIRST AUTHOR <EMAIL@ADDRESS>, YEAR\n\
#\n\
#, fuzzy\n\
msgid \"\"\n\
msgstr \"\"\n\
\"Project-Id-Version: PACKAGE VERSION\\n\"\
\"Report-Msgid-Bugs-To: \\n\"\n\
\"POT-Creation-Date: 2009-03-09 15:15+0100\\n\"\n\
\"PO-Revision-Date: YEAR-MO-DA HO:MI+ZONE\\n\"\n\
\"Last-Translator: FULL NAME <EMAIL@ADDRESS>\\n\"\n\
\"Language-Team: LANGUAGE <LL@li.org>\\n\"\n\
\"MIME-Version: 1.0\\n\"\n\
\"Content-Type: text/plain; charset=UTF-8\\n\"\n\
\"Content-Transfer-Encoding: 8bit\\n\"\n\n" + it->second;
		//std::cout << it->first << it->first << it->first << it->first << it->first << it->first << std::endl << std::endl;
		//std::cout << it->second << std::endl;
    }
	
	return 0;
}

