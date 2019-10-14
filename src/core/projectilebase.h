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

#ifndef __SUMWARS_CORE_PROJECTILEBASE_H__
#define __SUMWARS_CORE_PROJECTILEBASE_H__

#include "hybridimplementation.h"

/**
 * \struct ProjectileBasicData
 * \brief Enthaelt alle Daten fuer die Initialisierung eines Projectile Objekts
 */
struct ProjectileBasicData
{
	/**
	 * \var GameObject::Subtype m_subtype
	 * \brief Subtyp
	 */
	GameObject::Subtype m_subtype;
	
	/**
	 * \var char m_flags
	 * \brief einige Einstellungen
	 */
	char m_flags;
		
	/**
	 * \var float m_radius
	 * \brief Radius des das Objekts
	 */
	float m_radius;
	
	/**
	 * \var float m_lifetime
	 * \brief maximale Lebensdauer
	 */
	float m_lifetime;
	
	/**
	 * \var m_counter
	 * \brief Zaehler, Verwendung je Art des Projektils und aktuellem Zustand
	 */
	int m_counter;
	
	/**
	 * \brief Prozentsatz zu dem der Timer abgelaufen ist, wenn der Effekt ausgeloest wird
	 */
	float m_crit_percent;
	
	/**
	 * \var GameObject::Subtype m_new_projectile_type
	 * \brief Typ des neuen Geschosses, das vom aktuellen erzeugt wird
	 */
	GameObject::Subtype m_new_projectile_type;
	
	/**
	 * \var std::string m_implementation
	 * \brief Steuert die Verhaltensweise
	 */
	std::string m_implementation;
	
	/**
	 * \var HybridImplementation m_effect
	 * \brief beschreibt Effekt beim Auftreffen beziehungsweise bei Ablauf des Timers
	 */
	HybridImplementation m_effect;
};

class Projectile;

#endif // __SUMWARS_CORE_PROJECTILEBASE_H__
