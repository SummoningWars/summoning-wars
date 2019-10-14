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

#ifndef __SUMWARS_TOOLTIP_SETTINGS_H__
#define __SUMWARS_TOOLTIP_SETTINGS_H__

// Allow the use of the std::string class
#include <string>

// Allow the use of the std::stringstream.
#include <sstream>

/**
	This class contains generic tooltip settings (such as colours). It is used as a namespace, encapsulating related functions.
	@author Augustin Preda.
*/
class TooltipSettings
{
public:
	/**
		Get the default colour to be used in a tooltip. 
		@author Augustin Preda.
	*/
	static std::string getDefaultHexColourCode ()
	{
		std::string ret ("FFEFEFEF"); // previously "FF2F2F2F"
		return ret;
	}

	/**
		Get the default colour to be used in a tooltip, in the format that would be applied to a CEGUI text.
		@author Augustin Preda.
	*/
	static std::string getDefaultCeguiColour ()
	{
		std::stringstream ss;
		ss << "[colour='" << getDefaultHexColourCode () << "']";
		return ss.str ();
	}


	/**
		Get the magical colour to be used in a tooltip. 
		@author Augustin Preda.
	*/
	static std::string getMagicalHexColourCode ()
	{
		std::string ret ("FFA0FFA0"); // previously "FF00C000"
		return ret;
	}


	/**
		Get the magical colour to be used in a tooltip, for an item laying on the ground.
		@author Augustin Preda.
	*/
	static std::string getGroundItemMagicalHexColourCode ()
	{
		std::string ret ("FFA0A0FF"); // previously "FF00C000"
		return ret;
	}


	/**
		Get the magical colour to be used in a tooltip, in the format that would be applied to a CEGUI text.
		@author Augustin Preda.
	*/
	static std::string getMagicalCeguiColour ()
	{
		std::stringstream ss;
		ss << "[colour='" << getMagicalHexColourCode () << "']";
		return ss.str ();
	}

	/**
		Get the rare colour to be used in a tooltip. 
		@author Augustin Preda.
	*/
	static std::string getRareHexColourCode ()
	{
		std::string ret ("FF55A3FF"); // previously "FF2573D9"
		return ret;
	}

	/**
		Get the rare colour to be used in a tooltip, in the format that would be applied to a CEGUI text.
		@author Augustin Preda.
	*/
	static std::string getRareCeguiColour ()
	{
		std::stringstream ss;
		ss << "[colour='" << getRareHexColourCode () << "']";
		return ss.str ();
	}

	/**
		Get the unique colour to be used in a tooltip. 
		@author Augustin Preda.
	*/
	static std::string getUniqueHexColourCode ()
	{
		std::string ret ("FFFF5555"); // previously "FFFF0000"
		return ret;
	}

	/**
		Get the unique colour to be used in a tooltip, in the format that would be applied to a CEGUI text.
		@author Augustin Preda.
	*/
	static std::string getUniqueCeguiColour ()
	{
		std::stringstream ss;
		ss << "[colour='" << getUniqueHexColourCode () << "']";
		return ss.str ();
	}


	/**
		Get the quest colour to be used in a tooltip. 
		@author Augustin Preda.
	*/
	static std::string getQuestHexColourCode ()
	{
		std::string ret ("FFFFA655"); // previously "FFC05600"
		return ret;
	}

	/**
		Get the quest colour to be used in a tooltip, in the format that would be applied to a CEGUI text.
		@author Augustin Preda.
	*/
	static std::string getQuestCeguiColour ()
	{
		std::stringstream ss;
		ss << "[colour='" << getQuestHexColourCode () << "']";
		return ss.str ();
	}


	/**
		Get the quest colour to be used in a tooltip. 
		@author Augustin Preda.
	*/
	static std::string getLevelRequirementHexColourCode ()
	{
		std::string ret ("FFFF0000"); // previously "FFC05600"
		return ret;
	}

	/**
		Get the quest colour to be used in a tooltip, in the format that would be applied to a CEGUI text.
		@author Augustin Preda.
	*/
	static std::string getLevelRequirementCeguiColour ()
	{
		std::stringstream ss;
		ss << "[colour='" << getLevelRequirementHexColourCode () << "']";
		return ss.str ();
	}

};

#endif // __SUMWARS_TOOLTIP_SETTINGS_H__