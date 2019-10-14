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

#include "damage.h"
#include "eventsystem.h"
#include "gettext.h"
#include "itemfactory.h"
#include "tooltipsettings.h"

Damage::Damage()
{
	init();
}

void Damage::toString(CharConv* cv)
{

	int i;
	for (i=0;i<4;i++)
	{
		cv->toBuffer(m_min_damage[i]);
	}

	for (i=0;i<4;i++)
	{
		cv->toBuffer(m_max_damage[i]);
	}

	for (i=0;i<4;i++)
	{
		cv->toBuffer(m_multiplier[i]);
	}

	cv->toBuffer(m_attack);
	cv->toBuffer(m_power);
	cv->toBuffer(m_crit_perc);

	for (i=0;i<NR_STATUS_MODS;i++)
	{
		cv->toBuffer(m_status_mod_power[i]);
	}

	cv->toBuffer(m_special_flags);
	cv->toBuffer(m_extra_dmg_race);

}


void Damage::fromString(CharConv* cv)
{

	int i;
	for (i=0;i<4;i++)
	{
		cv->fromBuffer(m_min_damage[i]);
	}

	for (i=0;i<4;i++)
	{
		cv->fromBuffer(m_max_damage[i]);
	}

	for (i=0;i<4;i++)
	{
		cv->fromBuffer(m_multiplier[i]);
	}

	cv->fromBuffer(m_attack);
	cv->fromBuffer(m_power);
	cv->fromBuffer(m_crit_perc);

	for (i=0;i<NR_STATUS_MODS;i++)
	{
		cv->fromBuffer(m_status_mod_power[i]);
	}

	cv->fromBuffer(m_special_flags );
	cv->fromBuffer(m_extra_dmg_race);
}


void Damage::init()
{
	m_attack =0;
	m_power =0;
	m_attacker_id =0;
	m_crit_perc =0;
	m_special_flags =0;
	
	for (int i=0; i<NR_STATUS_MODS; i++)
	{
		m_status_mod_power[i] =0;
	}
	
	for (int i=0; i<NR_AI_MODS; i++)
	{
		m_ai_mod_power[i] =0;
	}
	
	// Multiplikatoren auf 1 setzen
	for (int i=0;i<4;i++)
	{
		m_min_damage[i] =0;
		m_max_damage[i] =0;
		m_multiplier[i]=1;
	}

	m_attacker_fraction = Fraction::HOSTILE_TO_ALL;
	m_extra_dmg_race = "norace";
}

void Damage::operator=(Damage& other)
{
	m_attack =other.m_attack;
	m_power =other.m_power;
	m_attacker_id =other.m_attacker_id;
	m_crit_perc = other.m_crit_perc;
	m_special_flags =other.m_special_flags;
	
	for (int i=0; i<NR_STATUS_MODS; i++)
	{
		m_status_mod_power[i] =other.m_status_mod_power[i];
	}
	
	for (int i=0; i<NR_AI_MODS; i++)
	{
		m_ai_mod_power[i] = other.m_ai_mod_power[i];
	}
	
	// Multiplikatoren auf 1 setzen
	for (int i=0;i<4;i++)
	{
		m_min_damage[i] =other.m_min_damage[i];
		m_max_damage[i] =other.m_max_damage[i];
		m_multiplier[i]=other.m_multiplier[i];
	}

	m_attacker_fraction = other.m_attacker_fraction;
	m_extra_dmg_race = other.m_extra_dmg_race;
}

std::string Damage::getDamageTypeName(DamageType dt)
{
	switch(dt)
	{
		case PHYSICAL:
			return gettext("Physical");
		case FIRE:
			return gettext("Fire");
		case ICE:
			return gettext("Ice");
		case AIR:
			return gettext("Air");
	}
	return "";
}

std::string Damage::getDamageResistanceName(DamageType dt)
{
	switch(dt)
	{
		case PHYSICAL:
			return gettext("Physical resistance");
		case FIRE:
			return gettext("Fire resistance");
		case ICE:
			return gettext("Ice resistance");
		case AIR:
			return gettext("Air resistance");
	}
	return "";
}

std::string Damage::getStatusModName(StatusMods sm)
{
	switch(sm)
	{
		case BLIND:
			return gettext("blind");
		case POISONED:
			return gettext("poisoned");
		case MUTE:
			return gettext("mute");
		case CONFUSED:
			return gettext("confused");
		case BERSERK:
			return gettext("berserk");
		case PARALYZED:
			return gettext("paralyzed");
		case FROZEN:
			return gettext("frozen");
		case BURNING:
			return gettext("burning");

	}
	return "";
}

void Damage::normalize()
{
	for (int i=0;i<4;i++)
	{
		m_min_damage[i] *= m_multiplier[i];
		m_max_damage[i] *= m_multiplier[i];
		m_multiplier[i]=1;
	}
}

float Damage::getSumMinDamage()
{
	float s=0;
	for (int i =0;i<4;i++)
		s+=m_min_damage[i];
	return s;
}

float Damage::getSumMaxDamage()
{
	float s=0;
	for (int i =0;i<4;i++)
		s+=m_max_damage[i];
	return s;
}

std::string Damage::getDamageString(Damage::Usage usage, std::string rarityColor, std::bitset<32>* magic_mods)
{
	// this mapping is necessary because order of element is differen in item and damage x(
	int elemmap[4] = {0,3,2,1};
	std::string defaultColor = TooltipSettings::getDefaultCeguiColour ();
	
	std::ostringstream out_stream;
	out_stream.str("");
	bool first = true;
	int i;
	// Schaden anzeigen
	for (i=0;i<4;i++)
	{
		if (m_max_damage[i]>0)
		{
			if (!first)
			{
				out_stream << "\n";
			}

			if (magic_mods && magic_mods->test(ItemFactory::DAMAGE_PHYS_MOD+ elemmap[i])) 
				out_stream << rarityColor;
			
			first = false;
			out_stream <<getDamageTypeName((DamageType) i) <<": "<<(int) m_min_damage[i]<<"-"<< (int) m_max_damage[i];
			
			if (magic_mods && magic_mods->test(ItemFactory::DAMAGE_PHYS_MOD+ elemmap[i])) 
				out_stream << defaultColor;
		}
	}

	// Multiplikatoren anzeigen
	for (i=0;i<4;i++)
	{
		if (m_multiplier[i]!=1 && (m_max_damage[i]>0 || usage ==ITEM))
		{
			if (!first)
				out_stream << "\n";

			if (magic_mods && magic_mods->test(ItemFactory::DAMAGE_MULT_PHYS_MOD+ elemmap[i])) 
				out_stream << rarityColor;
			
			first = false;
			if (m_multiplier[i]>1)
			{
				out_stream <<getDamageTypeName((DamageType) i) <<": +"<<(int) (100*(m_multiplier[i]-1))<< "% "<< gettext("damage");
			}
			else
			{
				out_stream <<getDamageTypeName((DamageType) i) <<": -"<<(int) (100*(1-m_multiplier[i]))<< "% "<< gettext("damage");
			}
			
			if (magic_mods && magic_mods->test(ItemFactory::DAMAGE_MULT_PHYS_MOD+ elemmap[i])) 
				out_stream << defaultColor;
		}
	}

	if (usage != NORMAL)
	{
		// Attackewert anzeigen
		if (m_attack>0)
		{
			if (!first)
				out_stream << "\n";

			first = false;
			if (m_special_flags & UNBLOCKABLE)
			{
				out_stream <<gettext("can not be blocked");
			}
			else
			{
				if (magic_mods && magic_mods->test(ItemFactory::ATTACK_MOD)) 
					out_stream << rarityColor;
				
				out_stream <<gettext("Attack")<<": "<<(int) m_attack;
				
				if (magic_mods && magic_mods->test(ItemFactory::ATTACK_MOD)) 
					out_stream << defaultColor;
			}
		}

		// Durchschlagskraft anzeigen
		if (m_power>0)
		{
			if (!first)
				out_stream << "\n";

			first = false;
			if (m_special_flags & IGNORE_ARMOR)
			{
				out_stream << gettext("ignores armor");
			}
			else
			{
				if (magic_mods && magic_mods->test(ItemFactory::POWER_MOD)) 
					out_stream << rarityColor;
				
				out_stream <<gettext("Power")<<": "<<(int) m_power;// << "\n";
				
				if (magic_mods && magic_mods->test(ItemFactory::POWER_MOD)) 
					out_stream << defaultColor;
			}
		}

		// Chance auf kritische Treffer
		if (m_crit_perc>0 && !first)
		{
			out_stream << "\n";
			out_stream << rarityColor;
			out_stream << gettext("Chance for critical hit")<<": "<<(int) (100*m_crit_perc)<<"%";
			out_stream << defaultColor;
		}
	}

	// Statusveraenderungen
	for (i=0;i<8;i++)
	{
		if (m_status_mod_power[i]>0)
		{
			if (!first)
				out_stream << "\n";

			first = false;
			
			out_stream << rarityColor;
			out_stream <<getStatusModName((StatusMods) i) <<": "<<m_status_mod_power[i];
			out_stream << defaultColor;
		}
 	}

	return out_stream.str();

}

int Damage::getValue(std::string valname)
{
	if (valname =="fire_dmg")
	{
		Vector v(m_min_damage[FIRE], m_max_damage[FIRE]);
		EventSystem::pushVector(EventSystem::getLuaState(), v);
		return 1;
	}
	else if (valname =="fire_mult")
	{
		lua_pushnumber(EventSystem::getLuaState() , m_multiplier[FIRE]);
		return 1;
	}
	else if (valname =="ice_dmg")
	{
		Vector v(m_min_damage[ICE], m_max_damage[ICE]);
		EventSystem::pushVector(EventSystem::getLuaState(), v);
		return 1;
	}
	else if (valname =="ice_mult")
	{
		lua_pushnumber(EventSystem::getLuaState() , m_multiplier[ICE]);
		return 1;
	}
	else if (valname =="air_dmg")
	{
		Vector v(m_min_damage[AIR], m_max_damage[AIR]);
		EventSystem::pushVector(EventSystem::getLuaState(), v);
		return 1;
	}
	else if (valname =="air_mult")
	{
		lua_pushnumber(EventSystem::getLuaState() , m_multiplier[AIR]);
		return 1;
	}
	else if (valname =="phys_dmg")
	{
		Vector v(m_min_damage[PHYSICAL], m_max_damage[PHYSICAL]);
		EventSystem::pushVector(EventSystem::getLuaState(), v);
		return 1;
	}
	else if (valname =="phys_mult")
	{
		lua_pushnumber(EventSystem::getLuaState() , m_multiplier[PHYSICAL]);
		return 1;
	}
	else if (valname =="attack")
	{
		lua_pushnumber(EventSystem::getLuaState() , m_attack);
		return 1;
	}
	else if (valname =="power")
	{
		lua_pushnumber(EventSystem::getLuaState() , m_power);
		return 1;
	}
	else if (valname =="crit_chance")
	{
		lua_pushnumber(EventSystem::getLuaState() , m_crit_perc);
		return 1;
	}
	else if (valname =="blind")
	{
		lua_pushnumber(EventSystem::getLuaState() , m_status_mod_power[BLIND]);
		return 1;
	}
	else if (valname =="poisoned")
	{
		lua_pushnumber(EventSystem::getLuaState() , m_status_mod_power[POISONED]);
		return 1;
	}
	else if (valname =="berserk")
	{
		lua_pushnumber(EventSystem::getLuaState() , m_status_mod_power[BERSERK]);
		return 1;
	}
	else if (valname =="confused")
	{
		lua_pushnumber(EventSystem::getLuaState() , m_status_mod_power[CONFUSED]);
		return 1;
	}
	else if (valname =="mute")
	{
		lua_pushnumber(EventSystem::getLuaState() , m_status_mod_power[MUTE]);
		return 1;
	}
	else if (valname =="paralyzed")
	{
		lua_pushnumber(EventSystem::getLuaState() , m_status_mod_power[PARALYZED]);
		return 1;
	}
	else if (valname =="frozen")
	{
		lua_pushnumber(EventSystem::getLuaState() , m_status_mod_power[FROZEN]);
		return 1;
	}
	else if (valname =="burning")
	{
		lua_pushnumber(EventSystem::getLuaState() , m_status_mod_power[BURNING]);
		return 1;
	}
	else if (valname =="taunt")
	{
		lua_pushnumber(EventSystem::getLuaState() , m_ai_mod_power[TAUNT]);
		return 1;
	}
	else if (valname =="blockable")
	{
		lua_pushboolean(EventSystem::getLuaState() , !(m_special_flags & UNBLOCKABLE));
		return 1;
	}
	else if (valname =="ignore_armor")
	{
		lua_pushboolean(EventSystem::getLuaState() , (m_special_flags & IGNORE_ARMOR));
		return 1;
	}
	else if (valname =="visualize")
	{
		lua_pushboolean(EventSystem::getLuaState() , !(m_special_flags & NOVISUALIZE ));
		return 1;
	}
	else if (valname =="visualize_small")
	{
		lua_pushboolean(EventSystem::getLuaState() , (m_special_flags & VISUALIZE_SMALL ));
		return 1;
	}
	else if (valname =="extra_dmg_race")
	{
		lua_pushstring(EventSystem::getLuaState() , m_extra_dmg_race.c_str());
		return 1;
	}
	else if (valname =="attacker")
	{
		lua_pushinteger(EventSystem::getLuaState() , m_attacker_id);
		return 1;
	}
	return 0;
}

bool Damage::setValue(std::string valname)
{
	if (valname =="fire_dmg")
	{
		Vector v = EventSystem::getVector(EventSystem::getLuaState(),-1);
		m_min_damage[FIRE] = v.m_x;
		m_max_damage[FIRE] = v.m_y;
		return true;
	}
	else if (valname =="fire_mult")
	{
		m_multiplier[FIRE] = lua_tonumber(EventSystem::getLuaState(),-1);
		lua_pop(EventSystem::getLuaState(), 1);
		return true;
	}
	else if (valname =="ice_dmg")
	{
		Vector v = EventSystem::getVector(EventSystem::getLuaState(),-1);
		m_min_damage[ICE] = v.m_x;
		m_max_damage[ICE] = v.m_y;
		return true;
	}
	else if (valname =="ice_mult")
	{
		m_multiplier[ICE] = lua_tonumber(EventSystem::getLuaState(),-1);
		lua_pop(EventSystem::getLuaState(), 1);
		return true;
	}
	else if (valname =="air_dmg")
	{
		Vector v = EventSystem::getVector(EventSystem::getLuaState(),-1);
		m_min_damage[AIR] = v.m_x;
		m_max_damage[AIR] = v.m_y;
		return true;
	}
	else if (valname =="air_mult")
	{
		m_multiplier[AIR] = lua_tonumber(EventSystem::getLuaState(),-1);
		lua_pop(EventSystem::getLuaState(), 1);
	}
	else if (valname =="phys_dmg")
	{
		Vector v = EventSystem::getVector(EventSystem::getLuaState(),-1);
		m_min_damage[PHYSICAL] = v.m_x;
		m_max_damage[PHYSICAL] = v.m_y;
		return true;
	}
	else if (valname =="phys_mult")
	{
		m_multiplier[PHYSICAL] = lua_tonumber(EventSystem::getLuaState(),-1);
		lua_pop(EventSystem::getLuaState(), 1);
	}
	else if (valname =="attack")
	{
		m_attack = lua_tonumber(EventSystem::getLuaState(),-1);
		lua_pop(EventSystem::getLuaState(), 1);
		return true;
	}
	else if (valname =="power")
	{
		m_power = lua_tonumber(EventSystem::getLuaState(),-1);
		lua_pop(EventSystem::getLuaState(), 1);
		return true;
	}
	else if (valname =="crit_chance")
	{
		m_crit_perc = lua_tonumber(EventSystem::getLuaState(),-1);
		lua_pop(EventSystem::getLuaState(), 1);
		return true;
	}
	else if (valname =="blind")
	{
		m_status_mod_power[BLIND] = (short) lua_tonumber(EventSystem::getLuaState(),-1);
		lua_pop(EventSystem::getLuaState(), 1);
		return true;
	}
	else if (valname =="poisoned")
	{
		m_status_mod_power[POISONED] = (short) lua_tonumber(EventSystem::getLuaState(),-1);
		lua_pop(EventSystem::getLuaState(), 1);
		return true;
	}
	else if (valname =="berserk")
	{
		m_status_mod_power[BERSERK] = (short) lua_tonumber(EventSystem::getLuaState(),-1);
		lua_pop(EventSystem::getLuaState(), 1);
		return true;
	}
	else if (valname =="confused")
	{
		m_status_mod_power[CONFUSED] = (short) lua_tonumber(EventSystem::getLuaState(),-1);
		lua_pop(EventSystem::getLuaState(), 1);
		return true;
	}
	else if (valname =="mute")
	{
		m_status_mod_power[MUTE] = (short) lua_tonumber(EventSystem::getLuaState(),-1);
		lua_pop(EventSystem::getLuaState(), 1);
		return true;
	}
	else if (valname =="paralyzed")
	{
		m_status_mod_power[PARALYZED] = (short) lua_tonumber(EventSystem::getLuaState(),-1);
		lua_pop(EventSystem::getLuaState(), 1);
		return true;
	}
	else if (valname =="frozen")
	{
		m_status_mod_power[FROZEN] = (short) lua_tonumber(EventSystem::getLuaState(),-1);
		lua_pop(EventSystem::getLuaState(), 1);
		return true;
	}
	else if (valname =="burning")
	{
		m_status_mod_power[BURNING] = (short) lua_tonumber(EventSystem::getLuaState(),-1);
		lua_pop(EventSystem::getLuaState(), 1);
		return true;
	}
	else if (valname =="taunt")
	{
		m_ai_mod_power[TAUNT] = (short) lua_tonumber(EventSystem::getLuaState(),-1);
		lua_pop(EventSystem::getLuaState(), 1);
		return true;
	}
	else if (valname =="blockable")
	{
		bool b;
		b = lua_toboolean(EventSystem::getLuaState(),-1);
		lua_pop(EventSystem::getLuaState(), 1);
		if (!b)
		{
			m_special_flags |= UNBLOCKABLE ;
		}
		else
		{
			m_special_flags &= ~UNBLOCKABLE ;
		}
		return true;

	}
	else if (valname =="ignore_armor")
	{
		bool b;
		b = lua_toboolean(EventSystem::getLuaState(),-1);
		lua_pop(EventSystem::getLuaState(), 1);
		if (b)
		{
			m_special_flags |= IGNORE_ARMOR ;
		}
		else
		{
			m_special_flags &= ~IGNORE_ARMOR ;
		}
		return true;
	}
	else if (valname =="visualize")
	{
		bool b;
		b = lua_toboolean(EventSystem::getLuaState(),-1);
		lua_pop(EventSystem::getLuaState(), 1);
		if (!b)
		{
			m_special_flags |= NOVISUALIZE ;
		}
		else
		{
			m_special_flags &= ~NOVISUALIZE;
		}
		return true;
	}
	else if (valname =="visualize_small")
	{
		bool b;
		b = lua_toboolean(EventSystem::getLuaState(),-1);
		lua_pop(EventSystem::getLuaState(), 1);
		if (b)
		{
			m_special_flags |= VISUALIZE_SMALL;
		}
		else
		{
			m_special_flags &= ~VISUALIZE_SMALL;
		}
		return true;
	}
	else if (valname =="extra_dmg_race")
	{
		m_extra_dmg_race= lua_tostring(EventSystem::getLuaState(),-1);
		lua_pop(EventSystem::getLuaState(), 1);
		return true;
	}
	else if (valname =="attacker")
	{
		int i;
		i = lua_tointeger(EventSystem::getLuaState(),-1);
		lua_pop(EventSystem::getLuaState(), 1);
		m_attacker_id = i;
		return true;
	}

	return false;
}

