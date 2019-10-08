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

#if defined(WIN32)
#include "windows.h"

#endif
//INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
int main (int argc, char *argv[]) {

	try
	{

	// Applikation anlegen
	Application* app = new Application(argv[0]);

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
		DEBUG("Program died with exception %s", e.getMessage().c_str());
	}
	// Ende
	return 0;
}
