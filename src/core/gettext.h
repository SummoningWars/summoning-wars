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

#ifndef GETTEXT_H
#define GETTEXT_H


#ifdef gettext
    #undef gettext
#endif
#include <libintl.h>


#include <locale.h>
#include <string>
#include <map>

/**
 * \class Gettext
 * \brief Hilfsklasse fuer die Internationalisierung
 */
class Gettext
{
	public:
		/**
		 * \fn static void init(const char* locale)
		 * \brief initializes Gettext
		 * \param locale Language
		 * \param pathToTranslationFiles Path to the translation files
		 */
		static void init(const char* locale, std::string pathToTranslationFiles);

		/**
		 * \fn static const char* getLocale()
		 * \brief Gibt die aktuelle Sprache aus
		 */
		static const char* getLocale();

		/**
		 * \fn static void setLocale(const char* locale)
		 * \brief Setzt die aktuelle Sprache
		 * \param locale Sprache
		 */
		static void setLocale(const char* locale);

		/**
		 * \fn static bool getLocaleChanged()
		 * \brief Gibt aus, ob die Sprache geaendert wurde. Das Bit wird dabei automatisch auf false zurueckgesetzt
		 */
		static bool getLocaleChanged();

#ifdef WIN32
		/**
		 * \brief Specifig Windows locale setting function.
		 */
		static bool setWinThreadLocale (const std::string& newLocale);

		/**
		 * \brief Windows specific language code retrieval.
		 * \param languageString the short language description, without locale (e.g. "de", "en", "it")
		 * \note Make sure that the function preInit is called before this function, so the proper mappings are set up.
		 * \return The language codes, like 0x0407 (German)
		 */
		static int getLanguageCodeFromString (const std::string& languageString);

#endif

	private:

		/**
		 * \brief Perform any OS specific pre-initialization steps.
		 */
		static void preInit ();

		/**
		 * \var std::string m_locale
		 * \brief aktuelle Sprache
		 */
		static std::string m_locale;

		/**
		 * \var static bool m_changed
		 * \brief Auf true gesetzt, wenn die Sprache gerade geaendert wurde (bis zum ersten Aufruf von getLocalChanged)
		 */
		static bool m_changed;

#ifdef WIN32
		static std::map <std::string, int> winLanguageMappings_;
#endif
};
#endif

