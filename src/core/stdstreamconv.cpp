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

#include "stdstreamconv.h"



void StdStreamConv::toBuffer(const std::string& s)
{
	// empty string and strings with spaces must be modified to be able to retrieve correctly
	// empty string -> "#empty#"
	// space -> "#~#"
	// Lets hope nobody tries to send a string with #~# inside
	if (s.empty())
	{
		(*m_stream) << "#empty# ";
	}
	else
	{
		std::string cpy(s);
		int pos = cpy.find(' ');
		while (pos != std::string::npos)
		{
			cpy.replace(pos,1,"#~#");
			pos = cpy.find(' ',pos+1);
		}
		(*m_stream) << cpy << " ";
	}
}

void StdStreamConv::fromBuffer(std::string& s)
{
	(*m_stream) >> s ;
	if (s == "#empty#")
	{
		s.clear();
	}
		
	int pos = s.find("#~#");
	while (pos != std::string::npos)
	{
		s.replace(pos,3," ");
		pos = s.find("#~#",pos+1);
	}

}

