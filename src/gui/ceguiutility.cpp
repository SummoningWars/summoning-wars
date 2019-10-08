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

#include "ceguiutility.h"
#include "CEGUI/CEGUI.h"
#include <iostream>
#include <algorithm>
#include "debug.h"

std::string CEGUIUtility::stripColours(const std::string &input)
{
    std::string output = input;

    std::string::size_type pos = output.find("[");
    while (pos != std::string::npos)
    {
        output.erase(pos,pos+19);
        pos = output.find("[");
    }

    return output;
}

std::list< std::string > CEGUIUtility::getTextAsList(const std::string &text)
{
    std::list<std::string> l;
    std::string temp = text;
	
    std::string::size_type pos = text.find_first_of(LINE_ENDING);
    while (pos != std::string::npos)
    {
        // ignore new line directly at the beginning of the string
        if (pos != 0)
        {
            l.push_back(temp.substr(0, pos));
            temp.erase(0, pos+ERASE_CNT);
            pos = temp.find(LINE_ENDING);
        }
        else
		{
			temp.erase(0, ERASE_CNT);
			//l.push_back(" ");
			pos = temp.find(LINE_ENDING);
		}
    }
    
    if (temp != "")
	{
		l.push_back(temp);
	}
	
    return l;
}

CEGUI::UVector2 CEGUIUtility::getWindowSizeForText(std::list<std::string> list, CEGUI::Font *font, std::string &added)
{
    using namespace CEGUI;

    float textWidth = 0.0f;
    float textHeight = 0.0f;
    CEGUI::Size screenSize = CEGUI::System::getSingleton().getRenderer()->getDisplaySize();

    added = "";
	size_t count = 0;
	
    std::list<std::string>::iterator iter = list.begin();
    while (iter != list.end())
    {
		std::string s = *iter;
		
		if(s.size() == 0)
		{
			iter++;
			continue;
		}
		std::string::size_type word_pos = 0;
		while ( word_pos!=std::string::npos )
		{
			word_pos = s.find ( LINE_ENDING, word_pos );
			if ( word_pos != std::string::npos )
			{
				++count;

				// start next search after this word
				word_pos += 2;
			}
		 }
		
		
		added += s + "\n";

        float tempwidth = font->getTextExtent(stripColours(s).c_str()) ;
        if (tempwidth > textWidth)
            textWidth = tempwidth;

        iter++;
    }
	size_t size = list.size();

	// increase the counter intil we have at least one line break more than we have text
	while(count <= size)  
		count++;

	textHeight = count * (font->getLineSpacing());

    return UVector2(UDim((textWidth/screenSize.d_width) + 0.05f, 0), UDim(textHeight / screenSize.d_height, 0) );
}

CEGUI::UVector2 CEGUIUtility::getWindowSizeForText(std::string text, CEGUI::Font* font)
{
    std::list<std::string> l = getTextAsList(text);
    std::string s;
    return getWindowSizeForText(l, font, s);
}

std::string CEGUIUtility::getStdColourAsString(int col)
{
	switch(col)
	{
		case Red:
			return "[colour='FFFF0000']";
		case Green:
			return "[colour='FF00FF00']";
		case Blue:
			return "[colour='FF0000FF']";
		case White:
			return "[colour='FFFFFFFF']";
		case Black:
			return "[colour='FF000000']";
		default:
			return "";
	}		
}

std::string CEGUIUtility::getColourizedString(int colour, std::string text, int afterColour)
{
	if(!afterColour > 0)
		return getStdColourAsString(colour) + text;
	else
		return getStdColourAsString(colour) + text + getStdColourAsString(afterColour);
}

const size_t CEGUIUtility::getNextWord(const CEGUI::String& in_string, size_t start_idx, CEGUI::String& out_string) 
{
	out_string = CEGUI::TextUtils::getNextWord(in_string, start_idx, CEGUI::TextUtils::DefaultWrapDelimiters);
	
	return out_string.length();
}

FormatedText CEGUIUtility::fitTextToWindow(const CEGUI::String& text, float maxWidth, TextFormatting fmt, CEGUI::Font *font, float x_scale)
{
/*	if ((fmt == LeftAligned) || (fmt == Centred) || (fmt == RightAligned) || (fmt == Justified))
	{
		return std::count(text.begin(), text.end(), static_cast<CEGUI::utf8>('\n')) + 1;
	}
	*/

	CEGUI::String newText((CEGUI::utf8*)text.c_str());
	
	// handle wraping cases
	size_t lineStart = 0, lineEnd = 0;
	CEGUI::String	sourceLine;

	CEGUI::String  whitespace = CEGUI::TextUtils::DefaultWhitespace;
	CEGUI::String	thisLine, thisWord;
	size_t	line_count = 0, currpos = 0;
	
	size_t fullPosCtr = 0;
	
	while (lineEnd < text.length())
	{
		if ((lineEnd = text.find_first_of(LINE_ENDING, lineStart)) == CEGUI::String::npos)
		{
			lineEnd = text.length();
		}
		
		sourceLine = text.substr(lineStart, lineEnd - lineStart);
		lineStart = lineEnd + 1;
		
		// get first word.
		currpos = getNextWord(sourceLine, 0, thisLine);
		
		// while there are words left in the string...
		while (CEGUI::String::npos != sourceLine.find_first_not_of(whitespace, currpos))
		{
			// get next word of the string...
			currpos += getNextWord(sourceLine, currpos, thisWord);
			
			float davor = (font->getTextExtent(thisLine, x_scale));
			float danach = (font->getTextExtent(thisLine, x_scale) + font->getTextExtent(thisWord, x_scale));
			
			// if the new word would make the string too long
			if ((font->getTextExtent(thisLine, x_scale) + font->getTextExtent(thisWord, x_scale)) > maxWidth)
			{
				// too long, so that's another line of text
				fullPosCtr += thisLine.size();
				line_count++;
				
				// remove whitespace from next word - it will form start of next line
				thisWord = thisWord.substr(thisWord.find_first_not_of(whitespace));
				
				int sz = thisWord.size();
				newText.insert(fullPosCtr+1, LINE_ENDING);
				fullPosCtr += 2;
				
				// reset for a new line.
				thisLine.clear();
			}
			
			// add the next word to the line
			thisLine += thisWord;
		}
		
		fullPosCtr += thisLine.size();
		// plus one for final line
		line_count++;
	}
	FormatedText formt;
	formt.text = newText;
	formt.lines = line_count;
	return formt;
}


