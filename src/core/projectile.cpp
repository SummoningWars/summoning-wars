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

#include "projectile.h"
#include "world.h"
#include "objectfactory.h"
#include "creature.h"


Projectile::Projectile(Subtype subtype,  int id)
:	GameObject(id),
	m_damage(0),
	m_flags(0),
	m_max_radius(0),
	m_goal_object(0),
	m_timer(0),
	m_timer_limit(1500),
	m_crit_percent(0),
	m_counter(0),
	m_fraction(0),
	m_implementation()
{
	setSubtype(subtype);
	setLayer(WorldObject::LAYER_AIR);
	setType("PROJECTILE");
	setBaseType(PROJECTILE);
	
	setState(STATE_FLYING,false);

	float r=0.1;
	// Radius, Timerlaufzeit und Status je nach Typ setzen
	Subtype st = getSubtype();

	getShape()->m_radius =r;
	setAngle(0);
	
	clearNetEventMask();
}

Projectile::Projectile(ProjectileBasicData &data,int id)
:	GameObject(id),
	m_fraction(0)
{
	m_damage =0;
	setSubtype(data.m_subtype);
	m_timer =0;
	setLayer(WorldObject::LAYER_AIR);
	setType("PROJECTILE");
	setBaseType(PROJECTILE);
	
	m_flags =data.m_flags;
	setState(STATE_FLYING,false);
	m_counter =data.m_counter;
	m_goal_object =0;
	m_timer_limit = data.m_lifetime;
	m_implementation = data.m_implementation;
	getShape()->m_radius = data.m_radius;
	m_crit_percent = data.m_crit_percent;
	setAngle(0);
	
	if (m_implementation == "fly")
	{
		setState(STATE_FLYING,false);
	}
	else if (m_implementation == "fly_guided")
	{
		setState(STATE_FLYING,false);
	}
	else if (m_implementation == "grow")
	{
		m_max_radius = data.m_radius;
		getShape()->m_radius=0;
		setState(STATE_GROWING,false);
	}
	else if (m_implementation == "grow_effect_at_each")
	{
		m_max_radius = data.m_radius;
		getShape()->m_radius=0;
		setState(STATE_GROWING,false);
	}
	else if (m_implementation == "stationary")
	{
		setState(STATE_STABLE,false);
	}
	else if (m_implementation == "stationary_multi_effect")
	{
		setState(STATE_STABLE,false);
		m_timer_limit /= m_counter;
	}
	clearNetEventMask();
	DEBUGX("type %s flags %i",getSubtype().c_str(), m_flags);
}


bool Projectile::update(float time)
{
	// Zeit die beim aktuellen Durchlauf der Schleife verbraucht wird
	float dtime;
	// Liste der getroffenen Objekte
	WorldObjectList hitobj;
	WorldObjectList::iterator i;

	while (time>0.1)
	{
		hitobj.clear();
		dtime = time;

		// Timer erhoehen
		if (dtime > m_timer_limit-m_timer)
		{
			// Grenze erreicht
			dtime =m_timer_limit-m_timer;
			m_timer = m_timer_limit;
		}
		else
		{
			m_timer += dtime;
		}
		time -= dtime;
		
		if (dtime <=0)
			return false;

		switch (getState())
		{
			case STATE_FLYING:
				// Status fliegend behandeln
				handleFlying(dtime);

				break;


			case STATE_GROWING:
				// Status wachsend behandeln
				handleGrowing(dtime);

				break;

			case STATE_STABLE:
				// Status stabil behandeln
				handleStable(dtime);

				break;

			case STATE_DESTROYED:
				DEBUGX("destroyed");
				setDestroyed(true);
				time =0;
				break;


			default:
				time =0;
				break;

		}
	}

	DEBUGX("update end");
	return true;

}

void Projectile::handleFlying(float dtime)
{
	WorldObjectList hitobj;
	WorldObjectList::iterator i;
	WorldObject* hit;
	Vector dir,newdir(0,0);
	Vector sdir,hitpos,ndir;
	Vector pos = getPosition(), newpos;
	float rnew,rmin;
	
	bool do_destroy = false;
	
	Vector speed = getSpeed();

	WorldObject* creator=0;
	if (m_damage!= 0 && m_damage->m_attacker_id != 0)
	{
		creator = getRegion()->getObject(m_damage->m_attacker_id);
	}
	
	if (m_implementation == "fly_guided")
	{
		// Lenkpfeil

		// Zielobjekt
		hit =0;

		if (m_goal_object!=0)
		{
			// Zielobjekt ist per ID vorgegeben, Objekt von der Welt holen
			hit = getRegion()->getObject(m_goal_object);
		}

		if (hit==0)
		{
			// kein Zielobjekt vorhanden, automatisch eines suchen

			// Kreis um aktuelle Position mit Umkreis 5
			Shape s;
			s.m_center = pos;
			s.m_type = Shape::CIRCLE;
			s.m_radius = 5;


			// alle Objekte im Kreis suchen
			getRegion()->getObjectsInShape(&s,&hitobj,getLayer(), WorldObject::CREATURE,creator);

			// alle Objekte als potentielle Ziele loeschen, die dem Erschaffer des Projektils nicht feindlich gesinnt sind
			for (i=hitobj.begin();i!=hitobj.end();)
			{
				if (World::getWorld()->getRelation(getFraction(),*i) != Fraction::HOSTILE)
				{
					i=hitobj.erase(i);
				}
				else
					++i;
			}

			// Suchen jenes Zieles welches am besten zu erreichen ist
			rmin = 3*sqr(s.m_radius);
			if (!hitobj.empty())
			{
				// normieren der aktuellen Geschwindigkeit
				sdir = speed;
				sdir.normalize();

				// Durchmustern der potentiellen Ziele
				for (i=hitobj.begin();i!=hitobj.end();++i)
				{

					hit= (*i);

					// Koordinaten des Potentiellen Zieles
					hitpos = hit->getShape()->m_center;

					// Richtung von aktueller Position zum Ziel, normiert
					dir = hitpos - pos;
					ndir = dir;
					ndir.normalize();

					// effektiver Abstand: normaler Abstand minus 4* skalarprodukt(aktuelle Flugrichtung, Zielrichtung)
					// Ziele fuer die eine scharfe Wendung noetig ist, werden schlechter bewertet
					rnew = -4*(ndir*sdir);
					rnew += sqr(dir.m_x) + sqr(dir.m_y);

					//DEBUG("rnew %f",rnew);
					// Wenn aktuelles Objekt besser ist als bisher bestes
					if (rnew<rmin)
					{
						// neue Richtung setzen
						rmin = rnew;
						newdir = dir;

					}

				}
			}
		}
		else
		{
			// es gibt ein vorgegebenes Zielobjekt
			// Richtung ist Richtung zum Zielobjekt
			newdir = hit->getShape()->m_center - pos;

		}
		
		// Richtung nur aendern, wenn ein Ziel gefunden wurde
		if (hit != 0)
		{
			DEBUGX("chasing %i %s",hit->getId(), hit->getSubtype().c_str());
			// aktuelle absolute Geschwindigkeit
			float v = speed.getLength();
			
			// normieren der Zielrichtung
			newdir.normalize();
			newdir *= v;

			float p =0.3*dtime / 50;
			if (p>1)
				p=1;
			DEBUGX("p = %f",p);
			DEBUGX("newdir %f %f",newdir.m_x,newdir.m_y);
			
			// Neue Richtung ergibt sich aus Linearkombination von aktueller Richtung und Zielrichtung
			speed = speed*(1-p) + newdir*p;
			DEBUGX("new speed %f %f",speed.m_x,speed.m_y);

			// neue Richtung normieren
			speed.normalize();
			speed *= v;

			if (World::getWorld()->timerLimit(0))
			{
				addToNetEventMask(NetEvent::DATA_SPEED);
			}
			
			hitobj.clear();

			setSpeed(speed,false);
		}
		
	}

	if (m_timer >= m_timer_limit)
	{
		DEBUGX("destroyed after timeout");
		do_destroy = true;
	}

	DEBUGX("pos %f %f",pos.m_x, pos.m_y);
	
	// neue Koordinaten nach Ablauf des Zeitquantums
	newpos = pos + getSpeed()*dtime;

	// Linie zwischen alter und neuer Position
	Line line(pos,newpos);


	// Objekt an der aktuellen Position suchen
	getRegion()->getObjectsOnLine(line,&hitobj,getLayer(),WorldObject::CREATURE | WorldObject::FIXED,creator);
	if (hitobj.empty())
	{
		getRegion()->getObjectsInShape(getShape(),&hitobj,getLayer(),WorldObject::CREATURE,creator);
	}
	
	// Alle Objekte herausfiltern die verbuendet sind, sowie das zuletzt gerade getroffene Objekt
	if (!hitobj.empty())
	{
		i = hitobj.begin();
		hit = (*i);

		while (!hitobj.empty() && (World::getWorld()->getRelation(getFraction(),hit->getFraction()) == Fraction::ALLIED || m_hit_objects_ids.count(hit->getId()) >0))
		{
			i=hitobj.erase(i);
			if (i!=hitobj.end())
				hit=(*i);
		}
	}

	if (!hitobj.empty())
	{
		DEBUGX("hit object");
		i = hitobj.begin();
		hit = (*i);
		
		if (World::getWorld()->isServer())
		{
			doEffect(hit);
		}
		
		//DEBUG("hit object %p",hit);

		//DEBUG("hit object %i at %f %f",hit->getId(),hit->getGeometry()->m_shape.m_coordinate_x,hit->getGeometry()->m_shape.m_coordinate_y);
		m_hit_objects_ids.insert(hit->getId());
		
		if (m_flags & PIERCING)
		{
			// Projektil fliegt weiter
			
		}
		else
		{
			// Projektil wird zerstoert
			DEBUGX("hit obj %i",hit->getId());
			do_destroy = true;
			m_timer=0;
		}

		// true, wenn das Projektil zu einem weiteren Ziel weiterspringt
		bool bounce = false;
		if (m_flags & BOUNCING)
			bounce = true;

		if (m_flags & PROB_BOUNCING && World::getWorld()->isServer())
		{
			// zufaelliges weiterspringen, Chance 50%
			if (rand()<RAND_MAX*0.5)
			{
				SW_DEBUG("prob bounce");
				bounce = true;
			}
		}

		if (hit->isCreature() && bounce)
		{
			DEBUGX("bouncing");
			Vector speed = getSpeed();
			// Projektil hat ein Lebewesen getroffen, springt weiter
			setState(STATE_FLYING);
			do_destroy = false;
			
			float v = speed.getLength();

			// Kreis mit Radius 5 um aktuelle Position
			Shape s;
			s.m_center = pos;
			s.m_type = Shape::CIRCLE;
			s.m_radius = 5;

			// Alle Objekte im Kreis suchen
			hitobj.clear();
			getRegion()->getObjectsInShape(&s,&hitobj,WorldObject::LAYER_AIR,WorldObject::CREATURE,creator);
			rmin = sqr(s.m_radius);
			hit =0;

			for (i=hitobj.begin();i!=hitobj.end();++i)
			{
				// Durchmustern der potentiellen Ziele
				DEBUGX("testing obj %i, lid %i",(*i)->getId(),lid);

				// bereits getroffene Objekte ausschliessen
				if (m_hit_objects_ids.count((*i)->getId()) > 0)
					continue;

				// alle nicht feindlich gesinnten Objekte ausschliessen
				if (World::getWorld()->getRelation(getFraction(),(*i)) != Fraction::HOSTILE)
					continue;

				// Abstand zur aktuellen Position ermitteln
				hitpos = (*i)->getShape()->m_center - newpos;

				rnew = sqr(hitpos.m_x)+sqr(hitpos.m_y);
				DEBUGX("radius %f",rnew);
				// Das Objekt herraussuchen, welches den minimalen Abstand aufweist
				if (rnew < rmin)
				{
					rmin = rnew;
					hit = (*i);
				}
			}

			if (hit!=0)
			{
				// Es wurde ein Ziel gefunden
				DEBUGX("next obj %i",hit->getId());
				DEBUGX("counter %i",m_counter);

				// Neue Richtung ist Vektor von aktueller Position zum neuen Ziel
				dir = hit->getShape()->m_center - newpos;
				dir.normalize();
				setSpeed(dir *v);

				DEBUGX("dir %f %f",dir.m_x,dir.m_y);
				m_timer =0;
				
				if (World::getWorld()->isServer())
				{
					// Schaden pro Sprung um 20% reduzieren
					if (m_flags & BOUNCING)
					{
						for (int i=0; i<4; i++)
							m_damage->m_multiplier[i] *= 0.7;
					}
				}

			}
			else
			{
				// kein Ziel gefunden, Projektil zerstoeren
				do_destroy = true;
			}

			// Zaehler,nach hinreichend vielen Spruengen Projektil zerstoeren
			m_counter --;
			if (m_counter <= 0)
			{
				do_destroy = true;
			}
		}

	}
	
	if (do_destroy)
	{
		destroy();
	}
	
	// neue Koordinaten setzen
	setPosition(newpos);

}

void Projectile::handleGrowing(float dtime)
{
	WorldObjectList hitobj;
	WorldObjectList::iterator i;
	WorldObject* hit;

	// Radius erhoehen
	getShape()->m_radius += m_max_radius* dtime/(m_timer_limit);

	if (World::getWorld()->isServer())
	{
		// Schaden an die neu getroffenen Lebewesen austeilen
		Shape s;
		s.m_center = getShape()->m_center;
		s.m_type = Shape::CIRCLE;
		s.m_radius = getShape()->m_radius;

		// Alle Objekte suchen die sich in dem Kreis befinden
		getRegion()->getObjectsInShape(getShape(),&hitobj,getLayer(),WorldObject::CREATURE,0);
		DEBUGX("last hit id = %i",lid);
		
		// Schaden austeilen
		for (i=hitobj.begin();i!=hitobj.end();++i)
		{
			hit = (*i);
			DEBUGX("covering obj %i",hit->getId());

			// Kein Schaden an nicht feindliche Objekte austeilen
			if (World::getWorld()->getRelation(getFraction(),hit) != Fraction::HOSTILE)
				continue;


			// kein Schaden an das bereits getroffene Objekt austeilen
			if (m_hit_objects_ids.count(hit->getId()) > 0)
			{
				continue;
			}

			// Schaden austeilen
			doEffect(hit);
			m_hit_objects_ids.insert(hit->getId());
		}
		
	}

	if (m_timer >= m_timer_limit)
	{
		// Standardverhalten Projektil zerstoeren
		destroy();
	}

}

void Projectile::handleStable(float dtime)
{

	if (m_timer / m_timer_limit >= m_crit_percent && (m_timer-dtime) / m_timer_limit < m_crit_percent)
	{
		doEffect();
	}
	
	if (m_timer >= m_timer_limit && World::getWorld()->isServer())
	{
		// Timer Limit erreicht
		
		// Schaden wird in Wellen austeilt
		m_counter --;
		if (m_counter >0)
		{
				// naechste Welle: Timer zuruecksetzen, Zaehler erhoehen
			m_timer =0;
		}
		else
		{
			destroy();
		}
	}
}


void Projectile::doEffect(GameObject* target)
{
	ProjectileBasicData* pdata = ObjectFactory::getProjectileData(getSubtype());
	if (pdata ==0)
		return;
	
	// nur auf dem Server Effect ausfuehren
	if (World::getWorld()->isServer() == false)
		return;
	
	std::list<std::string>::iterator kt;
	for (kt = pdata->m_effect.m_cpp_impl.begin(); kt !=  pdata->m_effect.m_cpp_impl.end(); ++kt)
	{
		if (*kt == "dmg_at_target")
		{
			// Wenn bisher kein Ziel angegeben, wird eines im Bereich des Projektils gesucht
			if (target ==0)
			{
				
				WorldObjectList hitobj;
				WorldObjectList::iterator i;
			
				getRegion()->getObjectsInShape(getShape(),&hitobj,getLayer(),WorldObject::CREATURE,0);
				
				WorldObject* hit =0;
				if (!hitobj.empty())
				{
					hit = hitobj.front();
				}
				
				while (!hitobj.empty() && (World::getWorld()->getRelation(getFraction(),hit->getFraction()) == Fraction:: ALLIED ))
				{
					i = hitobj.begin();
					i=hitobj.erase(i);
					if (i!=hitobj.end())
						hit=(*i);
				}
				
				target = hit;
			}
			
			Creature* ctarget = dynamic_cast<Creature*>(target);
			if (ctarget != 0)
			{
				ctarget->takeDamage(m_damage);
			}
		}
		else if (*kt == "dmg_at_enemies_in_radius")
		{
			WorldObjectList hitobj;
			WorldObjectList::iterator i;
			
			getRegion()->getObjectsInShape(getShape(),&hitobj,getLayer(),WorldObject::CREATURE,0);
			for (i=hitobj.begin();i!=hitobj.end();++i)
			{
							// Schaden austeilen
				(*i)->takeDamage(m_damage);
			}
		}
		else if (*kt == "create_projectile")
		{
			if (World::getWorld()->isServer())
			{
				Projectile* pr = ObjectFactory::createProjectile(pdata->m_new_projectile_type);
				if (pr !=0)
				{
					pr->setDamage(m_damage);
					pr->setSpeed(getSpeed());
					getRegion()->insertProjectile(pr,getShape()->m_center);
				}
			}
		}
		else if (*kt == "thunderstorm")
		{
			WorldObjectList hitobj;
			WorldObjectList::iterator i;
			WorldObject* hit;
			
			float r = getShape()->m_radius;
			
			// three attempts to find a legal target
			for (int ii=0; ii<3; ii++)
			{
				// Punkt in der Flaeche des Zaubers auswuerfeln
				Vector dir = Vector(r, r);
				Vector pos = getShape()->m_center;
				
				while (dir.getLength() > r)
				{
					dir.m_x = r*(1 - 2*Random::random());
					dir.m_y = r*(1 - 2*Random::random());
				}

				// Kreis um den ausgewuerfelten Punkt mit Radius 2.0
				Shape s;
				s.m_center = dir+pos;
				s.m_type = Shape::CIRCLE;
				s.m_radius = 2.0;
				hitobj.clear();
				
				// Alle Objekte in dem Kreis suchen
				getRegion()->getObjectsInShape(&s,&hitobj,getLayer(),WorldObject::CREATURE,0);

				// Alle nicht feindlichen Objekte aus der Liste entfernen
				if (!hitobj.empty())
				{
					i = hitobj.begin();
					hit = (*i);
					while (i != hitobj.end() && World::getWorld()->getRelation(getFraction(),hit) != Fraction::HOSTILE)
					{
						i=hitobj.erase(i);
						if (i!=hitobj.end())
							hit=(*i);

					}
				}
				
				// stop if target found
				if (!hitobj.empty())
					break;
			}
			
			if (!hitobj.empty())
			{
				// Ziel gefunden
				i = hitobj.begin();
				hit = (*i);

				DEBUGX("hit obj %i",hit->getId());

				// beim Ziel neues Projektil erzeugen
				if (World::getWorld()->isServer())
				{
					Projectile* pr = ObjectFactory::createProjectile(pdata->m_new_projectile_type);
					if (pr !=0)
					{
						pr->setDamage(m_damage);
						getRegion()->insertProjectile(pr,hit->getShape()->m_center);
					}
				}
			}
			else
			{
				DEBUGX("nothing hit");
			}
		}
	}
	
	if (pdata->m_effect.m_lua_impl != LUA_NOREF)
	{
		
		EventSystem::setRegion(getRegion());
		EventSystem::setDamage(m_damage);
		
		EventSystem::pushVector(EventSystem::getLuaState(), getShape()->m_center);
		lua_setglobal(EventSystem::getLuaState(), "projectile_position");
		
		lua_pushnumber(EventSystem::getLuaState(),m_goal_object);
		lua_setglobal(EventSystem::getLuaState(), "target");
		
		
		EventSystem::pushVector(EventSystem::getLuaState(),getSpeed());
		lua_setglobal(EventSystem::getLuaState(), "projectile_speed");
		
		EventSystem::executeCodeReference(pdata->m_effect.m_lua_impl);
		
		EventSystem::setDamage(0);
		
	}
}

void Projectile::destroy()
{
	
	setState(STATE_DESTROYED);
	
	ProjectileBasicData* pdata = ObjectFactory::getProjectileData(getSubtype());
	if (pdata ==0)
		return;
	
	if (m_flags & EXPLODES)
	{
		if (World::getWorld()->isServer())
		{
			Projectile* pr = ObjectFactory::createProjectile(pdata->m_new_projectile_type);
			if (pr !=0)
			{
				pr->setDamage(m_damage);
				getRegion()->insertProjectile(pr,getShape()->m_center);
				pr->setSpeed(getSpeed());
				
				if (m_flags & MULTI_EXPLODES)
				{
					pr->addFlags(MULTI_EXPLODES);
				}
			}
		}
	}
	else if (m_flags & MULTI_EXPLODES)
	{
		// Flag mehrfach explodierend gesetzt
		DEBUGX("multiexploding");
		Vector dir = getSpeed();
		dir.normalize();
		dir *= getShape()->m_radius;
							
		// Schaden halbieren

		// vier neue Projektile erzeugen
		if (World::getWorld()->isServer())
		{
			Damage dmg;
			dmg = (*m_damage);
			for (int i=0;i<4;i++)
			{
				dmg.m_multiplier[i] *= 0.5;
			}
			
			dir.rotate(PI / 4);
			Projectile* pr;
			pr = ObjectFactory::createProjectile(getSubtype());
			if (pr != 0)
			{
				pr->setDamage(&dmg);
				pr->setMaxRadius(1);
				getRegion()->insertProjectile(pr,getPosition() + dir );
			}
								
			dir.rotate(PI / 2);
			pr = ObjectFactory::createProjectile(getSubtype());
			if (pr != 0)
			{
				pr->setDamage(&dmg);
				pr->setMaxRadius(1);
				getRegion()->insertProjectile(pr,getPosition() + dir);
			}
	
			dir.rotate(PI / 2);
			pr = ObjectFactory::createProjectile(getSubtype());
			if (pr != 0)
			{
				pr->setDamage(&dmg);
				pr->setMaxRadius(1);
				getRegion()->insertProjectile(pr,getPosition() + dir);
			}
	
			dir.rotate(PI / 2);
			pr = ObjectFactory::createProjectile(getSubtype());
			if (pr != 0)
			{
				pr->setDamage(&dmg);
				pr->setMaxRadius(1);
				getRegion()->insertProjectile(pr,getPosition() + dir);
			}
		}
	}
	
	
}

void Projectile::toString(CharConv* cv)
{
	GameObject::toString(cv);
	
	cv->toBuffer(getFraction());
	cv->toBuffer(m_timer);
	cv->toBuffer(m_timer_limit);
	cv->toBuffer(getSpeed().m_x);
	cv->toBuffer(getSpeed().m_y);
	cv->toBuffer(m_flags);
	cv->toBuffer(m_max_radius);
	cv->toBuffer(m_goal_object);
	cv->toBuffer(m_implementation);
}

void Projectile::fromString(CharConv* cv)
{
	GameObject::fromString(cv);
	
	
	cv->fromBuffer(m_fraction);
	cv->fromBuffer(m_timer);
	cv->fromBuffer(m_timer_limit);
	Vector speed;
	cv->fromBuffer(speed.m_x);
	cv->fromBuffer(speed.m_y);
	setSpeed(speed);
	cv->fromBuffer(m_flags);
	cv->fromBuffer(m_max_radius);
	cv->fromBuffer(m_goal_object);
	cv->fromBuffer(m_implementation);
}

void Projectile::writeNetEvent(NetEvent* event, CharConv* cv)
{
	if (event->m_data & NetEvent::DATA_SPEED)
	{
		cv->toBuffer(getShape()->m_center.m_x);
		cv->toBuffer(getShape()->m_center.m_y);
		cv->toBuffer(getSpeed().m_x);
		cv->toBuffer(getSpeed().m_y);
	}

	if (event->m_data & NetEvent::DATA_GOAL_OBJECT)
	{
		cv->toBuffer(m_goal_object);
	}

	if (event->m_data & NetEvent::DATA_TIMER)
	{
		cv->toBuffer(m_timer_limit);
	}

	if (event->m_data & NetEvent::DATA_MAX_RADIUS)
	{
		cv->toBuffer(m_max_radius);
		cv->toBuffer(getShape()->m_radius);
	}
}

void Projectile::processNetEvent(NetEvent* event, CharConv* cv)
{
	if (event->m_data & NetEvent::DATA_SPEED)
	{
		Vector pos;
		cv->fromBuffer(pos.m_x);
		cv->fromBuffer(pos.m_y);
		setPosition(pos);
		
		Vector speed;
		cv->fromBuffer(speed.m_x);
		cv->fromBuffer(speed.m_y);
		setSpeed(speed);
	}


	if (event->m_data & NetEvent::DATA_GOAL_OBJECT)
	{
		cv->fromBuffer(m_goal_object);
	}

	if (event->m_data & NetEvent::DATA_TIMER)
	{
		cv->fromBuffer(m_timer_limit);
		m_timer =0;
	}

	if (event->m_data & NetEvent::DATA_MAX_RADIUS)
	{
		cv->fromBuffer(m_max_radius);
		cv->fromBuffer(getShape()->m_radius);
	}
}

float Projectile::getActionPercent()
{
	return (m_timer / m_timer_limit);
}

std::string Projectile::getActionString()
{
	switch (getState())
	{
		case STATE_FLYING: 
			return "fly"; break;
		case STATE_GROWING: 
			return "grow"; break;
		case STATE_STABLE: 
			return "stable"; break;
			
		default:
			return "noaction"; break;
		
	}
}

void Projectile::getFlags(std::set<std::string>& flags)
{
	
}

void Projectile::setDamage(Damage* dmg)
{
	if (m_damage != 0)
		delete m_damage;
	
	if (dmg !=0)
	{
		m_damage = new Damage;
		(*m_damage) = (*dmg); 
		setFraction(dmg->m_attacker_fraction);
	}
	else
	{
		m_damage =0;
	}
}

