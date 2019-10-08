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

#ifndef CLIPBOARD_H
#define CLIPBOARD_H

// The used singleton manager.
#include "OgreSingleton.h"

// CEGUI includes
#include "CEGUI/CEGUI.h"

#include <string>

/**
 * \brief Summoning Wars Utility namespace.
 */
namespace SWUtil
{
	/**
	 * \brief Class to interract with the OS clipboard.
	 * Initial commit: Augustin Preda, 2011.10.28... (man I hope this gets updated sometimes soom :)
	 */
	class Clipboard
		: public Ogre::Singleton <Clipboard>
	{
	public:
		/**
		 * \brief Constructor.
		 */
		Clipboard ();

		/**
		 * \brief Copy operation (from CEGUI control into clipboard).
		 * \return true if operation was successful, false otherwise.
		 */
		virtual bool copy (bool cutInsteadOfCopy = false);

		/**
		 * \brief Cut operation (from CEGUI control into clipboard).
		 * \return true if operation was successful, false otherwise.
		 */
		virtual bool cut ();

		/**
		 * \brief Paste operation (from clipboard into CEGUI control).
		 * \return true if operation was successful, false otherwise.
		 */
		virtual bool paste ();

	protected:


		/**
		 * \brief Internal operation of writing text to clipboard.
		 * This will be OS dependent, so the functionality may not be supported on all systems!
		 * 
		 * \param textToCopy The text to copy to the clipboard.
		 * \return true if operation was successful, false otherwise.
		 */
		bool _copyToClipboard (const std::string& textToCopy);

		/**
		 * \brief Internal operation of reading text from clipboard.
		 * This will be OS dependent, so the functionality may not be supported on all systems!
		 *
		 * \return string contained in clipboard, empty string if operation was unsuccessful.
		 */
		std::string _readFromClipboard () const;
	};


}
#endif //CLIPBOARD_H
