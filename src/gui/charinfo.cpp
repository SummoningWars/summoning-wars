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

#include "charinfo.h"
#include "player.h"
#include "damage.h"

#include "ceguiutility.h"

CharInfo::CharInfo (Document* doc)
	: Window(doc)
{
	// The CharInfo window and holder
	CEGUI::FrameWindow* char_info = (CEGUI::FrameWindow*) (CEGUIUtility::loadLayoutFromFile ("characterscreen.layout"));
	if (!char_info)
	{
		SW_DEBUG("WARNING: Failed to load [%s]", "characterscreen.layout");
	}

	CEGUI::Window* char_info_holder = CEGUIUtility::loadLayoutFromFile ("characterscreen_holder.layout");
	if (!char_info_holder)
	{
		SW_DEBUG("WARNING: Failed to load [%s]", "characterscreen_holder.layout");
	}
	
	CEGUI::Window* wndHolder = CEGUIUtility::getWindowForLoadedLayoutEx (char_info_holder, "CharInfo_Holder");
	CEGUI::Window* wndCharInfo = CEGUIUtility::getWindowForLoadedLayoutEx (char_info, "CharInfo");
	if (wndHolder && wndCharInfo)
	{
		CEGUIUtility::addChildWidget (wndHolder, wndCharInfo);
	}
	else
	{
		if (!wndHolder)
    {
      SW_DEBUG ("ERROR: Unable to get the window holder for char screen.");
    }

		if (!wndCharInfo) 
    {
      SW_DEBUG ("ERROR: Unable to get the window for char screen.");
    }
	}

	m_window = char_info_holder;
	
	char_info->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&Window::consumeEvent, (Window*) this));
	char_info->setWantsMultiClickEvents(false);

	// Bestandteile des Charakterfensters hinzufuegen
	CEGUI::PushButton* btn;
	CEGUI::Window* label;


	
	// Label Level
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "CharInfo/charinfo_aux/Playerinfo/__auto_contentpane__/LevelValueLabel");
	label->setText("1");

	// Label Staerke (Wert)
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "CharInfo/charinfo_aux/Attributes/__auto_contentpane__/StrengthValueLabel");
	label->setText("20");

	// Button Staerke erhoehen
	btn = static_cast<CEGUI::PushButton*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "CharInfo/charinfo_aux/Attributes/__auto_contentpane__/StrengthAddButton"));
	btn->setID(CreatureBaseAttr::STRENGTH);
	btn->setWantsMultiClickEvents(false);
	btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&CharInfo::onIncreaseAttributeButtonClicked, this));
	
	// Label Geschick (Wert)
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "CharInfo/charinfo_aux/Attributes/__auto_contentpane__/DexterityValueLabel");
	label->setText("20");

	// Button Geschick erhoehen
	btn = static_cast<CEGUI::PushButton*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "CharInfo/charinfo_aux/Attributes/__auto_contentpane__/DexterityAddButton"));
	btn->setID(CreatureBaseAttr::DEXTERITY);
	btn->setWantsMultiClickEvents(false);
	btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&CharInfo::onIncreaseAttributeButtonClicked, this));
	
	// Label Willenskraft (Wert)
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "CharInfo/charinfo_aux/Attributes/__auto_contentpane__/WillpowerValueLabel");
	label->setText("20");

	// Button Willenskraft erhoehen
	btn = static_cast<CEGUI::PushButton*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "CharInfo/charinfo_aux/Attributes/__auto_contentpane__/WillpowerAddButton"));
	btn->setID(CreatureBaseAttr::WILLPOWER);
	btn->setWantsMultiClickEvents(false);
	btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&CharInfo::onIncreaseAttributeButtonClicked, this));

	// Label Zauberkraft (Wert)
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "CharInfo/charinfo_aux/Attributes/__auto_contentpane__/MagicpowerValueLabel");
	label->setText("20");

	// Button Zauberkraft erhoehen
	btn = static_cast<CEGUI::PushButton*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "CharInfo/charinfo_aux/Attributes/__auto_contentpane__/MagicpowerAddButton"));
	btn->setID(CreatureBaseAttr::MAGIC_POWER);
	btn->setWantsMultiClickEvents(false);
	btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&CharInfo::onIncreaseAttributeButtonClicked, this));
	
	// Label  Attributpunkte (Wert)
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "CharInfo/charinfo_aux/Attributes/__auto_contentpane__/AttrPointsValueLabel");
	label->setText("5");

	// Label Exp (Wert)
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "CharInfo/charinfo_aux/Playerinfo/__auto_contentpane__/ExperienceValueLabel");
	label->setText("0/100");

	// Label HP (Wert)
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "CharInfo/charinfo_aux/Playerinfo/__auto_contentpane__/HitpointsValueLabel");
	label->setText("0/100");

	// Label Attacke (Wert)
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "CharInfo/charinfo_aux/Offense/__auto_contentpane__/AttackValueLabel");
	label->setText("20");

	// Label Attacke (Wert)
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "CharInfo/charinfo_aux/Offense/__auto_contentpane__/AttackSpeedValueLabel");
	label->setText("20");

	// Label Reichweite (Wert)
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "CharInfo/charinfo_aux/Offense/__auto_contentpane__/RangeValueLabel");
	label->setText("2");

	// Label Durchschlagskraft
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "CharInfo/charinfo_aux/Offense/__auto_contentpane__/PowerValueLabel");
	label->setText("30");

	// Label Ruestung
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "CharInfo/charinfo_aux/Defense/__auto_contentpane__/ArmorValueLabel");
	label->setText("40");

	// Label Block
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "CharInfo/charinfo_aux/Defense/__auto_contentpane__/BlockValueLabel");
	label->setText("20");

	// Label Basisschaden
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "CharInfo/charinfo_aux/Offense/__auto_contentpane__/BaseDmgLabel");
	label->setText((CEGUI::utf8*) gettext("Base damage"));

	// Label Basisschaden (Wert)
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "CharInfo/charinfo_aux/Offense/__auto_contentpane__/BaseDmgValueLabel");
	label->setText("5-10");

	// Label Skill1 Schaden
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "CharInfo/charinfo_aux/Offense/__auto_contentpane__/Skill1DmgLabel");
	label->setText("Skill1");

	// Label Skill1 Schaden (Wert)
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "CharInfo/charinfo_aux/Offense/__auto_contentpane__/Skill1DmgValueLabel");
	label->setText("10-20");

	// Label Skill2 Schaden
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "CharInfo/charinfo_aux/Offense/__auto_contentpane__/Skill2DmgLabel");
	label->setText("Skill2");

	// Label Skill2 Schaden (Wert)
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "CharInfo/charinfo_aux/Offense/__auto_contentpane__/Skill2DmgValueLabel");
	label->setText("20-30");

	// Label physische Resistenz (Wert)
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "CharInfo/charinfo_aux/Defense/__auto_contentpane__/ResistPhysValueLabel");
	label->setText("0");

	// Label Feuerresistenz (Wert)
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "CharInfo/charinfo_aux/Defense/__auto_contentpane__/ResistFireValueLabel");
	label->setText("0");

	// Label Feuerresistenz (Wert)
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "CharInfo/charinfo_aux/Defense/__auto_contentpane__/ResistAirValueLabel");
	label->setText("0");

	// Label Eisresistenz (Wert)
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "CharInfo/charinfo_aux/Defense/__auto_contentpane__/ResistIceValueLabel");
	label->setText("0");
	
	// If the panel also has an auto-close button, connect it to the Cancel/Abort event.
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "CharInfo/__auto_closebutton__");
	if (label)
	{
		label->subscribeEvent(CEGUI::Window::EventMouseClick, CEGUI::Event::Subscriber(&CharInfo::onCloseButtonClicked, this));
	}

	updateTranslation();
}



 void CharInfo::update()
{
	// Spielerobjekt
	Player* player = m_document->getLocalPlayer();
	FightStatistic* fstat = &(player->getFightStatistic());
	std::string tooltip;
	CEGUI::String ttext="";

	// Fenstermanager
	CEGUI::PushButton* btn;
	CEGUI::Window* label;
	std::ostringstream out_stream;

	// Label Name
	// Prepare the name
	out_stream.str("");
	out_stream.str(player->getName().getRawText());
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "CharInfo");
	if (label->isPropertyPresent ("Text"))
	{
		label->setProperty ("Text", (CEGUI::utf8*) out_stream.str().c_str());
	}


	// Label Klasse
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "CharInfo/charinfo_aux/Playerinfo/__auto_contentpane__/ClassValueLabel");
	PlayerBasicData* pdata = ObjectFactory::getPlayerData(player->getSubtype());
	if (pdata != 0)
	{
		out_stream.str(gettext(pdata->m_name.c_str()));
	}
	else
	{
		out_stream.str("");
	}
	
	if (label->getText() != (CEGUI::utf8*) out_stream.str().c_str())
	{
		SW_DEBUG("set class label");
		label->setText((CEGUI::utf8*) out_stream.str().c_str());
	}

	// Label Level
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "CharInfo/charinfo_aux/Playerinfo/__auto_contentpane__/LevelValueLabel");
	out_stream.str("");
	out_stream << (int) player->getBaseAttr()->m_level;
	if (label->getText()!= (CEGUI::utf8*) out_stream.str().c_str())
	{
		label->setText((CEGUI::utf8*) out_stream.str().c_str());
	}

	// Label Staerke
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "CharInfo/charinfo_aux/Attributes/__auto_contentpane__/StrengthValueLabel");
	out_stream.str("");
	out_stream << player->getBaseAttrMod()->m_strength;
	if (label->getText()!= (CEGUI::utf8*) out_stream.str().c_str())
	{
		label->setText((CEGUI::utf8*) out_stream.str().c_str());
	}

	// Label Zauberkraft
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "CharInfo/charinfo_aux/Attributes/__auto_contentpane__/MagicpowerValueLabel");
	out_stream.str("");
	out_stream << player->getBaseAttrMod()->m_magic_power;
	if (label->getText()!= (CEGUI::utf8*) out_stream.str().c_str())
	{
		label->setText((CEGUI::utf8*) out_stream.str().c_str());
	}

	// Label Willenskraft
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "CharInfo/charinfo_aux/Attributes/__auto_contentpane__/WillpowerValueLabel");
	out_stream.str("");
	out_stream << player->getBaseAttrMod()->m_willpower;
	if (label->getText()!= (CEGUI::utf8*) out_stream.str().c_str())
	{
		label->setText((CEGUI::utf8*) out_stream.str().c_str());
	}

	// Label Geschick
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "CharInfo/charinfo_aux/Attributes/__auto_contentpane__/DexterityValueLabel");
	out_stream.str("");
	out_stream << player->getBaseAttrMod()->m_dexterity;
	if (label->getText()!= (CEGUI::utf8*) out_stream.str().c_str())
	{
		label->setText((CEGUI::utf8*) out_stream.str().c_str());
	}

	// Buttons zum erhoehen von Attributen sind nur sichtbar wenn Attributpunkt zu verteilen sind
	bool add_but_vis = false;
	if ( player->getAttributePoints()>0)
	{
		add_but_vis=true;
	}

	// Sichtbarkeit der Buttons setzen
	btn = static_cast<CEGUI::PushButton*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "CharInfo/charinfo_aux/Attributes/__auto_contentpane__/StrengthAddButton"));
	btn->setVisible(add_but_vis);
	btn = static_cast<CEGUI::PushButton*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "CharInfo/charinfo_aux/Attributes/__auto_contentpane__/DexterityAddButton"));
	btn->setVisible(add_but_vis);
	btn = static_cast<CEGUI::PushButton*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "CharInfo/charinfo_aux/Attributes/__auto_contentpane__/WillpowerAddButton"));
	btn->setVisible(add_but_vis);
	btn = static_cast<CEGUI::PushButton*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "CharInfo/charinfo_aux/Attributes/__auto_contentpane__/MagicpowerAddButton"));
	btn->setVisible(add_but_vis);

	// Label freie Attributspunkte
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "CharInfo/charinfo_aux/Attributes/__auto_contentpane__/AttrPointsValueLabel");
	out_stream.str("");
	out_stream << gettext("You have") << " " << player->getAttributePoints() << " " << gettext("attribute points to spend");
	//label->setVisible(add_but_vis);
	if (label->getText()!= (CEGUI::utf8*) out_stream.str().c_str())
	{
		label->setText((CEGUI::utf8*) out_stream.str().c_str());
	}

	// Label Ruestung
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "CharInfo/charinfo_aux/Defense/__auto_contentpane__/ArmorValueLabel");
	out_stream.str("");
	out_stream << player->getBaseAttrMod()->m_armor;
	if (label->getText()!= (CEGUI::utf8*) out_stream.str().c_str())
	{
		label->setText((CEGUI::utf8*) out_stream.str().c_str());
	}
	
	out_stream.str("");
	if (fstat->m_last_attacker.getTranslation() != "")
	{
		out_stream << gettext("Damage dealt by ")<<fstat->m_last_attacker.getTranslation()<<": "<<(int) (fstat->m_damage_got_perc*100) << "%";
	}
	if (label->getTooltipText() != (CEGUI::utf8*) out_stream.str().c_str())
	{
		label->setTooltipText((CEGUI::utf8*) out_stream.str().c_str());
	}

	// Label Attacke
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "CharInfo/charinfo_aux/Offense/__auto_contentpane__/AttackValueLabel");
	out_stream.str("");
	out_stream << player->getBaseDamage().m_attack;
	if (label->getText()!= (CEGUI::utf8*) out_stream.str().c_str())
	{
		label->setText((CEGUI::utf8*) out_stream.str().c_str());
	}
	
	out_stream.str("");
	if (fstat->m_last_attacked.getTranslation()  != "")
	{
		out_stream << gettext("Chance to hit ")<< fstat->m_last_attacked.getTranslation() <<": "<<(int) (fstat->m_hit_chance*100) << "%";
	}
	if (label->getTooltipText() != (CEGUI::utf8*) out_stream.str().c_str())
	{
		label->setTooltipText((CEGUI::utf8*) out_stream.str().c_str());
	}

	// Label Block
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "CharInfo/charinfo_aux/Defense/__auto_contentpane__/BlockLabel" );
	CEGUI::utf8* blocktext;
	if (player->getShield() != 0)
	{	
		blocktext = (CEGUI::utf8*) gettext("Block");
	}
	else
	{
		blocktext = (CEGUI::utf8*) gettext("Evade");
	}
	if (label->getText() != blocktext)
	{
		label->setText(blocktext);
	}
	
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "CharInfo/charinfo_aux/Defense/__auto_contentpane__/BlockValueLabel");
	out_stream.str("");
	out_stream << player->getBaseAttrMod()->m_block;
	if (label->getText()!= (CEGUI::utf8*) out_stream.str().c_str())
	{
		label->setText((CEGUI::utf8*) out_stream.str().c_str());
	}
	
	out_stream.str("");
	if (fstat->m_last_attacker.getTranslation() != "")
	{
		out_stream << gettext("Chance to get hit by ")<<fstat->m_last_attacker.getTranslation()<<": "<<(int) (100-fstat->m_block_chance*100) << "%";
	}
	if (label->getTooltipText() != (CEGUI::utf8*) out_stream.str().c_str())
	{
		label->setTooltipText((CEGUI::utf8*) out_stream.str().c_str());
	}

	// Label HP
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "CharInfo/charinfo_aux/Playerinfo/__auto_contentpane__/HitpointsValueLabel");
	out_stream.str("");
	out_stream << (int) player->getDynAttr()->m_health <<"/" <<(int) player->getBaseAttrMod()->m_max_health;
	if (label->getText()!= (CEGUI::utf8*) out_stream.str().c_str())
	{
		label->setText((CEGUI::utf8*) out_stream.str().c_str());
	}

	// Label Exp
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "CharInfo/charinfo_aux/Playerinfo/__auto_contentpane__/ExperienceValueLabel");
	out_stream.str("");
	out_stream << (unsigned int) player->getDynAttr()->m_experience <<"/" <<(unsigned int) player->getBaseAttr()->m_max_experience;
	if (label->getText()!= (CEGUI::utf8*) out_stream.str().c_str())
	{
		label->setText((CEGUI::utf8*) out_stream.str().c_str());
	}

	// Label Phys Resistenz
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "CharInfo/charinfo_aux/Defense/__auto_contentpane__/ResistPhysValueLabel");
	out_stream.str("");
	out_stream << player->getBaseAttrMod()->m_resistances[Damage::PHYSICAL];
	if (label->getText()!= (CEGUI::utf8*) out_stream.str().c_str())
	{
		label->setText((CEGUI::utf8*) out_stream.str().c_str());
	}

	// Label Feuer Resistenz
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "CharInfo/charinfo_aux/Defense/__auto_contentpane__/ResistFireValueLabel");
	out_stream.str("");
	out_stream << player->getBaseAttrMod()->m_resistances[Damage::FIRE];
	if (label->getText()!= (CEGUI::utf8*) out_stream.str().c_str())
	{
		label->setText((CEGUI::utf8*) out_stream.str().c_str());
	}

	// Label Eis Resistenz
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "CharInfo/charinfo_aux/Defense/__auto_contentpane__/ResistIceValueLabel");
	out_stream.str("");
	out_stream << player->getBaseAttrMod()->m_resistances[Damage::ICE];
	if (label->getText()!= (CEGUI::utf8*) out_stream.str().c_str())
	{
		label->setText((CEGUI::utf8*) out_stream.str().c_str());
	}

	// Label Luft Resistenz
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "CharInfo/charinfo_aux/Defense/__auto_contentpane__/ResistAirValueLabel");
	out_stream.str("");
	out_stream << player->getBaseAttrMod()->m_resistances[Damage::AIR];
	if (label->getText()!= (CEGUI::utf8*) out_stream.str().c_str())
	{
		label->setText((CEGUI::utf8*) out_stream.str().c_str());
	}

	// Label Angriffsgeschwindigkeit
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "CharInfo/charinfo_aux/Offense/__auto_contentpane__/AttackSpeedValueLabel");
	out_stream.str("");
	out_stream << 0.001*MathHelper::Min((short) 5000, player->getBaseAttrMod()->m_attack_speed);
	if (label->getText()!= (CEGUI::utf8*) out_stream.str().c_str())
	{
		label->setText((CEGUI::utf8*) out_stream.str().c_str());
	}

	// Label Reichweite
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "CharInfo/charinfo_aux/Offense/__auto_contentpane__/RangeValueLabel");
	out_stream.str("");
	out_stream << player->getBaseAttrMod()->m_attack_range;
	if (label->getText()!= (CEGUI::utf8*) out_stream.str().c_str())
	{
		label->setText((CEGUI::utf8*) out_stream.str().c_str());
	}

	// Label Durchschlagskraft
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "CharInfo/charinfo_aux/Offense/__auto_contentpane__/PowerValueLabel");
	out_stream.str("");
	out_stream << player->getBaseDamage().m_power;
	if (label->getText()!= (CEGUI::utf8*) out_stream.str().c_str())
	{
		label->setText((CEGUI::utf8*) out_stream.str().c_str());
	}
	
	out_stream.str("");
	if (fstat->m_last_attacked.getTranslation() != "")
	{
		out_stream << gettext("Damage dealt to  ")<< fstat->m_last_attacked.getTranslation()<<": "<<(int) (fstat->m_damage_dealt_perc*100) << "%";
	}
	if (label->getTooltipText() != (CEGUI::utf8*) out_stream.str().c_str())
	{
		label->setTooltipText((CEGUI::utf8*) out_stream.str().c_str());
	}

	Damage dmgb = player->getBaseDamage();
	Damage dmgl = player->getLeftDamage();
	Damage dmgr = player->getRightDamage();
	
	dmgb.normalize();
	dmgl.normalize();
	dmgr.normalize();
	
	
	// Schaden Basisattacke
	float minb=dmgb.getSumMinDamage();
	float maxb=dmgb.getSumMaxDamage();
	// Schaden Attacke links
	float minl=dmgl.getSumMinDamage();
	float maxl=dmgl.getSumMaxDamage();
	// Schaden Attacke rechts
	float minr=dmgr.getSumMinDamage();
	float maxr=dmgr.getSumMaxDamage();



	// Label Schaden Basisattacke
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window,  "CharInfo/charinfo_aux/Offense/__auto_contentpane__/BaseDmgLabel");
	out_stream.str("");
	if (pdata != 0)
	{
		ttext = (CEGUI::utf8*) (Action::getDescription(pdata->m_base_ability).c_str());
		out_stream << Action::getName(pdata->m_base_ability);	
	}
	else
	{
		ttext= (CEGUI::utf8*) "";
	}
	
	if (label->getText()!= (CEGUI::utf8*) out_stream.str().c_str())
	{
		label->setText((CEGUI::utf8*) out_stream.str().c_str());
	}
	
	if (ttext != label->getTooltipText())
	{
		label->setTooltipText(ttext);
	}

	// Label Basisschaden
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "CharInfo/charinfo_aux/Offense/__auto_contentpane__/BaseDmgValueLabel");
	out_stream.str("");
	out_stream << (int) minb << "-" << (int) maxb;
	if (label->getText()!= (CEGUI::utf8*) out_stream.str().c_str())
	{
		label->setText((CEGUI::utf8*) out_stream.str().c_str());
	}
	tooltip =dmgb.getDamageString();
	if (tooltip != label->getTooltipText())
	{
		label->setTooltipText(tooltip);
	}

	// Label Attacke links
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "CharInfo/charinfo_aux/Offense/__auto_contentpane__/Skill1DmgLabel");
	out_stream.str("");
	out_stream << Action::getName(player->getLeftAction());
	if (label->getText()!= (CEGUI::utf8*) out_stream.str().c_str())
	{
		label->setText((CEGUI::utf8*) out_stream.str().c_str());
	}
	
	out_stream.str("");
	out_stream << Action::getDescription(player->getLeftAction());
	CEGUI::String str((CEGUI::utf8*)  out_stream.str().c_str());
	if (label->getTooltipText() != str)
	{
		
		label->setTooltipText(str);
	}

	
	// Label Schaden Attacke links
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "CharInfo/charinfo_aux/Offense/__auto_contentpane__/Skill1DmgValueLabel");
	out_stream.str("");
	out_stream << (int) minl << "-" << (int) maxl;
	if (label->getText()!= (CEGUI::utf8*) out_stream.str().c_str())
	{
		label->setText((CEGUI::utf8*) out_stream.str().c_str());
	}
	ttext = (CEGUI::utf8*) (dmgl.getDamageString().c_str());
	if (ttext != label->getTooltipText())
	{
		label->setTooltipText(ttext);
	}
	



	// Label Attacke rechts
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "CharInfo/charinfo_aux/Offense/__auto_contentpane__/Skill2DmgLabel");
	out_stream.str("");
	out_stream << Action::getName(player->getRightAction());
	if (label->getText()!= (CEGUI::utf8*) out_stream.str().c_str())
	{
		label->setText((CEGUI::utf8*) out_stream.str().c_str());
	}
	
	out_stream.str("");
	out_stream << Action::getDescription(player->getRightAction());
	CEGUI::String str2((CEGUI::utf8*)  out_stream.str().c_str());
	if (label->getTooltipText() != str2)
	{
		
		label->setTooltipText(str2);
	}

	// Label Schaden Attacke rechts
	Action::TargetType dist = Action::getActionInfo(player->getRightAction())->m_target_type;
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "CharInfo/charinfo_aux/Offense/__auto_contentpane__/Skill2DmgValueLabel");
	if (dist == Action::MELEE || dist == Action::RANGED || dist == Action::CIRCLE)
	{	
		out_stream.str("");
		out_stream << (int) minr << "-" << (int) maxr;
		if (label->getText()!= (CEGUI::utf8*) out_stream.str().c_str())
		{
			label->setText((CEGUI::utf8*) out_stream.str().c_str());
		}
		ttext = (CEGUI::utf8*) dmgr.getDamageString().c_str();
		if (ttext != label->getTooltipText())
		{
			label->setTooltipText(ttext);
		}
	}
	else
	{
		if (label->getText()!= "")
			label->setText("");
		if (label->getTooltipText()!="")
			label->setTooltipText("");
	}
	
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "CharInfo/charinfo_aux/Playerinfo/__auto_contentpane__/Portrait");
	std::string portraitname = player->getEmotionImage("normal");
	if (label->getProperty("Image") != portraitname)
	{
		label->setProperty("Image",portraitname);
	}
	
}

void CharInfo::updateTranslation()
{
	CEGUI::Window* label;

	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "CharInfo");
	if (label->isPropertyPresent ("Text"))
	{
		label->setProperty ("Text", (CEGUI::utf8*) gettext("Name"));
	}
	
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "CharInfo/charinfo_aux/Playerinfo");
	label->setText((CEGUI::utf8*) gettext("Player info"));
	
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "CharInfo/charinfo_aux/Attributes");
	label->setText((CEGUI::utf8*) gettext("Attributes"));
	
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "CharInfo/charinfo_aux/Offense");
	label->setText((CEGUI::utf8*) gettext("Offense"));
	
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "CharInfo/charinfo_aux/Defense");
	label->setText((CEGUI::utf8*) gettext("Defense"));

	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "CharInfo/charinfo_aux/Playerinfo/__auto_contentpane__/LevelLabel");
	label->setText((CEGUI::utf8*) gettext("Level"));

	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "CharInfo/charinfo_aux/Playerinfo/__auto_contentpane__/ClassLabel");
	label->setText((CEGUI::utf8*) gettext("Class"));
	
	
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "CharInfo/charinfo_aux/Attributes/__auto_contentpane__/StrengthLabel");
	label->setText((CEGUI::utf8*) gettext("Strength"));
	label->setTooltipText((CEGUI::utf8*) gettext("Strength increases hitpoints, melee damage and attack power."));
	
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "CharInfo/charinfo_aux/Attributes/__auto_contentpane__/DexterityLabel");
	label->setText((CEGUI::utf8*) gettext("Dexterity"));
	label->setTooltipText((CEGUI::utf8*) gettext("Dexterity increases ranged damage, attack accuracy and attack speed"));
	
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "CharInfo/charinfo_aux/Attributes/__auto_contentpane__/WillpowerLabel");
	label->setText((CEGUI::utf8*) gettext("Willpower"));
	label->setTooltipText((CEGUI::utf8*) gettext("Willpower decreases the duration and the impact of status changes \n and increases magical melee damage."));
	
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "CharInfo/charinfo_aux/Attributes/__auto_contentpane__/MagicpowerLabel");
	label->setText((CEGUI::utf8*) gettext("Magic power"));
	label->setTooltipText((CEGUI::utf8*) gettext("Magic power increases the damage of magical attacks."));

	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "CharInfo/charinfo_aux/Playerinfo/__auto_contentpane__/ExperienceLabel" );
 	label->setText((CEGUI::utf8*) gettext("Experience"));
	label->setTooltipText((CEGUI::utf8*) gettext("Current experience and experience necessary for next level-up."));
	
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "CharInfo/charinfo_aux/Playerinfo/__auto_contentpane__/HitpointsLabel");
	label->setText((CEGUI::utf8*) gettext("Hitpoints"));
	label->setTooltipText((CEGUI::utf8*) gettext("Current and maximal hitpoints."));
	
	
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "CharInfo/charinfo_aux/Offense/__auto_contentpane__/AttackLabel" );
	label->setText((CEGUI::utf8*) gettext("Attack"));
	label->setTooltipText((CEGUI::utf8*) gettext("Attack value determines the chance to hit enemies that can block or evade."));
	
	
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "CharInfo/charinfo_aux/Offense/__auto_contentpane__/AttackSpeedLabel" );
	label->setText((CEGUI::utf8*) gettext("Attacks/s"));
	label->setTooltipText((CEGUI::utf8*) gettext("Number of attacks per second."));
	
	
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window,  "CharInfo/charinfo_aux/Offense/__auto_contentpane__/RangeLabel");
	label->setText((CEGUI::utf8*) gettext("Range"));
	label->setTooltipText((CEGUI::utf8*) gettext("Weapon Range in meters."));
	
	
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "CharInfo/charinfo_aux/Offense/__auto_contentpane__/PowerLabel" );
	label->setText((CEGUI::utf8*) gettext("Power"));
	label->setTooltipText((CEGUI::utf8*) gettext("Attack power influences the damage dealt to armored enemies."));
	
	
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window,  "CharInfo/charinfo_aux/Defense/__auto_contentpane__/ArmorLabel");
	label->setText((CEGUI::utf8*) gettext("Armor"));
	label->setTooltipText((CEGUI::utf8*) gettext("Armor reduces the damage of physical attacks."));
	
	
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "CharInfo/charinfo_aux/Defense/__auto_contentpane__/BlockLabel" );
	label->setTooltipText((CEGUI::utf8*) gettext("Block value determines the chance to avoid damage from physical attacks."));
	
	
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window,  "CharInfo/charinfo_aux/Defense/__auto_contentpane__/ResistPhysLabel");
	label->setText((CEGUI::utf8*) gettext("Physical"));
	
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "CharInfo/charinfo_aux/Defense/__auto_contentpane__/ResistAirLabel" );
	label->setText((CEGUI::utf8*) gettext("Air"));
	
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "CharInfo/charinfo_aux/Defense/__auto_contentpane__/ResistIceLabel" );
	label->setText((CEGUI::utf8*) gettext("Ice"));
	
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "CharInfo/charinfo_aux/Defense/__auto_contentpane__/ResistFireLabel" );
	label->setText((CEGUI::utf8*) gettext("Fire"));
	
	
}


bool CharInfo::onIncreaseAttributeButtonClicked(const CEGUI::EventArgs& evt)
{
	const CEGUI::MouseEventArgs& we =
			static_cast<const CEGUI::MouseEventArgs&>(evt);
	unsigned int id = we.window->getID();
	DEBUGX("left button pressed on skill %i",id);
	m_document->increaseAttribute((CreatureBaseAttr::Attribute) id);
	return true;
}

bool CharInfo::onCloseButtonClicked(const CEGUI::EventArgs& evt)
{
	m_document->onButtonCharInfoClicked();
	return true;
}



