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

#include "translatablestring.h"
#include "debug.h"

TranslatableString::TranslatableString()
{
	m_text = "";
	m_domain = "";
	m_locale = "";
	m_translation = "";
}

void TranslatableString::setText(std::string text, std::vector<std::string> substrings)
{
	m_text = text;
	m_substrings = substrings;
	m_is_translated = false;
}

void TranslatableString::setTextDomain(std::string domain)
{
	m_domain = domain;
	m_is_translated = false;
}

void TranslatableString::operator=(std::string text)
{
	m_text = text;
	m_is_translated = false;
}

void TranslatableString::operator=(const TranslatableString& other)
{
	m_text = other.m_text;
	m_substrings = other.m_substrings;
	m_translation = other.m_translation;
	m_domain = other.m_domain;
	m_locale = other.m_locale;
	m_is_translated = other.m_is_translated;
}

const std::string& TranslatableString::getTranslation()
{
	if (m_domain == "" || m_text == "")
		return m_text;
	
	// Only translate text, if not translated yet or locale changed
	bool retranslate = !m_is_translated;
	if (!retranslate && Gettext::getLocale() != m_locale)
		retranslate = true;
	
	if (retranslate)
	{
		m_translation = dgettext(m_domain.c_str(),m_text.c_str());
		
		// Now replace $1 to $9 and $$
		size_t pos = m_translation.find('$');
		while (pos != std::string::npos)
		{
			if (pos+1 < m_translation.size())
			{
				std::string replace;
				char next = m_translation.at(pos+1);
				if (next == '$')
				{
					replace = "$";
				}
				else if (next >= '1' && next <='9' && m_substrings.size() > (next - '1'))
				{
					replace = m_substrings[next-'1'];
				}
				
				m_translation.replace(pos,2,replace);
				pos += replace.size();
			}
			
			pos = m_translation.find('$',pos+1);
		}
		
		m_is_translated = true;
		m_locale = Gettext::getLocale();
	}
	
	return m_translation;
}

void TranslatableString::toString(CharConv* cv)
{
	cv->toBuffer(m_text);
	cv->toBuffer(m_domain);
	cv->toBuffer(static_cast<int>(m_substrings.size()));
	for (int i=0; i<m_substrings.size(); i++)
	{
		cv->toBuffer(m_substrings[i]);
	}
}

void TranslatableString::fromString(CharConv* cv)
{
	cv->fromBuffer(m_text);
	cv->fromBuffer(m_domain);
	int size;
	cv->fromBuffer(size);
	m_substrings.resize(size);
	for (size_t i=0; i<m_substrings.size(); i++)
	{
		cv->fromBuffer(m_substrings[i]);
	}
	m_is_translated = false;
}


int TranslatableString::size()
{
	int ret = m_text.size();
	for (std::vector<std::string>::iterator it = m_substrings.begin(); it != m_substrings.end(); ++it)
	{
		ret += it->size() -2;
	}
	return ret;
}