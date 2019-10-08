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

#include "creature.h"
#include "eventsystem.h"
#include "math.h"
#include "itemfactory.h"
#include "mathhelper.h"

//Constructors/Destructors
Creature::Creature(int id) : WorldObject(id)
{
	if (!Creature::init())
	{
		DEBUG("Creature::init() hat false zurückgeliefert");
	}
}


Creature::~Creature()
{
	if (m_small_path_info)
	{
		delete m_small_path_info;
	}

	if (m_medium_path_info)
	{
		delete m_medium_path_info;
	}

	if (m_big_path_info)
	{
		delete m_big_path_info;
	}

	if (m_small_flying_path_info)
	{
		delete m_small_flying_path_info;
	}

	if (m_path_info)
	{
		delete m_path_info;
	}

	if (m_equipement)
	{
		delete m_equipement;
	}

}

//Methods
bool Creature::init()
 {
	 DEBUGX("Creature::init");

	bool tmp=true;

	// init action/command
	m_action.m_type = "noaction";

	m_command.m_type = "noaction";
	m_command.m_damage_mult=1;
	m_next_command.m_type = "noaction";

	m_trade_id=0;
	m_speak_id =0;
	m_dialogue_id =0;
	m_equipement =0;

	m_interaction_flags = COLLISION_DETECTION | USABLE;

	// reset speed
	setSpeed(Vector(0,0));

	// reset pathfind data
	m_small_path_info=0;
	m_small_flying_path_info=0;
	m_medium_path_info=0;
	m_big_path_info=0;
	m_path_info=0;
	m_pathfind_counter =0;

	// clear timers
	for (int i=0; i<NR_TIMERS; i++)
	{
		m_timers[i] = 0;
		m_timers_max[i] = 0;
	}

	m_dyn_attr.m_last_attacker_id=0;
	m_dyn_attr.m_experience=0;

	// clear status mods
	int i;
	for (i=0;i<NR_STATUS_MODS;i++)
	{
		m_dyn_attr.m_status_mod_time[i]=0;
		m_dyn_attr.m_status_mod_immune_time[i]=0;

	}
	m_dyn_attr.m_temp_mods.clear();

	for (i=0;i<NR_EFFECTS;i++)
	{
		m_dyn_attr.m_effect_time[i]=0;
	}

	setState(STATE_ACTIVE,false);
	m_script_command_timer =0;
	clearCommand(true,true);

	m_action.m_action_equip = Action::NO_WEAPON;
	m_action.m_elapsed_time = 0.0;
	m_action.m_time = 2000.0;

	m_dyn_attr.m_health = getBaseAttr()->m_max_health;
	getBaseAttrMod()->m_max_health = getBaseAttr()->m_max_health;
	getBaseAttrMod()->m_power =1;
	m_base_attr.m_max_experience =1;

	clearNetEventMask();

	m_refname = "";
	getTradeInfo().m_trade_partner =0;
	getTradeInfo().m_last_sold_item=0;

	m_emotion_set="";

	return tmp;
}

bool Creature::destroy()
{
	DEBUGX("destroy");

	WorldObject::destroy();
    return true;
}


void Creature::die()
{
	clearFlags();
	// especially removes booster flags etc
	removeAllBaseAttrMod();

	// set status to dieing
	getRegion()->changeObjectGroup(this,DEAD);
	setState(STATE_DIEING);
	m_action.m_type ="die";
	DEBUGX("object died: %p",this);
	m_action.m_time =getActionTime("die");

	m_action.m_elapsed_time =0;

	// send information via network
	addToNetEventMask(NetEvent::DATA_ACTION | NetEvent::DATA_STATUS_MODS | NetEvent::DATA_FLAGS);

	// trigger for the scripting system
	Trigger* tr = new Trigger("unit_die");
	tr->addVariable("unit",getId());
	getRegion()->insertTrigger(tr);
}

bool Creature::canBeAttacked()
{
	return (getState() == STATE_ACTIVE && getDialogueId() == 0);
}

int Creature::getTimerNr(Action::ActionType action)
{
	std::map<std::string, AbilityInfo>::iterator it;
	it = getBaseAttr()->m_abilities.find(action);

	if (it != getBaseAttr()->m_abilities.end())
	{
		// -1 means use the default timer for this action
		// else return the timer number set for this object specifically
		if (it->second.m_timer_nr != -1)
		{
			return it->second.m_timer_nr;
		}

		Action::ActionInfo* aci = Action::getActionInfo(action);
		if (aci ==0)
		{
			return 0;
		}

		return aci->m_timer_nr;
	}
	return 0;
}

float Creature::getTimer(Action::ActionType action)
{
	std::map<std::string, AbilityInfo>::iterator it;
	it = getBaseAttr()->m_abilities.find(action);

	if (it != getBaseAttr()->m_abilities.end())
	{
		// -1 means use the default timer for this action
		// else return the timer number set for this object specifically
		if (it->second.m_timer_nr != -1 && it->second.m_timer != -1)
			return it->second.m_timer;

		Action::ActionInfo* aci = Action::getActionInfo(action);
		if (aci ==0)
			return 0;

		return aci->m_timer;
	}
	return 0;
}

float Creature::getActionTime(Action::ActionType action)
{
	std::map<std::string, AbilityInfo>::iterator it;
	it = getBaseAttrMod()->m_abilities.find(action);

	if (it != getBaseAttrMod()->m_abilities.end())
	{
		// if time set for this object specifically is inequal zero, use this data
		// otherwise, use the default time for the action
		if (it->second.m_time != 0)
			return it->second.m_time;

		Action::ActionInfo* aci = Action::getActionInfo(action);
		if (aci ==0)
		{
			DEBUG("Information for action %s missing ",action.c_str());
			return 0;
		}

		return aci->m_standard_time;
	}
	DEBUG("creature %s cant use action %s",getSubtype().c_str(), action.c_str());
	return 0;
}

void Creature::initAction()
{
	// if idle animation is running, just continue
	if (m_action.m_type== "noaction" && m_action.m_elapsed_time>0)
	{
		return;
	}

	if (m_action.m_type!= "noaction")
	{
		DEBUGX("init Action %s", m_action.m_type.c_str());
	}

	m_action.m_elapsed_time = 0;
	Action::ActionInfo* aci = Action::getActionInfo(m_action.m_type);
	
	// special effect: mute
	if (m_dyn_attr.m_status_mod_time[Damage::MUTE]>0 )
	{
		// if action is affected by mute
		if (aci->m_flags & Action::MUTE_AFFECTED)
		{
			// use the base action
			m_action.m_type = aci->m_base_action;
			DEBUG("using Base Action due to mute");
		}
	}

	// get timer and cooldown for this action
	int timernr = getTimerNr(m_action.m_type);
	float timer = getTimer(m_action.m_type);

	if (timer != 0)
	{
		DEBUGX("timer nr %i time %f Action %s",timernr,timer,m_action.m_type.c_str());
	}

	// endurance skill:
	// reduce cooldown by 15%
	if (checkAbility("endurance") && timer ==1)
	{
		DEBUGX("ausdauer");
		timer *= 0.85;
	}
	
	// ionisation skill:
	// reduce cooldown by 20%
	if (checkAbility("ionisation") 
		&& (m_action.m_type == "lightning" || m_action.m_type == "lightning_strike"
		|| m_action.m_type == "thunderstorm" || m_action.m_type == "thunderstorm2"
		|| m_action.m_type == "chain_lightning" || m_action.m_type == "chain_lightning2"))
	{
		timer *= 0.8;
	}


	// check if the required timer is free
	// if timer is blocked, fall back to base action
	if (timernr != 0)
	{
		if (m_timers[timernr-1] ==0)
		{
			// Timer is free, start it
			m_timers[timernr-1] = timer;
			m_timers_max[timernr-1] = timer;
			addToNetEventMask(NetEvent::DATA_TIMER);
		}
		else
		{
			// Timer is running, use base action
			m_action.m_type = aci->m_base_action;
			aci = Action::getActionInfo(m_action.m_type);

		}
	}

	// set default duration of the action
	m_action.m_time = getActionTime(m_action.m_type);

	Action::ActionType baseact = "noaction";
	Action::ActionInfo* ainfo = Action::getActionInfo(m_action.m_type);
	if (ainfo != 0)
	{
		baseact = ainfo->m_base_action;
	}

	// modify walk time according to walk speed
	if (baseact == "walk")
	{
			m_action.m_time = 1000000 / getBaseAttrMod()->m_walk_speed;
			Vector speed = getSpeed();
			speed *= getBaseAttr()->m_step_length/m_action.m_time;
			setSpeed(speed);

			DEBUGX("walk time %f walk speed %i",m_action.m_time,getBaseAttrMod()->m_walk_speed);
			DEBUGX("pos %f %f speed %f %f",getShape()->m_center.m_x, getShape()->m_center.m_y, getSpeed().m_x,getSpeed().m_y);
	}
	else
	{
		setSpeed(Vector(0,0));
	}



	// modify phyical attacks and magical basic attack by attack speed
	if (baseact == "attack" || baseact == "range_attack" || baseact == "holy_attack" || m_action.m_type == "magic_attack")
	{
		float atksp = MathHelper::Min((short) 5000,getBaseAttrMod()->m_attack_speed);


		m_action.m_time *= 1000000/atksp;

	}

	DEBUGX("resulting time %f",m_action.m_time);
	// rotate the object 
	if (aci->m_target_type != Action::SELF && m_action.m_type != "take_item" && m_action.m_type != "noaction")
	{

		setAngle((m_action.m_goal - getShape()->m_center).angle());

	}
	if (baseact == "walk")
	{
		setAngle(getShape()->m_angle = getSpeed().angle());
	}

	// set additional information for the animation
	m_action.m_action_equip = getActionEquip();

	addToNetEventMask(NetEvent::DATA_ACTION);

	if (m_action.m_time ==0)
	{
		clearCommand(false,true);
		m_action.m_type = "noaction";
	}

	// end command if no action was computed
	if (m_action.m_type == "noaction" && m_command.m_type !="noaction")
	{
		clearCommand(false,true);
	}
	DEBUGX("action %s time %f angle %f",m_action.m_type.c_str(), m_action.m_time,getShape()->m_angle);
}

void Creature::performAction(float &time)
{
	// abort idle action if some command is present
	if (m_action.m_type == "noaction" && m_command.m_type != "noaction")
	{
		return;
	}


	// reciprocal of action duration
	float rezt = 1/m_action.m_time;

	// completion in percent before and after the time
	float p1 = rezt * m_action.m_elapsed_time,p2;

	// time really consumed by the current action
	float dtime;

	// true if current action is finished
	bool finish = false;


	// test if action is finished
	if (time >= m_action.m_time-m_action.m_elapsed_time)
	{
		// action finished
		p2=1;
		dtime = m_action.m_time-m_action.m_elapsed_time;
		time -= dtime;
		finish = true;
	}
	else
	{
		// action not finished
		m_action.m_elapsed_time += time;
		dtime =time;
		time=0;
		p2 = rezt * m_action.m_elapsed_time;
	}

	// check time limits for scripted commands
	// abort command if limit is exceeded
	if (m_script_command_timer>0)
	{
		m_script_command_timer -= time;
		if (m_script_command_timer<=0)
		{
			m_script_command_timer += time;
			clearCommand(false,false);
		}
	}

	if (!World::getWorld()->isServer() && m_action.m_type != "noaction")
	{
		DEBUGX("perform Action %s for %f msec, %f / %f done", m_action.m_type.c_str(),dtime,m_action.m_elapsed_time,m_action.m_time);
	}


	// special logic for walking
	if (m_action.m_type == "walk")
	{

		// check for collisions
		collisionDetection(dtime);

		// move to new location
		Vector newpos = getShape()->m_center + getSpeed()*dtime;
		moveTo(newpos,false);

	}


	Action::ActionInfo* ainfo = Action::getActionInfo(m_action.m_type);
	if (ainfo == 0)
	{
		ERRORMSG("action information missing for action %s",m_action.m_type.c_str());
		m_action.m_type ="noaction";
		clearCommand(false,true);
		return;

	}

	// critical point of the action
	// completion percent where the real effect occurs
	float pct = ainfo->m_critical_perc;

	if (m_action.m_type != "noaction")
	{
		DEBUGX("pos %f %f  speed %f %f  pct %f",getShape()->m_center.m_x, getShape()->m_center.m_y, getSpeed().m_x,getSpeed().m_y,p1);
	}

	// Triple Shot has three critical points
	if (m_action.m_type == "triple_shot" || m_action.m_type == "guided_triple_shot" ||  m_action.m_type =="whirl_blow")
	{
		if (p1>0.3)
			pct = 0.6;
		if (p1>0.6)
			pct = 0.9;
	}

	// target object
	WorldObject* goalobj =0;
	Vector goal = m_action.m_goal;


	// check if the critical point was passed with the current time share
	if (p1<pct && pct <=p2 && World::getWorld()->isServer())
	{
		// real effect of the action is done now
		DEBUGX("critical point %f %f %f",p1,pct,p2);

		// status mod blind
		// modify the direction of the action
		if (m_dyn_attr.m_status_mod_time[Damage::BLIND]>0)
		{
			// create circle around the real target of the action
			// diameter = half distance to target
			// select random point in the circle as new target
			m_action.m_goal_object_id =0;
			Vector dir;
			dir = goal - getShape()->m_center;
			float d = dir.getLength();
			float r = 0.5*d;
			Vector gadd(r,r);
			while (gadd.getLength() > r)
			{
				gadd.m_x = r-2*r*rand()*1.0/RAND_MAX;
				gadd.m_y = r-2*r*rand()*1.0/RAND_MAX;
			}
			goal += gadd;

		}

		// get the target object
		if (m_action.m_goal_object_id!=0 && m_action.m_type!= "take_item")
		{
			// target object given by ID, get it from the Region
			goalobj = getRegion()->getObject(m_action.m_goal_object_id);
		}
		else
		{
			// No target Object ID specifiec
			// look for an object at the target position
			DEBUGX("no Goal ID!");
			if (ainfo->m_target_type == Action::MELEE && m_action.m_type!= "take_item")
			{
				DEBUGX("Searching goal %f %f",goal.m_x,goal.m_y);
				goalobj = getRegion()->getObjectAt(goal,LAYER_AIR);
				
				// second try: search Object on line between attacker and target point
				if (goalobj == 0)
				{
					WorldObjectList hitobj;
					Line line(getShape()->m_center, goal);
					getRegion()->getObjectsOnLine( line, &hitobj, LAYER_AIR);
					
					// get the first object that is really hostile
					WorldObjectList::iterator it;
					for (it = hitobj.begin(); it != hitobj.end(); ++it)
					{
						if (World::getWorld()->getRelation(this->getFraction(), *it) == Fraction::HOSTILE)
						{
							goalobj = *it;
							break;
						}
					}
				}
				DEBUGX("got object %p",goalobj);
			}
		}

		// Party boosting spells are target seeking
		if (ainfo->m_target_type == Action::PARTY)
		{
			Shape s;
			s.m_type = Shape::CIRCLE;
			s.m_center = goal;
			s.m_radius =8;
			WorldObjectList res;
			WorldObjectList::iterator it;

			getRegion()->getObjectsInShape(&s, &res, LAYER_ALL,CREATURE,0);

			// search for the closest friendly player
			float dist = 10000;
			goalobj = this;

			for (it = res.begin(); it != res.end(); ++it)
			{
				if (World::getWorld()->getRelation(getFraction(), (*it)->getFraction()) == Fraction::ALLIED)
				{
					if (goal.distanceTo((*it)->getShape()->m_center) < dist)
					{
						goalobj = (*it);
						dist = goal.distanceTo((*it)->getShape()->m_center);
						DEBUGX("chose player %i with distance %f",(*it)->getId(),dist);
					}
				}
			}

		}
		
		// Target object as Creature*
		Creature* cgoal=0;

		// if no target found yet (for melee)
		// search object at the position where the weapon hits
		if (goalobj ==0 && ainfo->m_target_type == Action::MELEE && m_action.m_type!= "take_item")
		{
			goalobj = getRegion()->getObjectAt(goal,LAYER_AIR);
		}

		// check if the target is a creature
		if (goalobj !=0)
		{
			if (!goalobj->isCreature())
			{
				cgoal =0;
				// lost the target, abort
			}
			else
			{
				cgoal = (Creature*) goalobj;
				goal = cgoal->getShape()->m_center;

				DEBUGX("goal object %p",cgoal);
			}
		}
		// end of logic for searching a target object

		// finally do the critical part
		performActionCritPart(goal, goalobj);
	}	// end of logic for doing the critical part
	
	
	// If the action was finished, the command might be complete
	if (finish)
	{
		// compute distance to target
		float disttogoal = getShape()->m_center.distanceTo(goal);
		if (m_action.m_goal_object_id!=0 && m_action.m_type!= "take_item")
		{
			goalobj = getRegion()->getObject(m_action.m_goal_object_id);
			if (goalobj != 0)
			{
				goal = goalobj->getShape()->m_center;
				disttogoal = goalobj->getShape()->getDistance(*(getShape()));
			}

		}

		// create triggers for the scripting system
		DEBUGX("finished action");
		if (m_action.m_type == "walk")
		{
			Trigger* tr = new Trigger("unit_moved");
			tr->addVariable("unit",getId());
			getRegion()->insertTrigger(tr);

			if (getType() == "PLAYER")
			{
				Trigger* tr = new Trigger("player_moved");
				tr->addVariable("player",getId());
				getRegion()->insertTrigger(tr);
			}

		}

		// complex logic for checking if the command is complete
		// walk is complete if distance to target is smaller than step_length
		// most other commands are complete if the action named in the command was completed (like bash command ends with a bash action)
		// completing the base action of the command action is sufficient as well (e.g. completing a bash command with a melee attack)
		// 
		// charge and storm_charge get special logic
		Action::ActionType baseact = ainfo->m_base_action;
		if ((((m_action.m_type == m_command.m_type) || m_action.m_type == baseact) && m_action.m_type != "walk") || (m_command.m_type == "walk" && disttogoal < getBaseAttr()->m_step_length
				  && !(m_command.m_type == "charge" || m_command.m_type == "storm_charge")))
		{
			// command is complete
			bool recalc = false;

			if (m_command.m_type == "charge" || m_command.m_type == "storm_charge")
			{
				recalc = true;
			}
			DEBUGX("finished command %s (base %s) with action %s",m_command.m_type.c_str(),baseact.c_str(),m_action.m_type.c_str() );
			if (m_command.m_type != "noaction")
			{
				addToNetEventMask(NetEvent::DATA_COMMAND);
				DEBUGX("goal %f %f goalobj %i",goal.m_x, goal.m_y, m_action.m_goal_object_id);
				clearCommand(true,false);
			}
			m_command.m_type = "noaction";
			m_action.m_elapsed_time=0;
			m_command.m_damage_mult = 1;


			// recalc damage (required for charge)
			if (recalc)
			{
				recalcDamage();
			}
		}

		// action is complete
		if (m_action.m_type != "noaction")
		{
			addToNetEventMask(NetEvent::DATA_ACTION);
		}
		m_action.m_type = "noaction";
		m_action.m_elapsed_time =0;
		addToNetEventMask(NetEvent::DATA_ACTION);
	}	// end if action complete
}

void Creature::performActionCritPart(Vector goal, WorldObject* goalobj)
{
	// Target object as Creature* pointer
	Creature* cgoal =0;
	int cgoalid = 0;
	if (goalobj !=0 && !goalobj->isCreature())
	{
		cgoal =0;
	}
	else
	{
		cgoal = (Creature*) goalobj;
		if (cgoal != 0)
		{
			cgoalid= goalobj->getId();
		}
	}

   Action::ActionInfo* ainfo = Action::getActionInfo(m_action.m_type);
   if (ainfo ==0 )
	   return;


	WorldObjectList res;
	res.clear();
	WorldObjectList::iterator it;

	// own center
	Vector &pos = getShape()->m_center;

	Creature* cr =0;
	Projectile* pr =0;

	// base attribute modifications inflicted by the action
	CreatureBaseAttrMod cbam;

	// dynamic attribute modifications inflicted by the action
	CreatureDynAttrMod cdam;

	// calculate the modifiers
	calcActionAttrMod(m_action.m_type,cbam,cdam);

	// type of the missile that might be created by the action
	Projectile::Subtype projtype = ainfo->m_projectile_type;

	// skill windarrows
	if (projtype == "arrow" && m_base_attr_mod.m_special_flags & WIND_ARROWS)
		projtype = "wind_arrow";

	// skill ice arrows
	if (projtype == "arrow" && m_base_attr_mod.m_special_flags & ICE_ARROWS)
		projtype = "ice_arrow";

	// Loop over all effects of the action
	std::list<std::string>::iterator kt;
	for (kt = ainfo->m_effect.m_cpp_impl.begin(); kt != ainfo->m_effect.m_cpp_impl.end(); ++kt)
	{
		// Shape initialized with the shape of this object
		Shape s;
		s.m_center = pos;
		s.m_type = Shape::CIRCLE;
		s.m_radius = ainfo->m_radius;

		if (s.m_radius < 0)
		{
			s.m_radius = getShape()->m_radius + m_command.m_range;
		}

		// Daten fuer Geschosse: Zielrichtung und Startpunkt
		Vector dir = goal - pos;
		Vector dir2,tdir;
		dir.normalize();

		// starting point for missiles:
		// border of this object plus 5% margin
		Vector sproj;
		Fraction::Id fr = m_fraction;
		sproj = pos + dir*1.05*s.m_radius;

		if (*kt == "dmg_at_target")
		{
			// just dish out damage
			if (cgoal != 0)
			{
				cgoal->takeDamage(&m_damage);
			}
		}
		else if (*kt == "dmg_at_enemies_in_radius")
		{
			getRegion()->getObjectsInShape(&s, &res,LAYER_AIR,CREATURE,this);

			// damage all enemies in a circle around this object
			for (it=res.begin();it!=res.end();++it)
			{
				if ((*it)->isCreature())
				{
					cr = (Creature*) (*it);
					cr->takeDamage(&m_damage);

				}
			}
		}
		else if (*kt == "dmg_at_enemies_around_target")
		{
			s.m_center = goal;
			getRegion()->getObjectsInShape(&s, &res,LAYER_AIR,CREATURE,this);

			// damage all enemies in a circle around the target point
			for (it=res.begin();it!=res.end();++it)
			{
				if ((*it)->isCreature())
				{
					cr = (Creature*) (*it);
					cr->takeDamage(&m_damage);

				}
			}
		}
		else if (*kt == "basemod_at_self")
		{
			// apply base modifier on itself
			applyBaseAttrMod(&cbam);
		}
		else if (*kt == "basemod_at_allies_in_radius")
		{
			// apply base modifier on all allies in a circle around this object
			s.m_center = pos;
			getRegion()->getObjectsInShape(&s, &res, LAYER_AIR,CREATURE,0);
			for (it=res.begin();it!=res.end();++it)
			{
				if (World::getWorld()->getRelation(fr,(*it)) == Fraction::ALLIED)
				{
					if ((*it)->isCreature())
					{
						cgoal = (Creature*) (*it);
						cgoal->applyBaseAttrMod(&cbam);
					}
				}
			}
		}
		else if (*kt == "basemod_at_target")
		{
			// apply base modifier on the target object
			if (cgoal != 0)
			{
				cgoal->applyBaseAttrMod(&cbam);
			}
		}
		else if (*kt == "basemod_at_enemies_in_radius")
		{
			// apply base modifier on all enemies in a circle around this object
			s.m_center = pos;
			getRegion()->getObjectsInShape(&s, &res, LAYER_AIR,CREATURE,0);
			for (it=res.begin();it!=res.end();++it)
			{
				if (World::getWorld()->getRelation(fr,(*it)) == Fraction::HOSTILE)
				{
					if ((*it)->isCreature())
					{
						cgoal = (Creature*) (*it);
						cgoal->applyBaseAttrMod(&cbam);
					}
				}
			}
		}
		else if (*kt == "dynmod_at_self")
		{
			// apply dyn modifier on this object
			applyDynAttrMod(&cdam);
		}
		else if (*kt == "dynmod_at_allies_in_radius")
		{
			// apply base modifier on all allies in a circle around this object
			s.m_center = pos;
			getRegion()->getObjectsInShape(&s, &res, LAYER_AIR,CREATURE,0);
			for (it=res.begin();it!=res.end();++it)
			{
				if (World::getWorld()->getRelation(fr,(*it)) == Fraction::ALLIED)
				{
					if ((*it)->isCreature())
					{
						cgoal = (Creature*) (*it);
						cgoal->applyDynAttrMod(&cdam);
					}
				}
			}
		}
		else if (*kt == "dynmod_at_target")
		{
			// apply dyn modifier on the target object
			if (cgoal != 0)
			{
				cgoal->applyDynAttrMod(&cdam);
			}
		}
		else if (*kt == "dynmod_at_enemies_in_radius")
		{
			// apply dyn modifier on all enemies in a circle around this object
			s.m_center = pos;
			getRegion()->getObjectsInShape(&s, &res, LAYER_AIR,CREATURE,0);
			for (it=res.begin();it!=res.end();++it)
			{
				if (World::getWorld()->getRelation(fr,(*it)) == Fraction::HOSTILE)
				{
					if ((*it)->isCreature())
					{
						cgoal = (Creature*) (*it);
						cgoal->applyDynAttrMod(&cdam);
					}
				}
			}
		}
		else if (*kt == "proj_at_target")
		{
			// create a (stationary) missile at the target
			pr = ObjectFactory::createProjectile(projtype);
			if (pr !=0)
			{
				pr->setDamage(&m_damage);
				pr->addFlags(ainfo->m_projectile_flags);
				pr->setGoalObject(cgoalid);
				if (ainfo->m_projectile_counter !=0)
				{
					pr->setCounter(ainfo->m_projectile_counter);
				}
				if (ainfo->m_projectile_radius != 0)
				{
					pr->setRadius(ainfo->m_projectile_radius);
				}
				getRegion()->insertProjectile(pr,goal);
			}
		}
		else if (*kt == "proj_fly_at_target")
		{
			// shoot a flying missile at the target
			pr = ObjectFactory::createProjectile(projtype);

			if (pr !=0)
			{
				pr->setGoalObject(cgoalid);
				pr->setDamage(&m_damage);
				pr->addFlags(ainfo->m_projectile_flags);
				if (ainfo->m_projectile_counter !=0)
				{
					pr->setCounter(ainfo->m_projectile_counter);
				}
				if (ainfo->m_projectile_radius != 0)
				{
					pr->setRadius(ainfo->m_projectile_radius);
				}
				pr->setSpeed(dir*(ainfo->m_projectile_speed/1000000));

				getRegion()->insertProjectile(pr,sproj);
			}
		}
		else if (*kt == "proj_at_self")
		{

			// create a (stationary) missile at the own position
			pr = ObjectFactory::createProjectile(projtype);
			if (pr !=0)
			{
				pr->setDamage(&m_damage);
				pr->addFlags(ainfo->m_projectile_flags);
				if (ainfo->m_projectile_counter !=0)
				{
					pr->setCounter(ainfo->m_projectile_counter);
				}
				DEBUGX("proj radius %f",ainfo->m_projectile_radius);
				if (ainfo->m_projectile_radius != 0)
				{
					pr->setRadius(ainfo->m_projectile_radius);
				}
				getRegion()->insertProjectile(pr,pos);
			}
		}
		else if (*kt == "proj_at_enemies_around_target")
		{
			// create a (stationary) missile at all enemies in a circle around the target
			s.m_center = goal;
			getRegion()->getObjectsInShape(&s, &res,LAYER_AIR,CREATURE,this);

			for (it=res.begin();it!=res.end();++it)
			{
				if ((*it)->isCreature() && World::getWorld()->getRelation(fr,(*it)) == Fraction::HOSTILE)
				{
					pr = ObjectFactory::createProjectile(projtype);
					pr->setDamage(&m_damage);
					pr->addFlags(ainfo->m_projectile_flags);
					if (ainfo->m_projectile_counter !=0)
					{
						pr->setCounter(ainfo->m_projectile_counter);
					}
					if (ainfo->m_projectile_radius != 0)
					{
						pr->setRadius(ainfo->m_projectile_radius);
					}
					cr = (Creature*) (*it);
					getRegion()->insertProjectile(pr,cr->getShape()->m_center);
				}
			}
		}
		else if (*kt == "proj_at_enemies_in_radius")
		{
			// create a (stationary) missile at all enemies in a circle around this object
			s.m_center = pos;
			getRegion()->getObjectsInShape(&s, &res,LAYER_AIR,CREATURE,this);

			for (it=res.begin();it!=res.end();++it)
			{
				if ((*it)->isCreature() && World::getWorld()->getRelation(fr,(*it)) == Fraction::HOSTILE)
				{
					pr = ObjectFactory::createProjectile(projtype);
					if (pr !=0)
					{
						pr->setDamage(&m_damage);
						pr->addFlags(ainfo->m_projectile_flags);
						if (ainfo->m_projectile_counter !=0)
						{
							pr->setCounter(ainfo->m_projectile_counter);
						}
						if (ainfo->m_projectile_radius != 0)
						{
							pr->setRadius(ainfo->m_projectile_radius);
						}
						cr = (Creature*) (*it);
						getRegion()->insertProjectile(pr,cr->getShape()->m_center);
					}
				}
			}
		}
		// specialized implementations
		else if (*kt == "hammer_bash")
		{
			// hammer bash: inflict reduced damage to enemies around the target
			m_damage.m_multiplier[Damage::PHYSICAL] *= 0.5;
			s.m_center = goal;
			s.m_radius = 1.5;
			getRegion()->getObjectsInShape(&s, &res,LAYER_AIR,CREATURE,this);

			for (it=res.begin();it!=res.end();++it)
			{
				if ((*it)->isCreature() && (*it)!=goalobj)
				{
					cr = (Creature*) (*it);
					cr->takeDamage(&m_damage);

				}
			}
		}
		else if (*kt == "multishot" )
		{

			// create 5 arrows
			// third arrows shots directly at the target, the rest scatters
			for (int i=-2;i<=2;i++)
			{
				dir2 = dir;
				dir2.m_x += i*0.15*dir.m_y;
				dir2.m_y -= i*0.15*dir.m_x;
				dir2.normalize();

				sproj = pos+dir2* 1.05 * s.m_radius;
				pr = ObjectFactory::createProjectile(projtype);
				if (pr !=0)
				{
					pr->setDamage(&m_damage);
					pr->setSpeed(dir2/80);

					getRegion()->insertProjectile(pr,sproj);
				}
			}

		}
		else if (*kt == "volley_shot" )
		{
			// create 7 arrows
			// third arrows shots directly at the target, the rest scatters
			for (int i=-3;i<=3;i++)
			{
				dir2 = dir;
				dir2.m_x += i*0.1*dir.m_y;
				dir2.m_y -= i*0.1*dir.m_x;
				dir2.normalize();

				sproj = pos+dir2* 1.05 * s.m_radius;
				pr = ObjectFactory::createProjectile(projtype);
				if (pr !=0)
				{
					pr->setDamage(&m_damage);
					pr->setSpeed(dir2/80);
					getRegion()->insertProjectile(pr,sproj);
				}
			}

		}
		else if (*kt == "battle_cry" )
		{
			// affect all enemies in a radius around this object
			// half attack speed and walk speed
			// duration: 10 seconds
			cbam.m_time =10000;

			getRegion()->getObjectsInShape(&s, &res,LAYER_AIR,CREATURE,this);

			for (it=res.begin();it!=res.end();++it)
			{
				if ((*it)->isCreature() && World::getWorld()->getRelation(getFraction(),(*it)->getFraction()) ==  Fraction::HOSTILE)
				{
					cr = (Creature*) (*it);
					cbam.m_dwalk_speed = -cr->getBaseAttrMod()->m_walk_speed/2;
					cbam.m_dattack_speed = -cr->getBaseAttrMod()->m_attack_speed/2;
					cr->applyBaseAttrMod(&cbam);
				}
			}
		}
		else if (*kt == "scare" )
		{
			// affect all enemies in a radius around this object
			// inflict taunt mod with probability and duration based on their willpower
			getRegion()->getObjectsInShape(&s, &res,LAYER_AIR,CREATURE,this);

			for (it=res.begin();it!=res.end();++it)
			{
				if ((*it)->isCreature() && World::getWorld()->getRelation(getFraction(),(*it)->getFraction()) ==  Fraction::HOSTILE)
				{
					cr = (Creature*) (*it);
					float rel = m_damage.m_ai_mod_power[TAUNT]*1.0 / cr->getBaseAttrMod()->m_willpower;
					float chance = atan(rel)/(3.1415/2);
					
					// make them run away with 50% chance
					if (Random::random() < chance && Random::random() < 0.5)
					{
						// vector pointing away from the caster
						Vector dir = cr->getShape()->m_center - getShape()->m_center;
						// rotate by [-90,90] degreee at random
						float angle = 3.14159*(Random::random()-0.5)*0.5;
						dir.rotate(angle);
						
						float length = MathHelper::Min(8.0f, 3*rel);
						dir.normalize();
						dir *= length;
						
						Vector goal = cr->getShape()->m_center + dir;
						Command cmd;
						cmd.m_goal = goal;
						cmd.m_goal_object_id = 0;
						cmd.m_range = 1;
						cmd.m_type = "walk";
						cr->insertScriptCommand(cmd,5000);
					}
				}
			}
		}		
		else if (*kt == "berserk")
		{
			// inflict berserk on itself
			m_dyn_attr.m_status_mod_time[Damage::BERSERK] = 15000;
			addToNetEventMask(NetEvent::DATA_STATUS_MODS);
		}
		else if (*kt == "speak")
		{
			// try to start a dialogue
			// check that the target object is a creature as well
			// check that neither on has already a dialog or is blocked by script
			if (goalobj != 0 && goalobj->isCreature() && getDialogueId() ==0)
			{
				cr = static_cast<Creature*>(goalobj);
				if (cr->getDialogueId() ==0 && !(cr->hasScriptCommand()))
				{
					// rotate speaker towards each other
					Vector dir = getShape()->m_center - cr->getShape()->m_center;
					setAngle(dir.angle() + PI);
					
					// set up the dialogue
					Dialogue* dia = new Dialogue(getRegion(), cr->getRefName());
					EventSystem::setDialogue(dia);
					dia->addSpeaker(getId(),"player");
					dia->addSpeaker(goalobj->getId(),cr->getRefName());
					dia->addSpeaker(goalobj->getId(),"npc");
					dia->setSpeakerPosition(getId(), Dialogue::LEFT);
					dia->setSpeakerPosition(goalobj->getId(), Dialogue::RIGHT);
					dia->changeTopic("start");

					if (dia->isFinished())
					{
						// no dialogue options available
						delete dia;
					}
					else
					{
						getRegion()->insertDialogue(dia);
						cr->setAngle(dir.angle());
						EventSystem::setDialogue(0);
					}
				}
			}
		}
		else if (*kt == "use")
		{
			// use some object
			if (goalobj)
				goalobj->reactOnUse(getId());
		}

	}	// end loop over C++ implementations

	if (ainfo->m_effect.m_lua_impl != LUA_NOREF)
	{
		// effect decribed by lua code
		// setup environment for action lua code:
		// set Region and Damage object
		// set variables for target, self, projectile_type and projectile_speed
		EventSystem::setRegion(getRegion());
		EventSystem::setDamage(&m_damage);

		EventSystem::pushVector(EventSystem::getLuaState(), goal);
		lua_setglobal(EventSystem::getLuaState(), "goal");

		int target =0;
		if (goalobj !=0)
			target = goalobj->getId();
		lua_pushnumber(EventSystem::getLuaState(),target);
		lua_setglobal(EventSystem::getLuaState(), "target");

		lua_pushnumber(EventSystem::getLuaState(),getId());
		lua_setglobal(EventSystem::getLuaState(), "self");

		lua_pushstring(EventSystem::getLuaState(),projtype.c_str());
		lua_setglobal(EventSystem::getLuaState(), "projectile_type");

		lua_pushnumber(EventSystem::getLuaState(),ainfo->m_projectile_speed);
		lua_setglobal(EventSystem::getLuaState(), "projectile_speed");

		// execute the lua code
		EventSystem::executeCodeReference(ainfo->m_effect.m_lua_impl);

		EventSystem::setDamage(0);

	}

	// monster hunter skill
	// if the target has below 0 HP now, apply some mod
	if ((checkAbility("monster_hunter") || (checkAbility("monster_slayer"))) && cgoal && cgoal->getDynAttr()->m_health<0)
	{
		DEBUGX("monster killed, apply mod");
		CreatureBaseAttrMod cbam;

		// modifications:
		// 5% addition strenght
		// additional attack speed in case of upgraded skill
		cbam.m_dstrength = getBaseAttr()->m_strength / 20;
		cbam.m_time = 10000;
		if (checkAbility("monster_slayer"))
		{
			cbam.m_dattack_speed = 100;
		}
		applyBaseAttrMod(&cbam);

	}
}

void Creature::collisionDetection(float time)
{
	if (getSpeed().getLength() < 0.000001)
		return;

	WorldObjectList result;
	// predicted new position
	Vector newpos = getShape()->m_center + getSpeed()*time;

	// circle around the new position
	// (new base circle plus 5% margin)
	Shape scopy;
	scopy.m_radius = getShape()->m_radius*1.05;
	scopy.m_center = newpos;
	scopy.m_type = Shape::CIRCLE;

	// layer used for collision
	short layer = getLayer();

	// get colliding objects
	getRegion()->getObjectsInShape(&(scopy),&result,layer, CREATURE | FIXED,this);

	if (result.size()!=0)
	{
		// there are colliding objects
		DEBUGX("aktuelle Koordinaten %f %f",getShape()->m_center.m_x, getShape()->m_center.m_y);
		WorldObjectList::iterator i;

		Shape* s2;
        // loop over all colliding objects
		for (i= result.begin();i!=result.end();++i)
		{
			DEBUGX("Kollision %i",(*i)->getId());
			s2 =(*i)->getShape();
			// end movement if the object is the target
			if ((*i)->getId() == getCommand()->m_goal_object_id)
			{
				setSpeed(Vector(0,0));
				break;
			}

			// if action is charge, end it
			if (m_command.m_type == "charge" || m_command.m_type == "storm_charge")
			{
				DEBUGX("charge goal object %i",(*i)->getId());
				m_command.m_goal_object_id = (*i)->getId();
				setSpeed(Vector(0,0));
				return;
			}
			else
			{
				// handle collision otherwise
				handleCollision(s2);
			}

		}

		// speed vector might have changed
		// check if speed is not effectively 0
		if (getSpeed().getLength() > 0.000001)
		{
			// new predicted position after the frame
			newpos = getShape()->m_center + getSpeed()*time;


			// new base circle (again)
			scopy.m_center = newpos;
			DEBUGX("neue Koordinaten %f %f",newpos.m_x, newpos.m_y);
			scopy.m_radius = getShape()->m_radius;
			result.clear();

			// check for colliding objects again
			getRegion()->getObjectsInShape(&(scopy),&result,layer, CREATURE | FIXED,this);

			// loop over colliding objects
			bool change;
			for (i= result.begin();i!=result.end();++i)
			{
				DEBUGX("Kollision %i",(*i)->getId());
				s2 =(*i)->getShape();
				// handle collisions (again)
				change = handleCollision(s2);

				// if direction was changed again, abort the action
				if (change)
				{
					DEBUGX("still colliding");
					setSpeed(Vector(0,0));
					break;
				}

			}
		}

		// if speed is reduced to (almost) zero, the object is stuck
		// first increase the counter, if it reaches a limit abort the movement
		if (getSpeed().getLength() <= 0.000001)
		{
			m_pathfind_counter +=15;
			DEBUGX("counter %f",m_pathfind_counter);
			if (m_pathfind_counter>=50)
			{
				clearCommand(false,true);
			}
		}

	}
}

bool Creature::handleCollision(Shape* s2)
{
	// own position
	Vector pos = getShape()->m_center;

	// position of the colliding object
	Vector cpos = s2->m_center;

	DEBUGX("collision %f %f",cpos.m_x, cpos.m_y);
	DEBUGX("player pos %f %f",pos.m_x, pos.m_y);
	DEBUGX("old speed %f %f", getSpeed().m_x, getSpeed().m_y);

	// project own position on the border of the collidion object
	Vector proj = s2->projectionOnBorder(pos);
	if (proj.distanceTo(pos) < 0.0001)
		proj = cpos;

	DEBUGX("projection %f %f",proj.m_x, proj.m_y);

	// Vektor pointing away from the colliding object
	Vector dir = pos - proj;
	if (cpos.distanceTo(proj) > cpos.distanceTo(pos))
		dir = proj - pos;

	DEBUGX("direction %f %f",dir.m_x, dir.m_y);

	// all directions with an angle less than 90 degree with dir are feasible
	// because they do not reduce the distance to the colliding object
	// this is also important for the case, that objects are already overlapping accidently:
	// you may always move out of the obstacle, but not further in
	
	// find the feasible direction closest to the original speed
	// abort, if this required a turn of more than 90 degree
	Vector sp = getSpeed();
	if (dir*sp < 0)
	{
		sp.normalPartTo(dir);
		setSpeed(sp);
	}
	else
	{
		return false;
	}

	// scale the new speed vector to the original absolute speed
	Vector speed = getSpeed();
	DEBUGX("new speed %f %f", getSpeed().m_x, getSpeed().m_y);
	if (speed.getLength() < 0.000001)
	{
		setSpeed(Vector(0,0));
	}
	else
	{
		speed.normalize();
		DEBUGX("new speed %f %f", getSpeed().m_x, getSpeed().m_y);
		speed *= getBaseAttr()->m_step_length/m_action.m_time;
		setSpeed(speed);
	}

	return true;

}

bool Creature::hasScriptCommand()
{
	return (m_script_command_timer>0 || !m_script_commands.empty());
}

void Creature::insertScriptCommand(Command &cmd, float time)
{
	if (!hasScriptCommand())
	{
		clearCommand(false);
	}
	m_script_commands.push_back(std::make_pair(cmd,time));
	DEBUGX("insert script command %s at %i flag %i",cmd.m_type.c_str(),cmd.m_goal_object_id,cmd.m_flags);
}

void Creature::clearScriptCommands()
{
	if (hasScriptCommand())
	{
		clearCommand(false,true);
	}

	m_script_commands.clear();
	m_script_command_timer =0;
}


void Creature::updateCommand()
{
	// Change command if:
	// a) neither cutscene nor scripting dictates a command and user has entered a new command
	// b) scripted commands exist and current command is complete
	DEBUGX("next command: %s ",m_next_command.m_type.c_str());
	if ((!hasScriptCommand() && m_next_command.m_type != "noaction" && !getRegion()->getCutsceneMode()) ||
			 (!m_script_commands.empty() && m_command.m_type == "noaction"))
	{
		// priority: scripted command superseeds user command
		if (!m_script_commands.empty())
		{

			m_command = m_script_commands.front().first;
			m_script_command_timer = m_script_commands.front().second;
			if (! (m_command.m_flags & Command::REPEAT) )
			{
				m_script_commands.pop_front();

			}
			DEBUGX("script command %s at %i time %f",m_command.m_type.c_str(), m_command.m_goal_object_id, m_script_command_timer);
		}
		else
		{
			m_command.m_type=m_next_command.m_type;
			m_command.m_goal = m_next_command.m_goal;
			m_command.m_goal_object_id=m_next_command.m_goal_object_id;
			m_command.m_range=m_next_command.m_range;
			m_command.m_damage_mult = 1;

			// Naechstes Kommando auf nichts setzen
			m_next_command.m_type = "noaction";

			addToNetEventMask(NetEvent::DATA_NEXT_COMMAND);
		}

		addToNetEventMask(NetEvent::DATA_COMMAND);


	}

	// commands resulting from status mods
	calcStatusModCommand();
}

void Creature::calcAction()
{
	updateCommand();

	// if no command exists, idle
	if (m_command.m_type == "noaction")
	{
		if (m_action.m_type!= "noaction")
		{
			addToNetEventMask(NetEvent::DATA_ACTION);
		}
		m_action.m_type = "noaction";
		return;

	}



	DEBUGX("calc action for command %s",m_command.m_type.c_str());
	addToNetEventMask(NetEvent::DATA_ACTION);

	// range of the command
	// how close you need to get to the target to start the final action
	float range = m_command.m_range;

	/*
	if (m_command.m_type == "walk")
	{
		range =getShape()->m_radius;
	}
	*/

	if (m_command.m_type == "take_item")
		range = 2;

	// target vector
	Vector goal = m_command.m_goal;
	m_action.m_goal = goal;

	// target object
	WorldObject* goalobj=0;

	// own position
	Vector& pos = getShape()->m_center;

	// distance to the target
	float dist = pos.distanceTo(goal)-getShape()->m_radius;

	// get the target from the ID
	if ( m_command.m_goal_object_id !=0 && m_command.m_type != "take_item")
	{
		DEBUGX("goal ID: %i",m_command.m_goal_object_id);
		goalobj = getRegion()->getObject(m_command.m_goal_object_id);

		if (goalobj ==0)
		{
			// lost the target, abort the command
			m_action.m_type = "noaction";
			m_action.m_elapsed_time =0;
			clearCommand(false,true);
			return;
		}

		// check that the Target is active
		if (goalobj->getState() != STATE_ACTIVE)
		{
			DEBUGX("refused to interact with inactive object %i",m_command.m_goal_object_id);
			m_action.m_type = "noaction";
			m_action.m_elapsed_time =0;
			clearCommand(false,true);
			return;
		}

		// if target is given by an object, set the target vector to its position
		goal = goalobj->getShape()->m_center;

		// update distance
		dist = getShape()->getDistance(*(goalobj->getShape()));
	}

	// if action is take_item, the ID refers to a dropped item
	if (m_command.m_type == "take_item")
	{
		DropItem* di = getRegion()->getDropItem(m_command.m_goal_object_id);
		if (di ==0)
		{
			// item lost, abort the command
			m_action.m_type = "noaction";
			m_action.m_elapsed_time =0;
			clearCommand(false,true);
			return;
		}
		else
		{
			Vector pos = di->getPosition();
			goal = pos;
			dist = getShape()->m_center.distanceTo(pos);
		}
	}


	// special logic for charge
	if ((m_command.m_type == "charge" || m_command.m_type == "storm_charge") &&  
		(m_action.m_goal_object_id == 0 || dist > range))
	{
		// calculate position where the next walk command will land
		Vector dir = goal-pos;
		dir.normalize();
		dir *= range;
		
		// Shape where the creature will land
		// and line to this point
		Shape s = *getShape();
		s.m_center = pos + dir;
		Line line(pos,s.m_center);
		
		// captures (most of) the creatures that will be hit during the walk
		WorldObjectList hitobj;
		WorldObjectList::iterator i;
		getRegion()->getObjectsInShape(&s,&hitobj,getLayer(), WorldObject::CREATURE,this);
		getRegion()->getObjectsOnLine(line,&hitobj,getLayer(),WorldObject::CREATURE,this);
		
		// Remove all that are not hostile
		WorldObject* hit;
		i = hitobj.begin();
		hit = (*i);
		while (!hitobj.empty() && (World::getWorld()->getRelation(getFraction(),hit->getFraction()) != Fraction::HOSTILE))
		{
			i=hitobj.erase(i);
			if (i!=hitobj.end())
				hit=(*i);
		}
		
		// hit an object, set it as new target
		if (!hitobj.empty())
		{
			i = hitobj.begin();
			hit = (*i);
			DEBUGX("hit object %i %s", hit->getId(), hit->getName().getTranslation().c_str());
			
			m_action.m_goal_object_id = hit->getId();
			dist =pos.distanceTo(hit->getShape()->m_center);
		}
		else
		{
			range=0;
			dist = 1;
		}
	}	// end of special logic for charge


	// check if the action is one where range matters:
	// melee attack
	// walk
	// berserk active (always go to melee range)
	if (Action::getActionInfo(m_command.m_type)->m_target_type == Action::MELEE 
		|| Action::getActionInfo(m_command.m_type)->m_base_action == "walk" 
		|| m_dyn_attr.m_status_mod_time[Damage::BERSERK]>0)
	{
		DEBUGX("range %f dist %f",range,dist);

		// check if target is in range
		if (range > dist)
		{
			// target is in range
			// if command is walk, just stop, else, do the final action
			if (m_command.m_type != "walk")
			{
				m_action.m_type = m_command.m_type;
				m_action.m_goal_object_id = m_command.m_goal_object_id;
				m_action.m_goal = goal;
			}
			else
			{
				m_action.m_type = "noaction";
				clearCommand(true,false);
			}

		}
		else
		{
			// target is too far away, walk 
			m_action.m_type = "walk";
			// special logic for charge
			if ((m_command.m_type == "charge" || m_command.m_type == "storm_charge")  )
			{
				DEBUGX("Charge");

				// only compute the direction the first time,
				// this is when multiplier is 1
				// else, just accelarates
				if (m_command.m_damage_mult>2)
				{
					// not the first step charge, accelerate
					m_command.m_damage_mult += 2;

					// adapt multiplier according to speed
					Vector speed = getSpeed();
					speed.normalize();

					if (m_command.m_type == "charge")
						m_command.m_damage_mult *= 0.7;
					else
						m_command.m_damage_mult *= 0.8;

					// update the damage
					recalcDamage();

					speed *= sqrt(m_command.m_damage_mult);
					setSpeed(speed);
				}
				else
				{
					// first step of the charging run
					// direction is just towards the target
					DEBUGX("calc charge dir");
					Vector dir = goal-pos;
					dir.normalize();
					setSpeed(dir);

					if (dir.getLength()!=0)
					{
						m_command.m_damage_mult = (m_command.m_damage_mult+2);
						dir *= sqrt(m_command.m_damage_mult);
						setSpeed(dir);
					}
					else
					{
						m_action.m_type = "noaction";
						m_action.m_elapsed_time =0;
					}
				}
			}	// End of special logic for charge
			else
			{
				// compute the walk direction
				calcWalkDir(goal, goalobj);

			}
		}
		m_action.m_goal_object_id = m_command.m_goal_object_id;

	}
	else
	{
		// range does not matter, execute the final action
		m_action.m_type = m_command.m_type;
		m_action.m_goal_object_id = m_command.m_goal_object_id;
		m_action.m_goal = goal;
	}

	// if no action was computed, clear the command
	if (m_action.m_type == "noaction")
	{
		clearCommand(true,false);
	}
}

Action::ActionType Creature::getBerserkAction()
{
	// default: just do normal melee attacks
	Action::ActionType act = "attack";
	
	if (getBaseAttrMod()->m_abilities.count(act) == 0)
	{
		// this happens, when melee attack is not available
		// choose one of the other offensive skills at random
		std::map<std::string, AbilityInfo>::iterator it;
		for (it = getBaseAttrMod()->m_abilities.begin(); it != getBaseAttrMod()->m_abilities.end(); ++it)
		{
			if (it->first == "noaction" || it->first == "die" || it->first == "walk" || it->first == "dead" 
				|| it->first == "take_item" || it->first == "use" || it->first == "speak" || it->first == "trade")
				continue;
			
			Action::ActionInfo* aci2 = Action::getActionInfo(it->first);
			if (aci2 != 0 && aci2->m_timer_nr == 0 && (aci2->m_target_type == Action::MELEE || aci2->m_target_type == Action::RANGED))
			{
				act = it->first;
				DEBUGX("set berserk action to %s",act.c_str());
				
				break;
			}
		}
	}
	return act;
}

void Creature::calcStatusModCommand()
{
	// own position
	Vector& pos = getShape()->m_center;

	if (m_dyn_attr.m_status_mod_time[Damage::BERSERK]>0)
	{
		// berserk action
		int id =0;
		float rmin =1000;

		Vector wpos , goal(0,0);

		// circle with radius 8 around itself
		Shape s;
		s.m_type = Shape::CIRCLE;
		s.m_radius =8;
		s.m_center = pos;
		WorldObjectList res;
		WorldObjectList::iterator i;
		res.clear();

		// get all creatures in the circle
		getRegion()->getObjectsInShape(&s, &res,LAYER_AIR,CREATURE,this);
		if (res.empty())
		{
			WorldObject* wo = getRegion()->getObject(getDynAttr()->m_last_attacker_id);
			if (wo != 0)
			{
				res.push_back(wo);
			}
		}

		// choose the closest, hostile, active creature
		for (i=res.begin();i!= res.end();++i)
		{
			if ((*i)->getState() != STATE_ACTIVE)
			{
				continue;
			}

			DEBUGX("checking obj %i",(*i)->getId());
			if (World::getWorld()->getRelation(m_fraction,*i) == Fraction::HOSTILE)
			{
				DEBUGX("hostile");
				wpos = (*i)->getShape()->m_center;

				if (pos.distanceTo(wpos)<rmin)
				{
					rmin =pos.distanceTo(wpos);
					id = (*i)->getId();
					goal = wpos;
				}

			}
		}

		// if target was found
		if (id !=0)
		{
			getDynAttr()->m_timer.start();
			DEBUGX("attack id %i",id);
			// attack !
			m_command.m_type = getBerserkAction();
			m_command.m_goal_object_id = id;
			m_command.m_goal = goal;
			m_command.m_range = getBaseAttrMod()->m_attack_range;

			// reduce the range to force shooters in to melee
			if (m_command.m_range >4)
			{
				DEBUGX("capped range for berserk");
				m_command.m_range= 2;
			}

		}
		else
		{
			// no target was found, end berserk mode
			m_command.m_type = "noaction";
			if (getDynAttr()->m_timer.getTime() >1000)
			{
				m_dyn_attr.m_status_mod_time[Damage::BERSERK] =0;
				addToNetEventMask(NetEvent::DATA_STATUS_MODS);
			}

		}
		addToNetEventMask(NetEvent::DATA_COMMAND);
	}
	
	// status mod confused
	// only compute this on server because of heavy random influence
	if (m_command.m_type != "noaction" 
		&& Random::randi(3) >0
		&& m_dyn_attr.m_status_mod_time[Damage::CONFUSED]>0 
		&& World::getWorld()->isServer())
	{
		// compute current vector to target
		Vector goal = m_command.m_goal;
		if (m_command.m_goal_object_id != 0)
		{
			WorldObject* obj = getRegion()->getObject(m_command.m_goal_object_id);
			if (obj != 0)
			{
				goal = obj->getShape()->m_center;
			}
		}
		
		// change v by a random angle
		Vector v = goal- getShape()->m_center;
		float angle = 3.14159*(Random::random()-0.5)*0.5;
		v.rotate(angle);

		// 30% chance to reduce distance to melee
		bool melee= false;
		if (Random::random() < 0.0)
		{
			v.normalize();
			v *= (m_base_attr_mod.m_attack_range + getShape()->m_radius);
			melee = true;
		}
		
		Vector newgoal = getShape()->m_center + v;

		// position where we will land with the new movement vector
		Shape s;
		s.m_center =newgoal;
		s.m_type = Shape::CIRCLE;
		s.m_radius = getShape()->m_radius;
		WorldObject* wo =0;
		WorldObjectList res;
		WorldObjectList::iterator it;

		// check something colliding is at the new target point
		getRegion()->getObjectsInShape(&s,&res,LAYER_AIR,CREATURE | FIXED,this);

		if (!res.empty())
		{
			// colliding object found, attack it (no matter if hostile!)
			int num = Random::randi(res.size());
			it = res.begin();
			for (int i=0; i<num; i++)
			{
				++it;
			}
			wo = *it;
			newgoal = wo->getShape()->m_center;
		}
		
		// 30% chance to change the action walk <-> attack
		float r = Random::random();
		if (r < 0.3 && !melee)
		{
			DEBUGX("confused: swap command");
			if (m_command.m_type == "walk")
			{
				m_command.m_type = getBerserkAction();
				m_command.m_range = getBaseAttrMod()->m_attack_range;
			}
			else
			{
				m_command.m_type = "walk";
				m_command.m_range = 2;
			}
		}
		m_command.m_goal = newgoal;
		m_command.m_goal_object_id =0;
		addToNetEventMask(NetEvent::DATA_COMMAND);

		DEBUGX("confused command %s to %f %f",m_command.m_type.c_str(), newgoal.m_x, newgoal.m_y);
	}
	
}

void Creature::calcWalkDir(Vector goal,WorldObject* goalobj)
{
	// maximal distance where normal pathfinding is used
	// this determines the size of the gradient field
	int pathmaxdist = 16;

	// quality of the search, quadratic impact on the computation time
	int qual=4;

	float recalctime = 500;

	bool highquality = true;

	if (getType() == "MONSTER")
	{
		highquality = false;
		pathmaxdist = 16;
		qual = 2;
		recalctime = 1000;
	}

	// own position
	Vector& pos = getShape()->m_center;
	Vector dir;

	// every creature has a gradient field leading to its position
	// if the target is a creature, use the gradient field provieded by the creature
	if (goalobj !=0 && goalobj->isCreature() )
	{
		DEBUGX("using pot field of object %i",goalobj->getId());
		Creature* cr = (Creature*) goalobj;
		cr->getPathDirection(pos,getRegionId(), 2*getShape()->m_radius, getLayer(),dir);
	}
	else
	{
		DEBUGX("using own pot field");
		// target position is not a creature
		// we need another gradient field with the target point as *sink*

		// true, if the gradient field need recalculation
		bool recalc = false;

		// true, if the gradient field is not used, but the plain direction to the target
		bool direct = false;

		// use direct direction if distance is too high for gradient field use
		if (fabs(pos.m_x-goal.m_x)>pathmaxdist || fabs(pos.m_y-goal.m_y)>pathmaxdist)
		{
			direct = true;
		}

		if (m_path_info ==0)
		{
			// data structure does not exist yet
			DEBUGX("allocating new pot field");
			m_path_info = new PathfindInfo;


			int dim = 2*pathmaxdist*qual+1;

			// array with *height*/potential of each field
			m_path_info->m_pot = new Matrix2d<float>(dim,dim);
			// array of blocked fields
			m_path_info->m_block = new Matrix2d<char>(dim,dim);

			// sink of the field is the target point
			m_path_info->m_start= goal;

			m_path_info->m_dim = dim;
			m_path_info->m_layer= LAYER_BASE | LAYER_AIR;
			m_path_info->m_region=getRegionId();
			m_path_info->m_base_size = getShape()->m_radius*2;
			m_path_info->m_quality=qual;
			m_path_info->m_id = getId();
			// recalculate
			recalc = true;

		}
		else
		{

			if ( m_path_info->m_region != getRegionId())
			{
				// gradient field belongs to another region, recalculate
				m_path_info->m_region=getRegionId();
				recalc = true;
			}

			// recalculate if target has changed considerably
			float d = m_path_info->m_start.distanceTo(goal);
			if (d>1)
				recalc = true;

			// recalculate if the field is too old
			if (m_path_info->m_timer > recalctime)
				recalc = true;

		}

		if (!direct)
		{
			if (recalc)
			{
				// recalculate
				DEBUGX("recalc walk field");

				// center of the field is the own position
				m_path_info->m_center.m_x= goal.m_x +roundf(pos.m_x-goal.m_x);
				m_path_info->m_center.m_y=goal.m_y +roundf(pos.m_y-goal.m_y);
				// sink is the target point
				m_path_info->m_start= goal;

				// age of the data
				m_path_info->m_timer=0;

				// matrix of blocked fields
				World::getWorld()->calcBlockmat(m_path_info);

				// compute the potential
				World::getWorld()->calcPotential(m_path_info);

			}

			// compute direction based on the potential field
			World::getWorld()->calcPathDirection(m_path_info, pos, dir);
		}
		else
		{
			// just use the direct position->target direction
			DEBUGX("using direct way");
			dir = goal - pos;
		}

	}

	// direction is computed, now scale it
	dir.normalize();

	if (dir.getLength() ==0)
	{
		// did not compute any valid direction, abort the command
		clearCommand(true,false);
		m_action.m_type = "noaction";
		m_action.m_elapsed_time =0;
		m_command.m_damage_mult=1;
		addToNetEventMask(NetEvent::DATA_COMMAND | NetEvent::DATA_ACTION);
		return;
	}
	else
	{
		// if walk direction changed by more than 90 degree, increase the pathfind counter
		// abort if the counter hits a limit
		// this is used to detect if creatures get stuck and walk up and down
		Vector oldspeed = getSpeed();
		if (oldspeed * dir <0)
		{
			m_pathfind_counter +=10;
			DEBUGX("counter %f",m_pathfind_counter);
			if (m_pathfind_counter >= 50)
			{
				clearCommand(false,true);
				m_action.m_type = "noaction";
				m_action.m_elapsed_time =0;
				m_command.m_damage_mult=1;
				addToNetEventMask(NetEvent::DATA_COMMAND | NetEvent::DATA_ACTION);
			}
		}
		else
		{
			// decrease the counter, if pathfinding was successful
			m_pathfind_counter -=8;
			if (m_pathfind_counter<0)
				m_pathfind_counter =0;
		}

		setSpeed(dir);
	}
}

void Creature::clearCommand(bool success, bool norepeat)
{
	DEBUGX("%s clear command %s %f",getRefName().c_str(),m_command.m_type.c_str() , m_script_command_timer);
	// if the command was created by script
	if (hasScriptCommand() && m_command.m_type!= "noaction")
	{
		// create the event for the scripting engine
		// telling that the command is complete
		m_script_command_timer =0;
		DEBUGX("command %s by %s ended with success %i",m_command.m_type.c_str(), getRefName().c_str(), success);
		Trigger* tr = new Trigger("command_complete");
		tr->addVariable("unit",getId());
		tr->addVariable("command",m_command.m_type);
		tr->addVariable("success",success);
		tr->addVariable("last_command",!(hasScriptCommand()));
		getRegion()->insertTrigger(tr);

		// a repeated command stays in the script command queue
		// if repeated commands should be delete as well, remove it from the queue, too
		if (norepeat && (m_command.m_flags & Command::REPEAT) && !m_script_commands.empty())
		{
			m_script_commands.pop_front();
		}

		// create an event if the script command queue is empty
		if (m_script_commands.empty())
		{
			Trigger* tr = new Trigger("all_commands_complete");
			tr->addVariable("unit",getId());
			tr->addVariable("command",m_command.m_type);
			tr->addVariable("success",success);
			getRegion()->insertTrigger(tr);
		}
	}
	
	// clear all the data fields
	m_command.m_type = "noaction";
	m_command.m_damage_mult = 1;
	m_command.m_goal = Vector(0,0);
	m_command.m_goal_object_id =0;
	m_command.m_range =1;
	m_command.m_flags =0;
	m_script_command_timer =0;
	addToNetEventMask(NetEvent::DATA_COMMAND);
}

bool Creature::update (float time)
{
	WorldObject::update(time);

	// timer just for debugging
	Timer timer;
	timer.start();

	if (m_action.m_elapsed_time> m_action.m_time)
	{
		DEBUG("elapsed time %f all time %f",m_action.m_elapsed_time,	m_action.m_time);
	}

	DEBUGX("Update des Creatureobjekts [%i] wird gestartet", getId());
	// set to false if errors occur
	bool result=true;


	// reduce speech timers
	if (!m_speak_text.empty())
	{
		if (m_speak_text.m_time < time)
		{
			m_speak_text.clear();
		}
		else
		{
			m_speak_text.m_time -= time;
			m_speak_text.m_displayed_time += time;
		}
	}



	// reduce ability cooldown timers
	for (int i=0; i<NR_TIMERS; i++)
	{
		m_timers[i] -= time;
		if (m_timers[i] < 0)
			m_timers[i] = 0;
	}


	// increase age of pathfinding information
	if (m_path_info)
		m_path_info->m_timer += time;

	if (m_small_path_info)
		m_small_path_info->m_timer += time;

	if (m_medium_path_info)
		m_medium_path_info->m_timer += time;

	if (m_big_path_info)
		m_big_path_info->m_timer += time;

	if (m_small_flying_path_info)
		m_small_flying_path_info->m_timer += time;



	// reduce remaining time of status mods and modificaions
	for (int i=0;i<NR_STATUS_MODS;i++)
	{
		if (m_dyn_attr.m_status_mod_time[i] >0)
		{
			m_dyn_attr.m_status_mod_time[i] -= time;
			if (m_dyn_attr.m_status_mod_time[i]<0)
			{
				// end status mod
				m_dyn_attr.m_status_mod_time[i]=0;
				// if confused or berserk ends, cancel the current action
				// as it is usually unintended and harmful
				if (i==Damage::BERSERK || i==Damage::CONFUSED)
				{
					m_command.m_type = "noaction";
					addToNetEventMask(NetEvent::DATA_COMMAND);
				}
			}
		}

		m_dyn_attr.m_status_mod_immune_time[i] -= time;
		if (m_dyn_attr.m_status_mod_immune_time[i]<0)
		{
			m_dyn_attr.m_status_mod_immune_time[i]=0;
		}

	}

	// reduce time for other effects
	for (int i=0;i<NR_EFFECTS;i++)
	{
		m_dyn_attr.m_effect_time[i] -= time;
		if (m_dyn_attr.m_effect_time[i]<0)
		{
			m_dyn_attr.m_effect_time[i]=0;
		}
	}

	// reduce time for base attribute modifiers (boosters/ mali)
	// true if recalculation of the modified base attributes is required
	bool recalc = false;
	std::list<CreatureBaseAttrMod>::iterator i;
	for (i=m_dyn_attr.m_temp_mods.begin();i!=m_dyn_attr.m_temp_mods.end();)
	{

		i->m_time -= time;
		if (i->m_time <=0)
		{
			// duration expired, remove the modification
			DEBUGX("removing base attr mod");
			recalc |= removeBaseAttrMod((CreatureBaseAttrMod*) &(*i));
			i=m_dyn_attr.m_temp_mods.erase(i);
		}
		else
		{
			++i;
		}

	}

	if (recalc)
	{
		// recalculate the modified base attributes
		DEBUGX("modifizierte Attribute neu berechnen");
		calcBaseAttrMod();

	}



	// other special effects
	if (World::getWorld()->isServer())
	{
		if (m_base_attr_mod.m_special_flags & FLAMEARMOR && World::getWorld()->timerLimit(1))
		{
			// flame armor

			// flame armor does damage over timer
			// so damage depends on the frame time
			Damage d;
			d.m_min_damage[Damage::FIRE] = 500*m_base_attr_mod.m_magic_power*0.0003;
			d.m_max_damage[Damage::FIRE] = 500*m_base_attr_mod.m_magic_power*0.0005;
			d.m_multiplier[Damage::FIRE]=1;
			d.m_attacker_fraction = m_fraction;
			d.m_special_flags |= Damage::NOVISUALIZE;

			WorldObjectList res;
			res.clear();
			WorldObjectList::iterator it;

			// circle around own position with radius 1+ own_radius
			Shape s;
			s.m_center  = getShape()->m_center;
			s.m_type = Shape::CIRCLE;
			s.m_radius = getShape()->m_radius+1;
			Creature* cr;

			// damage all hostile creatures in the circle
			getRegion()->getObjectsInShape(&s, &res,LAYER_AIR,CREATURE,this);
			for (it=res.begin();it!=res.end();++it)
			{
				if ((*it)->isCreature())
				{
					cr = (Creature*) (*it);
					cr->takeDamage(&d);

				}
			}

		}
	}	// end flame armor

	// effect of time-dependant status mods
	if (getState()==STATE_ACTIVE)
	{
		if (World::getWorld()->isServer() && World::getWorld()->timerLimit(1))
		{
			// poisoned
			if (m_dyn_attr.m_status_mod_time[Damage::POISONED]>0)
			{
				// Damage per second is 1/90 of max. HP
				DEBUGX("poisoned");
				m_dyn_attr.m_health -= 500*m_base_attr_mod.m_max_health / 90000;
				addToNetEventMask(NetEvent::DATA_HP);
			}

			// burning
			if (m_dyn_attr.m_status_mod_time[Damage::BURNING]>0)
			{
				// Damage per second is 1/60 of max. HP at 0 fire resistance
				DEBUGX("burning");
				m_dyn_attr.m_health -= (100-m_base_attr_mod.m_resistances[Damage::FIRE])*500*m_base_attr_mod.m_max_health / 6000000;
				addToNetEventMask(NetEvent::DATA_HP);
			}
		}

		// frozen: reduce the time for actions to 0
		if (m_dyn_attr.m_status_mod_time[Damage::FROZEN]>0)
		{
			// keine Zeit fuer Aktionen
			time=0;
		}

		// paralyzed: reduce the time for actions by 60%
		if (m_dyn_attr.m_status_mod_time[Damage::PARALYZED]>0)
		{
			time *= 0.4;
		}

		// if health drops to 0, die
		if (m_dyn_attr.m_health <= 0)
		{
			die();
		}
	}

	// loop doing actions as long a time it left
	while (time>0)
	{
		switch (getState())
		{
			case STATE_ACTIVE:
				// if health drops to 0, die
				DEBUGX("health %f",m_dyn_attr.m_health);
				if (m_dyn_attr.m_health <= 0)
				{
					die();
				}
				else
				{
					// while time is left
					DEBUGX("Objekt aktiv");

					while (time>0)
					{
						// calculate a new action if none is set so far
						if (m_action.m_type == "noaction")
						{
							calcAction();
							initAction();
							calcDamage(m_action.m_type,m_damage);
						}

						// and execute it
						performAction(time);

					}
				}
				break;

			// special state: dieing
 			case STATE_DIEING:
				performAction(time);
				if (m_action.m_type == "noaction")
				{
					setState(STATE_DEAD);
					m_action.m_type = "dead";
					m_action.m_time = 1000;
					// flying creatures are deleted faster
					if ((getLayer() & LAYER_BASE) ==0)
					{
						m_action.m_time = 50;
					}

					addToNetEventMask(NetEvent::DATA_ACTION);

					Trigger* tr = new Trigger("unit_dead");
					tr->addVariable("unit",getId());
					getRegion()->insertTrigger(tr);
				}
				break;

			case STATE_DEAD:
				performAction(time);
				if (m_action.m_type == "noaction")
				{
					setDestroyed(true);
				}

			default:
				DEBUGX("unknown state: %i",getState());
				time=0 ;
		}

	}

	if (timer.getTime()>10)
	{
		DEBUGX("object %i update time %f",getId(), timer.getTime());
	}
	return result;
}



void Creature::gainExperience (float exp)
{
	if (getType() != "PLAYER")
		return;


	// add the experience
	m_dyn_attr.m_experience += exp;
	addToNetEventMask(NetEvent::DATA_EXPERIENCE);

	// levelup until experience is below max experience
	// allows to get several levelups at once
	while (m_dyn_attr.m_experience>= m_base_attr.m_max_experience)
	{
		gainLevel();
	}
}

void Creature::gainLevel()
{
	// the real work is done in Player::gainLevel
	addToNetEventMask(NetEvent::DATA_ATTRIBUTES_LEVEL);
}

void  Creature::calcActionAttrMod(Action::ActionType act,CreatureBaseAttrMod & bmod, CreatureDynAttrMod& dmod)
{
	bmod.init();
	dmod.init();

	Action::ActionInfo* ainfo = Action::getActionInfo(m_action.m_type);
	if (ainfo ==0 )
		return;

	// cpp Implementations
	// mainly setting special flags, because they still rely on enums...
	std::list<std::string>::iterator kt;
	for (kt = ainfo->m_base_mod.m_cpp_impl.begin(); kt != ainfo->m_base_mod.m_cpp_impl.end(); ++kt)
	{
		if (*kt == "flamesword")
		{
			bmod.m_xspecial_flags |= (FLAMESWORD | FIRESWORD);
		}
		else if (*kt == "firesword")
		{
			bmod.m_xspecial_flags |= FIRESWORD;
		}
		else if (*kt == "flamearmor")
		{
			bmod.m_xspecial_flags |= FLAMEARMOR;
		}
		else if (*kt == "crit_hits")
		{
			bmod.m_xspecial_flags |= CRIT_HITS;
		}
		else if (*kt == "ice_arrows")
		{
			bmod.m_xspecial_flags |= ICE_ARROWS;
		}
		else if (*kt == "frost_arrows")
		{
			bmod.m_xspecial_flags |= (ICE_ARROWS | FROST_ARROWS);
		}
		else if (*kt == "wind_arrows")
		{
			bmod.m_xspecial_flags |= WIND_ARROWS;
		}
		else if (*kt == "storm_arrows")
		{
			bmod.m_xspecial_flags |= (WIND_ARROWS | STORM_ARROWS);
		}
		else if (*kt == "wind_walk")
		{
			bmod.m_xspecial_flags |= WIND_WALK;
		}
		else if (*kt == "burning_rage")
		{
			bmod.m_xspecial_flags |=  BURNING_RAGE;
		}
		else if (*kt == "static_shield")
		{
			bmod.m_xspecial_flags |= STATIC_SHIELD;
		}
	}

	// Lua Implementation
	if (ainfo->m_base_mod.m_lua_impl != LUA_NOREF)
	{
		// only variable set in the code is
		// self == own ID
		EventSystem::setRegion(getRegion());
		// set the base mod as *current object*
		// and lua code with automatically write to it
		EventSystem::setCreatureBaseAttrMod(&bmod);

		lua_pushnumber(EventSystem::getLuaState(),getId());
		lua_setglobal(EventSystem::getLuaState(), "self");

		// execute the lua chunk
		EventSystem::executeCodeReference(ainfo->m_base_mod.m_lua_impl);
		EventSystem::setCreatureBaseAttrMod(0);
	}

	// Cpp Implementation for dynamic attribute mod
	for (kt = ainfo->m_dyn_mod.m_cpp_impl.begin(); kt != ainfo->m_dyn_mod.m_cpp_impl.end(); ++kt)
	{
		// everything can be done with lua now
	}

	// Lua Implementation for dynamic attribute mod
	if (ainfo->m_dyn_mod.m_lua_impl != LUA_NOREF)
	{
		// only variable set in the code is
		// self == own ID
		EventSystem::setRegion(getRegion());
		// set the dyn mod as *current object*
		// and lua code with automatically write to it
		EventSystem::setCreatureDynAttrMod(&dmod);

		lua_pushnumber(EventSystem::getLuaState(),getId());
		lua_setglobal(EventSystem::getLuaState(), "self");

		// execute the lua chunk
		EventSystem::executeCodeReference(ainfo->m_dyn_mod.m_lua_impl);
		EventSystem::setCreatureDynAttrMod(0);
	}
}

void Creature::calcDamage(Action::ActionType act,Damage& dmg)
{
	// initialize with zeros
	dmg.init();

	// set fraction to avoid friendly fire
	dmg.m_attacker_id = getId();
	dmg.m_attacker_fraction = m_fraction;

	for (int i=0;i<4;i++)
		dmg.m_multiplier[i]=1;

	if (act == "noaction")
		return;

	DEBUGX("Calc Damage for action %s",act.c_str());
	Action::ActionInfo* ainfo = Action::getActionInfo(act);
	if (ainfo ==0 )
		return;

	// loop over C++ damage implementations
	std::list<std::string>::iterator kt;
	for (kt = ainfo->m_damage.m_cpp_impl.begin(); kt != ainfo->m_damage.m_cpp_impl.end(); ++kt)
	{

		// damage provided by attributes and equipement
		calcBaseDamage(*kt,dmg);

		// modify the damage according to the ability
		calcAbilityDamage(*kt,dmg);
	}

	// lua implementation of the damage
	if (ainfo->m_damage.m_lua_impl != LUA_NOREF)
	{
		// only variable set in the code is
		// self == own ID
		EventSystem::setRegion(getRegion());
		// set the damage as *current object*
		// and lua code with automatically write to it
		EventSystem::setDamage(&dmg);

		lua_pushnumber(EventSystem::getLuaState(),getId());
		lua_setglobal(EventSystem::getLuaState(), "self");

		// execute the lua chunk
		EventSystem::executeCodeReference(ainfo->m_damage.m_lua_impl);
		EventSystem::setDamage(0);

	}

	// modifications by passive skills
	if (checkAbility("critical_strike"))
	{
		// 10% extra critical chance
		dmg.m_crit_perc += 0.1;
	}

	if (checkAbility("chill"))
	{
		// 20% additional ice damage
		dmg.m_multiplier[Damage::ICE] *= 1.2;
	}


	// inflame: add some burning proportional to damage and magic power
	if (checkAbility("inflame") && dmg.m_min_damage[Damage::FIRE]>0 )
	{
		dmg.m_status_mod_power[Damage::BURNING] += MathHelper::Min(int(dmg.m_min_damage[Damage::FIRE]) , m_base_attr_mod.m_magic_power);
	}


	// modifications due to buffs

	// firesword
	if (m_base_attr_mod.m_special_flags & FIRESWORD)
	{
		// add fire damage
		// but reduce physical damage
		dmg.m_min_damage[Damage::PHYSICAL]*=0.75;
		dmg.m_max_damage[Damage::PHYSICAL]*=0.75;
		dmg.m_min_damage[Damage::FIRE] += MathHelper::Min(m_base_attr_mod.m_magic_power*4.0f,dmg.m_min_damage[Damage::PHYSICAL]);
		dmg.m_max_damage[Damage::FIRE] += MathHelper::Min(m_base_attr_mod.m_magic_power*6.0f,dmg.m_max_damage[Damage::PHYSICAL]);

		// remove the ice damage
		dmg.m_min_damage[Damage::ICE]=0;
		dmg.m_max_damage[Damage::ICE]=0;

		if (m_base_attr_mod.m_special_flags & FLAMESWORD)
		{
			// for flamesword also add burning
			dmg.m_status_mod_power[Damage::BURNING] += m_base_attr_mod.m_magic_power*3;
		}
		// convert the physical multiplier to fire
		dmg.m_multiplier[Damage::FIRE] *= dmg.m_multiplier[Damage::PHYSICAL];
		dmg.m_multiplier[Damage::PHYSICAL]=1;
	}

	// elven eyes
	if (m_base_attr_mod.m_special_flags & CRIT_HITS)
	{
		// +20% extra critical chance
		dmg.m_crit_perc += 0.2;
	}

	// ice arrows
	if (m_base_attr_mod.m_special_flags & ICE_ARROWS)
	{
		// reduce physical damage
		// but add ice damage
		// remove fire damage
		dmg.m_min_damage[Damage::ICE] += MathHelper::Min(m_base_attr_mod.m_magic_power*4.0f,dmg.m_min_damage[Damage::PHYSICAL]);
		dmg.m_max_damage[Damage::ICE] += MathHelper::Min(m_base_attr_mod.m_magic_power*6.0f,dmg.m_max_damage[Damage::PHYSICAL]);
		dmg.m_min_damage[Damage::PHYSICAL]*=0.5;
		dmg.m_max_damage[Damage::PHYSICAL]*=0.5;
		dmg.m_min_damage[Damage::FIRE]=0;
		dmg.m_max_damage[Damage::FIRE]=0;

		if (m_base_attr_mod.m_special_flags & FROST_ARROWS)
		{
			// for frost arrows also add freeze
			dmg.m_status_mod_power[Damage::FROZEN] += m_base_attr_mod.m_magic_power*3;
		}
		// convert the physical multiplier to ice
		dmg.m_multiplier[Damage::ICE] *= dmg.m_multiplier[Damage::PHYSICAL];
		dmg.m_multiplier[Damage::PHYSICAL]=1;
	}

	// wind arrows
	if (m_base_attr_mod.m_special_flags & WIND_ARROWS)
	{
		// reduce physical damage
		// but add air damage
		dmg.m_min_damage[Damage::AIR] += MathHelper::Min(m_base_attr_mod.m_magic_power*4.0f,dmg.m_min_damage[Damage::PHYSICAL]);
		dmg.m_max_damage[Damage::AIR] += MathHelper::Min(m_base_attr_mod.m_magic_power*6.0f,dmg.m_max_damage[Damage::PHYSICAL]);
		dmg.m_min_damage[Damage::PHYSICAL]*=0.5;
		dmg.m_max_damage[Damage::PHYSICAL]*=0.5;

		if (m_base_attr_mod.m_special_flags & STORM_ARROWS)
		{
			// for storm arrows also add paralyze and even more damage
			dmg.m_status_mod_power[Damage::PARALYZED] += m_base_attr_mod.m_magic_power*2;
			dmg.m_multiplier[Damage::AIR] *= 1.4;
		}
		dmg.m_multiplier[Damage::AIR] *= dmg.m_multiplier[Damage::PHYSICAL];
		dmg.m_multiplier[Damage::PHYSICAL]=1;
	}

	// damage modifications due to status mods
	// blind
	if (m_dyn_attr.m_status_mod_time[Damage::BLIND]>0)
	{
		// no critical hits, reduce precision
		dmg.m_crit_perc=0;
		dmg.m_attack *= 0.5;
	}

	// confused
	if (m_dyn_attr.m_status_mod_time[Damage::CONFUSED]>0)
	{
		// set fraction to hostile to all -> allow friendly fire
		dmg.m_attacker_fraction = Fraction::HOSTILE_TO_ALL;
	}

	// skill burning rage
	if (getBaseAttrMod()->m_special_flags & BURNING_RAGE)
	{
		// 50% addition physical damage
		dmg.m_multiplier[Damage::PHYSICAL] *= 1.5;
	}
}

void Creature::calcBaseDamage(std::string impl ,Damage& dmg)
{
	CreatureBaseAttr* basm = getBaseAttrMod();
	if (impl == "attack")
	{
		// base action is normal melee attack
		// damage based on strength (primary) and dexterity (secondary)
		DEBUGX("base str %i mod str %i",m_base_attr.m_strength,basm->m_strength);

		dmg.m_min_damage[Damage::PHYSICAL] += basm->m_strength/4;
		dmg.m_min_damage[Damage::PHYSICAL] += basm->m_dexterity/10;
		dmg.m_max_damage[Damage::PHYSICAL] += basm->m_strength/3;
		dmg.m_max_damage[Damage::PHYSICAL] += basm->m_dexterity/8;


		dmg.m_attack += basm->m_attack;
		dmg.m_attack += basm->m_dexterity/2;

		dmg.m_power += basm->m_power;
		dmg.m_power += basm->m_strength/2;

		dmg.m_special_flags = Damage::NOFLAGS;

	}

	if (impl == "magic_attack")
	{
		// base action is normal magic missile
		// damage based on magic power (primary) and willpower (secondary)
		dmg.m_min_damage[Damage::FIRE] += basm->m_magic_power/10;
		dmg.m_min_damage[Damage::FIRE] += basm->m_willpower/20;
		dmg.m_max_damage[Damage::FIRE] += basm->m_magic_power/6;
		dmg.m_max_damage[Damage::FIRE] += basm->m_willpower/15;
		dmg.m_special_flags |= Damage::UNBLOCKABLE | Damage::IGNORE_ARMOR;
		dmg.m_attack=0;

	}

	if (impl == "magic_attack_fire")
	{
		// base action is magic fire missile
		// damage based on magic power (primary) and willpower (secondary)
		dmg.m_min_damage[Damage::FIRE] += basm->m_magic_power/6;
		dmg.m_max_damage[Damage::FIRE] += basm->m_magic_power/3;
		dmg.m_special_flags |= Damage::UNBLOCKABLE | Damage::IGNORE_ARMOR;
		dmg.m_attack=0;
	}

	if (impl == "magic_attack_ice")
	{
		// base action is magic ice missile
		// damage based on magic power (primary) and willpower (secondary)
		dmg.m_min_damage[Damage::ICE] += basm->m_magic_power/5;
		dmg.m_max_damage[Damage::ICE] += basm->m_magic_power/4;
		dmg.m_special_flags |= Damage::UNBLOCKABLE | Damage::IGNORE_ARMOR;
		dmg.m_attack=0;
	}

	if (impl == "magic_attack_air")
	{
		// base action is magic air missile
		// damage based on magic power (primary) and willpower (secondary)
		dmg.m_min_damage[Damage::AIR] += basm->m_magic_power/8;
		dmg.m_max_damage[Damage::AIR] += basm->m_magic_power/2;
		dmg.m_special_flags |= Damage::UNBLOCKABLE | Damage::IGNORE_ARMOR;
		dmg.m_attack=0;
	}

	if (impl == "range_attack")
	{
		// base action is normal ranged attack
		// damage based on dexterity (primary) and strength (secondary)
		dmg.m_min_damage[Damage::PHYSICAL] += basm->m_strength/10;
		dmg.m_min_damage[Damage::PHYSICAL] += basm->m_dexterity/6;
		dmg.m_max_damage[Damage::PHYSICAL] += basm->m_strength/8;
		dmg.m_max_damage[Damage::PHYSICAL] += basm->m_dexterity/4;

		dmg.m_attack += basm->m_attack;
		dmg.m_attack += basm->m_dexterity/2;

		dmg.m_power += basm->m_power;
		dmg.m_power += basm->m_strength/2;

		dmg.m_special_flags = Damage::NOFLAGS;

	}

	if (impl == "holy_attack")
	{
		// base action is basic holy attack
		// damage based on willpower (primary) and strength (secondary)
		dmg.m_min_damage[Damage::PHYSICAL] += basm->m_strength/9;
		dmg.m_min_damage[Damage::PHYSICAL] += basm->m_willpower/6;
		dmg.m_max_damage[Damage::PHYSICAL] += basm->m_strength/6;
		dmg.m_max_damage[Damage::PHYSICAL] += basm->m_willpower/3;

		dmg.m_attack += basm->m_attack;
		dmg.m_attack += basm->m_dexterity/2;

		dmg.m_power += basm->m_power;
		dmg.m_power += basm->m_strength/2;

		dmg.m_special_flags = Damage::NOFLAGS;

	}
}

void Creature::calcAbilityDamage(std::string impl, Damage& dmg)
{
	// few special modifications that can not be done in lua
	// apply the speed multiplier for charge
	if (impl == "charge")
	{
		dmg.m_multiplier[Damage::PHYSICAL] *= m_command.m_damage_mult;
		dmg.m_attack *=m_command.m_damage_mult*0.5;
	}
	else if (impl == "storm_charge")
	{

		dmg.m_multiplier[Damage::PHYSICAL] *= m_command.m_damage_mult;
		dmg.m_attack *=m_command.m_damage_mult*0.5;
		dmg.m_status_mod_power[Damage::PARALYZED] += (short)  (m_base_attr_mod.m_strength*m_command.m_damage_mult*0.2);
	}
	else if (impl == "small_numbers")
	{
		dmg.m_special_flags |= Damage::VISUALIZE_SMALL;
	}
}


void Creature::recalcDamage()
{
	// interesting code only in Player::recalcDamage
}

void Creature::calcBaseAttrMod()
{

	int i;
	// if HP or max HP is changed, the ratio is preserved
	float hppercent = 1;
	if (m_base_attr_mod.m_max_health != 0)
	{
		hppercent = m_dyn_attr.m_health / m_base_attr_mod.m_max_health;
	}
	// set all values to the base value
	m_base_attr_mod.m_armor = m_base_attr.m_armor;
	m_base_attr_mod.m_block =m_base_attr.m_block;
	m_base_attr_mod.m_attack =m_base_attr.m_attack;
	m_base_attr_mod.m_power =m_base_attr.m_power;
	m_base_attr_mod.m_strength =m_base_attr.m_strength;
	m_base_attr_mod.m_dexterity =m_base_attr.m_dexterity;
	m_base_attr_mod.m_willpower =m_base_attr.m_willpower;
	m_base_attr_mod.m_magic_power =m_base_attr.m_magic_power;
	m_base_attr_mod.m_walk_speed =m_base_attr.m_walk_speed;
	m_base_attr_mod.m_attack_speed =m_base_attr.m_attack_speed;
	m_base_attr_mod.m_attack_range =m_base_attr.m_attack_range;
	m_base_attr_mod.m_level =m_base_attr.m_level;
	m_base_attr_mod.m_max_health =m_base_attr.m_max_health;
	m_base_attr_mod.m_immunity =m_base_attr.m_immunity;

	m_dyn_attr.m_health = hppercent * m_base_attr.m_max_health;

	for (i=0;i<4;i++)
	{
		m_base_attr_mod.m_resistances[i] = m_base_attr.m_resistances[i];
		m_base_attr_mod.m_resistances_cap[i] = m_base_attr.m_resistances_cap[i];

	}

	m_base_attr_mod.m_abilities = m_base_attr.m_abilities;


	m_base_attr_mod.m_special_flags = m_base_attr.m_special_flags;

	// Reapply all current modifications, but do not add the to the list
	// they are on the list already
	std::list<CreatureBaseAttrMod>::iterator j;
	for (j=m_dyn_attr.m_temp_mods.begin(); j!= m_dyn_attr.m_temp_mods.end();++j)
	{
		DEBUGX("%f / %f",getDynAttr()->m_health, getBaseAttr()->m_max_health);

		applyBaseAttrMod(&(*j),false);
	}

	// modifications due to passive skills
	if (checkAbility("resist_ice"))
	{
		m_base_attr_mod.m_resistances[Damage::ICE] += 20;
		m_base_attr_mod.m_resistances[Damage::AIR] += 20;

	}

	if (checkAbility("resist_air"))
	{
		m_base_attr_mod.m_resistances[Damage::ICE] += 10;
		m_base_attr_mod.m_resistances[Damage::AIR] += 10;
		m_base_attr_mod.m_resistances_cap[Damage::ICE] += 10;
		m_base_attr_mod.m_resistances_cap[Damage::AIR] += 10;

	}
}


bool Creature::takeDamage(Damage* d)
{
	// Check if it can be attacked
	// false if dying or inactive for instance
	if (!canBeAttacked())
		return false;

	DEBUGX("take Damage %i",getId());
	// check if own fraction and the damage fraction are hostile to each other
	if (World::getWorld()->getRelation(d->m_attacker_fraction,this) != Fraction::HOSTILE)
	{
		// no friendly fire
		DEBUGX("not hostile, no dmg");
		DEBUGX("fractions %i %i",d->m_attacker_fraction, this->getFraction());
		return false;
	}


	// check if you are not the damage-dealer
	// (even with confused other other mods, one can never damage itself)
	if (d->m_attacker_id == getId())
	{
		return false;
	}

	bool blocked = false;

	float dmg=0,dmgt;
	short res;
	float rez = 1.0/RAND_MAX;
	// test for block
	float blockchance =0;
	if (!(d->m_special_flags & Damage::UNBLOCKABLE))
	{
		float block = m_base_attr_mod.m_block  ;
		if (m_dyn_attr.m_status_mod_time[Damage::BLIND]>0)
		{
			// blind reduced block chance
			block *= 0.5;
		}

		// compute the block chance the make the roll
		if (d->m_attack>0 && block>0)
		{
			blockchance = 1-atan(d->m_attack/block)/(3.1415/2);


			DEBUGX("attack %f block %f -> blockchance %f",d->m_attack,block, blockchance);
			if (Random::random()<blockchance)
			{
				// blocked, no damage
				blocked = true;
			}
		}



	}

	m_dyn_attr.m_last_attacker_id = d->m_attacker_id;

	// avoid some status mods due to passive skills
	if (checkAbility("concentration"))
	{
		d->m_status_mod_power[Damage::CONFUSED]=0;
	}

	if (checkAbility("mental_wall"))
	{
		d->m_status_mod_power[Damage::BERSERK]=0;
	}

	// check for critical hits
	bool critical = false;
	if (rand()*rez <d->m_crit_perc)
	{
		DEBUGX("critical");
		critical = true;
		d->m_multiplier[Damage::PHYSICAL] *= 3;
	}


	// compute the damage

	// physical damage first
	dmgt = d->m_min_damage[Damage::PHYSICAL] + rand()*rez *(d->m_max_damage[Damage::PHYSICAL] -d->m_min_damage[Damage::PHYSICAL]);
	dmgt *= d->m_multiplier[Damage::PHYSICAL];

	// resistance
	res = MathHelper::Min (m_base_attr_mod.m_resistances_cap[Damage::PHYSICAL],m_base_attr_mod.m_resistances[Damage::PHYSICAL]);
	dmgt *= 0.01*(100-res);

	// armor rating
	float armor = m_base_attr_mod.m_armor;

	// increase armore due to steadfast ability
	float physfaktor = 1.0;
	if (checkAbility("steadfast"))
	{
		float maxhealth = getBaseAttrMod()->m_max_health;
		physfaktor = 1.0 - 0.5*(maxhealth - m_dyn_attr.m_health)/maxhealth;
	}
	dmgt *= physfaktor;

	// reduce the damage due to armor
	float armorfak = 1.0;
	if (armor>0 && !(d->m_special_flags & Damage::IGNORE_ARMOR) && dmgt>0)
	{
		armorfak = atan(d->m_power/armor)/(3.1415/2);
		DEBUGX("power %f armor %f -> damage perc. %f",d->m_power, armor, armorfak);
		dmgt = dmgt*armorfak;
	}

	dmg += dmgt;

	DEBUGX("phys dmg %f",dmgt);

	// update the statistics shows in the character screen
	WorldObject* wo = getRegion()->getObject(d->m_attacker_id);
	FightStatistic* fstat= &(getFightStatistic());
	FightStatistic* attfstat=0;
	Creature* attacker = dynamic_cast<Creature*>(getRegion()->getObject(d->m_attacker_id));
	if (wo !=0  && wo->getType() == "PLAYER")
	{
		attfstat = &(attacker->getFightStatistic());
	}

	// update own statistic data
	if (wo !=0 && getType()=="PLAYER")
	{
		if (fabs(fstat->m_block_chance - blockchance) >0.01
				  || fabs(fstat->m_damage_got_perc - armorfak) >0.01
				  || fstat->m_last_attacker != wo->getName())
		{
			fstat->m_block_chance = blockchance;
			fstat->m_last_attacker = wo->getName();
			fstat->m_damage_got_perc = armorfak;
			addToNetEventMask(NetEvent::DATA_FIGHT_STAT);

			DEBUGX("blockchance %f damage perc %f attacker %s",blockchance, armorfak, fstat->m_last_attacker.getTranslation().c_str());
		}
	}

	// update attackers statistic data
	if (attacker !=0 && attacker->getType()=="PLAYER")
	{
		attacker->updateFightStat(1-blockchance, armorfak,getName());
	}

	// if damage was blocked end here
	if (blocked)
	{
		FloatingText::Size size = FloatingText::NORMAL;
		if (d->m_special_flags & Damage::VISUALIZE_SMALL)
		{
			size =FloatingText::SMALL;
		}
		getRegion()->createFloatingText(TranslatableString("Miss", "menu"),getShape()->m_center,size);
		// play sound
		getRegion()->playSound("block", getShape()->m_center, 1.0, true);
		
		return true;
	}

	// remaining three damage types
	int i;
	for (i=Damage::AIR;i<=Damage::FIRE;i++)
	{
		dmgt = d->m_min_damage[i] + rand()*rez *(d->m_max_damage[i] -d->m_min_damage[i]);
		dmgt *= d->m_multiplier[i];

		// apply resistance
		res = MathHelper::Min(m_base_attr_mod.m_resistances_cap[i],m_base_attr_mod.m_resistances[i]);
		dmgt *=0.01*(100-res);

		DEBUGX("dmg %i min %f max %f real %f",i,d->m_min_damage[i],d->m_max_damage[i],dmgt);
		dmg += dmgt;

	}

	float t;
	// apply status mods: loop over status mods
	for (i=0;i<NR_STATUS_MODS;i++)
	{
		// check that mod power is >0
		// and this creature is neither permanent nor temporily immune
		if (d->m_status_mod_power[i]>0 && !(m_base_attr_mod.m_immunity & (1 << i)) && m_dyn_attr.m_status_mod_immune_time[i]==0)
		{
			// compute chance to be affected
			float rel;
			if (m_base_attr_mod.m_willpower == 0)
			{
				rel = 1000000;
			}
			else
			{
				rel = d->m_status_mod_power[i]*1.0 / m_base_attr_mod.m_willpower;
			}
			float chance = atan(rel)/(3.1415/2);
			DEBUGX("mod %i modpow %i wp %i chance %f",i,d->m_status_mod_power[i],m_base_attr_mod.m_willpower,chance);
			
			// make the roll
			if (Random::random()<chance)
			{
				// apply modification
				// duration depends on the type and on modpower/willpower ratio
				float times[NR_STATUS_MODS] = {15000 /* BLIND*/,10000 /*POISONED*/ ,10000 /*BERSERK*/ ,8000 /*CONFUSED*/,15000 /*MUTE*/,4000 /*PARALYZED*/,2500 /*FROZEN*/,10000 /*BURNING*/};
				t = rel * times[i];
				if (t>m_dyn_attr.m_status_mod_time[i] && t>500)
				{
					m_dyn_attr.m_status_mod_time[i] =t;
					addToNetEventMask(NetEvent::DATA_STATUS_MODS);
				}

				DEBUGX("applying status mod %i for %f ms",i,t);
			}
		}
	}

	DEBUGX("sum dmg %f / %f",dmg,getDynAttr()->m_health );

	// triple damage, if the damage is especially efficient agains the own race
	if (d->m_extra_dmg_race == getRace() && getRace() != "")
	{
		DEBUGX("triple damage");
		dmg *= 3;
		critical = true;
	}
	
	// defensive passive abilities agains specific races (priest)
	if (attacker != 0 && attacker->getRace() != "")
	{
		if ((checkAbility("dmgreduce_undead") && attacker->getRace() == "undead")
			|| (checkAbility("dmgreduce_demon") && attacker->getRace() == "demon")
			|| (checkAbility("dmgreduce_elemental") && attacker->getRace() == "elemental"))
		{
			dmg *= (2.0/3.0);
		}
	}

	// reduce the hitpoints (finally)
	getDynAttr()->m_health -= dmg;

	// apply bleeding effect
	if (dmg>0)
	{
		m_dyn_attr.m_effect_time[CreatureDynAttr::BLEEDING] = MathHelper::Max(m_dyn_attr.m_effect_time[CreatureDynAttr::BLEEDING],250.0f);
		addToNetEventMask(NetEvent::DATA_HP | NetEvent::DATA_EFFECTS);
	}

	// if health drops below 0, die
	if (getDynAttr()->m_health <= 0)
	{
		die();
	}

	// static shield can trigger if more than 2% of the HP were lost
	if ((m_base_attr_mod.m_special_flags & STATIC_SHIELD) && dmg > m_base_attr_mod.m_max_health * 0.02)
	{
		// compute static shield damage
		Damage dmg;
		dmg.m_status_mod_power[Damage::PARALYZED] = m_base_attr_mod.m_magic_power;
		dmg.m_min_damage[Damage::AIR] = m_base_attr_mod.m_magic_power*0.2;
		dmg.m_max_damage[Damage::AIR] = m_base_attr_mod.m_magic_power*0.3;
		dmg.m_multiplier[Damage::AIR]=1;
		dmg.m_attacker_fraction = m_fraction;

		// create the static shield effect
		Projectile* pr = ObjectFactory::createProjectile("static_shield");
		if (pr !=0)
		{
			pr->setDamage(&dmg);
			pr->getShape()->m_radius =getShape()->m_radius+1;
			getRegion()->insertProjectile(pr,getShape()->m_center);
		}
	}


	// create event for the scripting engine
	Trigger* tr = new Trigger("unit_hit");
	tr->addVariable("defender",getId());
	tr->addVariable("attacker",d->m_attacker_id);
	tr->addVariable("damage",dmg);
	getRegion()->insertTrigger(tr);

	WorldObject::takeDamage(d);

	// create the damage visualization
	if (getRegion()!= 0 && World::getWorld()->isServer() && dmg>0.5 && !(d->m_special_flags & Damage::NOVISUALIZE))
	{
		FloatingText::Size size = FloatingText::NORMAL;
		if ((d->m_special_flags & Damage::VISUALIZE_SMALL) && !critical)
		{
			size =FloatingText::SMALL;
		}
		getRegion()->visualizeDamage(int(dmg + 0.5),getShape()->m_center,size);
	}

	return true;
}

void Creature::applyDynAttrMod(CreatureDynAttrMod* mod)
{
	m_dyn_attr.m_health += mod->m_dhealth;

	if (m_dyn_attr.m_health>m_base_attr_mod.m_max_health)
	{
		m_dyn_attr.m_health=m_base_attr_mod.m_max_health;
	}

	if (mod->m_dhealth !=0)
	{
		addToNetEventMask( NetEvent::DATA_HP);
	}


	for (int i = 0;i< NR_STATUS_MODS;i++)
	{
		if (mod->m_dstatus_mod_immune_time[i]>0)
		{
			m_dyn_attr.m_status_mod_immune_time[i] = MathHelper::Max(m_dyn_attr.m_status_mod_immune_time[i],mod->m_dstatus_mod_immune_time[i]);
			m_dyn_attr.m_status_mod_time[i]=0;

			if (i==Damage::CONFUSED || i==Damage::BERSERK)
			{
				clearCommand(true,false);
				addToNetEventMask(NetEvent::DATA_COMMAND | NetEvent::DATA_ACTION);

			}
			addToNetEventMask(NetEvent::DATA_STATUS_MODS);
		}
	}
}

void Creature::applyBaseAttrMod(CreatureBaseAttrMod* mod, bool add)
{
	int i;
	// add all the delta values
	float oldmaxhp = m_base_attr_mod.m_max_health;

	// dexterity influences attack speed only up to a value of 80 for balancing reasons
	if (m_base_attr_mod.m_dexterity + mod->m_ddexterity < 80)
	{
		m_base_attr_mod.m_attack_speed += mod->m_ddexterity*10;
	}
	else if (m_base_attr_mod.m_dexterity < 80)
	{
		m_base_attr_mod.m_attack_speed += (80-m_base_attr_mod.m_dexterity)*10;
	}

	m_base_attr_mod.m_armor +=mod->m_darmor;
	m_base_attr_mod.m_block +=mod->m_dblock;
	m_base_attr_mod.m_strength +=mod->m_dstrength;
	m_base_attr_mod.m_dexterity +=mod->m_ddexterity;
	m_base_attr_mod.m_willpower +=mod->m_dwillpower;
	m_base_attr_mod.m_magic_power +=mod->m_dmagic_power;
	m_base_attr_mod.m_walk_speed +=mod->m_dwalk_speed;
	m_base_attr_mod.m_max_health += mod->m_dmax_health;
	m_base_attr_mod.m_max_health += mod->m_dstrength*5;
	m_dyn_attr.m_health *= m_base_attr_mod.m_max_health /oldmaxhp;

	m_base_attr_mod.m_attack +=mod->m_dattack;
	m_base_attr_mod.m_power +=mod->m_dpower;

	m_base_attr_mod.m_attack_speed +=mod->m_dattack_speed;

	// check which data to send via network
	if (mod->m_dwalk_speed!=0 )
	{
		addToNetEventMask(NetEvent::DATA_ATTACK_WALK_SPEED);
	}
	if (mod->m_dattack_speed !=0 || mod->m_ddexterity!=0)
	{
		addToNetEventMask(NetEvent::DATA_ATTACK_WALK_SPEED);
	}
	if (mod->m_dmax_health !=0 || mod->m_dstrength!=0)
	{
		addToNetEventMask(NetEvent::DATA_ATTRIBUTES_LEVEL | NetEvent::DATA_HP);
	}

	// check a few lower bounds
	m_base_attr_mod.m_strength = MathHelper::Max(m_base_attr_mod.m_strength,(short) 1);
	m_base_attr_mod.m_dexterity = MathHelper::Max(m_base_attr_mod.m_dexterity,(short) 1);
	m_base_attr_mod.m_willpower = MathHelper::Max(m_base_attr_mod.m_willpower,(short) 1);
	m_base_attr_mod.m_magic_power = MathHelper::Max(m_base_attr_mod.m_magic_power,(short) 1);
	m_base_attr_mod.m_walk_speed = MathHelper::Max(m_base_attr_mod.m_walk_speed,(short) 200);
	m_base_attr_mod.m_attack_speed = MathHelper::Max(m_base_attr_mod.m_attack_speed,(short) 200);

	// add resistances
	for (i=0;i<4;i++)
	{
		m_base_attr_mod.m_resistances[i] += mod->m_dresistances[i];
		m_base_attr_mod.m_resistances_cap[i] += mod->m_dresistances_cap[i];

	}

	// skills are managed with bitmasks, OR them
	std::set<std::string>::iterator it;
	for (it = mod->m_xabilities.begin(); it != mod->m_xabilities.end(); ++it )
	{
		m_base_attr_mod.m_abilities[*it].m_time =0;
		addToNetEventMask(NetEvent::DATA_ABILITIES);
	}

	// set flags
	if (mod->m_flag != "")
	{
		setFlag(mod->m_flag, true);
		addToNetEventMask(NetEvent::DATA_FLAGS);
	}

	// immunities are stored in bitmasks, OR them
	m_base_attr_mod.m_special_flags |= mod->m_xspecial_flags;
	m_base_attr_mod.m_immunity |= mod->m_ximmunity;

	if (mod->m_xspecial_flags!=0)
	{
		addToNetEventMask(NetEvent::DATA_FLAGS);
	}

	// add it to the list
	if (mod->m_time!=0 && add)
	{
		m_dyn_attr.m_temp_mods.push_back(*mod);

		// update damage
		recalcDamage();
	}

}

bool Creature::removeBaseAttrMod(CreatureBaseAttrMod* mod)
{
	int i;
	bool ret = false;
	float oldmaxhp = m_base_attr_mod.m_max_health;

	// substract all the deltas
	m_base_attr_mod.m_armor -=mod->m_darmor;
	m_base_attr_mod.m_block -=mod->m_dblock;
	m_base_attr_mod.m_strength -=mod->m_dstrength;
	m_base_attr_mod.m_dexterity -=mod->m_ddexterity;
	m_base_attr_mod.m_willpower -=mod->m_dwillpower;
	m_base_attr_mod.m_magic_power -=mod->m_dmagic_power;
	m_base_attr_mod.m_walk_speed -=mod->m_dwalk_speed;
	m_base_attr_mod.m_attack_speed -=mod->m_dattack_speed;
	m_base_attr_mod.m_max_health -= mod->m_dstrength*5;

	// dexterity influences attack speed only up to a value of 80 for balancing reasons
	if (m_base_attr_mod.m_dexterity + mod->m_ddexterity < 80)
	{
		m_base_attr_mod.m_attack_speed -= mod->m_ddexterity*10;
	}
	else if (m_base_attr_mod.m_dexterity < 80)
	{
		m_base_attr_mod.m_attack_speed -= (80-m_base_attr_mod.m_dexterity)*10;
	}


	m_dyn_attr.m_health *= m_base_attr_mod.m_max_health /oldmaxhp;

	m_base_attr_mod.m_attack -=mod->m_dattack;
	m_base_attr_mod.m_power -=mod->m_dpower;

	// check which data to send via network
	if (mod->m_dwalk_speed!=0)
	{
		addToNetEventMask(NetEvent::DATA_ATTACK_WALK_SPEED);
	}
	if (mod->m_dattack_speed !=0 || mod->m_ddexterity!=0)
	{
		addToNetEventMask(NetEvent::DATA_ATTACK_WALK_SPEED);
	}
	if (mod->m_dmax_health !=0 || mod->m_dstrength!=0)
	{
		addToNetEventMask(NetEvent::DATA_ATTRIBUTES_LEVEL | NetEvent::DATA_HP);
	}

	for (i=0;i<4;i++)
	{
		m_base_attr_mod.m_resistances[i] -= mod->m_dresistances[i];
		m_base_attr_mod.m_resistances_cap[i] -= mod->m_dresistances_cap[i];

	}

	// remove flag
	// recalculate the modified attribute, because the flag might be still activated by another mod
	if (mod->m_flag != "")
	{
		setFlag(mod->m_flag, false);
		ret = true;
	}

	// recalculate if abilities were altered
	// because you can not know if the ability was available before applying the mod
	if ( mod->m_xabilities.size() !=0)
	{
		ret = true;
		addToNetEventMask(NetEvent::DATA_ABILITIES);
	}

	// remove flag
	// recalculate the modified attribute, because the flag might be still activated by another mod
	if (mod->m_xspecial_flags!=0)
	{
		addToNetEventMask(NetEvent::DATA_FLAGS);
		ret = true;
	}

	if( mod->m_ximmunity!=0)
		ret = true;

	recalcDamage();

	return ret;
}

void Creature::removeAllBaseAttrMod()
{
	m_dyn_attr.m_temp_mods.clear();
	addToNetEventMask(NetEvent::DATA_FLAGS);
	addToNetEventMask(NetEvent::DATA_ABILITIES);
	addToNetEventMask(NetEvent::DATA_ATTRIBUTES_LEVEL | NetEvent::DATA_HP);
	addToNetEventMask(NetEvent::DATA_ATTACK_WALK_SPEED);

	calcBaseAttrMod();
}

void Creature::getPathDirection(Vector pos,short region, float base_size, short layer, Vector& dir)
{
	// maximal distance where the gradient field is used
	int pathmaxdist = 16;

	PathfindInfo** pi= &m_small_path_info;
	int bsize =1;
	// true if the other object is flying
	bool fly=false;

	// select the gradient field based on information about the other object
	if (base_size>1)
	{
		DEBUGX("switching to medium size info");
		pi = &m_medium_path_info;
		bsize=2;
	}
	if (base_size>2)
	{
		DEBUGX("switching to big size info");
		pi = &m_big_path_info;
		bsize=4;
	}

	if ((layer & LAYER_BASE) ==0)
	{
		DEBUGX("switching to flying info");
		pi = &m_small_flying_path_info;
		bsize=1;
		fly = true;
	}

	// true, if direct position->target vector should be used
	bool direct = false;

	// true, if gradient field has to be recalculated
	bool recalc = false;

	// position of the target
	// (itself, as the goal is to get to this object)
	Vector goal = getShape()->m_center;


	if (*pi == 0)
	{
		// data structure does not exist yet
		*pi = new PathfindInfo;
		int qual=4;
		int dim = 2*pathmaxdist * qual / bsize +1;

		// matrix containing *height*/potential of fields
		(*pi)->m_pot = new Matrix2d<float>(dim,dim);
		// matrix containing blocked fields
		(*pi)->m_block = new Matrix2d<char>(dim,dim);

		(*pi)->m_dim = dim;

		// layer to search for obstacles
		(*pi)->m_layer= LAYER_BASE | LAYER_AIR;
		if (fly)
		{
			(*pi)->m_layer= LAYER_AIR;
		}
		
		(*pi)->m_region=getRegionId();
		(*pi)->m_base_size = bsize;
		(*pi)->m_quality=qual;
		(*pi)->m_id = getId();
		recalc = true;
		DEBUGX("recalc: no pathinfo so far");
	}
	else
	{
		if ( (*pi)->m_region != getRegionId())
		{
			// gradient field belongs to another region, recalculate
			(*pi)->m_region=getRegionId();
			recalc = true;
			DEBUGX("recalc: new in Region");
		}

		// target is in another region, do not return any direction
		if ( region != getRegionId())
		{
			dir = Vector(0,0);
			return;
		}

		// distance of current position <-> own position when last gradient field was computed
		float d = (*pi)->m_start.distanceTo(goal);

		// distance of path searching object to itself
		float d2 = (*pi)->m_start.distanceTo(pos);

		if (fabs(goal.m_x - pos.m_x)>pathmaxdist || fabs(goal.m_y - pos.m_y)>pathmaxdist)
		{
			// direct connection, if other object is too far away
			direct = true;
		}
		else if (d>(*pi)->m_base_size && d > 0.25*d2)
		{
			// target has moved considerably, recalculate
			DEBUGX("recalc: goal has moved");
			recalc = true;
		}

		// data is too old, recalculate
		if ((*pi)->m_timer >500)
		{
			DEBUGX("recalc: info too old");
			recalc = true;
		}

	}

	if (recalc && !direct)
	{
		// recalculate the gradient field
		DEBUGX("recalc");
		// center and sink of the field is the own position
		(*pi)->m_start= goal;
		(*pi)->m_center= goal;
		(*pi)->m_timer =0;

		// get matrix of blocked fields
		bool suc;
		suc = World::getWorld()->calcBlockmat(*pi);
		if (suc!=true)
			direct = true;
		else
		{
			// calculate the field
			suc = World::getWorld()->calcPotential(*pi);
			if (suc!=true)
				direct = true;
		}
	}

	if (direct)
	{
		DEBUGX("direct way");
		// use the direct vector to the target
		dir = goal - pos;
		dir.normalize();

	}
	else
	{
		DEBUGX("use pot field");
		// use the gradient field to compute the direction
		World::getWorld()->calcPathDirection(*pi, pos, dir);
	}
}

void Creature::toString(CharConv* cv)
{
	DEBUGX("Creature::tostring");
	WorldObject::toString(cv);
	cv->toBuffer(m_refname);

	m_action.toString(cv);
	m_command.toString(cv);
	m_next_command.toString(cv);

	cv->toBuffer(m_dyn_attr.m_health);
	cv->toBuffer(m_base_attr_mod.m_max_health);
	DEBUGX("write offset: %i",cv->getBitStream()->GetNumberOfBitsUsed());



	// status mods
	char c=0;
	for (int i=0;i<NR_STATUS_MODS;i++)
	{
		if (m_dyn_attr.m_status_mod_time[i]>0)
		{
			c |= (1 <<i );
		}
	}
	cv->toBuffer(c);
	for (int i=0;i<NR_STATUS_MODS;i++)
	{
		if (m_dyn_attr.m_status_mod_time[i]>0)
		{
			cv->toBuffer(m_dyn_attr.m_status_mod_time[i]);
		}
	}

	// effects
	for (int i=0;i<NR_EFFECTS;i++)
	{
		cv->toBuffer(m_dyn_attr.m_effect_time[i]);
	}

	cv->toBuffer(getBaseAttrMod()->m_special_flags);

	cv->toBuffer(getBaseAttr()->m_step_length);
	cv->toBuffer(getBaseAttrMod()->m_attack_speed);
	cv->toBuffer(getBaseAttrMod()->m_walk_speed);
	int nr_timers = NR_TIMERS;
	if (cv->getVersion() < 17)
	{
		nr_timers = 2;
	}
	for (int i=0; i< nr_timers; i++)
	{
		cv->toBuffer(m_timers[i]);
		cv->toBuffer(m_timers_max[i]);
	}

	cv->toBuffer(static_cast<short>(getBaseAttrMod()->m_abilities.size()));
	std::map<std::string, AbilityInfo>::iterator it;
	for (it= getBaseAttrMod()->m_abilities.begin(); it != getBaseAttrMod()->m_abilities.end(); ++it)
	{
		cv->toBuffer(it->first);
		cv->toBuffer(it->second.m_timer_nr);
		cv->toBuffer(it->second.m_time);
		cv->toBuffer(it->second.m_timer);
	}

	cv->toBuffer(getSpeed().m_x);
	cv->toBuffer(getSpeed().m_y);

	cv->toBuffer(m_emotion_set);
}

void Creature::fromString(CharConv* cv)
{
	WorldObject::fromString(cv);
	cv->fromBuffer(m_refname);

	m_action.fromString(cv);
	m_command.fromString(cv);
	m_next_command.fromString(cv);

	cv->fromBuffer(m_dyn_attr.m_health);
	cv->fromBuffer(m_base_attr_mod.m_max_health);
	m_base_attr.m_max_health = m_base_attr_mod.m_max_health;
	DEBUGX("read offset: %i",cv->getBitStream()->GetReadOffset());
	// status mods
	char c=0;
	cv->fromBuffer(c);

	for (int i=0;i<NR_STATUS_MODS;i++)
	{
		if (c & (1 <<i ))
		{
			cv->fromBuffer(m_dyn_attr.m_status_mod_time[i]);
		}
	}


	for (int i=0;i<NR_EFFECTS;i++)
	{
		cv->fromBuffer(m_dyn_attr.m_effect_time[i]);
	}

	cv->fromBuffer(getBaseAttrMod()->m_special_flags);

	cv->fromBuffer(getBaseAttr()->m_step_length);
	cv->fromBuffer(getBaseAttrMod()->m_attack_speed);
	cv->fromBuffer(getBaseAttrMod()->m_walk_speed);
	int nr_timers = NR_TIMERS;
	if (cv->getVersion() < 17)
	{
		nr_timers = 2;
	}
	for (int i=0; i<nr_timers; i++)
	{
		cv->fromBuffer(m_timers[i]);
		cv->fromBuffer(m_timers_max[i]);
	}

	short nr;
	cv->fromBuffer(nr);
	Action::ActionType type;
	getBaseAttrMod()->m_abilities.clear();
	int timer_nr;
	float time, timer;
	for (int i=0; i<nr; i++)
	{
		cv->fromBuffer(type);
		cv->fromBuffer(timer_nr);
		cv->fromBuffer(time);
		cv->fromBuffer(timer);
		getBaseAttrMod()->m_abilities[type].m_timer_nr = timer_nr;
		getBaseAttrMod()->m_abilities[type].m_timer =timer;
		getBaseAttrMod()->m_abilities[type].m_time = time;
	}

	Vector speed;
	cv->fromBuffer(speed.m_x);
	cv->fromBuffer(speed.m_y);
	setSpeed(speed);

	cv->fromBuffer(m_emotion_set);
}

bool Creature::checkAbility(Action::ActionType act)
{
	if (act == "noaction")
		return true;

	return ( m_base_attr_mod.m_abilities.count(act) > 0 );
}


float Creature::getTimerPercent(int timer)
{
	if (timer != 0)
	{
		if (m_timers_max[timer-1] ==0)
			return 0;

		return m_timers[timer-1] / m_timers_max[timer-1];
	}

	return 0;
}


void Creature::writeNetEvent(NetEvent* event, CharConv* cv)
{
	WorldObject::writeNetEvent(event,cv);

	if (event->m_data & NetEvent::DATA_NAME)
	{
		cv->toBuffer(m_refname);
	}

	if (event->m_data & NetEvent::DATA_COMMAND)
	{
		m_command.toString(cv);
	}

	if (event->m_data & NetEvent::DATA_ACTION)
	{
		m_action.toString(cv);
		cv->toBuffer(getShape()->m_center.m_x);
		cv->toBuffer(getShape()->m_center.m_y);
		cv->toBuffer(getShape()->m_angle);

		DEBUGX("sending action %s angle %f",m_action.m_type.c_str(), getShape()->m_angle);
		if (m_action.m_type!="noaction")
		{
			float acttime = m_action.m_time - m_action.m_elapsed_time;
			Vector goal = getShape()->m_center + getSpeed() * acttime;
		}
	}

	if (event->m_data & NetEvent::DATA_HP)
	{
		cv->toBuffer(getDynAttr()->m_health);
	}

	if (event->m_data & NetEvent::DATA_ATTRIBUTES_LEVEL)
	{
		cv->toBuffer(getBaseAttrMod()->m_max_health);
	}

	if (event->m_data & NetEvent::DATA_EFFECTS)
	{
		for (int i=0;i<NR_EFFECTS;i++)
		{
			cv->toBuffer(m_dyn_attr.m_effect_time[i]);
		}
	}

	if (event->m_data & NetEvent::DATA_STATUS_MODS)
	{
		char c=0;
		for (int i=0;i<NR_STATUS_MODS;i++)
		{
			if (m_dyn_attr.m_status_mod_time[i]>0)
			{
				c |= (1 <<i );
			}
		}
		cv->toBuffer(c);

		for (int i=0;i<NR_STATUS_MODS;i++)
		{
			if (m_dyn_attr.m_status_mod_time[i]>0)
			{
				cv->toBuffer(m_dyn_attr.m_status_mod_time[i]);
			}
		}
	}

	if (event->m_data & NetEvent::DATA_TIMER)
	{
		int nr_timers = NR_TIMERS;
		if (cv->getVersion() < 17)
			nr_timers = 2;
		for (int i=0; i<nr_timers; i++)
		{
			cv->toBuffer(m_timers[i]);
		}
		for (int i=0; i<nr_timers; i++)
		{
			cv->toBuffer(m_timers_max[i]);
		}
	}

	if (event->m_data & NetEvent::DATA_STATE)
	{
		cv->toBuffer((char) getState());
		DEBUGX("sending state %i %i",getId(),getState());
	}

	if (event->m_data & NetEvent::DATA_ATTACK_WALK_SPEED)
	{
		cv->toBuffer(getBaseAttrMod()->m_walk_speed);
		cv->toBuffer(getBaseAttrMod()->m_attack_speed);
	}


	if (event->m_data & NetEvent::DATA_NEXT_COMMAND)
	{
		m_next_command.toString(cv);
	}

	if (event->m_data & NetEvent::DATA_ABILITIES)
	{
		cv->toBuffer(static_cast<short>(getBaseAttrMod()->m_abilities.size()));
		std::map<std::string, AbilityInfo>::iterator it;
		for (it= getBaseAttrMod()->m_abilities.begin(); it != getBaseAttrMod()->m_abilities.end(); ++it)
		{
			cv->toBuffer(it->first);
			cv->toBuffer(it->second.m_timer_nr);
			cv->toBuffer(it->second.m_time);
			cv->toBuffer(it->second.m_timer);
			if (World::getWorld()->getVersion() >= 18)
			{
				cv->toBuffer(it->second.m_rating);
			}
		}

	}

	if (event->m_data & NetEvent::DATA_FLAGS )
	{
		cv->toBuffer(getBaseAttrMod()->m_special_flags);
	}

	if (event->m_data & NetEvent::DATA_EXPERIENCE)
	{
		cv->toBuffer(getDynAttr()->m_experience);
	}

	if (event->m_data & NetEvent::DATA_SPEED)
	{
		cv->toBuffer(getSpeed().m_x);
		cv->toBuffer(getSpeed().m_y);
	}

	if (event->m_data & NetEvent::DATA_ATTRIBUTES_LEVEL)
	{
		cv->toBuffer(getBaseAttr()->m_level);
		cv->toBuffer(getBaseAttr()->m_max_experience);
		cv->toBuffer(getBaseAttr()->m_max_health);
		cv->toBuffer(getBaseAttr()->m_strength);
		cv->toBuffer(getBaseAttr()->m_dexterity);
		cv->toBuffer(getBaseAttr()->m_willpower);
		cv->toBuffer(getBaseAttr()->m_magic_power);
	}

	if (event->m_data & NetEvent::DATA_SPEAK_TEXT)
	{
		getSpeakText().m_text.toString(cv);
		cv->toBuffer(getSpeakText().m_emotion);
		cv->toBuffer(getSpeakText().m_time);
		cv->toBuffer(getSpeakText().m_in_dialogue);
		cv->toBuffer(getSpeakText().m_displayed_time);
		cv->toBuffer(m_emotion_set);
	}

	if (event->m_data & NetEvent::DATA_TRADE_INFO)
	{
		cv->toBuffer(m_trade_info.m_trade_partner);
		cv->toBuffer(m_trade_info.m_price_factor);
	}

	if (event->m_data & NetEvent::DATA_DIALOGUE)
	{
		cv->toBuffer(m_dialogue_id);
	}
}


void Creature::processNetEvent(NetEvent* event, CharConv* cv)
{
	WorldObject::processNetEvent(event,cv);

	if (event->m_data & NetEvent::DATA_NAME)
	{
		std::string name;
		cv->fromBuffer(name);
		m_refname = name;
	}

	DEBUGX("object %i processing NetEvent %i data %i",getId(),event->m_type, event->m_data);
	if (event->m_data & NetEvent::DATA_COMMAND)
	{
		m_command.fromString(cv);
		DEBUGX("got Command %s",m_command.m_type.c_str());
	}

	// most data is just set instantly
	// speed and position are interpolated
	float  atime = m_action.m_time - m_action.m_elapsed_time;

	Vector newpos= getShape()->m_center;
	Vector newspeed;
	Vector goal = newpos;

	bool newact= false;
	bool newmove = false;
	float delay = cv->getDelay();
	float newangle=getShape()->m_angle;

	if (delay>1000)
	{
		DEBUG("got packet with delay %f %f",cv->getDelay(),delay);
	}


	if (event->m_data & NetEvent::DATA_ACTION)
	{


		m_action.fromString(cv);

		cv->fromBuffer(newpos.m_x);
		cv->fromBuffer(newpos.m_y);
		cv->fromBuffer(newangle);
		newact = true;
	}



	if (event->m_data & NetEvent::DATA_HP)
	{

		cv->fromBuffer(getDynAttr()->m_health);
	}

	if (event->m_data & NetEvent::DATA_ATTRIBUTES_LEVEL)
	{
		cv->fromBuffer(getBaseAttrMod()->m_max_health);
	}

	if (event->m_data & NetEvent::DATA_EFFECTS)
	{
		for (int i=0;i<NR_EFFECTS;i++)
		{
			cv->fromBuffer(m_dyn_attr.m_effect_time[i]);
		}
	}

	if (event->m_data & NetEvent::DATA_STATUS_MODS)
	{
		char ctmp;
		cv->fromBuffer(ctmp);
		for (int i=0;i<NR_STATUS_MODS;i++)
		{
			if (ctmp & (1 <<i ))
			{
				cv->fromBuffer(m_dyn_attr.m_status_mod_time[i]);
			}
		}
	}

	if (event->m_data & NetEvent::DATA_TIMER)
	{
		int nr_timers = NR_TIMERS;
		if (cv->getVersion() < 17)
			nr_timers = 2;
		for (int i=0; i<nr_timers; i++)
		{
			cv->fromBuffer(m_timers[i]);
		}
		for (int i=0; i<nr_timers; i++)
		{
			cv->fromBuffer(m_timers_max[i]);
		}
	}

	if (event->m_data & NetEvent::DATA_STATE)
	{
		State oldstate = getState();
		char ctmp;
		cv->fromBuffer(ctmp);

		// few internal states are not copied
		if (ctmp <=STATE_DEAD || ctmp >=STATE_STATIC)
		{
			if (oldstate != STATE_DIEING && oldstate != STATE_DEAD && (ctmp==STATE_DIEING || ctmp == STATE_DEAD))
			{
				die();
			}
			setState((State) ctmp);
			DEBUGX("object %i changed state from %i to %i",getId(),oldstate,ctmp);
		}

	}

	if (event->m_data & NetEvent::DATA_ATTACK_WALK_SPEED)
	{
		cv->fromBuffer(getBaseAttrMod()->m_walk_speed);
		cv->fromBuffer(getBaseAttrMod()->m_attack_speed);
	}

	if (event->m_data & NetEvent::DATA_NEXT_COMMAND)
	{
		m_next_command.fromString(cv);
	}

	if (event->m_data & NetEvent::DATA_ABILITIES)
	{
		short nr;
		cv->fromBuffer(nr);
		int timer_nr;
		float time, timer;
		float rating;
		Action::ActionType type;
		for (int i=0; i<nr; i++)
		{
			cv->fromBuffer(type);
			cv->fromBuffer(timer_nr);
			cv->fromBuffer(time);
			cv->fromBuffer(timer);
			if (World::getWorld()->getVersion() >= 18)
			{
				cv->fromBuffer(rating);
				getBaseAttr()->m_abilities[type].m_rating = rating;
			}
			getBaseAttr()->m_abilities[type].m_timer_nr = timer_nr;
			getBaseAttr()->m_abilities[type].m_timer = timer;
			getBaseAttr()->m_abilities[type].m_time = time;
			calcBaseAttrMod();
		}

	}

	if (event->m_data & NetEvent::DATA_FLAGS )
	{
		cv->fromBuffer(getBaseAttrMod()->m_special_flags);
	}

	if (event->m_data & NetEvent::DATA_EXPERIENCE)
	{
		cv->fromBuffer(getDynAttr()->m_experience);
	}

	if (event->m_data & NetEvent::DATA_SPEED)
	{
		cv->fromBuffer(newspeed.m_x);
		cv->fromBuffer(newspeed.m_y);
		if (newspeed.getLength() > 0.00001f)
		{
			newmove = true;
		}

	}

	if (event->m_data & NetEvent::DATA_ATTRIBUTES_LEVEL)
	{
		cv->fromBuffer(getBaseAttr()->m_level);
		cv->fromBuffer(getBaseAttr()->m_max_experience);
		cv->fromBuffer(getBaseAttr()->m_max_health);
		cv->fromBuffer(getBaseAttr()->m_strength);
		cv->fromBuffer(getBaseAttr()->m_dexterity);
		cv->fromBuffer(getBaseAttr()->m_willpower);
		cv->fromBuffer(getBaseAttr()->m_magic_power);
	}

	if (event->m_data & NetEvent::DATA_SPEAK_TEXT)
	{
		getSpeakText().clear();
		getSpeakText().m_text.fromString(cv);
		cv->fromBuffer(getSpeakText().m_emotion);
		cv->fromBuffer(getSpeakText().m_time);
		cv->fromBuffer(getSpeakText().m_in_dialogue);
		cv->fromBuffer(getSpeakText().m_displayed_time);
		cv->fromBuffer(m_emotion_set);
	}

	if (event->m_data & NetEvent::DATA_TRADE_INFO)
	{
		cv->fromBuffer(m_trade_info.m_trade_partner);
		cv->fromBuffer(m_trade_info.m_price_factor);
	}

	if (event->m_data & NetEvent::DATA_DIALOGUE)
	{
		int id;
		cv->fromBuffer(id);
		setDialogue(id);
	}

	// interpolation code for movement
	if (newmove)
	{
		// compute final position of the movement
		float acttime = m_action.m_time - m_action.m_elapsed_time;
		goal = newpos + newspeed * acttime;

		/*
		timeval tv;
		gettimeofday(&tv, NULL);
		DEBUG("time elapsed ingame %f delay %i system time %i",etime,cv->getDelay(),tv.tv_usec/1000);
		*/
		// time remaining to reach this point
		float goaltime = acttime;
		if (event->m_data & NetEvent::DATA_ACTION)
		{
			goaltime -= delay;
		}
		if (goaltime < 100)
		{

			DEBUGX("time to reach goal %f (prev time %f) delay %f",goaltime,atime,cv->getDelay());

		}
		DEBUGX("previously planned time %f",atime);

		if (goaltime <0)
		{
			// you should be already there, jump to the target position
			moveTo(goal);
		}
		else
		{
			DEBUGX("pos %f %f speed %f %f", getShape()->m_center.m_x, getShape()->m_center.m_y, getSpeed().m_x, getSpeed().m_y);
			// adjust speed, so that the target position is reached in time
			Vector calcspeed = (goal - getShape()->m_center) * (1/goaltime);
			setSpeed(calcspeed);

			// if calculated speed gets to high
			// reduce speed and do a jump instead
			if (calcspeed.getLength() > 4* newspeed.getLength())
			{
				moveTo(goal - newspeed * goaltime);
				setSpeed(newspeed);
			}
			DEBUGX("goal %f %f newspeed %f %f", goal.m_x, goal.m_y, getSpeed().m_x, getSpeed().m_y);
		}

	}

	// action interpolation code
	if (newact)
	{
		m_action.m_elapsed_time += delay;

		if (m_action.m_elapsed_time> m_action.m_time)
		{
			// action should be completed already
			if (!newmove)
			{
				moveTo(newpos);
				getShape()->m_angle = newangle;
			}

			m_action.m_type = "noaction";
			m_action.m_elapsed_time =0;

		}
		else
		{
			// adjust the angle
			if (!newmove && m_action.m_type != "noaction")
			{
				// if the action is not walk, jump to the correct position
				// TODO: perhaps allow some small differences here
				moveTo(newpos);
				getShape()->m_angle = newangle;

			}

			Action::ActionInfo* ainfo = Action::getActionInfo(m_action.m_type);
			if (ainfo !=0 && ainfo->m_base_action == "walk" && newmove)
			{
				setAngle(getSpeed().angle());

			}
		}
	}
}

int Creature::getValue(std::string valname)
{
	if (valname == "emotionset")
	{
		lua_pushstring(EventSystem::getLuaState() , m_emotion_set.c_str() );
		return 1;
	}
	else if (valname.substr(0,5) == "timer")
	{
		std::string number = valname.substr(5);
		std::stringstream stream(number);
		int timer = 0;
		stream >> timer;
		if (timer > 0 && timer <= NR_TIMERS)
		{
			lua_pushnumber(EventSystem::getLuaState(),	m_timers[timer-1]);
			return 1;
		}
		return 0;
	}
	else if (valname == "ignored_by_ai")
	{
		lua_pushboolean(EventSystem::getLuaState(), bool(getBaseAttrMod()->m_special_flags & IGNORED_BY_AI));
		return 1;
	}

	if (valname.find("base:") == 0)
	{
		std::string val2 = valname.substr(5);
		int ret = getBaseAttr()->getValue(val2);
		if (ret >0)
		{
			return ret;
		}
	}

	int ret = getBaseAttrMod()->getValue(valname);
	if (ret >0)
	{
		return ret;
	}

	ret = m_dyn_attr.getValue(valname);
	if (ret >0)
	{
		return ret;
	}

	ret = WorldObject::getValue(valname);
	return ret;

}

bool Creature::setValue(std::string valname)
{
	if (valname == "emotionset")
	{
		m_emotion_set = lua_tostring(EventSystem::getLuaState() ,-1);
		lua_pop(EventSystem::getLuaState(), 1);
		addToNetEventMask(NetEvent::DATA_SPEAK_TEXT);
		return true;
	}
	else if (valname.substr(0,5) == "timer")
	{
		std::string number = valname.substr(5);
		std::stringstream stream(number);
		int timer = 0;
		stream >> timer;
		if (timer > 0 && timer < NR_TIMERS)
		{
			m_timers[timer-1] =  lua_tonumber(EventSystem::getLuaState(),-1);
			lua_pop(EventSystem::getLuaState(), 1);
			addToNetEventMask(NetEvent::DATA_TIMER);
			return true;
		}
		return false;
	}
	else if (valname == "ignored_by_ai")
	{
		bool ignore = lua_toboolean(EventSystem::getLuaState() ,-1);
		lua_pop(EventSystem::getLuaState(), 1);
		if (ignore)
		{
			getBaseAttr()->m_special_flags |= IGNORED_BY_AI;
		}
		else
		{
			getBaseAttr()->m_special_flags &= (~IGNORED_BY_AI);
		}
		calcBaseAttrMod();
		return true;
	}

	bool ret;
	ret = getBaseAttr()->setValue(valname, getEventMaskRef());
	if (ret)
	{
		calcBaseAttrMod();
		return ret;
	}

	ret = m_dyn_attr.setValue(valname, getEventMaskRef());
	if (ret)
	{
		if (m_dyn_attr.m_health >0 && (getState() == STATE_DIEING || getState() == STATE_DEAD))
		{
			if (getType() == "PLAYER")
			{
				getRegion()->changeObjectGroup(this,PLAYER);
			}
			else
			{
				getRegion()->changeObjectGroup(this,CREATURE);
			}
			setState(STATE_ACTIVE);

			m_action.m_type ="noaction";
			m_action.m_elapsed_time =0;
			addToNetEventMask(NetEvent::DATA_ACTION | NetEvent::DATA_STATE);
		}

		// if experience was changed
		// gain levelups until experience is smaller than max experience
		while (m_dyn_attr.m_experience>= m_base_attr.m_max_experience && m_dyn_attr.m_experience>0)
		{
			gainLevel();
		}

		return ret;
	}

	ret = WorldObject::setValue(valname);
	return ret;
}

void Creature::clearSpeakText()
{
	addToNetEventMask(NetEvent::DATA_SPEAK_TEXT);
	m_speak_text.clear();
}

void Creature::speakText(CreatureSpeakText& text)
{
	// text from dialogues has higher priority
	// do not overwrite dialogue text with non-dialogue text
	if (text.m_in_dialogue == false && !m_speak_text.empty() && m_speak_text.m_in_dialogue)
	{
		return;
	}

	addToNetEventMask(NetEvent::DATA_SPEAK_TEXT);

	m_speak_text = text;
	m_speak_text.m_displayed_time = 0;

	DEBUGX("speak %s for %f ms",text.m_text.getTranslation().c_str(), text.m_time);
}

Creature* Creature::getTradePartner()
{
	if (m_trade_info.m_trade_partner == 0)
		return 0;

	if (getRegion() ==0 )
		return 0;

	return dynamic_cast<Creature*>(getRegion()->getObject(m_trade_info.m_trade_partner));
}

void Creature::sellItem(short position, Item* &item, int& gold)
{
	if (getEquipement() !=0)
	{
		// get the item to be sold from the inventory
		Item* it = 0;
		getEquipement()->swapItem(it,position);

		if (it != 0 && gold >= it->m_price)
		{
			item = it;

			// the gold variable references to the buyers gold
			// so substract the price
			gold -= item->m_price;
			item->m_price = (int) ((item->m_price+0.999f) / m_trade_info.m_price_factor );

			// inform other clients via network
			NetEvent event;
			event.m_type =  NetEvent::TRADER_ITEM_SELL;
			event.m_data = position;
			event.m_id = getId();

			getRegion()->insertNetEvent(event);

			// this ensures that the last sold item can be taken back for full price
			m_trade_info.m_last_sold_item = item;
		}
		else
		{
			getEquipement()->swapItem(it,position);
		}
	}
}

void Creature::buyItem(Item* &item, int& gold)
{
	DEBUGX("buy %p",item);
	if (item != 0 && getEquipement() !=0)
	{
		NPCTrade& tradeinfo = Dialogue::getNPCTrade(getRefName());
		float factor = tradeinfo.m_pay_multiplier;

		// gold refers to the sellers gold
		// so add the price
		// special rule: last item bought can be give back for the full price
		if (item == m_trade_info.m_last_sold_item)
		{
			gold +=(int) (item->m_price * m_trade_info.m_price_factor);
		}
		else
		{
			gold += MathHelper::Max(1, int(item->m_price*factor));
		}
		item->m_price = (int) (item->m_price * m_trade_info.m_price_factor);

		// insert the item into the traders inventory
		short pos = getEquipement()->insertItem(item,false,false);
		if (pos == Equipement::NONE)
		{
			delete item;
		}
		else
		{
			// inform other clients via network
			NetEvent event;
			event.m_type =  NetEvent::TRADER_ITEM_BUY;
			event.m_data = pos;
			event.m_id = getId();

			getRegion()->insertNetEvent(event);
		}
		item =0;
	}
}


void Creature::setDialogue(int id)
{
	if (id != m_dialogue_id)
	{
		DEBUGX("Creature %s has now Dialogue %i",getRefName().c_str(),id);
		m_dialogue_id = id;
		addToNetEventMask(NetEvent::DATA_DIALOGUE);

		if (id !=0)
		{
			getAction()->m_type = "noaction";
			m_action.m_elapsed_time =0;
			clearCommand(true,true);
		}
	}
}

void Creature::updateFightStat(float hitchance, float armorperc, TranslatableString attacked)
{
	FightStatistic* fstat = &(getFightStatistic());
	if (fabs(hitchance - fstat->m_hit_chance) > 0.01
		   || fabs(armorperc - fstat->m_damage_dealt_perc) > 0.01
		   || attacked != fstat->m_last_attacked)
	{
		fstat->m_hit_chance = hitchance;
		fstat->m_damage_dealt_perc = armorperc;
		fstat->m_last_attacked = attacked;
		addToNetEventMask(NetEvent::DATA_FIGHT_STAT);

		DEBUGX("hitchance %f damage perc %f attacked %s",hitchance, armorperc, attacked.c_str());
	}
}

std::string Creature::getActionString()
{
	if (getState() == STATE_DEAD)
		return "die";

	if (m_action.m_type == "noaction")
	{
		// Base class has a system for displaying animations without any ingame action
		// check this before playing an idle animation
		std::string act = WorldObject::getActionString();
		if (act != "")
		{
			return act;
		}

		// do a speak animation instead of idle, if speech text is present
		if (!getSpeakText().empty())
		{
			return "talk";
		}
	}

	return m_action.m_type;
}

float Creature::getActionPercent()
{
	// this ensures that the corpse is just lying there...
	if (getState() == STATE_DEAD)
		return 0.999999;

	if (m_action.m_type == "noaction")
	{
		// Base class has a system for displaying animations without any ingame action
		// check this before playing an idle animation
		std::string act = WorldObject::getActionString();
		if (act != "")
		{
			return WorldObject::getActionPercent();
		}

		// do a speak animation instead of idle, if speech text is present
		if (!getSpeakText().empty())
		{
			// periodic animation with period of 1000ms
			return fmod(getSpeakText().m_displayed_time,1000.0f)/1000.0f;
		}
	}

	return m_action.m_elapsed_time / m_action.m_time;
}

void Creature::getFlags(std::set<std::string>& flags)
{
	WorldObject::getFlags(flags);

	// status mods
	float* mods = m_dyn_attr.m_status_mod_time;
	static const std::string modnames[NR_STATUS_MODS]= {"blind", "poisoned", "berserk","confused", "mute", "paralyzed", "frozen", "burning" };
	for (int i=0; i<NR_STATUS_MODS; i++)
	{
		if (mods[i] >0)
		{
			flags.insert(modnames[i]);
		}
	}

	// other effects
	float* effects = m_dyn_attr.m_effect_time;
	static const std::string effectnames[NR_EFFECTS]= {"bleed"};
	for (int i=0; i<NR_EFFECTS; i++)
	{
		if (effects[i] >0)
		{
			flags.insert(effectnames[i]);
		}
	}

	// additional flags
	int flgs = m_base_attr_mod.m_special_flags;
	if (flgs & FIRESWORD)
		flags.insert("firesword");
	if (flgs & FLAMESWORD)
		flags.insert("flamesword");
	if (flgs & FLAMEARMOR)
		flags.insert("flamearmor");
	if (flgs & CRIT_HITS)
		flags.insert("critical_hits");
	if (flgs & ICE_ARROWS)
		flags.insert("ice_arrows");
	if (flgs & FROST_ARROWS)
		flags.insert("frost_arrows");
	if (flgs & WIND_ARROWS)
		flags.insert("wind_arrows");
	if (flgs & STORM_ARROWS)
		flags.insert("storm_arrows");
	if (flgs & WIND_WALK)
		flags.insert("wind_walk");
	if (flgs & BURNING_RAGE)
		flags.insert("burning_rage");
	if (flgs & STATIC_SHIELD)
		flags.insert("static_shield");
}

void Creature::clearFlags()
{
	WorldObject::clearFlags();

	for ( int i=0; i< NR_STATUS_MODS; i++)
	{
		m_dyn_attr.m_status_mod_time[i] =0;
	}

	m_base_attr_mod.m_special_flags=0;
}

std::string Creature::getEmotionImage(std::string emotion)
{
	EmotionSet* es = ObjectFactory::getEmotionSet(m_emotion_set);
	if (es ==0)
		return "";

	return es->getEmotionImage(emotion);
}


