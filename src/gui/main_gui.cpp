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


#include <stdio.h>
#include "application.h"
#include "config.h"
#include <physfs.h>
#include "sumwarshelper.h"

#if defined(WIN32)
#	include "windows.h"
#endif // if defined(WIN32)


#if defined(SUMWARS_USE_BREAKPAD)

#if defined (WIN32)
#   include "client/windows/handler/exception_handler.h"
#   include "client/windows/sender/crash_report_sender.h"
#endif

static bool breakpad_callback(const wchar_t *dump_path,
                              const wchar_t *id,
                              void *context,
                              EXCEPTION_POINTERS *exinfo,
                              MDRawAssertionInfo *assertion,
                              bool succeeded )
{
    if (succeeded)
    {
        printf("dump guid is %ws\n", id);
    }
    else
    {
        printf("dump failed\n");
    }
    fflush(stdout);

    return succeeded;
}
#endif // if defined(SUMWARS_USE_BREAKPAD)

void reset_cwd(int argc, char** argv)
{
#	ifdef _WIN32
	TCHAR szPath[MAX_PATH];

	if (GetModuleFileName(NULL, szPath, MAX_PATH))
	{
		std::string path(szPath);
		path.erase(path.find_last_of('\\'), std::string::npos); // strips the basename off
		SetCurrentDirectory(path.c_str());
	}
	else
	{
		// just exiting feels rather harsh, CWD might have been set correctly
		printf("Error: Cannot retrieve executable file path to set proper CWD (WinAPI reported: %d). Pray with me that it was already set correctly!\n", GetLastError());
	}

#	elif defined (__unix__)
	// UNIX is for proper badasses so it's not as simple there ;-D
	// we could rely on /proc but that's not portable
	// instead we will figure this out using argv[0]

	if (argc >= 1 && argv[0])
	{
		std::string passed_path(argv[0]);
		if (passed_path.length() >= 1)
		{
			passed_path.erase(passed_path.find_last_of('/'), std::string::npos); // strips the basename off

			if (passed_path[0] == '/')
			{
				chdir(passed_path.c_str());
			}
			else
			{
				char cwd[PATH_MAX];
				if (!getcwd(cwd, PATH_MAX))
				{
					printf("Error: Cannot retrieve current working directory, therefore can't reset it to point to directory of sumwars executable. Hopefully all goes well!\n");
					return;
				}

				std::string absolute_path = std::string(cwd) + "/" + passed_path;
				printf("Deduced absolute path: %s", absolute_path.c_str());
				chdir(absolute_path.c_str());
			}
		}
		else
		{
			printf("Error: Can't deduce directory of sumwars executable from passed arguments, therefore can't reset CWD. Hopefully all goes well!\n");
		}
	}
#	endif
}

//INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
int main (int argc, char **argv)
{
	// Initialise the PHYSFS library as soon as possible before anyone messes with argv or CWD
	if (PHYSFS_init(argv[0]) == 0)
	{
		printf("PHYSFS_init failed: %s\n", PHYSFS_getLastError ());
		return 1;
	}

    // Create the sumwars helper singleton
    SumwarsHelper *swh = new SumwarsHelper ();
    if(!swh->init())
    {
        printf("PHYSFS_init failed: %s\n", PHYSFS_getLastError ());
        return 1;
    }

#if defined SUMWARS_USE_BREAKPAD
    std::string s;
    s = swh->getStorageBasePath();
    s.append(swh->userPath());
    std::wstring path;
    path.assign(s.begin(), s.end());
    google_breakpad::ExceptionHandler eh(path,
                                         NULL,
                                         breakpad_callback,
                                         NULL,
                                         google_breakpad::ExceptionHandler::HANDLER_ALL);
#endif //#if defined SUMWARS_USE_BREAKPAD

#ifdef SUMWARS_PORTABLE_MODE
	// Portable mode relies on CWD (current working directory) to be set to the place where
	// the "sumwars" executable is.
	//
	// If you just run sumwars with ./sumwars that will already be the case, however when starting
	// it with a shortcut or by calling some/weird/directory/sumwars, CWD will be something we
	// don't expect.
	//
	// The code below remedies the situation in these cases by simply reseting CWD
	reset_cwd(argc, argv);
	//exit(0);
#endif

	try
	{

	// Applikation anlegen
	Application* app = new Application();

	// Debugging: Kommandozeilen Parameter auslesen und Savefile setzen
	std::string save;
	#ifndef WIN32
	if (argc>=2)
	{
		save = "~/.sumwars/save/";
		save += argv[1];
	}
	else
	{
		save = "./save/default.sav";
	}
	#else
        save = "./save/default.sav";
	#endif

	bool server = true;
	if (argc>=3)
	{
		if (std::string(argv[2]) == "-c")
		{
			server = false;
		}
	}
	
	if (argc>=2)
	{
		Document* doc = app->getDocument();
		doc->setSaveFile(save);
		
		doc->setServer(server);
		doc->setState(Document::START_GAME);
		doc->getGUIState()->m_sheet= Document::GAME_SCREEN;
		doc->getGUIState()->m_shown_windows = Document::NO_WINDOWS;
	}
	// laufen lassen

	/*
	try
	{

		app->run();
	}
	catch (std::exception &e)
	{
		ERRORMSG("Error message: %s",e.what());
	}
	*/
    
    app->run();
	
	
	// Applikation loeschen
	delete app;

	}
	/*
	catch (std::exception& e)
	{
		DEBUG("Program died with exception %s",e.what());
	}
	*/
	catch (CEGUI::Exception& e)
	{
		SW_DEBUG("Program died with exception %s", e.getMessage().c_str());
	}
	// Ende
	return 0;
}
