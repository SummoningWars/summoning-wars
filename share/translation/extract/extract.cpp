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
 * 
 * Tool for extracting translateable strings from Summoning Wars content files.
 * Copyright 2012 Michael 'Protogenes' Kunz
 * 
 * extract <prefix> [input files]
 * <prefix>: path that will prefix the destination files. Append / to place files in a folder!
 * [input files]: list of files that shall be processed, if missing the list is read from stdin.
 * 
 */


#include <cctype>
#include <functional>
#include <fstream>
#include <iostream>
#include <stack>
#include <string>
#include <sstream>
#include <Poco/DirectoryIterator.h>
#include <Poco/FileStream.h>
#include <Poco/NumberFormatter.h>
#include <Poco/Path.h>
#include <Poco/StreamCopier.h>
#include <Poco/SAX/Attributes.h>
#include <Poco/SAX/ContentHandler.h>
#include <Poco/SAX/InputSource.h>
#include <Poco/SAX/SAXParser.h>

const std::string cDomain = "sumwars";
const std::string cHeader = ("\
#, fuzzy\n\
msgid \"\"\n\
msgstr \"\"\n\
\"MIME-Version: 1.0\\n\"\n\
\"Content-Type: text/plain; charset=UTF-8\\n\"\n\
\"Content-Transfer-Encoding: 8bit\\n\"\n\
\"Project-Id-Version: PACKAGE VERSION\\n\"\n\
\"Last-Translator: Automatically generated\\n\"\n\
\"Language-Team: none\\n\"\n\
\"Language: \\n\"\n\
\"Plural-Forms: nplurals=2; plural=(n != 1);\\n\"\n\n\n\
");

typedef std::map<std::string, std::map<std::string, std::vector<std::string> > > TStrings; // domain -> strings -> locations

void addStrings(const std::string & Dom, std::string Str, const std::string & Loc, TStrings & Strings)
{
	if (!Str.empty())
	{
		for (auto i = Str.begin(); i != Str.end(); ++i)
		{ // escape characters
			if (*i == '"' && (i == Str.begin() || *(i-1) != '\\'))
				i = Str.insert(i, '\\');
		}

		Strings[Dom][Str].push_back(Loc);
	}
}

void handleLua(const std::string & Dom, std::istream & Str, size_t Streamsize, const std::string & Filename, size_t Line, TStrings & Strings)
{
	enum state_t {outside, underscore, parenthesis, inside} state = outside; // doesn't check for closing parenthesis
	char marks = 0;
	std::string buf;
	int i, l = 0;
	buf.reserve(Streamsize);
	
	for (i = Str.get(); Str.good(); l = i, i = Str.get())
	{
		if (state == inside || !isspace(i))
		{
			if (state == outside && i == '_')
				state = underscore;
			else if (state == underscore)
			{
				if ( i == '(')
				{
					state = parenthesis;
				}
				else
				{
					state = outside;
				}
			}
			else if (state == parenthesis && (i == '"' || i == '\''))
			{
				marks = i;
				state = inside;
			} else if (state == inside)
			{
				if (i == marks && l != '\\')
				{
					addStrings(Dom, buf, Filename + ':' + Poco::NumberFormatter::format(Line), Strings);
					state = outside;
					buf.clear();
				} else {

					switch (i)
					{ // unescape characters
						case '\'':  if (marks == '"' && l == '\\') *buf.rbegin() = '"'; else buf.push_back(i); break;
						case '"':   if (marks == '\'' && l == '\\') *buf.rbegin() = '\''; else buf.push_back(i); break;
						default: buf.push_back(i); break;
					}
				}
			}
		}
		if (i == '\n')
			Line++;
	}
}

class CXMLHandler : public Poco::XML::ContentHandler
{
	public:
		CXMLHandler(const std::string & Path, TStrings & Strings)
			:	m_Path(Path),
				m_Strings(Strings)
		{	
			TStack s;
			s.domain = cDomain;
			s.line = 0;
			m_Stack.push(s);
		};

		void startElement(const Poco::XML::XMLString & uri, const Poco::XML::XMLString & localName, const Poco::XML::XMLString & qname, const Poco::XML::Attributes & attrList)
		{
			TStack s;
			s.line = m_Locator?m_Locator->getLineNumber():0;
			if (attrList.getValue("", "domain").empty())
				s.domain = m_Stack.top().domain;
			else
				s.domain = attrList.getValue("", "domain");
			m_Stack.push(s);
			
			if (localName == "Item"
				|| localName == "Monster"
				|| localName == "Quest"
				|| localName == "Region"
				|| localName == "PlayerClass"
				|| localName == "Ability"
				|| localName == "Look")
			{
				addStrings(s.domain, attrList.getValue("", "name"), getLoc(), m_Strings);
			}
			if (localName == "Ability")
			{
				addStrings(s.domain, attrList.getValue("", "description"), getLoc(), m_Strings);
			}
			if (localName == "NPC")
			{
				addStrings(s.domain, attrList.getValue("", "refname"), getLoc(), m_Strings);
			}
			if (localName == "Topic")
			{
				addStrings(s.domain, attrList.getValue("", "start_option"), getLoc(), m_Strings);
			}
			if (localName == "SkilltreeTabs")
			{
				addStrings(s.domain, attrList.getValue("", "tab1"), getLoc(), m_Strings);
				addStrings(s.domain, attrList.getValue("", "tab2"), getLoc(), m_Strings);
				addStrings(s.domain, attrList.getValue("", "tab3"), getLoc(), m_Strings);				
			}
		};

		void endElement(const Poco::XML::XMLString & uri, const Poco::XML::XMLString & localName, const Poco::XML::XMLString & qname)
		{
			auto & s = m_Stack.top();
			if (!s.content.empty())
			{
				std::stringstream tmp(s.content);
				handleLua(s.domain, tmp, s.content.size(), m_Path, s.line, m_Strings);
			}
			m_Stack.pop();
		};
		void characters(const Poco::XML::XMLChar ch[], int start, int length)
		{
			m_Stack.top().content.append(ch+start, length);
		};
		void ignorableWhitespace(const Poco::XML::XMLChar ch[], int start, int length)
		{
			m_Stack.top().content.append(ch+start, length);
		};

		virtual void processingInstruction(const Poco::XML::XMLString& target, const Poco::XML::XMLString& data)
		{ };
		virtual void startPrefixMapping(const Poco::XML::XMLString& prefix, const Poco::XML::XMLString& uri)
		{ };
		virtual void endPrefixMapping(const Poco::XML::XMLString& prefix)
		{ };
		virtual void skippedEntity(const Poco::XML::XMLString& name)
		{ };
		virtual void startDocument()
		{ };
		virtual void endDocument()
		{ };
		virtual void setDocumentLocator(const Poco::XML::Locator* loc)
		{ m_Locator = loc; };
	private:
		const Poco::XML::Locator * m_Locator;
		std::string m_Path;
		TStrings & m_Strings;
		
		typedef struct TStack {
			std::string content;
			size_t line;
			std::string domain;
		} TStack;
		std::stack<TStack> m_Stack;
		
		std::string getLoc()
		{
			if (m_Locator)
				return m_Path + ':' + Poco::NumberFormatter::format(m_Locator->getLineNumber());
			else
				return m_Path;
		}
};

void handleXML(const Poco::Path & Path, TStrings & Strings)
{
	std::stringstream encapsulate;
	{
		Poco::FileInputStream file(Path.toString());
		std::string s;
		std::getline(file, s);
		if (s.find("<?") != std::string::npos)
			encapsulate << s << "<ROOT__ELEM>";
		else
			encapsulate << "<ROOT__ELEM>\n" << s;
		
		Poco::StreamCopier::copyStream(file, encapsulate);
		encapsulate << "</ROOT__ELEM>";
	}
	Poco::XML::InputSource source(encapsulate);
	CXMLHandler handler(Path.getFileName(), Strings);
	Poco::XML::SAXParser parser;
	parser.setContentHandler(&handler);
	parser.parse(&source);
}

void handleLuaFile(const Poco::Path & Path, TStrings & Strings)
{
	Poco::FileInputStream s(Path.toString());
	handleLua(cDomain, s, Poco::File(Path).getSize(), Path.getFileName(), 0, Strings);
}

void handleFile(const Poco::Path & Path, TStrings & Strings)
{
	try {
		size_t oldcount = 0, count = 0;
		std::for_each(Strings.begin(), Strings.end(), [&](const TStrings::value_type & v){ oldcount += v.second.size(); });
		
		std::clog << std::endl << Path.toString();
		if (Path.getExtension() == "xml")
			handleXML(Path, Strings);
		else if (Path.getExtension() == "lua")
			handleLuaFile(Path, Strings);
		else
			std::clog << " unknown format.";
		
			
		std::for_each(Strings.begin(), Strings.end(), [&](const TStrings::value_type & v){ count += v.second.size(); });
		std::clog << ' ' << (count-oldcount) << " strings added.";
	} catch (Poco::Exception & e)
	{
		std::cerr << std::endl << Path.toString() << " " << e.what() << ": " << e.message();
	} catch (std::exception & e)
	{
		std::cerr << std::endl << Path.toString() << " " << e.what();
	}
}

void handlePath(const Poco::Path & Path, TStrings & Strings)
{
	if (Poco::File(Path).isDirectory())
	{
		Poco::DirectoryIterator e;
		for (auto i = Poco::DirectoryIterator(Path); i != e; ++i)
		{
			handlePath(i->path(), Strings);
		}

	} else {
		handleFile(Path, Strings);
	}
}

int main(int argc, char ** argv)
{
	if (argc < 2)
	{
		std::cerr << argv[0] << " needs first argument for output directory!" << std::endl;
		return -1;
	}

	TStrings str;
	if (argc > 2)
	{
		for (int i = 2; i < argc; ++i)
			handlePath(argv[i], str);
		
	} else {
		std::string s;
		do {
			std::getline(std::cin, s);
			handlePath(s, str);
		} while (!s.empty());
	}

	std::clog << std::endl;
	auto de = str.cend();
	for (auto d = str.cbegin(); d != de; ++d)
	{
		std::string fn(argv[1] + d->first + ".poe");
		bool doheader = !Poco::File(fn).exists();
		
		std::clog << "Writing to " << fn << '\n';
		Poco::FileOutputStream fo(argv[1] + d->first + ".poe", std::ios::out | std::ios::app); // append to the files
		if (doheader)
			fo << cHeader;
			
		auto se = d->second.cend();
		for (auto s = d->second.cbegin(); s != se; ++s)
		{
			auto le = s->second.cend();
			for (auto l = s->second.cbegin(); l != le; ++l)
			{
				fo << "#: " << *l << '\n';
			}
			fo << "msgid  \"" << s->first << "\"\n";
			fo << "msgstr \"\"\n\n";
		}
	}

	return 0;
}
