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

#include "fstream"
#include "document.h"

#include "sound.h"
#include "music.h"

#include "networkstruct.h"
#include "projectile.h"
#include "party.h"
#include "debug.h"
#include "damage.h"
#include "itemlist.h"
#include "random.h"
#include "item.h"
#include "matrix2d.h"
#include "dropitem.h"
#include "world.h"
#include "player.h"
#include "timer.h"
#include "sumwarshelper.h"

#include "gettext.h"
#include "stdstreamconv.h"


#include "CEGUI/CEGUI.h"

#include <physfs.h>

// Constructors/Destructors
// Initialisiert Document zu Testzwecken
Document::Document()
{

	// Informationen zu Aktionen initialisieren
	Action::init();


	// Status der GUI setzen
	getGUIState()->m_left_mouse_pressed= false;
	getGUIState()->m_right_mouse_pressed= false;
	getGUIState()->m_middle_mouse_pressed= false;
	getGUIState()->m_shift_hold = false;
	getGUIState()->m_item_labels = false;
	getGUIState()->m_sheet= MAIN_MENU;
	getGUIState()->m_shown_windows = NO_WINDOWS;
	//getGUIState()->m_pressed_key = OIS::KC_UNASSIGNED;
	getGUIState()->m_pressed_key =0;
	getGUIState()->m_cursor_object ="";
	getGUIState()->m_cursor_object_id =0;
	getGUIState()->m_cursor_item_id =0;
	getGUIState()->m_right_mouse_pressed_time=0;
	getGUIState()->m_left_mouse_pressed_time=0;
	getGUIState()->m_hover_ability = "noaction";
	getGUIState()->m_prefer_right_skill = false;

	// Pointer/Inhalte mit 0 initialisieren
	m_gui_state.m_chat_window_content = "";
	m_options_timer.start();

	// aktuell eingestellte Aktionen setzen

	// Status setzen
	m_state = INACTIVE;

	m_modified =GUISHEET_MODIFIED | WINDOWS_MODIFIED;

	m_temp_player = 0;
	m_single_player = true;
}

void Document::startGame(bool server)
{
	m_server = server;

	// momentan: alle Spiele sind kooperativ
	int port = Options::getInstance()->getPort();
	int max_players = Options::getInstance()->getMaxNumberPlayers();
	std::string host = Options::getInstance()->getServerHost();

	if (m_single_player)
	{
		max_players = 1;
	}

	World::createWorld(server,port, true,max_players);

	if (server)
	{



	}
	else
	{
		ClientNetwork* net = static_cast<ClientNetwork*>(World::getWorld()->getNetwork());
		net->serverConnect((char*) host.c_str(),port);

	}
	m_state = LOAD_SAVEGAME;
}

void Document::setSaveFile(std::string s)
{
	std::string userPath = SumwarsHelper::userPath();
	userPath.append("/save/").append(s);

	std::fstream file(userPath.c_str(),std::ios::in| std::ios::binary);
	if (file.is_open())
	{
		char bin;
		unsigned char* data=0;
		m_save_file = userPath;

		DEBUG ("Loaded save file %s", s.c_str());

		file.get(bin);

		CharConv* save;
		DEBUGX("binary %i",bin);
		if (bin == '0')
		{
			save = new StdStreamConv(&file);
		}
		else
		{
			// binary savefile is not supported anymore
			ERRORMSG("binary Savegame is not supported anymore");
			return;
		}

		if (m_temp_player)
			delete m_temp_player;

		m_temp_player = new Player(0,"");
		m_temp_player->fromSavegame(save);

		delete save;
		if (data)
			delete[] data;


	}
	else
	{
		if (m_temp_player)
		{
			delete m_temp_player;
			m_temp_player =0;
		}
		m_save_file ="";
	}
	m_modified |= SAVEGAME_MODIFIED;
	file.close();
}

void Document::loadSavegame()
{
	// read savegame
	std::string fname = m_save_file;
	DEBUG("savegame is %s",fname.c_str());

	std::fstream file(fname.c_str(),std::ios::in| std::ios::binary);
	if (file.is_open())
	{
		char bin;
		unsigned char* data=0;

		file.get(bin);

		CharConv* save;
		// determine binary vs nonbinary savegile
		if (bin == '0')
		{
			save = new StdStreamConv(&file);
		}
		else
		{
			// binary savefile is not supported anymore
			ERRORMSG("binary Savegame is not supported anymore");
			return;
		}

		// update the player object, that is displayed in character preview and inventory
		if (m_temp_player)
			delete m_temp_player;

		m_temp_player = new Player(0,"");
		m_temp_player->fromSavegame(save);

		// notify the world of the new player

		std::stringstream sstream;
		StdStreamConv cv2(&sstream);

		Timer t;
		t.start();
		while (t.getTime() < 200)
		{
		}

		m_temp_player->toSavegame(&cv2);
		DEBUG("sending savegame");
		World::getWorld()->handleSavegame(&cv2);
		DEBUGX("sent savegame");

		// read shortkeys from the savegame
		m_ability_shortkey_map.clear();
		short key,dest;
		int nr =0;
		save->fromBuffer(nr);
		for (int i=0; i<nr; ++i)
		{
			save->fromBuffer(key);
			save->fromBuffer(dest);
			if (dest >= USE_SKILL_LEFT && dest < USE_SKILL_RIGHT + 200)
			{
				installShortkey((KeyCode) key, (ShortkeyDestination) dest);
			}
		}

		// update state to running
		// show main game screen
		m_state =RUNNING;
		m_gui_state.m_shown_windows = NO_WINDOWS;
		m_gui_state.m_sheet = GAME_SCREEN;
		m_modified = WINDOWS_MODIFIED | GUISHEET_MODIFIED;
		m_save_timer.start();

		file.close();
		if (data)
			delete[] data;
		delete save;
	}
	else
	{
		if (m_save_file == "")
		{
			// no save file chosen, go to character creation
			getGUIState()->m_shown_windows =CHAR_CREATE;
			m_modified = WINDOWS_MODIFIED | GUISHEET_MODIFIED ; 
			m_gui_state.m_sheet = MAIN_MENU;
			m_state = INACTIVE;
		}
		else
		{
			ERRORMSG("could not open savegame: %s", fname.c_str());
			m_state =SHUTDOWN;
		}
	}
	DEBUGX("done");
}

Document::~Document()
{

}
// Methods

void Document::onCreateNewCharButton()
{
	if (m_temp_player)
	{
		delete m_temp_player;
		m_temp_player =0;
	}

	m_modified |= SAVEGAME_MODIFIED;

	getGUIState()->m_shown_windows =CHAR_CREATE;
	m_modified |= WINDOWS_MODIFIED;

}


void Document::setNewCharacter(WorldObject::Subtype subtype, PlayerLook look)
{
	if (m_temp_player)
		delete m_temp_player;

	m_temp_player = new Player(0,subtype);
	m_temp_player->getPlayerLook() = look;

	m_modified |= SAVEGAME_MODIFIED;
}

bool Document::createNewCharacter(std::string name)
{
	if (m_temp_player)
	{
		std::string storagePath (SumwarsHelper::getSingletonPtr ()->userPath ());
		storagePath.append ("/save/");
        std::string path;

#ifndef __APPLE__
		// Windows & Linux
		path = storagePath;
#else
		path = PHYSFS_getUserDir();
        path.append("/Library/Application Support/Sumwars/save/");
#endif
        m_save_file = path;
		m_save_file += name;
		m_save_file += ".sav";

		m_temp_player ->setName(TranslatableString(name,""));

		DEBUGX("savefile %s",m_save_file.c_str());

		std::ifstream file(m_save_file.c_str());
		if (file.is_open())
		{
			//ERRORMSG("file exists: %s",m_save_file.c_str());
			return false;
		}

		writeSavegame(false);

		getGUIState()->m_shown_windows = Document::START_MENU;
		setModified(Document::WINDOWS_MODIFIED);
	}
	return true;
}

void Document::sendCommand(ClientCommand* comm)
{
	if (World::getWorld() != 0)
	{
		World::getWorld()->handleCommand(comm);
	}
}




void Document::installShortkey(KeyCode key,ShortkeyDestination dest)
{
	Options* opt = Options::getInstance();
	// do not use special keys nor overwrite global shortkeys
	if (opt->isSpecialKey(key) || opt->getMappedKey(dest) != 0)
		return;

	// key that was mapped to the action dest so far
	KeyCode oldkey = 0;
	std::map<KeyCode, ShortkeyDestination>::iterator it;
	for (it=m_ability_shortkey_map.begin(); it!= m_ability_shortkey_map.end();++it)
	{
		if (it->second == dest)
		{
			oldkey = it->first;
			break;
		}
	}
	// delete mapping
	if (oldkey != 0)
	{
		m_ability_shortkey_map.erase(oldkey);
	}

	// create new mapping
	m_ability_shortkey_map[key]=dest;
}

void Document::onButtonSendMessageClicked ( )
{
	DEBUGX("onbuttonmessageclicked");
}


void Document::onButtonSaveExitClicked ( )
{
	getGUIState()->m_shown_windows = SAVE_EXIT;
	m_modified |= WINDOWS_MODIFIED;

	if (m_state == INACTIVE)
	{
		saveSettings();
		m_state = END_GAME;
		return;
	}
}

void Document::onButtonSaveExitConfirm()
{


	if (m_state!=SHUTDOWN_REQUEST)
	{
		setState(SHUTDOWN_REQUEST);
	}
	else
	{
		setState(SHUTDOWN);
	}

	m_shutdown_timer =0;

	ClientCommand command;

	// Paket mit Daten fuellen
	// Button ist Speichern+Beenden, alle anderen Daten gleich 0
	command.m_button=BUTTON_SAVE_QUIT;
	command.m_goal = Vector(0,0);
	command.m_id=0;
	command.m_number=0;

	// Paket an den Server senden
	sendCommand(&command);

	getGUIState()->m_shown_windows = NO_WINDOWS;
	m_modified |= WINDOWS_MODIFIED;
}

void Document::onButtonSaveExitAbort()
{
	getGUIState()->m_shown_windows = NO_WINDOWS;
	m_modified |= WINDOWS_MODIFIED;
}

void Document::onButtonCredits()
{
	getGUIState()->m_shown_windows =CREDITS;
	m_modified =WINDOWS_MODIFIED;
}

void Document::onRightMouseButtonClick(Vector pos)
{
	ClientCommand command;

	// der lokale Spieler
	Player* pl = static_cast<Player*> (World::getWorld()->getLocalPlayer());
	if (pl==0)
		return;


	// herstellen der Koordinaten im Koordinatensystem des Spiels
	m_gui_state.m_clicked = pos;

	// Paket mit Daten fuellen
	command.m_button=RIGHT_MOUSE_BUTTON;
	command.m_goal = m_gui_state.m_clicked;
	command.m_action = pl->getRightAction();

	m_gui_state.m_left_mouse_pressed=false;
	m_gui_state.m_right_mouse_pressed_time=0;

	DEBUGX("angeklickte Koordinaten: %f %f",pos.m_x,pos.m_y);

	m_gui_state.m_clicked_object_id=0;
	command.m_id=0;

	int id = 0;
	if (getGUIState()->m_cursor_object_id != 0)
	{
		id = getGUIState()->m_cursor_object_id;
	}

	m_gui_state.m_clicked_object_id = id;
	command.m_id =id;

	command.m_number=0;

	if (command.m_id != 0)
	{
		DEBUGX ("Clicked object %i",command.m_id);
	}


	// Paket an den Server senden
	sendCommand(&command);

}

void Document::onLeftMouseButtonClick(Vector pos)
{

	ClientCommand command;

	// der lokale Spieler
	Player* pl = static_cast<Player*> (World::getWorld()->getLocalPlayer());
	if (pl==0)
		return;

	// Dialog is open, each klick is interpreted as *skip this text*
	if (pl->getDialogueId() != 0)
	{
		onSkipDialogueTextClicked();
		return;
	}

	// herstellen der Koordinaten im Koordinatensystem des Spiels

	m_gui_state.m_clicked = pos;


	// Paket mit Daten fuellen
	command.m_button=LEFT_MOUSE_BUTTON;
	if (m_gui_state.m_shift_hold)
	{
		command.m_button=LEFT_SHIFT_MOUSE_BUTTON;
	}
	command.m_goal = m_gui_state.m_clicked;
	command.m_action = pl->getLeftAction();

	m_gui_state.m_right_mouse_pressed=false;
	m_gui_state.m_left_mouse_pressed_time=0;


	DEBUGX("angeklickte Koordinaten: %f %f",pos.m_x,pos.m_y);

	m_gui_state.m_clicked_object_id=0;
	command.m_id=0;


	int id = 0;
	if (getGUIState()->m_cursor_object_id != 0)
	{
		id = getGUIState()->m_cursor_object_id;
	}

	m_gui_state.m_clicked_object_id = id;
	command.m_id =id;
	command.m_number=0;

	// Linksklick auf die eigene Figur unnoetig
	if (command.m_id == getLocalPlayer()->getId())
	{
		command.m_id=0;
	}

	if (command.m_id!=0)
	{
		DEBUGX("angeklicktes Objekt %i",command.m_id);
	}

	if (command.m_id ==0 && m_gui_state.m_cursor_item_id!=0)
	{
		// Item angeklickt
		command.m_action = "take_item";
		command.m_id = m_gui_state.m_cursor_item_id;

		DEBUGX("clicked at item %i",m_gui_state.m_cursor_item_id);
	}

	// Paket an den Server senden
	sendCommand(&command);


}

void Document::onMouseMove(float xrel, float yrel, float wheelrel)
{
	Player* player = getLocalPlayer();
	if (player!=0)
	{
		if (! getGUIState()->m_middle_mouse_pressed)
		{
			xrel= yrel =0;
		}
		player->getCamera().moveRelative(wheelrel*0.01,yrel*0.5,-xrel*0.5);
	}
}

void Document::onStartScreenClicked()
{
	if (getGUIState()->m_shown_windows == NO_WINDOWS)
	{
		//getGUIState()->m_shown_windows = SAVEGAME_LIST;
		getGUIState()->m_shown_windows = START_MENU;
		m_modified =WINDOWS_MODIFIED;
	}

	if (getGUIState()->m_shown_windows == CREDITS)
	{
		getGUIState()->m_shown_windows = START_MENU;
		m_modified =WINDOWS_MODIFIED;
	}
}

int Document::getObjectAt(float x,float y)
{
	// der lokale Spieler
	WorldObject* pl = World::getWorld()->getLocalPlayer();
	if (pl==0)
		return 0;

	// Region in der sich der lokale Spieler befindet
	Region* reg = pl->getRegion();
	if (reg ==0)
		return 0;

	WorldObject* obj = reg->getObjectAt(Vector(x,y));
	if (obj != 0)
		return obj->getId();

	return 0;

}


void Document::onButtonPartyApply(int id)
{
	ClientCommand command;
	command.m_button = BUTTON_APPLY;
	command.m_id = id;
	sendCommand(&command);
}

void Document::onButtonPartyAccept(int id)
{
	ClientCommand command;
	command.m_button = BUTTON_MEMBER_ACCEPT;
	command.m_id = id;
	sendCommand(&command);

}

void Document::onButtonPartyReject(int id)
{
	ClientCommand command;
	command.m_button = BUTTON_MEMBER_REJECT;
	command.m_id = id;
	sendCommand(&command);

}

void Document::onButtonPartyWar(int id)
{
	ClientCommand command;
	command.m_button = BUTTON_WAR;
	command.m_id = id;
	sendCommand(&command);

}

void Document::onButtonPartyPeace(int id)
{
	ClientCommand command;
	command.m_button = BUTTON_PEACE;
	command.m_id = id;
	sendCommand(&command);

}


void Document::onButtonKick(int id)
{
	ClientCommand command;
	command.m_button = BUTTON_KICK;
	command.m_id = id;
	sendCommand(&command);
}

void Document::onButtonPartyLeave()
{
	ClientCommand command;
	command.m_button = BUTTON_LEAVE;
	sendCommand(&command);
}

void Document::onItemLeftClick(short pos)
{
	ClientCommand command;
	command.m_button = BUTTON_ITEM_LEFT;
	command.m_id = pos;
	sendCommand(&command);

}

void Document::onItemRightClick(short pos)
{
	ClientCommand command;
	command.m_button = BUTTON_ITEM_RIGHT;
	command.m_id = pos;
	sendCommand(&command);

}

void Document::onTradeItemLeftClick(short pos)
{
	ClientCommand command;
	command.m_button = BUTTON_TRADE_ITEM_LEFT;
	command.m_id = pos;
	sendCommand(&command);

}

void Document::onTradeItemRightClick(short pos)
{
	ClientCommand command;
	command.m_button = BUTTON_TRADE_ITEM_RIGHT;
	command.m_id = pos;
	sendCommand(&command);

}

void Document::onTradeSellClicked()
{
	ClientCommand command;
	command.m_button = BUTTON_TRADE_SELL;
	command.m_id = 0;
	sendCommand(&command);
}

void Document::increaseAttribute(CreatureBaseAttr::Attribute attr)
{
	ClientCommand command;
	command.m_button = BUTTON_INCREASE_ATTRIBUTE;
	command.m_id = attr;
	DEBUGX("increasing attribute %i",attr);
	sendCommand(&command);
}

void Document::learnAbility(Action::ActionType act)
{
	ClientCommand command;
	command.m_button = BUTTON_LEARN_ABILITY;
	command.m_action = act;
	sendCommand(&command);
}

void Document::dropCursorItem()
{
	ClientCommand command;
	command.m_button = DROP_ITEM;
	command.m_id =0;
	command.m_number =0;
	sendCommand(&command);
}

void Document::dropGold(int value)
{
	ClientCommand command;
	command.m_button = DROP_ITEM;
	command.m_id =1;
	command.m_number =value;
	sendCommand(&command);
}

void Document::onDropItemClick(int id)
{
	ClientCommand command;
	command.m_button=LEFT_MOUSE_BUTTON;
	command.m_action = "take_item";
	command.m_id = id;

	// Paket an den Server senden
	sendCommand(&command);
}

void Document::onAnswerClick(int id)
{
	ClientCommand command;
	command.m_button=BUTTON_ANSWER;
	command.m_action = "speak_answer";
	command.m_id = id;

	// Paket an den Server senden
	sendCommand(&command);
}


void Document::emitDebugSignal(int i)
{
	ClientCommand command;
	command.m_button = DEBUG_SIGNAL;
	command.m_id = i;
	sendCommand(&command);
}

bool Document::checkSubwindowsAllowed()
{
	bool ok = true;
	ok &= (getState() == RUNNING || getState() == SHUTDOWN_REQUEST);
	ok &= (getGUIState()->m_shown_windows & SAVE_EXIT) == 0;
	ok &= (getGUIState()->m_shown_windows & (QUESTIONBOX | TRADE)) == 0;

	// Also check to see if the current player is involved in anything that doesn't allow 
	if (getLocalPlayer() != 0)
	{
		// Check if the region is currently in a cutscene mode. 
		// (The player shouldn't be allowed to open subwindows (E.g. the inventory) during a cutscene.
		if (getLocalPlayer()->getRegion() !=0)
		{
			ok &= !getLocalPlayer()->getRegion()->getCutsceneMode();

			// Also check if the current player is involved in a dialogue.
			// Add the check here, because the dialog is obtained from the region.
			if (getLocalPlayer ()->getDialogue () != 0)
			{
				ok = false;
			}
		}
	}
	return ok;
}



void Document::onButtonStartSinglePlayer ()
{
	// The player is a host himself (or herself)
	setServer (true);
	m_single_player = true;

	// Set the state and allow the connection to be established.
	setState (Document::START_GAME);
}



void Document::onButtonHostGame()
{
	DEBUG("Host Game");
	if (m_save_file == "")
	{
		// Show a notification.
		CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
		CEGUI::FrameWindow* message = (CEGUI::FrameWindow*) win_mgr.getWindow("WarningDialogWindow");
		message->setInheritsAlpha(false);
		message->setVisible(true);
		message->setModalState(true);
		win_mgr.getWindow( "WarningDialogLabel")->setText((CEGUI::utf8*) gettext("Please select a character first!"));

		DEBUG ("Warning: Tried to host a game without a selected char!");
		return;
	}
	getGUIState()->m_shown_windows |= HOST_GAME;
	getGUIState()->m_shown_windows &= ~START_MENU;

	m_modified |= WINDOWS_MODIFIED;
}

void Document::onButtonJoinGame()
{
	if (m_save_file == "")
	{
		// Show a notification.
		CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
		CEGUI::FrameWindow* message = (CEGUI::FrameWindow*) win_mgr.getWindow("WarningDialogWindow");
		message->setInheritsAlpha(false);
		message->setVisible(true);
		message->setModalState(true);
		win_mgr.getWindow( "WarningDialogLabel")->setText((CEGUI::utf8*) gettext("Please select a character first!"));

		DEBUG ("Warning: Tried to join a game without a selected char!");
		return;
	}
	getGUIState()->m_shown_windows |= JOIN_GAME;
	getGUIState()->m_shown_windows &= ~START_MENU;
	m_modified |= WINDOWS_MODIFIED;
}

void Document::onButtonStartHostGame()
{
	DEBUG("start multiplayer game");
	// Spieler ist selbst der Host
	setServer(true);
	m_single_player = false;

	// Verbindung aufbauen
	setState(Document::START_GAME);
}



void Document::onButtonStartJoinGame()
{
	setServer(false);
	m_single_player = false;
	
	// starten
	setState(Document::START_GAME);
}

void Document::onButtonInventoryClicked()
{
	if (!checkSubwindowsAllowed())
		return;

	// Inventar oeffnen wenn es gerade geschlossen ist und schliessen, wenn es geoeffnet ist
	if (getGUIState()->m_shown_windows & INVENTORY)
	{

		getGUIState()->m_shown_windows &= ~INVENTORY;

		// der lokale Spieler
		Player* pl = static_cast<Player*>( World::getWorld()->getLocalPlayer());
		if (pl==0)
			return;

		// Item das aktuell in der Hand ist fallen lassen
		if (pl->getEquipement()->getItem(Equipement::CURSOR_ITEM)!=0)
		{
			dropCursorItem();
		}
	}
	else
	{
		// wenn Inventar geoeffnet wird, dann Skilltree schliessen
		getGUIState()->m_shown_windows &= ~SKILLTREE;
		m_gui_state.m_pressed_key = 0;

		getGUIState()->m_shown_windows |= INVENTORY;



	}

	// Geoeffnete Fenster haben sich geaendert
	m_modified |= WINDOWS_MODIFIED;

}

void Document::onButtonCharInfoClicked()
{
	if (!checkSubwindowsAllowed())
		return;

	// Charakterinfo oeffnen wenn es gerade geschlossen ist und schliessen, wenn es geoeffnet ist
	if (getGUIState()->m_shown_windows & CHARINFO)
	{
		getGUIState()->m_shown_windows &= ~CHARINFO;
	}
	else
	{
		getGUIState()->m_shown_windows &= ~(PARTY | QUEST_INFO);

		getGUIState()->m_shown_windows |= CHARINFO;
	}

	// Geoeffnete Fenster haben sich geaendert
	m_modified |= WINDOWS_MODIFIED;
}

void Document::onButtonPartyInfoClicked()
{
	if (!checkSubwindowsAllowed())
		return;

	// PartyInfo oeffnen wenn es gerade geschlossen ist und schliessen, wenn er geoeffnet ist
	if (getGUIState()->m_shown_windows & PARTY)
	{
		getGUIState()->m_shown_windows &= ~PARTY;
	}
	else
	{
		// wenn PartyInfo geoeffnet wird, dann CharInfo schliessen
		getGUIState()->m_shown_windows &= ~(CHARINFO | QUEST_INFO);

		getGUIState()->m_shown_windows |= PARTY;
	}

	m_gui_state.m_pressed_key = 0;

	// Geoeffnete Fenster haben sich geaendert
	m_modified |= WINDOWS_MODIFIED;
}

void Document::onButtonSkilltreeClicked(bool skill_right, bool use_alternate)
{
	if (!checkSubwindowsAllowed())
		return;

	// Skilltree oeffnen wenn er gerade geschlossen ist und schliessen, wenn er geoeffnet ist
	if (getGUIState()->m_shown_windows & SKILLTREE)
	{
		getGUIState()->m_prefer_right_skill = false;
		getGUIState()->m_set_right_skill_alternate = false;
		getGUIState()->m_shown_windows &= ~SKILLTREE;
	}
	else
	{
		// wenn Skilltree geoeffnet wird, dann Inventar schliessen
		getGUIState()->m_shown_windows &= ~INVENTORY;

		getGUIState()->m_shown_windows |= SKILLTREE;
		getGUIState()->m_set_right_skill_alternate = use_alternate;
		getGUIState()->m_prefer_right_skill =skill_right;
	}

	m_gui_state.m_pressed_key = 0;

	// Geoeffnete Fenster haben sich geaendert
	m_modified |= WINDOWS_MODIFIED;
}

void Document::onButtonOpenChatClicked()
{
	// fuer Debugging sehr nuetzlich, das zuzulassen
	//if (!checkSubwindowsAllowed())
	//		return;

	// Cchatfenster oeffnen wenn es gerade geschlossen ist und schliessen, wenn es geoeffnet ist
	if (getGUIState()->m_shown_windows & CHAT)
	{
		getGUIState()->m_shown_windows &= ~CHAT;
	}
	else
	{
		getGUIState()->m_shown_windows |= CHAT;
	}

	// Geoeffnete Fenster haben sich geaendert
	m_modified |= WINDOWS_MODIFIED;
}

void Document::onButtonQuestInfoClicked()
{
	if (!checkSubwindowsAllowed())
		return;

	// Charakterinfo oeffnen wenn es gerade geschlossen ist und schliessen, wenn es geoeffnet ist
	if (getGUIState()->m_shown_windows & QUEST_INFO)
	{
		getGUIState()->m_shown_windows &= ~QUEST_INFO;
	}
	else
	{
		getGUIState()->m_shown_windows &= ~(PARTY | CHARINFO);

		getGUIState()->m_shown_windows |= QUEST_INFO;
	}

	// Geoeffnete Fenster haben sich geaendert
	m_modified |= WINDOWS_MODIFIED;
}

void Document::onButtonMinimapClicked()
{
	if (!checkSubwindowsAllowed())
		return;

	if (getGUIState()->m_shown_windows & MINIMAP)
	{
		getGUIState()->m_shown_windows &= ~MINIMAP;
	}
	else
	{
		//getGUIState()->m_shown_windows &= ~(PARTY | CHARINFO);

		getGUIState()->m_shown_windows |= MINIMAP;
	}

	// Geoeffnete Fenster haben sich geaendert
	m_modified |= WINDOWS_MODIFIED;
}


void Document::onButtonOptionsClicked()
{
	if (!checkSubwindowsAllowed() && getGUIState()->m_sheet ==  Document::GAME_SCREEN)
		return;

	if (getGUIState()->m_shown_windows & OPTIONS)
	{
		getGUIState()->m_shown_windows &= ~OPTIONS;
	}
	else
	{
		getGUIState()->m_shown_windows |= OPTIONS;
	}

	// Opened windows have changed.
	m_modified |= WINDOWS_MODIFIED;
}


void Document::onButtonCloseTrade()
{
	ClientCommand command;
	command.m_button = BUTTON_TRADE_END;
	command.m_id = 0;
	sendCommand(&command);
}

void Document::onButtonWorldmap()
{
	if (!checkSubwindowsAllowed() && getGUIState()->m_sheet ==  Document::GAME_SCREEN)
		return;

	if (getGUIState()->m_shown_windows & WORLDMAP)
	{
		if (getLocalPlayer()->isUsingWaypoint())
		{
			ClientCommand command;
			command.m_button = BUTTON_WAYPOINT_CHOSE;
			command.m_id = -1;
			sendCommand(&command);
		}
		else
		{
			getGUIState()->m_shown_windows &= ~	WORLDMAP;
			m_modified |= WINDOWS_MODIFIED;
		}
	}
	else
	{

		getGUIState()->m_shown_windows |= WORLDMAP;
		m_modified |= WINDOWS_MODIFIED;
	}


}

void Document::onButtonErrorDialogConfirm()
{
	getGUIState()->m_shown_windows &= ~MESSAGE;
	m_modified |= WINDOWS_MODIFIED;
    setState(SHUTDOWN_REQUEST);
}

void Document::onSkipDialogueTextClicked()
{
	ClientCommand command;
	command.m_button = BUTTON_SKIP_DIALOGUE_TEXT;
	sendCommand(&command);
}

void Document::onButtonWaypointClicked(int id)
{
	if (getLocalPlayer()->isUsingWaypoint())
	{
		ClientCommand command;
		command.m_button = BUTTON_WAYPOINT_CHOSE;
		command.m_id = id;
		sendCommand(&command);
	}
}

void Document::sendChatMessage(std::string msg)
{

	if (msg =="")
	{
		getGUIState()->m_shown_windows &= ~CHAT;
		m_modified |= WINDOWS_MODIFIED;
	}
	else
	{
		World::getWorld()->handleMessage(TranslatableString(msg));
	}
}


void Document::onSwapEquip()
{
	ClientCommand command;
	command.m_button = BUTTON_SWAP_EQUIP;
	sendCommand(&command);
}



void Document::setLeftAction(Action::ActionType act)
{

	// wenn kein Spieler gesetzt ist, dann keine Faehigkeit setzen
	// der lokale Spieler
	Player* player = static_cast<Player*>(World::getWorld()->getLocalPlayer());
	if (player==0)
		return;


	Action::ActionInfo* aci = Action::getActionInfo(act);

	// Testen ob die Faehigkeit zur Verfuegung steht
	if (!player->checkAbility(act))
	{
		// Faehigkeit steht nicht zur Verfuegung, abbrechen
		return;
	}

	// passive Faehigkeiten koennen nicht auf Maustasten gelegt werden
	if (aci->m_target_type == Action::PASSIVE)
		return;

	// Faehigkeiten die nicht auf einfachen Angriffen beruhen koennen nicht auf die linke Maustaste gelegt werden
	if (aci->m_base_action== "noaction")
		return;

	DEBUGX("Setting Action %s",act.c_str());
	m_gui_state.m_left_mouse_pressed = false;


	ClientCommand command;
	command.m_button = BUTTON_SET_LEFT_ACTION;
	command.m_action = act;
	sendCommand(&command);
}

void Document::setRightAction(Action::ActionType act)
{

	// wenn kein Spieler gesetzt ist, dann keine Faehigkeit setzen
	// der lokale Spieler
	Player* player = static_cast<Player*>(World::getWorld()->getLocalPlayer());
	if (player==0)
		return;

	Action::ActionInfo* aci = Action::getActionInfo(act);

	// Testen ob die Faehigkeit zur Verfuegung steht
	if (!player->checkAbility(act))
	{
		// Faehigkeit steht nicht zur Verfuegung, abbrechen
		return;
	}

	// passive Faehigkeiten koennen nicht auf Maustasten gelegt werden
	if (aci->m_target_type == Action::PASSIVE)
		return;


	DEBUGX("Setting Action %s",act.c_str());
	m_gui_state.m_right_mouse_pressed = false;

	ClientCommand command;
	command.m_button = BUTTON_SET_RIGHT_ACTION;
	if (getGUIState()->m_set_right_skill_alternate == true)
	{
		command.m_button = BUTTON_SET_ALTERNATE_RIGHT_ACTION;
	}
	command.m_action = act;
	sendCommand(&command);
}

void Document::onSwapRightAction()
{
	ClientCommand command;
	command.m_button = BUTTON_SWAP_RIGHT_ACTION;
	sendCommand(&command);
}

std::string Document::getAbilityDescription(Action::ActionType ability)
{
	std::ostringstream out_stream;
	out_stream.str("");

	// der lokale Spieler
	Player* player = static_cast<Player*>(World::getWorld()->getLocalPlayer());

	if (player !=0 )
	{
		// Struktur mit Informationen zur Aktion
		Action::ActionInfo* aci = Action::getActionInfo(ability);
		if (aci == 0)
		{
			out_stream << "no information on ability "<< ability;
			return out_stream.str();
		}
		// Name der Faehigkeit
		out_stream << Action::getName(ability);


		// Beschreibung
		out_stream << "\n" << Action::getDescription(ability);

		// Gibt an, ob der Spieler die Faehigkeit besitzt
		bool avlb = true;
		if (!player->checkAbility(ability))
		{
			// Spieler besitzt Faehigkeit nicht
			avlb = false;

			PlayerBasicData* pdata = ObjectFactory::getPlayerData(player->getSubtype());
			if (pdata !=0)
			{
				LearnableAbilityMap::iterator it;
				for (it = pdata->m_learnable_abilities.begin(); it != pdata->m_learnable_abilities.end(); ++it)
				{
					if (it->second.m_type == ability)
					{
						out_stream <<"\n"<< gettext("Required level")<<": " << it->second.m_req_level;
					}
				}
			}

		}

		// Timerinfo
		if (aci->m_timer_nr!=0)
		{
			out_stream <<"\n"<< gettext("Timer")<<" " << aci->m_timer_nr;
			out_stream <<"\n"<< gettext("Cast delay")<<": " << (int) aci->m_timer*0.001 << " s";
		}

		// Schaden
		// TODO: flexiblere Bedingung
		if (aci->m_target_type == Action::MELEE || aci->m_target_type == Action::RANGED || aci->m_target_type == Action::CIRCLE)
		{
			Damage dmg;
			player->calcDamage(ability,dmg);
			std::string dmgstring = dmg.getDamageString(Damage::ABILITY);
			if (dmgstring !="")
			{
				out_stream << "\n" << dmgstring;
			}
		}

	}

	return out_stream.str();
}

bool Document::onKeyPress(KeyCode key)
{

	// determine action mapped to the key
	ShortkeyDestination dest;
	dest = Options::getInstance()->getMappedDestination(key);

	if (m_gui_state.m_shown_windows & SKILLTREE && dest==0)
	{
		// Skilltree is displayed
		// keys are interpreted as attempt to create an ability shortkey

		Action::ActionType act = getGUIState()->m_hover_ability;
		if (act != "noaction" && getLocalPlayer()->checkAbility(act))
		{
			// Action exists and is available
			Action::ActionInfo* aci = Action::getActionInfo(act);

			// determine if the shortkey should map the ability for left or right mouse button
			bool left = true;
			if (getGUIState()->m_prefer_right_skill || aci->m_target_type == Action::PASSIVE || aci->m_base_action== "noaction")
			{
				left = false;
			}

			// determine the internal number of the ability
			int id =-1;
			Player* player = getLocalPlayer();
			std::map<int,LearnableAbility> &ablts = player->getLearnableAbilities();
			LearnableAbilityMap::iterator it;
			for (it = ablts.begin(); it != ablts.end(); ++it)
			{
				if (it->second.m_type == act)
				{
					id = it->first;
					break;
				}
			}

			if (id ==-1)
				return false;

			// create shortkey
			if (left)
			{
				installShortkey(key,(ShortkeyDestination) (USE_SKILL_LEFT+id));
				DEBUGX("left short key for action %s %i",act.c_str(), USE_SKILL_LEFT+id);
			}
			else
			{
				installShortkey(key,(ShortkeyDestination) (USE_SKILL_RIGHT+id));
				DEBUGX("right short key for action %s %i",act.c_str(),USE_SKILL_RIGHT+id );
			}
			return true;
		}

	}


	// if key was not mapped to a shortkey
	// check ability shortkeys
	if (dest == 0)
	{
		std::map<KeyCode, ShortkeyDestination>::iterator it = m_ability_shortkey_map.find(key);
		if (it != m_ability_shortkey_map.end())
			dest = it->second;
	}


	// execute the action, if any mapping was found
	if (dest != 0)
	{
		if (dest == SHOW_INVENTORY)
		{
			onButtonInventoryClicked();
		}
		else if (dest == SHOW_CHARINFO)
		{
			onButtonCharInfoClicked();
		}
		else if (dest == SHOW_QUESTINFO)
		{
			onButtonQuestInfoClicked();
		}
		else if (dest == SHOW_SKILLTREE)
		{
			onButtonSkilltreeClicked();
		}
		else if (dest == SHOW_MINIMAP)
		{
			onButtonMinimapClicked();
		}
		else if (dest == SHOW_OPTIONS)
		{
			onButtonOptionsClicked();
		}
		else if (dest == SWAP_EQUIP)
		{
			onSwapEquip();
		}
		else if (dest == SWAP_RIGHT_ACTION)
		{
			onSwapRightAction();
		}
		else if (dest == SHOW_ITEMLABELS)
		{
			getGUIState()->m_item_labels = true;
		}
		else if (dest >=USE_POTION && dest < USE_POTION+10)
		{
			onItemRightClick(Equipement::BELT_ITEMS + (dest-USE_POTION));
		}
		else if (dest>=USE_SKILL_LEFT && dest <USE_SKILL_RIGHT)
		{
			Player* player = getLocalPlayer();
			if (player != 0)
			{
				std::map<int,LearnableAbility> &ablts = player->getLearnableAbilities();

				if (ablts.count(dest-USE_SKILL_LEFT) >0)
				{
					setLeftAction(ablts[dest-USE_SKILL_LEFT].m_type);
				}
			}
		}
		else if (dest>=USE_SKILL_RIGHT && dest <USE_SKILL_RIGHT+200)
		{
			Player* player = getLocalPlayer();
			if (player != 0)
			{
				std::map<int,LearnableAbility> &ablts = player->getLearnableAbilities();
				if (ablts.count(dest-USE_SKILL_RIGHT) >0)
				{
					setRightAction(ablts[dest-USE_SKILL_RIGHT].m_type);
				}
			}
		}
		else if(dest == SHOW_PARTYMENU)
		{
			onButtonPartyInfoClicked();
		}
		else if(dest == SHOW_CHATBOX)
		{
			onButtonOpenChatClicked();

		}
		else if (dest == SHOW_MINIMAP)
		{
			if (getGUIState()->m_shown_windows & MINIMAP)
			{
				getGUIState()->m_shown_windows &= ~MINIMAP;
			}
			else
			{
				//getGUIState()->m_shown_windows &= ~(PARTY | QUEST_INFO);

				getGUIState()->m_shown_windows |= MINIMAP;
			}
			m_modified |= WINDOWS_MODIFIED;
		}
		else if(dest == SHOW_CHATBOX_NO_TOGGLE)
		{
			// Chatfenster oeffnen wenn es gerade geschlossen ist
			if (!(getGUIState()->m_shown_windows & CHAT))
			{
				getGUIState()->m_shown_windows |= CHAT;

				// Geoeffnete Fenster haben sich geaendert
				m_modified |= WINDOWS_MODIFIED;

			}

		}
		else if (dest == CLOSE_ALL)
		{
			if (m_gui_state.m_shown_windows == NO_WINDOWS)
			{
				onButtonSaveExitClicked();
			}
			else if (m_gui_state.m_shown_windows & WORLDMAP)
			{
				onButtonWorldmap();
			}
			else if (m_gui_state.m_shown_windows & TRADE)
			{
				onButtonCloseTrade();
			}
			else if (m_gui_state.m_shown_windows & QUESTIONBOX)
			{
				// Versuch, aktuelles Gespraech abzubrechen
				onAnswerClick(-1);
			}
			else if (m_gui_state.m_shown_windows & (HOST_GAME | JOIN_GAME | CHAR_CREATE ))
			{
				m_gui_state.m_shown_windows = Document::START_MENU;
				m_modified |= WINDOWS_MODIFIED;
			}
			else
			{
				m_gui_state.m_shown_windows =  NO_WINDOWS;
			// Geoeffnete Fenster haben sich geaendert
				m_modified |= WINDOWS_MODIFIED;
			}
		}
		else if (dest >= CHEAT && dest < CHEAT+10)
		{
			emitDebugSignal(dest-CHEAT);
		}
		else
		{
			return false;
		}

		return true;

	}
	return false;
}

bool  Document::onKeyRelease(KeyCode key)
{
	if (m_gui_state.m_pressed_key != 0)
	{
		m_gui_state.m_pressed_key = 0;
	}

	ShortkeyDestination dest = Options::getInstance()->getMappedDestination(key);
	if (dest != 0)
	{
		if (dest == SHOW_ITEMLABELS)
		{
			getGUIState()->m_item_labels = false;
		}
	}
	return true;
}

void Document::update(float time)
{
	// Welt eine Zeitscheibe weiter laufen lassen
	if (World::getWorld() != 0)
	{
		// game is paused for single player if save and exit window is shown
		if (!((getGUIState()->m_shown_windows & SAVE_EXIT) && m_single_player))
		{
			World::getWorld()->update(time);
		}

	}

	if (m_options_timer.getTime() > 60000)
	{
		saveSettings();
		m_options_timer.start();
	}

	DEBUGX("modified is %i",m_modified);
	DEBUGX("State is %i",m_state);
	NetStatus status;
	switch (m_state)
	{
		case INACTIVE:
			break;

		case START_GAME:
			startGame(m_server);
			break;

		case LOAD_SAVEGAME:
			status = NET_OK;
			if (!m_server)
			{
				status = World::getWorld()->getNetwork()->getSlotStatus();
			}
			if (m_server || status == NET_CONNECTED)
			{
				loadSavegame();
			}
			if (status == NET_REJECTED || status == NET_SLOTS_FULL || status == NET_TIMEOUT)
			{
				// Verbindung abgelehnt
				//m_state = SHUTDOWN;
                getGUIState()->m_shown_windows = MESSAGE;
                m_modified |= WINDOWS_MODIFIED;
			}


		case RUNNING:
			updateContent(time);

			if (m_save_timer.getTime() > 60000)
			{
				m_save_timer.start();

				writeSavegame();
				DEBUGX("saving");
			}

			if (!m_server)
			{
				if (World::getWorld()->getNetwork()->getSlotStatus() == NET_TIMEOUT)
				{
					setState(INACTIVE);
                    getGUIState()->m_shown_windows = MESSAGE;
                    m_modified |= WINDOWS_MODIFIED;
				}
			}

			break;

		case SHUTDOWN_REQUEST:
			updateContent(time);
			m_shutdown_timer += time;

			setState(SHUTDOWN_WRITE_SAVEGAME);

			if (m_shutdown_timer>400)
			{
				setState(SHUTDOWN);
			}
			break;

		case SHUTDOWN_WRITE_SAVEGAME:
			// Savegame schreiben
			writeSavegame();

			m_state = SHUTDOWN;
			break;

		case SHUTDOWN:
			// Spielwelt abschalten

			World::deleteWorld();

			getGUIState()->m_sheet= MAIN_MENU;
			getGUIState()->m_shown_windows = START_MENU;
			m_modified =WINDOWS_MODIFIED;

			m_state = INACTIVE;

			m_modified =GUISHEET_MODIFIED | WINDOWS_MODIFIED;

			saveSettings();
			break;

		default:
			break;
	}
	DEBUGX("done");
}

void Document::updateContent(float time)
{
	DEBUGX("update content");

	Player* player = static_cast<Player*>(World::getWorld()->getLocalPlayer());
	if (player==0)
	{
		DEBUGX("no local player");
		return;
	}

	// Fenster einstellen, die automatisch geoeffnet werden
	int wmask = getGUIState()->m_shown_windows;
	if (player->getTradeInfo().m_trade_partner != 0 && getGUIState()->m_shown_windows != SAVE_EXIT)
	{
		if (getGUIState()->m_shown_windows != (TRADE | INVENTORY))
		{
			getGUIState()->m_shown_windows = TRADE | INVENTORY;
			m_modified |= WINDOWS_MODIFIED;
		}
	}
	else if (player->getDialogueId() != 0
				|| (player->getRegion() !=0 && player->getRegion()->getCutsceneMode () == true))
	{
		// Chat ist fuer Debugging zugeschaltet
		if ((getGUIState()->m_shown_windows & (~(QUESTIONBOX | SAVE_EXIT | CHAT))) != 0)
		{
			getGUIState()->m_shown_windows &= (QUESTIONBOX  | SAVE_EXIT | CHAT);
			m_modified |= WINDOWS_MODIFIED;
		}
	}

	if (player->getTradeInfo().m_trade_partner == 0 && (wmask & TRADE))
	{
		getGUIState()->m_shown_windows &= ~(TRADE | INVENTORY);
		m_modified |= WINDOWS_MODIFIED;
	}

	if (player->isUsingWaypoint() && getGUIState()->m_shown_windows !=	WORLDMAP)
	{
		getGUIState()->m_shown_windows = WORLDMAP;
		m_modified |= WINDOWS_MODIFIED;
	}

	if (!player->isUsingWaypoint() && (wmask & WORLDMAP))
	{
		getGUIState()->m_shown_windows &= ~WORLDMAP;
		m_modified |= WINDOWS_MODIFIED;
	}

	/*
	if (player->getRegion() !=0 && player->getRegion()->getCutsceneMode())
	{
		if ((getGUIState()->m_shown_windows & ~SAVE_EXIT) != 0)
		{
			getGUIState()->m_shown_windows &= SAVE_EXIT;
			m_modified |= WINDOWS_MODIFIED;
		}
	}
*/
	if (m_gui_state.m_left_mouse_pressed)
	{
		m_gui_state.m_left_mouse_pressed_time += time;
		if (m_gui_state.m_left_mouse_pressed_time >= 200 ||
				  (m_gui_state.m_left_mouse_pressed_time >=40 && player->getAction()->m_type == "noaction") )
		{
			ClientCommand command;

			command.m_button=LEFT_MOUSE_BUTTON;
			if (m_gui_state.m_shift_hold)
			{
				command.m_button=LEFT_SHIFT_MOUSE_BUTTON;
			}
			updateClickedObjectId();

			command.m_goal = m_gui_state.m_clicked;
			command.m_id = m_gui_state.m_clicked_object_id;
			command.m_action = player->getLeftAction();
			command.m_number=0;

			m_gui_state.m_left_mouse_pressed_time=0;
			sendCommand(&command);

		}
	}

	if (m_gui_state.m_right_mouse_pressed)
	{
		DEBUGX("rechte Maustaste festgehalten");
		m_gui_state.m_right_mouse_pressed_time += time;

		if (m_gui_state.m_right_mouse_pressed_time>= 200 ||
				  (m_gui_state.m_left_mouse_pressed_time >=40 && player->getAction()->m_type == "noaction"))
		{
			ClientCommand command;
			updateClickedObjectId();

			command.m_button=RIGHT_MOUSE_BUTTON;
			command.m_goal = m_gui_state.m_clicked;
			command.m_id = m_gui_state.m_clicked_object_id;
			command.m_action = player->getRightAction();
			command.m_number=0;

			m_gui_state.m_right_mouse_pressed_time=0;
			sendCommand(&command);

		}

	}




}

void Document::updateClickedObjectId()
{
	Player* player = static_cast<Player*>(World::getWorld()->getLocalPlayer());
	Region* reg = player->getRegion();
	if (reg == 0)
		return;

	// select new target if the old one is gone or inactive
	if (m_gui_state.m_clicked_object_id != 0 && m_gui_state.m_cursor_object_id != 0)
	{
		GameObject* go = reg->getObject(m_gui_state.m_clicked_object_id);
		if (go == 0 || go->getState() != WorldObject::STATE_ACTIVE)
		{
			// TODO: better check if the target is appropriate for the action
			go = reg->getObject(m_gui_state.m_cursor_object_id);
			Creature* cr = dynamic_cast<Creature*>(go);
			if ( cr != 0 && World::getWorld()->getRelation(player->getFraction(),cr) == Fraction::HOSTILE)
			{
				m_gui_state.m_clicked_object_id = m_gui_state.m_cursor_object_id;
				DEBUGX("new target %i %i",m_gui_state.m_clicked_object_id, player->getId());
			}
		}
	}
}

void Document::writeSavegame(bool writeShortkeys)
{
	if (getLocalPlayer()==0)
		return;

	CharConv* save;



	std::stringstream* pstr = new std::stringstream;
	save = new StdStreamConv(pstr);

	getLocalPlayer()->toSavegame(save);

	// Shortkeys hinzufuegen
	if (writeShortkeys)
	{
		
		ShortkeyMap::iterator it;
		int nr =0;
		for (it = m_ability_shortkey_map.begin(); it != m_ability_shortkey_map.end(); ++it)
		{
			if (it->second >= USE_SKILL_LEFT && it->second < USE_SKILL_RIGHT + 200)
				nr ++;
		}
		save->printNewline();
		save->toBuffer(nr);
		save->printNewline();
		for (it = m_ability_shortkey_map.begin(); it != m_ability_shortkey_map.end(); ++it)
		{
			if (it->second >= USE_SKILL_LEFT && it->second < USE_SKILL_RIGHT + 200)
			{
				save->toBuffer(static_cast<short>(it->first));
				save->toBuffer(static_cast<short>(it->second));
				save->printNewline();
			}
		}
	}
	else
	{
		save->printNewline();
		save->toBuffer(0);
	}

	// Savegame schreiben (ansynchron)
	std::pair<Document*, CharConv*>* param = new std::pair<Document*, CharConv*>(this,save);
	
	// Savegame schreiben
	std::stringstream* stream = dynamic_cast<std::stringstream*> (static_cast<StdStreamConv*>(save)->getStream());
	char* bp=0;
	int len=0;
	char bin ='0';
	
	// binary savegames are not used anymore
	
	
	// Daten byteweise in Datei schreiben
	std::ofstream file;
	if (bin =='1')
	{
		file.open(m_save_file.c_str(),std::ios::out | std::ios::binary);
	}
	else
	{
		file.open(m_save_file.c_str());
	}
	if ( file.is_open() )
	{
		file << bin;
		if (bin =='1')
		{
			file.write((char*) (&len),4);
			file.write(bp,len);
		}
		else
		{
			file << stream->str();
			DEBUGX("save: \n %s",stream->str().c_str());
		}
		
		file.close();
	}
	else
	{
		ERRORMSG("cannot open save file: %s",m_save_file.c_str());
	}
	if (stream != 0)
	{
		delete stream;
	}
	

}


void Document::saveSettings()
{
    Options::getInstance()->writeToFile(SumwarsHelper::userPath() + "/options.xml");
}

void Document::loadSettings()
{
    Options::getInstance()->readFromFile(SumwarsHelper::userPath() + "/options.xml");
}

Player*  Document::getLocalPlayer()
{
	if (World::getWorld() ==0)
		return m_temp_player;

	return static_cast<Player*>(World::getWorld()->getLocalPlayer());
}
