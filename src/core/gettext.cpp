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

#include "gettext.h"
#include "debug.h"

#ifdef WIN32
	#define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#endif

std::string Gettext::m_locale;

#ifdef WIN32
std::map <std::string, int> Gettext::winLanguageMappings_;
#endif

bool Gettext::m_changed;

void Gettext::init(const char* locale, std::string pathToTranslationFiles)
{
	preInit ();

	bindtextdomain ("defend_dwarfenwall",pathToTranslationFiles.c_str());
	bind_textdomain_codeset ("defend_dwarfenwall","UTF-8");

	bindtextdomain ("fortify_dwarfenwall",pathToTranslationFiles.c_str());
	bind_textdomain_codeset ("fortify_dwarfenwall","UTF-8");

	bindtextdomain ("goblin_raid",pathToTranslationFiles.c_str());	
	bind_textdomain_codeset ("goblin_raid","UTF-8");

	bindtextdomain ("herb_collector",pathToTranslationFiles.c_str());
	bind_textdomain_codeset ("herb_collector","UTF-8");

	bindtextdomain ("joringsbridge",pathToTranslationFiles.c_str());
	bind_textdomain_codeset ("joringsbridge","UTF-8");

	bindtextdomain ("maylons_grave",pathToTranslationFiles.c_str());
	bind_textdomain_codeset ("maylons_grave","UTF-8");

	bindtextdomain ("menu",pathToTranslationFiles.c_str());
	bind_textdomain_codeset ("menu","UTF-8");

	bindtextdomain ("rescue_willard",pathToTranslationFiles.c_str());
	bind_textdomain_codeset ("rescue_willard","UTF-8");

	bindtextdomain ("rob_the_robber",pathToTranslationFiles.c_str());
	bind_textdomain_codeset ("rob_the_robber","UTF-8");

	bindtextdomain ("scout",pathToTranslationFiles.c_str());
	bind_textdomain_codeset ("scout","UTF-8");

	bindtextdomain ("sumwars",pathToTranslationFiles.c_str());
	bind_textdomain_codeset ("sumwars","UTF-8");

	bindtextdomain ("testquest",pathToTranslationFiles.c_str());
	bind_textdomain_codeset ("testquest","UTF-8");

	bindtextdomain ("the_ambush_of_the_undead",pathToTranslationFiles.c_str());
	bind_textdomain_codeset ("the_ambush_of_the_undead","UTF-8");

	bindtextdomain ("the_ambush",pathToTranslationFiles.c_str());
	bind_textdomain_codeset ("the_ambush","UTF-8");

	bindtextdomain ("the_besieged_farm",pathToTranslationFiles.c_str());
	bind_textdomain_codeset ("the_besieged_farm","UTF-8");

	bindtextdomain ("the_goblinslayer",pathToTranslationFiles.c_str());
	bind_textdomain_codeset ("the_goblinslayer","UTF-8");

	bindtextdomain ("the_poolmonster",pathToTranslationFiles.c_str());
	bind_textdomain_codeset ("the_poolmonster","UTF-8");

	bindtextdomain ("the_trader",pathToTranslationFiles.c_str());
	bind_textdomain_codeset ("the_trader","UTF-8");

	bindtextdomain ("tools",pathToTranslationFiles.c_str());
	bind_textdomain_codeset ("tools","UTF-8");

	bindtextdomain ("tutorial",pathToTranslationFiles.c_str());
	bind_textdomain_codeset ("tutorial","UTF-8");

	textdomain ("menu");

	// Augustin Preda, 2013.03.25: Made the code always call the setLocale, and added a special handling for Windows
	// This is done to fix the mantis bug with ID: 46 ( http://sumwars.org/mantis/view.php?id=46 )
	Gettext::setLocale (locale);

}


void Gettext::preInit ()
{
#if WIN32
	// Prepare the mappings.
	// -1 for an entry means it's unsupported!
    // http://msdn.microsoft.com/en-us/library/ms912047(WinEmbedded.10).aspx
	winLanguageMappings_["en"] = 0x0409; // en-US
	winLanguageMappings_["de"] = 0x0407; // de-DE
	winLanguageMappings_["it"] = 0x0410; // it-IT
	winLanguageMappings_["pl"] = 0x0415;
	winLanguageMappings_["pt"] = 0x0816; // pt-PT
	winLanguageMappings_["ru"] = 0x0419;
	winLanguageMappings_["uk"] = 0x0422;
    winLanguageMappings_["es"] = 0xC0A;
#endif
}


const char* Gettext::getLocale()
{
	return m_locale.c_str();
}

void Gettext::setLocale(const char* loc)
{
    std::string locale;
    if (loc)
    {
         locale = loc;
    }

    SW_DEBUG ("set new language [%s]",locale.c_str());

	if (locale != m_locale)
	{
#ifdef WIN32
		if (locale.empty())
		{
			locale = getDefaultUsableLocale ();
		}


		if (!locale.empty())
		{
			std::string win_locale(locale, 0, 2);
			std::string env = "LANGUAGE=" + win_locale;

			_putenv(env.c_str());
			SetEnvironmentVariable("LANGUAGE", win_locale.c_str());

			char lang[50];
			GetEnvironmentVariable("LANGUAGE",lang,50);
			DEBUGX("current language (GetEnvironmentVariable) %s",lang);
			DEBUGX("current language (getenv) %s",getenv("LANGUAGE"));

			if (setWinThreadLocale (win_locale) == 0)
			{
				SW_DEBUG ("Could not change locale to %s", win_locale.c_str ());
				return;
			}

			m_locale = locale;
			m_changed = true;
			return;
		}

#endif
        std::string extensions[3] = {".utf-8",".UTF-8",""};

        // try all possible extensions
        char* ret;
        bool success = false;
        std::string locale_ext;
        for (int i=0; i<3; i++)
        {
            locale_ext = locale;
            locale_ext += extensions[i];
#ifndef WIN32
			setenv("LANG", locale_ext.c_str(), 1);     // fixes gettext problems with Ubuntu
			setenv("LANGUAGE", locale_ext.c_str(), 1); // fixes gettext problems with Ubuntu
#endif
            ret = setlocale( LC_MESSAGES, locale_ext.c_str() );
            if (ret !=0)
            {
                m_locale = locale_ext;
                m_changed = true;
                success = true;
                DEBUGX("setting locale to %s",ret);
                break;
            }
        }


		if (!success)
		{
			DEBUGX("setting locale %s not succesful",locale);
		}
	}
	else
	{
#ifdef WIN32
		// old and new locale are identical.
		// But what if the locales are both identical and empty? A special case for Windows environments.
		if (locale.empty())
		{
			SW_DEBUG ("Gettext received empty locale. Initializing with OS default");
			locale = getDefaultUsableLocale ();
			SW_DEBUG ("Gettext received empty locale. Initialized with OS default: %s", locale.c_str ());
		}


		if (!locale.empty())
		{
			std::string win_locale(locale, 0, 2);
			std::string env = "LANGUAGE=" + win_locale;

			_putenv(env.c_str());
			SetEnvironmentVariable("LANGUAGE", win_locale.c_str());

			char lang[50];
			GetEnvironmentVariable("LANGUAGE",lang,50);
			DEBUGX("current language (GetEnvironmentVariable) %s",lang);
			DEBUGX("current language (getenv) %s",getenv("LANGUAGE"));

			if (setWinThreadLocale (win_locale) == 0)
			{
				SW_DEBUG ("Could not change locale to %s", win_locale.c_str ());
				return;
			}

			m_locale = locale;
			m_changed = true;
			return;
		}
#endif
	}
}

bool Gettext::getLocaleChanged()
{
	bool ret = m_changed;
	m_changed = false;
	return ret;
}


#if WIN32
bool Gettext::setWinThreadLocale (const std::string& newLocale)
{
	bool result (false);

	int newCode = getLanguageCodeFromString (newLocale);
	if (newCode == -1)
		return false;
	
	SW_DEBUG ("Setting locale to : %s", newLocale.c_str ());
	result = SetThreadLocale (newCode);

	return result;
}

int Gettext::getLanguageCodeFromString (const std::string& languageString)
{
	// Search for the language in the mapping.
	std::map <std::string, int>::iterator it = winLanguageMappings_.find  (languageString);
	if (it != winLanguageMappings_.end ())
	{
		return it->second;
	}
	return -1;
}



std::string Gettext::getDefaultUsableLocale ()
{
	char tmp[100];
	GetLocaleInfo(LOCALE_USER_DEFAULT,
					LOCALE_SISO639LANGNAME,
					tmp, sizeof(tmp));
	std::string myLocale = tmp;

	// Search for the language in the mapping.
	for (std::map <std::string, int>::iterator it = winLanguageMappings_.begin ();
		it != winLanguageMappings_.end (); ++ it)
	{
		if (it->first == myLocale)
		{
			return it->first;
		}
	}

	return std::string ("en");
}


#endif // WIN32
