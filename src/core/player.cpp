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

#include "player.h"
#include "eventsystem.h"
#include "itemfactory.h"
#include "options.h"
#include <tinyxml.h>

PlayerCamera::PlayerCamera()
{
	m_distance = 20;
	m_theta = 70;
	m_phi =270;

	m_goal_distance = 20;
	m_goal_theta = 70;
	m_goal_phi =270;
	m_time =0;
}

void PlayerCamera::moveTo(float distance, float theta, float phi, float time)
{
    float max_distance;
    float min_theta;
    max_distance = 20;
    min_theta = 45;
	// set print_fps to true to get some data
	if (Options::getInstance()->getDebugOption("restrict_camera","true") == "false")
	{
		max_distance = 60;
		min_theta = 5;
	}

	m_goal_distance = MathHelper::Max(MathHelper::Min(distance,max_distance),5.0f);
	m_goal_theta = MathHelper::Max(MathHelper::Min(theta,90.0f),min_theta);
	m_goal_phi = fmod(phi+360,360);

	// Phi so anpassen, dass jeweils der kuerzere Teilkreis genutzt wird
	if (m_phi - m_goal_phi > 180)
		m_phi -= 360;

	if (m_phi - m_goal_phi < -180)
		m_phi += 360;

	m_time = time;
}

void PlayerCamera::update(float time)
{
	if (m_time <= time)
	{
		m_distance = m_goal_distance;
		m_theta = m_goal_theta;
		m_phi = m_goal_phi;
	}
	else
	{
		float frac = time/m_time;
		m_distance = (1-frac)*m_distance + frac* m_goal_distance;
		m_theta =  (1-frac)*m_theta + frac*m_goal_theta;
		m_phi = (1-frac)*m_phi + frac* m_goal_phi;

		m_time -= time;
	}
}

//Constructors/Destructors

Player::Player( int id, Subtype subtype) : Creature( id)
{
	setSubtype(subtype);
	bool tmp=Player::init();
	if (!tmp)
	{
		DEBUG("Initialiserung des Players fehlgeschlagen!");
	}
}


Player::~Player()
{
	delete m_stash;
}

bool Player::destroy()
{
	DEBUGX("leave Party");
	DEBUGX("destroy");
	World::getWorld()->getParty(m_fraction)->removeMember(getId());
	return Creature::destroy();
}

//Operations

bool Player::init()
{
	if (World::getWorld() !=0)
	{
		m_revive_position = World::getWorld()->getPlayerStartLocation();
	}
	else
	{
		m_revive_position.first = "none";
		m_revive_position.second = "none";

	}

	//eigene Initialisierung
	CreatureBaseAttr* bas = getBaseAttr();
	CreatureDynAttr* dyn = getDynAttr();
	FightStatistic& fstat= getFightStatistic();

	setType("PLAYER");
	m_race = "human";

	getShape()->m_type = Shape::CIRCLE;
	getShape()->m_radius = 0.5;
	setLayer(LAYER_BASE | LAYER_AIR);
	getShape()->m_angle =0;


	m_attribute_points=0;
	m_skill_points=0;

	m_fraction = Fraction::NOFRACTION;

	m_secondary_equip = false;

	m_equipement = new Equipement(5,14,30);
	m_stash = new Equipement(15,21,30);
	m_equipement->setGold(50);

	m_candidate_party = -1;
	m_message_clear_timer =0;

	dyn->m_experience=0;

	fstat.m_last_attacker="";
	fstat.m_last_attacked="";
	fstat.m_hit_chance =0;
	fstat.m_damage_dealt_perc =0;
	fstat.m_damage_got_perc =0;
	fstat.m_block_chance =0;

	m_using_waypoint = false;


	PlayerBasicData* pdata = ObjectFactory::getPlayerData(getSubtype());

	if (pdata == 0)
		return true;

	(*bas) = pdata->m_base_attr;
	m_base_action = pdata->m_base_ability;
	m_left_action = pdata->m_base_ability;
	m_right_action = pdata->m_base_ability;
	m_right_alternate_action = pdata->m_base_ability;
	bas->m_abilities[pdata->m_base_ability].m_timer_nr = -1;
	m_learnable_abilities = pdata->m_learnable_abilities;

	std::list<GameObject::Subtype>::iterator it;
	Item* item;
	for( it = pdata->m_start_items.begin(); it != pdata->m_start_items.end(); ++it)
	{
		item = ItemFactory::createItem(ItemFactory::getBaseType(*it),*it);
		insertItem(item,true);
	}

	bas->m_level=1;
	bas->m_max_experience = 100;
	getBaseAttrMod()->m_max_health = getBaseAttr()->m_max_health;

	// Modifizierte Basisattribute erzeugen
	calcBaseAttrMod();

	clearNetEventMask();

	/*
	TiXmlDocument doc;
	TiXmlElement * elem;
	TiXmlElement * elem2;
	LearnableAbilityMap::iterator it;
	Action::ActionInfo* aci;
	std::list<Action::ActionType>::iterator jt;

	for (it = getLearnableAbilities().begin(); it != getLearnableAbilities().end(); ++it)
	{
		aci = Action::getActionInfo(it->second.m_type);
		elem = new TiXmlElement("LearnableAbility");
		elem->SetAttribute("type",it->second.m_type.c_str());
		elem->SetAttribute("skilltree_tab",it->second.m_skilltree_tab);
		elem->SetDoubleAttribute("skilltree_posx",it->second.m_skilltree_position.m_x);
		elem->SetDoubleAttribute("skilltree_posy",it->second.m_skilltree_position.m_y);
		elem->SetAttribute("required_level",aci->m_req_level);
		for(int i=0; i<3; i++)
		{
			if (aci->m_req_ability[i] != "noaction")
			{
				elem2 = new TiXmlElement("RequiredAbility");
				elem2->SetAttribute("type",aci->m_req_ability[i].c_str());
				elem->LinkEndChild(elem2);
			}
		}
		doc.LinkEndChild(elem);
	}
	doc.SaveFile("tmp.xml");
	*/
	return true;
}

bool Player::canBeAttacked()
{
	return (Creature::canBeAttacked() && !m_using_waypoint);
}

void  Player::revive()
{
	DEBUG("reviving");

	getRegion()->changeObjectGroup(this,PLAYER);
	getDynAttr()->m_health = getBaseAttrMod()->m_max_health;

	// keine Aktion/Kommando
	getAction()->m_type = "noaction";
	getCommand()->m_type = "noaction";
	getCommand()->m_damage_mult=1;
	getNextCommand()->m_type = "noaction";

	// Bewegung auf 0 setzen
	setSpeed(Vector(0,0));

	// Wegfindeinformationen auf 0 setzen
	m_small_path_info=0;
	m_small_flying_path_info=0;
	m_medium_path_info=0;
	m_big_path_info=0;
	m_path_info=0;

	// Timer nullen
	for (int i=0; i<NR_TIMERS; i++)
	{
		m_timers[i] = 0;
		m_timers_max[i] = 0;
	}	

	getDynAttr()->m_last_attacker_id=0;

	// Statusmods auf null setzen
	int i;
	for (i=0;i<NR_STATUS_MODS;i++)
	{
		getDynAttr()->m_status_mod_time[i]=0;
		getDynAttr()->m_status_mod_immune_time[i]=0;

	}
	getDynAttr()->m_temp_mods.clear();

	for (i=0;i<NR_EFFECTS;i++)
	{
		getDynAttr()->m_effect_time[i]=0;
	}

	setState(STATE_ACTIVE,false);
	clearCommand(false,true);
	getNextCommand()->m_type = "noaction";
	getNextCommand()->m_damage_mult = 1;
	getNextCommand()->m_goal = Vector(0,0);
	getNextCommand()->m_goal_object_id =0;
	getNextCommand()->m_range =1;

	getAction()->m_action_equip = Action::NO_WEAPON;
	getAction()->m_time =2000;
	getAction()->m_elapsed_time =0;

	calcBaseAttrMod();
}

bool Player::onGamefieldClick(ClientCommand* command)
{
	// keine Aktionen waehrend eines Dialogs oder Handel
	if (getDialogueId()!=0 || getTradeInfo().m_trade_partner !=0 || isUsingWaypoint())
	{
		return true;
	}

	if (getRegion()==0)
		return true;

	if (getRegion()->getCutsceneMode())
	{
		return true;
	}


	Command* com = getNextCommand();
	int dist;
	bool meleedir = false;
	WorldObject* wo;
	Fraction::Relation rel;

	// Actionen auf self brauchen kein Zielobjekt
	Action::ActionInfo* ainfo = Action::getActionInfo(command->m_action);
	if (ainfo == 0)
	{
		ERRORMSG("action information missing for action %s",command->m_action.c_str());
		dist = Action::SELF;
	}
	else
	{
		dist = ainfo->m_target_type;
	}

	if ( dist == Action::SELF || dist == Action::PARTY_MULTI || dist == Action::PARTY)
		command->m_id=0;


	// bei Take Item wird in der ID die ID des Items gespeichert
	if ( command->m_id!=0 && command->m_action != "take_item")
	{

		DEBUGX("Kommando erhalten, zielid: %i",command->m_id);
		wo = getRegion()->getObject(command->m_id);

		// Unterscheidung Zielobject vs kein Zielobject
		if (wo !=0)
		{
			rel = World::getWorld()->getRelation(m_fraction,wo);

			if (command->m_button == LEFT_MOUSE_BUTTON)
			{
				// Linke Maustaste: Aktion nur ausfuehren, wenn tatsaechlich ein passendenes Objekt vorhanden ist

				// Fuer Aktionen der Art Nah oder Fernkampf braucht man eine feindliche Kreatur
				if (rel == Fraction::HOSTILE && (dist == Action::MELEE || dist == Action::RANGED || dist == Action::CIRCLE))
				{

					if (wo->getState()==STATE_ACTIVE)
					{

						com->m_type =command->m_action;

						com->m_goal_object_id = command->m_id;
						com->m_goal = command->m_goal;
						com->m_range = getBaseAttrMod()->m_attack_range;
						DEBUGX("action range %f",getCommand()->m_range);
					}
					else
					{

					}
				}

				if (rel == Fraction::ALLIED && dist == Action::PARTY)
				{
					if (wo->getState()==STATE_ACTIVE)
					{
						if (getPosition().distanceTo(command->m_goal) < 16)
						{
							com->m_type =command->m_action;

							com->m_goal_object_id = command->m_id;
							com->m_goal = command->m_goal;
							com->m_range = getBaseAttrMod()->m_attack_range;
						}
						else
						{
							DEBUG("goal is too far away");
						}
					}
				}
				else if (wo->isCreature() && ( rel == Fraction::ALLIED || rel == Fraction::NEUTRAL) && wo != this)
				{
					com->m_type ="speak";

					com->m_goal_object_id = command->m_id;
					com->m_goal = command->m_goal;
					com->m_range = 3;
				}


				if (!wo->isCreature())
				{
					// festes Objekt benutzen
					com->m_type ="use";
					com->m_goal_object_id = command->m_id;
					com->m_range = 1.0;
					DEBUGX("use Object %i",command->m_id);
				}

			}
			else if (command->m_button == RIGHT_MOUSE_BUTTON)
			{
				com->m_type =command->m_action;
				com->m_goal_object_id = command->m_id;
				com->m_goal = command->m_goal;
				com->m_range = getBaseAttrMod()->m_attack_range;
				com->m_goal_object_id = command->m_id;
			}
			else if (command->m_button == LEFT_SHIFT_MOUSE_BUTTON)
			{
				com->m_type = command->m_action;
				com->m_goal = command->m_goal;
				com->m_range = getBaseAttrMod()->m_attack_range;
				com->m_goal_object_id =0;
				if (dist == Action::MELEE)
				{

					meleedir = true;
				}
				addToNetEventMask(NetEvent::DATA_NEXT_COMMAND);
			}
		}
		else
		{
			// Objekt existiert nicht ( Fehler?)
			com->m_type ="noaction";
		}

	}
	else
	{
		// Kein Zielobjekt gegeben
		DEBUGX("Kommando erhalten, Ziel (%f,%f) button %i action %s dist %i",command->m_goal.m_x, command->m_goal.m_y,command->m_button,command->m_action.c_str(),dist);

		if (command->m_button == LEFT_MOUSE_BUTTON)
		{

			// linke Maustaste -> immer laufen
			if (command->m_action == "take_item")
			{
				com->m_type = "take_item";
				com->m_goal_object_id =command->m_id;
				DEBUGX("take item");
			}
			else
			{
				com->m_type = "walk";
				com->m_goal_object_id =0;
				com->m_range = getShape()->m_radius;
			}
			com->m_goal = command->m_goal;

		}
		else if (dist == Action::SELF || dist == Action::CIRCLE)
		{
			// Aktionen auf selbst auf left_shift und rechts ausfuehren
			com->m_type = command->m_action;
			com->m_goal = Vector(0,0);
			com->m_goal_object_id =0;
		}
		else if (dist == Action::PARTY_MULTI || dist == Action::PARTY)
		{
			if (getPosition().distanceTo(command->m_goal) < 16)
			{
				com->m_type = command->m_action;
				com->m_goal = command->m_goal;
				com->m_goal_object_id =0;
			}
		}
		else if (dist == Action::RANGED)
		{
			// Fernkampf immer starten
			com->m_type = command->m_action;
			com->m_goal = command->m_goal;
			com->m_range = getBaseAttrMod()->m_attack_range;
			com->m_goal_object_id =0;
		}
		else if (dist == Action::MELEE && (command->m_button == LEFT_SHIFT_MOUSE_BUTTON  || command->m_button == RIGHT_MOUSE_BUTTON))
		{
			// Nahkampf nur dann starten, wenn shift gedrueckt wurde
			com->m_type = command->m_action;
			com->m_goal = command->m_goal;
			com->m_range = getBaseAttrMod()->m_attack_range;
			com->m_goal_object_id =0;
			meleedir = true;
		}
		else
		{
			com->m_type ="noaction";
		}

	}

	if (!checkAbility(com->m_type))
	{
		if (ainfo != 0)
		{
			DEBUG("Basisaktion von %s verwendet",com->m_type.c_str());
			com->m_type = ainfo->m_base_action;
		}
	}

	if (meleedir)
	{
		DEBUGX("meleedir");
		Vector dir;
		dir = com->m_goal - getShape()->m_center;
		float range = com->m_range + getShape()->m_radius;
		if ( dir.getLength()>= range)
		{
			dir.normalize();
			dir *= 0.95*range;
			com->m_goal = getShape()->m_center + dir;
		}
	}

	addToNetEventMask(NetEvent::DATA_NEXT_COMMAND);
	DEBUGX("resulting command %s goal %f %f id %i",com->m_type.c_str(),com->m_goal.m_x,com->m_goal.m_y, com->m_goal_object_id);

	return true;
}


bool Player::onItemClick(ClientCommand* command)
{
	// no actions for dead players
	if (getState() != STATE_ACTIVE)
	{
		return false;
	}
	
	short pos = command->m_id;

	if (m_secondary_equip)
	{
		if (pos == Equipement::WEAPON)
			pos = Equipement::WEAPON2;

		if (pos == Equipement::SHIELD)
			pos = Equipement::SHIELD2;

	}

	// das Item welches, des Spieler aktuell *in der Hand* hat
	Item* it = m_equipement->getItem(Equipement::CURSOR_ITEM);

	DEBUGX("got Item %p",it);

	if (command->m_button== BUTTON_ITEM_LEFT)
	{
		// Item soll verschoben werden
		if (it!=0)
		{

			// zeigt an, ob der Vorraussetzungen erfuellt sind
			bool req = true;

			if (pos <Equipement::CURSOR_ITEM)
			{

				// Item soll als Ausruestungsgegenstand benutzt werden
				req = checkItemRequirements(it).m_overall;
				pos = m_equipement-> getItemEquipPosition(it, m_secondary_equip, (Equipement::Position) pos);
			}
			else
			{
			// Item soll im Inventar abgelegt werden
			DEBUGX("swap in inventory");

			// Groesse die das Item haben muss
				Item::Size size = Item::BIG;

				if (pos >= Equipement::MEDIUM_ITEMS)
					size = Item::MEDIUM;
				if (pos >= Equipement::SMALL_ITEMS)
					size = Item::SMALL;

				if (it->m_size != size)
				{
					// Groesse stimmt nicht
					// Item vom Cursor nehmen
					Item* itm =0;
					m_equipement->swapItem(itm, Equipement::CURSOR_ITEM);

					// ins Inventar einfuegen
					insertItem(itm,false);
					req = false;
				}
				else if (pos>=Equipement::BELT_ITEMS && pos < Equipement::BELT_ITEMS_END)
				{
					if (it->m_useup_effect == 0 && it->m_subtype != "town_portal")
					{
						// non-consumable items must not placed in belt
						Item* itm =0;
						m_equipement->swapItem(itm, Equipement::CURSOR_ITEM);

						// ins Inventar einfuegen
						insertItem(itm,false);
						req = false;
					}
				}
			}

			if (req == false)
			{
			// Vorraussetzungen nicht erfuellt
				return true;
			}
		}

		if (pos == Equipement::NONE)
			return true;

		// swap Item into equipement
		if (pos < Equipement::CURSOR_ITEM)
		{
			swapEquipItems(Equipement::CURSOR_ITEM, (Equipement::Position) pos);
		}
		else
		{
			m_equipement->swapCursorItem(pos);
		}
	}
	else
	{
		// Item soll verbraucht werden
		// oder verkauft
		it = m_equipement->getItem(pos);
		if (it!=0)
		{
			if (getTradePartner() != 0)
			{
				bool stashtrade = false;
				if (getTradePartner() == this)
				{
					stashtrade = true;
				}
				
				it =0;
				int gold = getEquipement()->getGold();
				getEquipement()->swapItem(it,pos);
				if (!stashtrade)
				{
					getTradePartner()->buyItem(it,gold);
					getEquipement()->setGold(gold);
				}
				else
				{
					int pos = getStash()->insertItem(it,false,false);
					if (pos != Equipement::NONE)
					{
						it = 0;
					}
				}
				
				// wieder hinein getauscht, wenn Handel nicht erfolgreich
				getEquipement()->swapItem(it,pos);
				
				return true;
			}

			if (it->m_useup_effect==0)
			{
				// Item kann nicht verbraucht werden
				if (it->m_subtype =="town_portal" && getRegion() != 0 && getRegion()->getIdString() !=  m_revive_position.first)
				{
					std::stringstream stream;
					stream << "#townportal" << getId();
					getRegion()->setLocation(stream.str(), getShape()->m_center);

					RegionLocation regloc;
					regloc.first = getRegion()->getIdString();
					regloc.second = stream.str();
					setPortalPosition(regloc);

					getRegion()->insertPlayerTeleport(getId(), m_revive_position);
					clearCommand(true,true);
					
					// delete item
					Item* swap=0;
					m_equipement->swapItem(swap,pos);
					delete it;
				}
				else
				{
					// equip Item to Autoposition
					if (pos >= Equipement::CURSOR_ITEM)
					{
						bool req = checkItemRequirements(it).m_overall;
						Equipement::Position pos2 = m_equipement-> getItemEquipPosition(it, m_secondary_equip);
						
						if (req && pos2 <= Equipement::CURSOR_ITEM && pos2 != Equipement::NONE)
						{
							swapEquipItems((Equipement::Position) pos,pos2);
						}
						
					}
				}
				return true;
			}
			else
			{
				// Wirkung des Items
				applyDynAttrMod(it->m_useup_effect);
				
				// play sound
				getRegion()->playSound("use_potion", getShape()->m_center, 1.0, false);
			}

			// entfernen aus dem Inventar durch Tausch mit einem Nullpointer
			Item* swap=0;
			m_equipement->swapItem(swap,pos);

			// Typ und Subtyp des Items
			Item::Type type = it->m_type;
			Item::Subtype stype = it->m_subtype;

			// Item loeschen
			delete it;
			
			if (pos>=Equipement::BELT_ITEMS && pos < Equipement::BELT_ITEMS_END)
			{
				// Item befand sich im Guertel
				// suchen nach einem aehnlichen Item zum nachruecken
				// TODO: magic number: # small items -1
				for (int i=29; i>=0 ;i--)
				{
					it = m_equipement->getItem(Equipement::SMALL_ITEMS+i);

					if (it==0)
						continue;

					if (it->m_type == type && it->m_subtype == stype)
					{
						// Item zum tauschen gefunden
						swap=0;

						// von der aktuellen Position wegnehmen
						m_equipement->swapItem(swap,Equipement::SMALL_ITEMS+i);
						// an der Position des verbrauchten Items einfuegen
						m_equipement->swapItem(swap,pos);

						break;
					}
				}
			}
		}
	}
	return true;
}

void Player::swapEquipItems(Equipement::Position pos1, Equipement::Position pos2)
{
	if (pos1 < Equipement::CURSOR_ITEM || pos2 >=  Equipement::CURSOR_ITEM)
	{
		WARNING("function swapEquipItems requires a non equiped Items as argument 1 and an equipement slot as argument 2");
		return;
	}
	
	// do the swapping
	Item* itm=0;
	m_equipement->swapItem(itm,pos1);	// 0 in pos1, itm1 in itm
	m_equipement->swapItem(itm,pos2);	// itm1 in pos2, itm2, in itm
	m_equipement->swapItem(itm,pos1);	// itm2 in pos, 0 in itm
	
	if (World::getWorld()->isServer() && pos2 <Equipement::CURSOR_ITEM)
	{
		NetEvent event;
		event.m_id = getId();
		event.m_data = pos2;

		if (m_equipement->getItem(pos2) == 0)
		{
			event.m_type = NetEvent::PLAYER_ITEM_REMOVE;
		}
		else
		{
			event.m_type = NetEvent::PLAYER_ITEM_INSERT;
		}
		DEBUGX("event: %i at %i",event.m_type,event.m_data);
		World::getWorld()->insertNetEvent(event);
	}

	if (pos2==Equipement::WEAPON || pos2 == Equipement::WEAPON2)
	{
			// Waffe wurde angelegt, bei zweihaendig testen ob der Schild Slot frei ist
		Item* weapon = getWeapon();
		if (weapon!=0 && weapon->m_weapon_attr != 0 && weapon->m_weapon_attr->m_two_handed)
		{
			if (getShield()!=0)
			{
					// Schild Slot ist nicht frei
					// Schild muss entfernt werden
				short shpos= Equipement::SHIELD;
				if (pos2 == Equipement::WEAPON2)
					shpos = Equipement::SHIELD2;

				if (World::getWorld()->isServer())
				{
					NetEvent event;
					event.m_type =  NetEvent::PLAYER_ITEM_REMOVE;
					event.m_data = shpos;
					event.m_id = getId();

					DEBUGX("event: no item at %i",shpos);

					World::getWorld()->insertNetEvent(event);
				}

					// Wenn aktuell kein Item am Cursor gehalten wird
				if (m_equipement->getItem(Equipement::CURSOR_ITEM)==0)
				{
						// Schild aufnehmen
					m_equipement->swapCursorItem(shpos);
				}
				else
				{
						// Schild ins Inventar verschieben
					itm =0;
						// Schild aus dem Schildslot holen
					m_equipement->swapItem( itm,shpos);

					// wenn man sich auf Serverseite befindet: NetEvent generieren
					if (!getEquipement()->insertItem(itm))
					{
						// Einfuegen ins Inventar fehlgeschlagen
						// Item fallen lassen
						getRegion()->dropItem(itm,getShape()->m_center);
					}
				}
			}
		}
	}


	if (pos2==Equipement::SHIELD || pos2 == Equipement::SHIELD2)
	{
		// Schild soll verwendet werden, testen dass keine zweihaendige Waffe benutzt wird
		Item* weapon = getWeapon();
		if (weapon!=0 && weapon->m_weapon_attr != 0 && weapon->m_weapon_attr->m_two_handed)
		{

			// zweihaendige Waffe wird verwendet, muss entfernt werden
			short wpos= Equipement::WEAPON;
			if (pos2 == Equipement::SHIELD2)
				wpos = Equipement::WEAPON2;

			if (World::getWorld()->isServer())
			{
				NetEvent event;
				event.m_type =  NetEvent::PLAYER_ITEM_REMOVE;
				event.m_data = wpos;
				event.m_id = getId();

				DEBUGX("event: no item at %i",wpos);

				World::getWorld()->insertNetEvent(event);
			}

			m_equipement->swapCursorItem(wpos);
		}
	}

		// Wenn an der Ausruestung etwas geaendert wurde
	if (pos2 <Equipement::CURSOR_ITEM)
	{
		// modifizierte Basisattribute neu berechnen
		calcBaseAttrMod();
	}
}

short Player::insertItem(Item* itm, bool use_equip, bool emit_event)
{
	if (itm ==0)
	{
		ERRORMSG("tried to insert null item");
		return 0;
	}
	bool may_equip = use_equip && checkItemRequirements(itm).m_overall;

	short pos = getEquipement()->insertItem(itm,true,may_equip, m_secondary_equip);

	if (World::getWorld() != 0)
	{
		if (pos != Equipement::NONE)
		{
			// Gegenstand ins Inventar aufgenommen
			if (World::getWorld()->isServer() && getId() !=0 && emit_event)
			{

				NetEvent event;
				event.m_type =  NetEvent::PLAYER_ITEM_INSERT ;
				event.m_data = pos;
				event.m_id = getId();

				DEBUGX("event: item picked up %i",pos);

				World::getWorld()->insertNetEvent(event);
			}

			// Wenn an der Ausruestung etwas geaendert wurde
			if (pos <Equipement::CURSOR_ITEM)
			{
				// modifizierte Basisattribute neu berechnen
				calcBaseAttrMod();
			}
		}
		else
		{
			// Gegenstand passt nicht ins Inventar
			// wieder fallen lassen
			getRegion()->dropItem(itm,getShape()->m_center);
		}
	}
	return pos;
}

ItemRequirementsMet Player::checkItemRequirements(Item* itm)
{
    ItemRequirementsMet irm;
    
	// testen ob Level ausreicht
	if (getBaseAttr()->m_level < itm->m_level_req)
	{
		// Level Vorraussetzung nicht erfuellt
		DEBUGX("level too low: own level: %i item level: %i",getBaseAttr()->m_level,itm->m_level_req);
        irm.m_overall = false;
        irm.m_level = false;
	}

	// testen ob Item fuer die Charakterklasse zugelassen ist
	if (itm->m_char_req != "all" && itm->m_char_req != "15")
	{
		// alle Item Requirement Tags der Klasse durchmustern
		// wenn eines gefunden wird, dass in der Item Beschreibung enthalten wird, kann das Item verwendet werden

		PlayerBasicData* pdata = ObjectFactory::getPlayerData(getSubtype());
		if (pdata != 0)
		{
			std::list<std::string>::iterator it;
			for (it = pdata->m_item_req_tags.begin(); it != pdata->m_item_req_tags.end(); ++it)
			{
				if (itm->m_char_req.find(*it) != std::string::npos)
				{
					return irm;
				}
			}
		}
		irm.m_overall = false;
        irm.m_class = false;
	}

	return irm;
}

Item* Player::getWeapon()
{
	Item* si = m_equipement->getItem(Equipement::WEAPON);
	if (m_secondary_equip)
	{
		si = m_equipement->getItem(Equipement::WEAPON2);
	}
	return si;
}

Item* Player::getShield()
{
	Item* si = m_equipement->getItem(Equipement::SHIELD);
		if (m_secondary_equip)
	{
		si = m_equipement->getItem(Equipement::SHIELD2);
	}
	return si;
}

Action::ActionEquip Player::getActionEquip()
{
	Item* weapon = getWeapon();
	if (weapon ==0 || weapon->m_weapon_attr==0)
	{
		return Action::NO_WEAPON;
	}

	if (weapon->m_weapon_attr->m_two_handed)
	{
		return Action::TWO_HANDED;
	}
	else
	{
		return Action::	ONE_HANDED;
	}

}

void Player::increaseAttribute(CreatureBaseAttr::Attribute attr)
{
	switch (attr)
	{
		case (CreatureBaseAttr::STRENGTH):
			getBaseAttr()->m_strength++;
			getBaseAttr()->m_max_health += 5;
			getBaseAttrMod()->m_max_health += 5;
			getDynAttr()->m_health +=5;
			addToNetEventMask(NetEvent::DATA_HP );
			break;
		case (CreatureBaseAttr::DEXTERITY):
			getBaseAttr()->m_dexterity++;
			if (getBaseAttr()->m_dexterity < 80)
			{
				getBaseAttr()->m_attack_speed +=10;
			}
			addToNetEventMask(NetEvent::DATA_ATTACK_WALK_SPEED);
			break;
		case (CreatureBaseAttr::WILLPOWER):
			getBaseAttr()->m_willpower++;
			break;
		case (CreatureBaseAttr::MAGIC_POWER):
			getBaseAttr()->m_magic_power++;
			break;

	}
	
	getRegion()->playSound("increase_attribute", getShape()->m_center, 1.0, false);

	calcBaseAttrMod();
	addToNetEventMask(NetEvent::DATA_SKILL_ATTR_POINTS | NetEvent::DATA_ATTRIBUTES_LEVEL );

}

void Player::gainLevel()
{
	Creature::gainLevel();

	// Level um 1 erhöhen
	getBaseAttr()->m_level++;

	// heilen
	getDynAttr()->m_health =getBaseAttrMod()->m_max_health;

	// Erfahrungspunkte reduzieren
	getDynAttr()->m_experience-= getBaseAttr()->m_max_experience;

	if (getDynAttr()->m_experience<0)
	{
		getDynAttr()->m_experience=0;
	}

	// play sound
	getRegion()->playSound("levelup", getShape()->m_center, 1.0, false);

	// maximale Erfahrungspunkte erhoehen
	getBaseAttr()->m_max_experience += getBaseAttr()->m_level*100;
	getBaseAttr()->m_max_experience *= 1.14;


	// Attributpunkte hinzufuegen
	m_attribute_points +=5;

	if (getBaseAttr()->m_level %10 ==0 || (getBaseAttr()->m_level<50 &&  getBaseAttr()->m_level %5 ==0) || getBaseAttr()->m_level==7 || getBaseAttr()->m_level==3 || getBaseAttr()->m_level==2)
	{
		// Skillpunkt hinzufuegen
		m_skill_points ++;
	}

	// Schaden neu berechnen
	recalcDamage();

	addToNetEventMask(NetEvent::DATA_ATTRIBUTES_LEVEL | NetEvent::DATA_HP | NetEvent::DATA_EXPERIENCE);
	addToNetEventMask(NetEvent::DATA_SKILL_ATTR_POINTS);
}


bool Player::onClientCommand( ClientCommand* command, float delay)
{
	DEBUGX("Kommando (%f %f) button: %i id: %i action: %s",command->m_goal.m_x,command->m_goal.m_y,command->m_button,command->m_id, command->m_action.c_str());

	Party* 	p;
	Player* pl;
	Dialogue* dia;
	std::list< std::pair<std::string, std::string> >::iterator it;

	bool stashtrade;
	bool sell;
	Item* si;
	DropSlot ds;
	int gold;
	
	Item::Size itemsize;
	short pos;
	
	// Wahrscheinlichkeiten BIG, MEDIUM, SMALL, GOLD
	float prob[4] = {0.1, 0.2, 0.2, 0.2};

	// prob_size, min level, max_level, prob_magic, magic_power
	ds.init(prob, 0,20, 0.3, 2000);

	// Kopie des aktuellen Kommandos anlegen
	Command oldcommand = *(getNextCommand());

	switch(command->m_button)
	{

	// je nach dem welche Eingabe getaetigt wurde
		// Behandlung rechte Maustaste

		case LEFT_MOUSE_BUTTON:
		case RIGHT_MOUSE_BUTTON:
		case LEFT_SHIFT_MOUSE_BUTTON:
			onGamefieldClick(command);
			break;

		case BUTTON_APPLY:
		case BUTTON_PEACE:
		case BUTTON_WAR:
			pl = static_cast<Player*>(World::getWorld()->getPlayer(command->m_id));
			if (pl !=0)
			{
				p = pl->getParty();
				if (command->m_button == BUTTON_APPLY)
				{
					p->addCandidate(getId());
				}
				else if (command->m_button == BUTTON_WAR)
				{
					getParty()->setRelation(p->getId() , Fraction::HOSTILE);
				}
				else if (command->m_button == BUTTON_PEACE)
				{
					getParty()->setRelation(p->getId() ,  Fraction::NEUTRAL);
				}
			}
			break;
		case BUTTON_MEMBER_REJECT:
			getParty()->removeCandidate(command->m_id);
			break;


		case BUTTON_MEMBER_ACCEPT:
			pl = static_cast<Player*>(World::getWorld()->getPlayer(command->m_id));
			if (pl !=0)
			{
				pl->getParty()->removeMember(pl->getId());
				getParty()->acceptCandidate(command->m_id);
			}
			break;

		case BUTTON_KICK:
			getParty()->removeMember(command->m_id);
			World::getWorld()->getEmptyParty()->addMember(command->m_id);
			break;

		case BUTTON_LEAVE:
			if (World::getWorld()->isCooperative())
				break;

			if (getParty()->getNrMembers() ==1)
			{
				if (m_candidate_party >=0)
					World::getWorld()->getParty(m_candidate_party)->removeCandidate(getId());
			}
			else
			{
				getParty()->removeMember(getId());
				World::getWorld()->getEmptyParty()->addMember(getId());
			}
			break;

		case BUTTON_SET_LEFT_ACTION:
			m_left_action = (Action::ActionType) command->m_action;
			recalcDamage();
			break;

		case BUTTON_SET_RIGHT_ACTION:
			m_right_action = (Action::ActionType) command->m_action;
			recalcDamage();
			break;
			
		case BUTTON_SET_ALTERNATE_RIGHT_ACTION:
			m_right_alternate_action = (Action::ActionType) command->m_action;
			break;
			
		case BUTTON_SWAP_RIGHT_ACTION:
			std::swap(m_right_action, m_right_alternate_action);
			recalcDamage();
			break ;

			case BUTTON_ITEM_LEFT:
		case BUTTON_ITEM_RIGHT:
			onItemClick(command);
			break;

		case BUTTON_SAVE_QUIT:
			setState(STATE_QUIT,false);
			break;

		case BUTTON_SWAP_EQUIP:
			m_secondary_equip = !m_secondary_equip;

			calcBaseAttrMod();

			// play sound
			getRegion()->playSound("swap_equip", getShape()->m_center, 1.0, false);
			
			if (World::getWorld()->isServer())
			{
				NetEvent event;
				event.m_type =  NetEvent::PLAYER_ITEM_SWAP ;
				event.m_id = getId();
				World::getWorld()->insertNetEvent(event);
			}
			DEBUGX("switching equip");
			break;


		case BUTTON_INCREASE_ATTRIBUTE:
			if (m_attribute_points>0)
			{
				// noch freie Attributpunkte vorhanden
				m_attribute_points--;

				// entsprechendes Attribut erhoehen
				increaseAttribute((CreatureBaseAttr::Attribute) command->m_id);
			}
			break;

		case BUTTON_LEARN_ABILITY:
			if (m_skill_points>0)
			{
				// Skill punkte stehen zur Verfuegung
				if (checkAbilityLearnable(( Action::ActionType) command->m_action))
				{
					// Faehigkeit kann gelernt werden

					m_skill_points--;

					// Bit fuer die Faehigkeit setzen
					getBaseAttr()->m_abilities[command->m_action].m_time=0;
					
					// play sound
					getRegion()->playSound("select_skill", getShape()->m_center, 1.0, false);

					DEBUGX("lerne Faehigkeit %s", command->m_action.c_str());

					calcBaseAttrMod();

					addToNetEventMask( NetEvent::DATA_SKILL_ATTR_POINTS | NetEvent::DATA_ABILITIES);
				}
			}
			break;

		case DROP_ITEM:
			if (command->m_number ==0)
			{
				// normales Item
				si = getEquipement()->getItem(Equipement::CURSOR_ITEM);
				if (si !=0)
				{
					si=0;
					getEquipement()->swapItem(si,Equipement::CURSOR_ITEM);
					getRegion()->dropItem(si,getShape()->m_center);
				}
			}
			else
			{
				// Gold
				if (command->m_number <= getEquipement()->getGold())
				{
					si = ItemFactory::createGold(command->m_number);
					getRegion()->dropItem(si,getShape()->m_center);

					getEquipement()->setGold( getEquipement()->getGold() - command->m_number);
				}
			}
			break;

		case BUTTON_ANSWER:
			if (!World::getWorld()->isServer())
				break;

			dia = getDialogue();
			if (dia ==0)
			{
				ERRORMSG("answer without dialogue");
				break;
			}

			if (command->m_id==-1)
			{
				// Gespraech beenden
				dia->changeTopic("abort");
				break;
			}

			DEBUGX("changing topic to %s",it->second.c_str());
			EventSystem::setDialogue(dia);
			dia->chooseAnswer(getId(), command->m_id);
			EventSystem::setDialogue(0);
			break;

		case BUTTON_TRADE_END:
			if (!World::getWorld()->isServer())
				break;

			dia = getDialogue();
			if (dia ==0)
			{
				// This can only happen, when trading with you stash
				if (getTradePartner() == this)
				{
					setTradePartner(0);
					break;
				}
				else
				{
					ERRORMSG("answer without dialogue");
				}
				break;
			}
			dia->changeTopic("end");
			break;

		case BUTTON_TRADE_ITEM_LEFT:
		case BUTTON_TRADE_ITEM_RIGHT:
		case BUTTON_TRADE_SELL:
			if (getTradePartner() ==0)
			{
				break;
			}
			
			// you may *trade* with your stash, in this case the traderpartner is the object itself
			stashtrade = false;
			if (getTradePartner() == this)
			{
				stashtrade = true;
			}

			si = getEquipement()->getItem(Equipement::CURSOR_ITEM);
			sell = true;
			gold = getEquipement()->getGold();

			// Rechtklick kauft immer ein, Linksklick nur, wenn kein Item in der Hand
			if (command->m_button == BUTTON_TRADE_ITEM_RIGHT || (command->m_button == BUTTON_TRADE_ITEM_LEFT && si ==0))
			{
				sell = false;
			}

			if (sell)
			{
				// Item wird aus dem Inventar herraus genommen
				si =0;
				getEquipement()->swapItem(si,Equipement::CURSOR_ITEM);
				if (!stashtrade)
				{
					getTradePartner()->buyItem(si,gold);
				}
				else
				{
					// expected size of the item
					itemsize = Item::SIZE_NONE;
					if (command->m_id >= Equipement::BIG_ITEMS)
						itemsize = Item::BIG;
					if (command->m_id >= Equipement::MEDIUM_ITEMS)
						itemsize = Item::MEDIUM;
					if (command->m_id >= Equipement::SMALL_ITEMS)
						itemsize = Item::SMALL;
					
					if(si != 0)
					{
						if (itemsize == si->m_size)
						{
							getStash()->swapItem(si,command->m_id);
						}
						else
						{
							// item did not fit where the player placed it
							// just drop it into the first free place
							pos = getStash()->insertItem(si);
						
							// if insert was succesful, set pointer to 0
							// else, the item will be reinserted into player inventory
							if (pos != Equipement::NONE)
							{
								si = 0;
							}
						}
					}
				}

				// wieder hinein getauscht, wenn Handel nicht erfolgreich
				getEquipement()->swapItem(si,Equipement::CURSOR_ITEM);

			}
			else
			{
				si =0;
				if (!stashtrade)
				{
					getTradePartner()->sellItem(command->m_id,si, gold);
				}
				else
				{
					getStash()->swapItem(si,command->m_id);
				}

				if (si !=0)
				{
					if (command->m_button == BUTTON_TRADE_ITEM_RIGHT)
					{
						insertItem(si,true);
					}
					else
					{
						getEquipement()->swapItem(si,Equipement::CURSOR_ITEM);
					}
				}
			}
			
			if (!stashtrade)
			{
				getEquipement()->setGold(gold);
			}
			break;

		case DEBUG_SIGNAL:
			// Debugging
			if (command->m_id==0)
			{
				gainLevel();
			}
			else if (command->m_id==3)
			{

				si = ItemFactory::createItem(ds);
				bool ret = getEquipement()->insertItem(si);

				if (ret == false)
				{
					getEquipement()->clear();
					getEquipement()->insertItem(si);
				}

			}
			break;

		case BUTTON_WAYPOINT_CHOSE:

			if (World::getWorld()->isServer())
			{
				setUsingWaypoint(false);
				if (command->m_id>=0 && command->m_id!= getRegion()->getId())
				{
					DEBUGX("Player Teleport to Region %i",command->m_id);
					std::map<short,WaypointInfo>& winfos  = World::getWorld()->getWaypointData();
					RegionLocation regloc;
					regloc.first = winfos[command->m_id].m_name;
					regloc.second="WaypointLoc";
					getRegion()->insertPlayerTeleport(getId(), regloc);
					clearCommand(true,true);
				}
				else if (command->m_id == -999 && m_portal_position.first != "")
				{
					getRegion()->insertPlayerTeleport(getId(), m_portal_position);
					RegionLocation regloc;
					regloc.first = "";
					regloc.second="";
					setPortalPosition(regloc);
					clearCommand(true,true);
				}
			}
			break;

		case BUTTON_SKIP_DIALOGUE_TEXT:
			if (World::getWorld()->isServer())
			{
				dia = getDialogue();
				if (dia !=0)
				{
					dia->skipText(getId());
				}
			}
			break;


		default:
			DEBUG("unknown command: %i",command->m_button);
	}

	if (oldcommand != *getNextCommand() && delay>0)
	{
		// naechstes Kommando hat sich geaendert
		// die aktuelle Aktion basiert eventuell auf einem veralteten Kommando

		if (getAction()->m_type == "noaction" || getAction()->m_elapsed_time < delay)
		{
			// Aktion basiert auf veraltetem Kommando
			// abbrechen
			abortAction();

			// neue Aktion berechnen
			calcAction();
			initAction();
			calcDamage(getAction()->m_type,*(getDamage()));

			DEBUGX("new action %s time %f",getAction()->m_type.c_str(),getAction()->m_time);

			// Action entsprechend der Verzoegerung schneller ausfuehren
			// aber maximal doppelt so schnell
			float mult = MathHelper::Max(getAction()->m_time-delay, getAction()->m_time/2)/getAction()->m_time;
			DEBUGX("delay %f mult %f",delay,mult);
			if (getAction()->m_type == "walk")
			{
				// Laufgeschwindigkeit entsprechend erhoehen
				Vector speed = getSpeed();
				speed *= (1/mult);
				setSpeed(speed);

			}
			getAction()->m_time *= mult;
			DEBUGX("faster action time %f",getAction()->m_time);
		}

	}

	return true;
}

void Player::initAction()
{
	// automatically take up all gold in 2 meter radius
	if (getAction()->m_type != "noaction")
	{
		DropItemMap* itms = getRegion()->getDropItems();
		DropItemMap::iterator it, next;
		for ( it = itms->begin();it != itms->end();it = next)
		{
			next = it;
			++next;
			
			DropItem* di = it->second;
			// only items, that have finished falling down
			// only gold
			// only if distance < 2
			if (di->getItem()->m_size == Item::GOLD && di->getActionString() == "dropped" && di->getPosition().distanceTo(getPosition()) < 3)
			{
				// delete from Region and insert into player inventory
				Item* itm = di->getItem();
				getRegion()->deleteItem(di->getId());
				insertItem(itm,true);
			}
		}
	}
	
	Creature::initAction();
}

void Player::abortAction()
{
	float time = getAction()->m_elapsed_time;
	Action::ActionInfo* aci = Action::getActionInfo(getAction()->m_type);

	DEBUGX("abort Action %s (elapsed time %f)",getAction()->m_type.c_str(), time);


	if (getAction()->m_type == "walk")
	{
		// Position zurueck setzen
		moveTo(getPosition() - getSpeed()*time);
	}

	// Timer wieder zuruecksetzen
	if (aci->m_timer_nr!=0)
	{
		m_timers[aci->m_timer_nr-1] =0;
		m_timers_max[aci->m_timer_nr-1] =0;
	}

	addToNetEventMask(NetEvent::DATA_ACTION);
}

bool Player::update(float time)
{

	// Aufruf des updates für von Creature
	Creature::update(time);

	m_camera.update(time);

	// Player spezifische Updateroutine
	DEBUGX("Update des Playerobjekts [%i] wird gestartet", getId());

	// Debugging
	//return true;

	return true;
}

void Player::performActionCritPart(Vector goal, WorldObject* goalobj)
{
	if (getAction()->m_type == "take_item")
	{
		DEBUGX("take item");
		// Item suchen
		Item* itm = getRegion()->getItem(getCommand()->m_goal_object_id);

		if (itm !=0)
		{
			// aus der Region entfernen
			getRegion()->deleteItem(getCommand()->m_goal_object_id);

			// Item einfuegen
			insertItem(itm,true);
			
			// play sound
			getRegion()->playSound("collect_item", getShape()->m_center, 1.0, false);
			DEBUGX("take item %p",itm);
		}
		else
		{
			DEBUG("no item found at %f %f",goal.m_x,goal.m_y);
		}
	}
	else
	{
		Creature::performActionCritPart(goal,goalobj);
	}
}

bool Player::checkRole(std::string role)
{
	// all trifft immer zu
	if (role == "all" || role == "")
	{
		return true;
	}

	if (role == "male" && getGender()==MALE)
	{
		return true;
	}

	if (role == "female" && getGender()==FEMALE)
	{
		return true;
	}

	if (role == getSubtype())
	{
		return true;
	}

	bool leader = getParty()->getLeader() == getId();

	if (leader && role =="leader")
	{
		return true;
	}

	if (!leader && role == "non-leader")
	{
		return true;
	}

	return false;
}



void Player::calcBaseDamage(std::string impl,Damage& dmg)
{
	Creature::calcBaseDamage(impl,dmg);

	if (impl == "weapon_damage")
	{
		Damage::DamageType dmgtype = Damage::FIRE;
		float maxdmg =0;

		Item* item;
		for (int j=1; j<=8; j++)
		{
			item = getEquipement()->getItem(j);
			if (j == Equipement::WEAPON)
			{
				item = getWeapon();
			}
			if (j == Equipement::SHIELD)
				item = getShield();

			if (item!=0 && item->m_weapon_attr !=0)
			{
				// Schaden der Waffe
				Damage& wdmg=item->m_weapon_attr->m_damage;

				int i;



				// Schaden uebernehmen
				for (i=0;i<4;i++)
				{
					dmg.m_min_damage[i] += wdmg.m_min_damage[i];
					dmg.m_max_damage[i] += wdmg.m_max_damage[i];
					dmg.m_multiplier[i] *= wdmg.m_multiplier[i];

					if (j == Equipement::WEAPON)
					{
						if ((wdmg.m_min_damage[i]+ wdmg.m_max_damage[i]) * wdmg.m_multiplier[i]>maxdmg)
						{
							maxdmg = (wdmg.m_min_damage[i]+ wdmg.m_max_damage[i]) * wdmg.m_multiplier[i];
							dmgtype = (Damage::DamageType) i;
						}
					}
				}



				// Statusmods uebernehmen
				for (i=0;i<NR_STATUS_MODS;i++)
				{
					dmg.m_status_mod_power[i] += wdmg.m_status_mod_power[i];
				}

				// weitere Attribute
				dmg.m_power += wdmg.m_power;
				dmg.m_attack += wdmg.m_attack;
				dmg.m_crit_perc += wdmg.m_crit_perc;
				dmg.m_special_flags |= wdmg.m_special_flags;

			}

		}

		if (getSubtype() == "mage")
		{
			CreatureBaseAttr* basm = getBaseAttrMod();
			dmg.m_min_damage[Damage::FIRE] -= basm->m_magic_power/10;
			dmg.m_min_damage[Damage::FIRE] -= basm->m_willpower/20;
			dmg.m_max_damage[Damage::FIRE] -= basm->m_magic_power/6;
			dmg.m_max_damage[Damage::FIRE] -= basm->m_willpower/15;

			dmg.m_min_damage[dmgtype] += basm->m_magic_power/10;
			dmg.m_min_damage[dmgtype] += basm->m_willpower/20;
			dmg.m_max_damage[dmgtype] += basm->m_magic_power/6;
			dmg.m_max_damage[dmgtype] += basm->m_willpower/15;
		}
	}
}

void Player::calcBaseAttrMod()
{
	Creature::calcBaseAttrMod();
	Item* si;

	// Wirkung der Ausruestungsgegenstaende einfuegen

	// Waffe
	si = getWeapon();
	if (si!=0)
	{
		if (si->m_equip_effect !=0)
		{
			applyBaseAttrMod(si->m_equip_effect,false);
		}

		if (si->m_weapon_attr !=0)
		{
			getBaseAttrMod()->m_attack_speed += si->m_weapon_attr ->m_dattack_speed;
			getBaseAttrMod()->m_attack_range = si->m_weapon_attr ->m_attack_range;

			if (si->m_weapon_attr ->m_dattack_speed!=0)
			{
				addToNetEventMask(NetEvent::DATA_ATTACK_WALK_SPEED);
			}
		}
	}



	// Ruestung
	si = m_equipement->getItem(Equipement::ARMOR);
	if (si!=0 && si->m_equip_effect !=0)
	{
		applyBaseAttrMod(si->m_equip_effect,false);
	}

	// Helm
	si = m_equipement->getItem(Equipement::HELMET);
	if (si!=0 && si->m_equip_effect !=0)
	{
		applyBaseAttrMod(si->m_equip_effect,false);
	}

	// Schild
	si = getShield();
	if (si!=0 && si->m_equip_effect !=0)
	{
		applyBaseAttrMod(si->m_equip_effect,false);
	}

	// Handschuhe
	si = m_equipement->getItem(Equipement::GLOVES);
	if (si!=0 && si->m_equip_effect !=0)
	{
		applyBaseAttrMod(si->m_equip_effect,false);
	}

	// Ring links
	si = m_equipement->getItem(Equipement::RING_LEFT);
	if (si!=0 && si->m_equip_effect !=0)
	{
		applyBaseAttrMod(si->m_equip_effect,false);
	}

	// Ring rechts
	si = m_equipement->getItem(Equipement::RING_RIGHT);
	if (si!=0 && si->m_equip_effect !=0)
	{
		applyBaseAttrMod(si->m_equip_effect,false);
	}

	// Amulett
	si = m_equipement->getItem(Equipement::AMULET);
	if (si!=0 && si->m_equip_effect !=0)
	{
		applyBaseAttrMod(si->m_equip_effect,false);
	}

	// Faehigkeiten anwenden

	// Faehigkeit ausweichen
	if (checkAbility("evade"))
	{
		getBaseAttrMod()->m_block += getBaseAttrMod()->m_dexterity/3;
	}


	// Blockbonus durch Geschick 
	getBaseAttrMod()->m_block += getBaseAttrMod()->m_dexterity/ 4;

	// Faehigkeit Blocken
	if (checkAbility("block"))
	{
		getBaseAttrMod()->m_block += (short) (0.4f*getBaseAttrMod()->m_block);
	}

	// Schaden neu berechnen
	recalcDamage();
}

void Player::addMessage(std::string msg)
{
	if (msg == "")
		return;

	if (m_messages != "")
	{
		m_messages += "\n";
	}
	else
	{
		m_message_clear_timer = msg.size()*500;
	}
	m_messages += msg;

	// Anzahl Newlines zaehlen
	int cnt =0;
	int pos = 0;
	while (pos != -1)
	{

		cnt ++;
		pos=m_messages.find_first_of('\n',pos+1);
	}
	cnt --;
	while (cnt >10)
	{
		pos=m_messages.find_first_of('\n');
		m_messages = m_messages.substr(pos+1);
		cnt --;
	}
}


void Player::toString(CharConv* cv)
{
	Creature::toString(cv);
	cv->toBuffer(getBaseAttr()->m_level);
	m_look.toString(cv);
	
	// Items
	int cnt =0;
	Item* item;
	for ( short i = Equipement::ARMOR; i<= Equipement::SHIELD2; i++)
	{
		item = getEquipement()->getItem(i);
		if (item !=0)
			cnt++;
	}
	DEBUGX("number of items: %i",cnt);
	cv->toBuffer(cnt);
	
	for ( short i = Equipement::ARMOR; i<= Equipement::SHIELD2; i++)
	{
		item = getEquipement()->getItem(i);
		if (item !=0)
		{
			cv->toBuffer(i);
			item->toString(cv);

		}
	}
	// IDs der Wegpunkte
	cv->toBuffer(static_cast<int>(m_waypoints.size()));
	cv->printNewline();

	std::set<short>::iterator it;
	short regid;
	for (it = m_waypoints.begin(); it != m_waypoints.end(); ++it)
	{
		regid = (short) (*it);
		cv->toBuffer(regid);
	}
}


void Player::fromString(CharConv* cv)
{
	Creature::fromString(cv);
	
	cv->fromBuffer(getBaseAttr()->m_level);
	m_look.fromString(cv);
	m_emotion_set = m_look.m_emotion_set;
	int cnt;
	cv->fromBuffer(cnt);
	DEBUG("number of items: %i",cnt);
	for ( short i = 0; i< cnt; i++)
	{
		readItem(cv);
	}
	// IDs der Wegpunkte
	int nr=0;
	cv->fromBuffer(nr);

	short regid;
	m_waypoints.clear();
	for (int i=0; i<nr; i++)
	{
		cv->fromBuffer(regid);
		m_waypoints.insert(regid);
	}
}

void Player::readItem(CharConv* cv)
{
	short pos;
	char type;
	std::string subtype;
	int id;
	Item* item;

	cv->fromBuffer(pos);
	cv->fromBuffer(type);
	cv->fromBuffer(subtype);
	cv->fromBuffer(id);

	item = ItemFactory::createItem((Item::Type) type, subtype,id);
	if (item != 0)
	{
		item->fromString(cv);
		getEquipement()->swapItem(item,pos);

		if (item !=0)
		{
			delete item;
		}
	}
}

void Player::readItemComplete(CharConv* cv)
{
	short pos;
	char type;
	std::string subtype;
	int id;
	Item* item;

	cv->fromBuffer(pos);
	cv->fromBuffer(type);
	cv->fromBuffer(subtype);
	cv->fromBuffer(id);

	DEBUGX("reading Item for pos %i type %i subtype %s",pos,type,subtype.c_str());
	item = ItemFactory::createItem((Item::Type) type, subtype,id);
	item->fromStringComplete(cv);
	getEquipement()->swapItem(item,pos);

	if (item !=0)
		delete item;
}

void Player::recalcDamage()
{
	// nur, wenn das Spiel schon *richtig laeuft*
	if (World::getWorld() != 0 && World::getWorld()->getPlayer(getId()) == this)
	{
		calcDamage(m_base_action,m_base_damage);
		calcDamage(m_left_action,m_left_damage);
		calcDamage(m_right_action,m_right_damage);
	}
}

void Player::addWaypoint(short id, bool check_party)
{
	if (m_waypoints.count(id) ==0)
	{
		m_waypoints.insert(id);
		DEBUGX("inserted Waypoint %i for player %i",id, getId());

		// Partymitglieder auch ueber den Wegpunkt informieren
		if (check_party)
		{
			std::set<int>& members = getParty()->getMembers();

			Player* pl;
			std::set<int>::iterator it;
			for (it =members.begin(); it != members.end(); ++it)
			{
				pl = dynamic_cast<Player*>(World::getWorld()->getPlayer(*it));
				if (pl !=0 && pl != this)
				{
					// diese Mitspieler sollen nicht ueber ihre Partymember iterieren
					// um endlose Rekursion zu vermeiden
					pl->addWaypoint(id,false);
				}
			}
		}

		// Event das die Entdenkung des Wegpunktes beinhaltet
		if (World::getWorld()->isServer())
		{
			NetEvent event;
			event.m_type = NetEvent::PLAYER_WAYPOINT_DISCOVERED;
			event.m_id = getId();
			event.m_data = id;

			World::getWorld()->insertNetEvent(event);
		}
	}
}

bool Player::checkWaypoint(short id)
{
	// Spieler hat den Wegpunkt direkt
	if (m_waypoints.count(id) >0)
		return true;

	// Partyleader hat den Wegpunkt
	if (getParty()->getLeader()!= getId())
	{
		Player* leader =  dynamic_cast<Player*>(World::getWorld()->getPlayer(getParty()->getLeader()));
		if (leader !=0)
		{
			return leader->checkWaypoint(id);
		}
	}

	return false;
}

void Player::toSavegame(CharConv* cv)
{
	// Version richtig setzen
	cv->toBuffer(World::getVersion());
	cv->setVersion(World::getVersion());

	cv->toBuffer(getSubtype());
	cv->toBuffer(getName().getRawText());
	m_look.toString(cv);
	cv->printNewline();

	cv->toBuffer(getBaseAttr()->m_level);
	cv->toBuffer(getBaseAttr()->m_max_health);
	cv->printNewline();

	cv->toBuffer(getBaseAttr()->m_armor);
	cv->toBuffer(getBaseAttr()->m_attack);
	if (World::getVersion()>=10)
		cv->toBuffer(getBaseAttr()->m_power);
	cv->toBuffer(getBaseAttr()->m_strength);
	cv->toBuffer(getBaseAttr()->m_dexterity);
	cv->toBuffer(getBaseAttr()->m_magic_power);
	cv->toBuffer(getBaseAttr()->m_willpower);
	cv->printNewline();

	cv->toBuffer(m_attribute_points);
	cv->toBuffer(m_skill_points);
	cv->printNewline();

	cv->toBuffer(getBaseAttr()->m_walk_speed);
	cv->toBuffer(getBaseAttr()->m_attack_speed);
	cv->toBuffer(getBaseAttr()->m_attack_range);
	cv->printNewline();

	cv->toBuffer(getBaseAttr()->m_resistances[0]);
	cv->toBuffer(getBaseAttr()->m_resistances[1]);
	cv->toBuffer(getBaseAttr()->m_resistances[2]);
	cv->toBuffer(getBaseAttr()->m_resistances[3]);
	cv->printNewline();

	cv->toBuffer(getBaseAttr()->m_max_experience);
	cv->toBuffer(getDynAttr()->m_experience);
	cv->printNewline();

	cv->toBuffer(static_cast<short>(getBaseAttr()->m_abilities.size()));
	cv->printNewline();
	std::map<std::string, AbilityInfo>::iterator jt;
	for (jt= getBaseAttr()->m_abilities.begin(); jt != getBaseAttr()->m_abilities.end(); ++jt)
	{
		cv->toBuffer(jt->first);
		cv->toBuffer(jt->second.m_timer_nr);
		cv->toBuffer(jt->second.m_time);
		cv->toBuffer(jt->second.m_timer);
		cv->printNewline();
	}


	cv->toBuffer( m_base_action);
	cv->toBuffer(m_left_action);
	cv->toBuffer( m_right_action);
	if (World::getVersion()>=20)
	{
		cv->toBuffer( m_right_alternate_action);
	}
	cv->printNewline();

	cv->toBuffer(m_revive_position.first);
	cv->toBuffer(m_revive_position.second);
	cv->printNewline();

	// Items
	writeEquipement(cv);
	cv->printNewline();
	
	if (World::getVersion()>=19)
	{
		cv->toBuffer(m_secondary_equip);
	}
	
	if (World::getVersion() >=15)
	{
		getStash()->toStringComplete(cv);
	}
	cv->printNewline();
	

	// Questinformationen
	// Daten werden aus der lua Umgebung genommen, wenn die Welt schon laeuft
	// sonst werden sie im Spieler zwischengespeichert
	if (World::getWorld() == 0 || getId()==0)
	{
		std::list<std::string>::iterator it;
		for (it =m_lua_instructions.begin(); it !=m_lua_instructions.end(); ++it)
		{
			cv->toBuffer(static_cast<char>(1));
			cv->toBuffer(*it);
			cv->printNewline();
		}
		cv->toBuffer(static_cast<char>(0));

	}
	else
	{
		EventSystem::writeSavegame(cv);
		cv->printNewline();
	}

	// IDs der Wegpunkte
	cv->toBuffer((int) m_waypoints.size());
	cv->printNewline();

	std::set<short>::iterator it;
	short regid;
	for (it = m_waypoints.begin(); it != m_waypoints.end(); ++it)
	{
		regid = (short) (*it);
		cv->toBuffer(regid);
	}
}


void Player::fromSavegame(CharConv* cv, bool local)
{
	DEBUGX("from Savegame");


	int version;
	cv->fromBuffer(version);
	cv->setVersion(version);

	cv->fromBuffer(getSubtype());
	init();

	std::string name;
	cv->fromBuffer(name);
	setName(TranslatableString(name));

	m_look.fromString(cv);
	m_emotion_set = m_look.m_emotion_set;

	cv->fromBuffer(getBaseAttr()->m_level);
	cv->fromBuffer(getBaseAttr()->m_max_health);
	getDynAttr()->m_health = getBaseAttr()->m_max_health;

	cv->fromBuffer(getBaseAttr()->m_armor);
	cv->fromBuffer(getBaseAttr()->m_attack);
	if (version>=10)
		cv->fromBuffer(getBaseAttr()->m_power);
	else
		getBaseAttr()->m_power = 20;

	cv->fromBuffer(getBaseAttr()->m_strength);
	cv->fromBuffer(getBaseAttr()->m_dexterity);
	cv->fromBuffer(getBaseAttr()->m_magic_power);
	cv->fromBuffer(getBaseAttr()->m_willpower);

	cv->fromBuffer(m_attribute_points);
	cv->fromBuffer(m_skill_points );

	cv->fromBuffer(getBaseAttr()->m_walk_speed);
	cv->fromBuffer(getBaseAttr()->m_attack_speed);
	cv->fromBuffer(getBaseAttr()->m_attack_range);

	cv->fromBuffer(getBaseAttr()->m_resistances[0]);
	cv->fromBuffer(getBaseAttr()->m_resistances[1]);
	cv->fromBuffer(getBaseAttr()->m_resistances[2]);
	cv->fromBuffer(getBaseAttr()->m_resistances[3]);

	cv->fromBuffer(getBaseAttr()->m_max_experience);
	cv->fromBuffer (getDynAttr()->m_experience);

	short anr;
	cv->fromBuffer(anr);
	Action::ActionType type;
	getBaseAttr()->m_abilities.clear();
	int timer_nr;
	float time, timer;
	for (int i=0; i<anr; i++)
	{
		cv->fromBuffer(type);
		cv->fromBuffer(timer_nr);
		cv->fromBuffer(time);
		cv->fromBuffer(timer);
		getBaseAttr()->m_abilities[type].m_timer_nr = timer_nr;
		getBaseAttr()->m_abilities[type].m_timer =timer;
		getBaseAttr()->m_abilities[type].m_time = time;
	}

	DEBUGX("name %s class %s level %i",getName().c_str(), getSubtype().c_str(), getBaseAttr()->m_level);

	cv->fromBuffer( m_base_action);
	cv->fromBuffer(m_left_action);
	cv->fromBuffer( m_right_action);
	if (cv->getVersion()>=20)
	{
		cv->fromBuffer( m_right_alternate_action);
	}
	else
	{
		m_right_alternate_action = m_right_action;
	}

	cv->fromBuffer(m_revive_position.first);
	cv->fromBuffer(m_revive_position.second);
	// Items
	loadEquipement(cv);
	if (cv->getVersion()>=19)
	{
		cv->fromBuffer(m_secondary_equip);
	}
	if (cv->getVersion() >=15)
	{
		getStash()->fromStringComplete(cv);
	}

	getBaseAttrMod()->m_max_health = getBaseAttr()->m_max_health;
	calcBaseAttrMod();



	// Questinformationen
	// Daten werden aus der lua Umgebung genommen, wenn die Welt schon laeuft
	// sonst werden sie im Spieler zwischengespeichert
	if (World::getWorld() == 0 || getId()==0)
	{
		m_lua_instructions.clear();
		char c=1;
		std::string instr;
		while (c==1)
		{
			c=0;
			cv->fromBuffer(c);
			if (c==0)
				break;

			cv->fromBuffer(instr);
			m_lua_instructions.push_back(instr);
			DEBUGX("instructions: %s",instr.c_str());
		}
	}
	else
	{
		EventSystem::readSavegame(cv,getId(), local);
	}

	// IDs der Wegpunkte
	int nr=0;
	cv->fromBuffer(nr);

	short regid;
	m_waypoints.clear();
	for (int i=0; i<nr; i++)
	{
		cv->fromBuffer(regid);
		m_waypoints.insert(regid);
	}
}

void Player::writeEquipement(CharConv* cv)
{
	getEquipement()->toStringComplete(cv);
}

void Player::loadEquipement(CharConv* cv)
{
	getEquipement()->fromStringComplete(cv);
}


int Player::getValue(std::string valname)
{
	if (valname =="attribute_points")
	{
		lua_pushinteger(EventSystem::getLuaState() , m_attribute_points );
		return 1;
	}
	else if (valname =="skill_points")
	{
		lua_pushinteger(EventSystem::getLuaState() , m_skill_points );
		return 1;
	}
	else if (valname =="gold")
	{
		lua_pushinteger(EventSystem::getLuaState() , getEquipement()->getGold() );
		return 1;
	}
	else
	{
		int ret = Creature::getValue(valname);

		return ret;
	}
	return 0;
}

bool Player::setValue(std::string valname)
{
	if (valname == "attribute_points")
	{
		m_attribute_points = lua_tointeger(EventSystem::getLuaState() ,-1);
		lua_pop(EventSystem::getLuaState(), 1);
		addToNetEventMask(NetEvent::DATA_SKILL_ATTR_POINTS);
		return true;
	}
	else if (valname =="skill_points")
	{
		m_skill_points = lua_tointeger(EventSystem::getLuaState() ,-1);
		lua_pop(EventSystem::getLuaState(), 1);
		addToNetEventMask(NetEvent::DATA_SKILL_ATTR_POINTS);
		return true;
	}
	else if (valname =="gold")
	{
		int gold = lua_tointeger(EventSystem::getLuaState() ,-1);
		getEquipement()->setGold(gold);
		lua_pop(EventSystem::getLuaState(), 1);

		NetEvent event;
		event.m_type =  NetEvent::PLAYER_ITEM_INSERT ;
		event.m_data = Equipement::GOLD;
		event.m_id = getId();

		World::getWorld()->insertNetEvent(event);

		lua_pushinteger(EventSystem::getLuaState() , getEquipement()->getGold() );
		return true;
	}
	else
	{
		return  Creature::setValue(valname);
	}

	return false;
}

void Player::writeNetEvent(NetEvent* event, CharConv* cv)
{
	Creature::writeNetEvent(event,cv);

	if (event->m_data & NetEvent::DATA_REVIVE_LOCATION)
	{
		cv->toBuffer(m_revive_position.first);
		cv->toBuffer(m_revive_position.second);
		cv->toBuffer(m_portal_position.first);
		cv->toBuffer(m_portal_position.second);
		DEBUGX("writing revive position %s %s",m_revive_position.first.c_str(), m_revive_position.second.c_str())
	}

	if (event->m_data & NetEvent::DATA_SKILL_ATTR_POINTS)
	{
		cv->toBuffer(m_attribute_points);
		cv->toBuffer(m_skill_points);
	}

	if (event->m_data & NetEvent::DATA_WAYPOINT)
	{
		cv->toBuffer(m_using_waypoint);
	}

	if (event->m_data & NetEvent::DATA_FIGHT_STAT)
	{
		FightStatistic* fstat = &(getFightStatistic());
		fstat->m_last_attacker.toString(cv);
		fstat->m_last_attacked.toString(cv);
		cv->toBuffer(fstat->m_hit_chance);
		cv->toBuffer(fstat->m_damage_dealt_perc);
		cv->toBuffer(fstat->m_damage_got_perc);
		cv->toBuffer(fstat->m_block_chance);
	}
}

void Player::processNetEvent(NetEvent* event, CharConv* cv)
{
	Creature::processNetEvent(event,cv);

	if (event->m_data & NetEvent::DATA_REVIVE_LOCATION)
	{
		cv->fromBuffer(m_revive_position.first);
		cv->fromBuffer(m_revive_position.second);
		cv->fromBuffer(m_portal_position.first);
		cv->fromBuffer(m_portal_position.second);
		DEBUGX("reading revive position %s %s",m_revive_position.first.c_str(), m_revive_position.second.c_str())
	}

	if (event->m_data & NetEvent::DATA_SKILL_ATTR_POINTS)
	{
		cv->fromBuffer(m_attribute_points);
		cv->fromBuffer(m_skill_points);
	}

	if (event->m_data & NetEvent::DATA_WAYPOINT)
	{
		cv->fromBuffer(m_using_waypoint);
	}

	if (event->m_data & NetEvent::DATA_FIGHT_STAT)
	{
		FightStatistic* fstat = &(getFightStatistic());
		fstat->m_last_attacker.fromString(cv);
		fstat->m_last_attacked.fromString(cv);
		cv->fromBuffer(fstat->m_hit_chance);
		cv->fromBuffer(fstat->m_damage_dealt_perc);
		cv->fromBuffer(fstat->m_damage_got_perc);
		cv->fromBuffer(fstat->m_block_chance);
	}
}


void Player::setRevivePosition(RegionLocation regloc)
{
	m_revive_position = regloc;
	addToNetEventMask(NetEvent::DATA_REVIVE_LOCATION);
}

void Player::setPortalPosition(RegionLocation regloc)
{
	m_portal_position = regloc;
	addToNetEventMask(NetEvent::DATA_REVIVE_LOCATION);
}

void Player::setUsingWaypoint(bool val)
{
	DEBUGX("player %i using waypoint ",getId());
	m_using_waypoint = val;
	addToNetEventMask(NetEvent::DATA_WAYPOINT);
}

void Player::updateMessageTimer(float time)
{
	if (m_messages == "")
		return;

	m_message_clear_timer -= time;

	if (m_message_clear_timer <0)
	{
		int pos=m_messages.find_first_of('\n');
		if (pos == -1)
		{
			m_messages = "";
		}
		else
		{
			m_messages = m_messages.substr(pos+1);
			pos=m_messages.find_first_of('\n');
			if (pos == -1)
				pos = m_messages.size();

			m_message_clear_timer = pos * 500;
		}
	}
}

std::string Player::getActionWeaponSuffix()
{
	std::string ret ="";
	Item* weapon=getWeapon();
	if (weapon == 0 )
		ret += "#Unarmed";
	else if (weapon->m_weapon_attr ==0 || weapon->m_weapon_attr->m_two_handed == false)
		ret += "#OneHand";
	else if (weapon->m_weapon_attr->m_two_handed == true)
		ret += "#TwoHands";

	if (weapon != 0 && weapon->m_weapon_attr !=0)
	{
		ret += "#";
		ret += weapon->m_weapon_attr->m_weapon_type;
	}
	return ret;
}

std::string Player::getActionString()
{
	std::string ret = Creature::getActionString();
	ret += getActionWeaponSuffix();

	return ret;
}

void Player::insertLearnableAbility(Action::ActionType type, Vector position, int tab)
{
	int id = m_learnable_abilities.size();
	m_learnable_abilities[id].m_type = type;
	m_learnable_abilities[id].m_skilltree_position = position;
	m_learnable_abilities[id].m_id = id;
	m_learnable_abilities[id].m_skilltree_tab = tab;

}

void Player::insertLearnableAbility(LearnableAbility& ability)
{
	int id = m_learnable_abilities.size();
	m_learnable_abilities[id] = ability;
	m_learnable_abilities[id].m_id = id;
}

bool Player::checkAbilityLearnable(Action::ActionType at)
{
	if (checkAbility(at))
	{
		// Faehigkeit ist schon erlernt
		return false;
	}

	LearnableAbilityMap::iterator it;

	for (it = m_learnable_abilities.begin(); it != m_learnable_abilities.end(); ++it)
	{
		if (it->second.m_type == at)
		{
			// Levelvorraussetzungen
			if (getBaseAttr()->m_level < it->second.m_req_level)
				return false;

			// Faehigkeitenvorraussetzungen
			std::list<Action::ActionType>::iterator jt;
			for (jt = it->second.m_req_abilities.begin(); jt != it->second.m_req_abilities.end(); ++jt)
			{
				if (!checkAbility(*jt))
					return false;
			}
			return true;
		}
	}
	return false;
}

