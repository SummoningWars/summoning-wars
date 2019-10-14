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

#include "region.h"
#include "world.h"
#include "player.h"
#include "eventsystem.h"
#include "itemfactory.h"
#include "scriptobject.h"

//#include "music.h"

// Allow using the list structure (used to store music tracks in region data).
#include <list>

// Sound operations helper.
#include "soundhelper.h"

// Allow the use of the sound manager.
#include "gussound.h"

using gussound::SoundManager;

RegionData::RegionData()
{
	m_id = -1;
	m_dimx= 0;
	m_dimy = 0;
	m_area_percent = 0.0f;
	m_complexity = 0.0f;
	m_granularity = 0;
	m_exit_directions[0] = false;
	m_exit_directions[1] = false;
	m_exit_directions[2] = false;
	m_exit_directions[3] = false;
  
	for (int i=0; i<3; i++)
	{
		m_ambient_light[i] = 0.2;
		m_hero_light[i] = 1.0;
		m_directional_light[i] = 0.3;
	}
	m_has_waypoint = false;
}


RegionData::~RegionData()
{
	std::multimap<TriggerType, Event*>::iterator it;
	for (it = m_events.begin(); it != m_events.end(); ++it)
	{
		delete it->second;
	}
	
}

void RegionData::addObjectGroup(ObjectGroupName group_name, int prio, int number,float probability, bool decoration)
{
	ObjectGroupSet newgroup;
	newgroup.m_group_name = group_name;
	newgroup.m_probability = probability;
	newgroup.m_number = number;
	newgroup.m_probability = probability;
	newgroup.m_decoration = decoration;
	
	m_object_groups.insert(std::make_pair(prio,newgroup));
}

void RegionData::addEvent(TriggerType trigger, Event* event)
{
	m_events.insert(std::make_pair(trigger,event));
}

void RegionCamera::addPosition(Position& pos, float time)
{
	if (m_next_positions.empty() && World::getWorld()->isServer())
	{
		NetEvent event;
		event.m_type = NetEvent::REGION_CAMERA;
		event.m_id = m_region->getId();
		m_region->insertNetEvent(event);
	}
	m_next_positions.push_back(std::make_pair(pos,time));
}

void RegionCamera::update(float time)
{
	if (m_next_positions.empty())
		return;
	
	Position& pos = m_next_positions.front().first;
	float& rtime = m_next_positions.front().second;
	
	if (rtime <= time)
	{
		// naechstes Etappenziel wurde erreicht
		if (rtime != 0)
		{
			Trigger* tr = new Trigger("camera_move_complete");
			m_region->insertTrigger(tr);
		}
		
		m_position  = pos;
		m_next_positions.pop_front();
		
		if (m_next_positions.empty())
		{
			Trigger* tr = new Trigger("all_camera_moves_complete");
			m_region->insertTrigger(tr);
		}
		else
		{
			if ( World::getWorld()->isServer())
			{
				
				NetEvent event;
				event.m_type = NetEvent::REGION_CAMERA;
				event.m_id = m_region->getId();
				m_region->insertNetEvent(event);
			}
		}
	}
	else
	{
		// Phi so anpassen, dass jeweils der kuerzere Teilkreis genutzt wird
		if (m_position.m_phi - pos.m_phi > 180)
			m_position.m_phi -= 360;
	
		if (m_position.m_phi - pos.m_phi < -180)
			m_position.m_phi += 360;
		
		float frac = time/rtime;
		m_position.m_distance = (1-frac)*m_position.m_distance + frac* pos.m_distance;
		m_position.m_phi = (1-frac)*m_position.m_phi + frac* pos.m_phi;
		m_position.m_theta = (1-frac)*m_position.m_theta + frac* pos.m_theta;
		m_position.m_focus = m_position.m_focus*(1-frac) + pos.m_focus*frac;
		
		rtime -= time;
	}
}

void RegionCamera::toString(CharConv* cv)
{
	cv->toBuffer(m_position.m_distance);
	cv->toBuffer(m_position.m_phi);
	cv->toBuffer(m_position.m_theta);
	cv->toBuffer(m_position.m_focus.m_x);
	cv->toBuffer(m_position.m_focus.m_y);
	if (m_next_positions.empty())
	{
		cv->toBuffer(static_cast<char>('0'));
	}
	else
	{
		cv->toBuffer(static_cast<char>('1'));
		Position& pos = m_next_positions.front().first;
		
		cv->toBuffer(pos.m_distance);
		cv->toBuffer(pos.m_phi);
		cv->toBuffer(pos.m_theta);
		cv->toBuffer(pos.m_focus.m_x);
		cv->toBuffer(pos.m_focus.m_y);
		cv->toBuffer(m_next_positions.front().second);
			
	}
}

void RegionCamera::fromString(CharConv* cv)
{
	cv->fromBuffer(m_position.m_distance);
	cv->fromBuffer(m_position.m_phi);
	cv->fromBuffer(m_position.m_theta);
	cv->fromBuffer(m_position.m_focus.m_x);
	cv->fromBuffer(m_position.m_focus.m_y);
	
	char c='0';
	cv->fromBuffer(c);
	if (c =='1')
	{
		Position pos;
		
		cv->fromBuffer(pos.m_distance);
		cv->fromBuffer(pos.m_phi);
		cv->fromBuffer(pos.m_theta);
		cv->fromBuffer(pos.m_focus.m_x);
		cv->fromBuffer(pos.m_focus.m_y);
		
		float time;
		cv->fromBuffer(time);
		
		time -= cv->getDelay();
		
		if (time > 0)
		{
			addPosition(pos,time);
		}
		else
		{
			memcpy(&m_position,&pos,sizeof(Position));
		}
	}
}


void RegionLight::Light::update(float time)
{
	if (m_timer != 0)
	{
		if (m_timer < time)
		{
			for (int i=0; i<3; i++)
				m_value[i] = m_goal_value[i];
			m_timer =0;
		}
		else
		{
			float rel = time / m_timer;
			for (int i=0; i<3; i++)
			{
				m_value[i] = rel*m_goal_value[i] + (1-rel)*m_value[i];
			}
			m_timer -= time;
		}
	}
}


void RegionLight::Light::toString(CharConv* cv)
{
	for (int i=0; i<3; i++)
	{
		cv->toBuffer(m_value[i]);
	}
	for (int i=0; i<3; i++)
	{
		cv->toBuffer(m_goal_value[i]);
	}
	cv->toBuffer(m_timer);
}

void RegionLight::Light::fromString(CharConv* cv)
{
	for (int i=0; i<3; i++)
	{
		cv->fromBuffer(m_value[i]);
	}
	for (int i=0; i<3; i++)
	{
		cv->fromBuffer(m_goal_value[i]);
	}
	cv->fromBuffer(m_timer);	
}


void RegionLight::init(float* ambient, float* herolight, float* directional)
{
	for (int i=0; i<3; i++)
	{
		m_ambient_light.m_value[i] = ambient[i];
		m_hero_light.m_value[i] = herolight[i];
		m_directional_light.m_value[i] = directional[i];	
	}
}

void RegionLight::setLight(std::string lighttype, float* value, float time)
{
	Light* light =0;
	if (lighttype == "ambient")
		light = &m_ambient_light;
	if (lighttype == "hero")
		light = &m_hero_light;
	if (lighttype == "directional")
		light = &m_directional_light;
	
	if (light != 0)
	{
		if (time > 0)
		{
			for (int i=0; i<3; i++)
			{
				light->m_goal_value[i] = value[i];
			}
			light->m_timer = time;
		}
		else
		{
			
			for (int i=0; i<3; i++)
			{
				light->m_value[i] = value[i];
			}
			light->m_timer=0;
		}
		
		NetEvent event;
		event.m_type = NetEvent::REGION_LIGHT;
		event.m_id = m_region->getId();
		m_region->insertNetEvent(event);
	}
}

void RegionLight::toString(CharConv* cv)
{
	m_ambient_light.toString(cv);
	m_hero_light.toString(cv);
	m_directional_light.toString(cv);	
}

void RegionLight::fromString(CharConv* cv)
{
	m_ambient_light.fromString(cv);
	m_hero_light.fromString(cv);
	m_directional_light.fromString(cv);	
}

// Note: 
// This is not recommended normally: sending a partially constructed object (this)
// to an instantiated item (m_light). In this instance it works, since m_light only
// stores the pointer. If any other operations would be attempted, it could spell disaster.
Region::Region(short dimx, short dimy, short id, std::string name, RegionData* data):
	m_dimx(dimx),
	m_dimy(dimy),

	m_data_grid(dimx, dimy),
	m_height(dimx, dimy),

	m_id(id),
	m_name(name),
	m_cutscene_mode(false),

	m_revive_location(""),

	m_light(this)
{
	SW_DEBUG ("creating region [%s]", m_name.c_str ());

	m_data_grid.clear();
	m_height.clear();
	
	Trigger* tr = new Trigger("create_region");
	insertTrigger(tr);
	
	if (data)
	{
		m_light.init(data->m_ambient_light, data->m_hero_light, data->m_directional_light);
		m_music_tracks = data->m_music_tracks;
	}
	
	m_camera.m_region = this;

	// Also create a playlist for this region, bearing the same name.
	if (! SoundManager::getPtr ()->getMusicPlayer ()->isPlaylistRegistered (m_name))
	{
		SoundManager::getPtr ()->getMusicPlayer ()->registerPlaylist (m_name);
		SoundManager::getPtr ()->getMusicPlayer ()->setRepeat (true);
		SoundManager::getPtr ()->getMusicPlayer ()->setShuffle (true);
		SoundManager::getPtr ()->getMusicPlayer ()->setFadePreferrences (false, false, false);

		// The track names to be used are stored in rdata.
		if (data)
		{
			for (std::list <std::string>::const_iterator it = data->m_music_tracks.begin ();
				it != data->m_music_tracks.end (); ++ it)
			{
				try
				{
					// The region stores track names in the form:
					// - tread_lightly.ogg
					// - ambience_2.ogg
					// The music player needs the files including their full path (absolute or relative), in the form:
					// - .//./share/resources/music/tread_lightly.ogg
					// - .//./share/resources/music/ambience_2.ogg

					std::string fileName = SoundHelper::getNameWithPathForMusicTrack (*it);
#if 1
					// One could call the following function to add an "anonymous" track (with a name such as "track_01").
					SoundManager::getPtr ()->addPlaylistTrack (m_name, fileName);
#else
					// Or one could opt for the manual 2 step solution:
					SoundManager::getPtr ()->getRepository ()->addSound (*it	// sound name
																		, fileName	// sound file path
																		, false	// load into memory? don't do this for songs
																		, gussound::GSC_Music // category: music.
																		, true);	// allow only one instance of this sound to be played at one point.
					SoundManager::getPtr ()->getMusicPlayer ()->addTrackToPlaylist (m_name, *it);
#endif
				}
				catch (std::exception& e)
				{
					SW_DEBUG ("Region creation caught exception: %s", e.what ());
				}
			}
		}
	}

	//SoundManager::getPtr ()->getMusicPlayer ()->stop ();
	//SoundManager::getPtr ()->getMusicPlayer ()->play ();
}

Region::~Region()
{
	WorldObjectMap::iterator i;
	for (i=m_objects.begin(); i!=m_objects.end();++i)
	{
		delete i->second;
	}

	for (i=m_static_objects.begin(); i!=m_static_objects.end();++i)
	{
		delete i->second;
	}

	ProjectileMap::iterator j;
	for (j =  m_projectiles.begin(); j != m_projectiles.end(); ++j)
	{
		delete (j->second);
	}


	DropItemMap::iterator k;
	for (k =  m_drop_items.begin(); k != m_drop_items.end(); ++k)
	{
		if (k->second->getItem() !=0)
			delete k->second->getItem();
		delete k->second;
	}
	
	std::list<Trigger*>::iterator l;
	for (l = m_triggers.begin(); l != m_triggers.end(); ++l)
	{
		delete *l;
	}
	
	std::multimap<TriggerType, Event*>::iterator m;
	for (m= m_events.begin(); m!= m_events.end(); ++m)
	{
		delete m->second;
	}
	
	for (std::map<int,FloatingText*>::iterator it = m_floating_texts.begin(); it != m_floating_texts.end(); ++it)
	{
		delete it->second;
	}
	
	for (std::list<PlayedSound*>::iterator it = m_played_sounds.begin(); it != m_played_sounds.end(); ++it)
	{
		delete *it;
	}

	// Note: it could be possible to remove the playlist here... but that would mean we would have to instantly cut the music.
	// If we keep the named playlist alive, we can apply a fade-out, moving from the region playlist to the menu playlist.
	// Once we create a new region playlist, we can remove the old one. (And if the old one is using the same ID, meaning: the user
	// is loading the same region, we can even resume from where we were previously playing the track).
}

WorldObject* Region::getObject ( int id)
{
	WorldObjectMap::iterator iter;

	// Objekt im Binärbaum suchen

	iter = m_objects.find(id);

	// Testen ob ein Objekt gefunden wurde
	if (iter == m_objects.end())
	{
		// unter den statischen Objekten suchen
		iter = m_static_objects.find(id);

		// Testen ob ein Objekt gefunden wurde
		if (iter == m_static_objects.end())
		{
			// keins gefunden, NULL ausgeben
			return 0;
		}
		else
		{
			// Zeiger auf Objekt ausgeben
			return iter->second;
		}
	}
	else
	{
			// Zeiger auf Objekt ausgeben
			return iter->second;
	}
}

Projectile* Region::getProjectile(int id)
{
	ProjectileMap::iterator iter;

	// Objekt im Binärbaum suchen
	iter = m_projectiles.find(id);

	// Testen ob ein Objekt gefunden wurde
	if (iter == m_projectiles.end())
	{
		// keins gefunden, NULL ausgeben
		return 0;
	}
	else
	{
		// Zeiger auf Objekt ausgeben
		return iter->second;
	}
}

GameObject* Region::getGameObject(int id)
{
	GameObjectMap::iterator iter;

	iter = m_game_objects.find(id);

	// Testen ob ein Objekt gefunden wurde
	if (iter == m_game_objects.end())
	{
		return 0;
	}
	else
	{
		return iter->second;
	}
}

bool Region::getFreePlace(Shape* shape, short layer, Vector & pos, WorldObject* omit)
{
	// Menge der bereits getesteten Felder
	std::set<int> tfields;

	// Schlange mit zu testenden Feldern
	std::queue<int> fields;

	float c = 1.1;
	
	// Position in 0.5 x 0.5 Feldern
	int sx = (int) ((pos.m_x/c) + 0.5);
	int sy = (int) ((pos.m_y/c) + 0.5);
	
	float fx = pos.m_x - sx*c;
	float fy = pos.m_y - sy*c;
	
	int i;
	fields.push(sx*10000+sy);

	// Kopie der Form
	Shape s;
	memcpy(&s,shape,sizeof(Shape));

	WorldObjectList res;
	res.clear();

    // true, solange alle Felder von festen Objekten blockiert sind
    bool fixblock = true;

	// eine Stelle suchen an der das Objekt passt
	while (!fields.empty())
	{
		// erstes Feld aus der Queue nehmen
		i = fields.front();
		sx = i /10000;
		sy = i %10000;
		fields.pop();

		DEBUGX("testing field %f %f",sx*c,sy*c);

		// Testen ob dort keine festen Objekte sind
		s.m_center = Vector(sx*c +fx,sy*c + fy);

		res.clear();
		getObjectsInShape(&s,&res,layer,WorldObject::FIXED,omit,true);

		if (!res.empty())
		{
			// Objekt im Weg
			res.clear();

			// wenn alle bisherigen Plaetze von festen Objekten blockiert warten trotzdem weitersuchen
			if (!fixblock)
                continue;
		}
		else
		{
		    fixblock = false;
		}

		DEBUGX("no fixed obstacle");
		// Testen, ob dort nicht gerade eine Kreatur steht
		getObjectsInShape(&s,&res,layer,WorldObject::CREATURE,omit,true);
		if (res.empty() && !fixblock)
		{
			DEBUGX("field is free");
			// Stelle ist frei
			pos = s.m_center;

			return true;

		}
		else
		{
			DEBUGX("obstacle is %s at %f %f",obs->getNameId().c_str(), obs->getShape()->m_center.m_x,obs->getShape()->m_center.m_y);
			// Stelle ist besetzt
			tfields.insert(i);

			// Nachbarfelder in die Liste der zu untersuchenden Felder einfuegen
			if (sx>0)
			{
				i = (sx-1)*10000+sy;
				if (tfields.find(i)==tfields.end())
				{
					fields.push(i);
				}
			}

			if (sy>0)
			{
				i = (sx)*10000+sy-1;
				if (tfields.find(i)==tfields.end())
				{
					fields.push(i);
				}
			}

			if (sx<m_dimx*4-1)
			{
				i = (sx+1)*10000+sy;
				if (tfields.find(i)==tfields.end())
				{
					fields.push(i);
				}
			}

			if (sy<m_dimy*4-1)
			{
				i = (sx)*10000+sy+1;
				if (tfields.find(i)==tfields.end())
				{
					fields.push(i);
				}
			}

		}

	}

	// keine Stelle gefunden
	return false;

}

bool  Region::addObjectsInShapeFromGridunit(Shape* shape, Gridunit* gu, WorldObjectList* result, short layer, short group,WorldObject* omit, bool empty_test )
{
	WorldObject* wo=0;
	Shape* s=0;


	std::vector<WorldObject*>& arr = gu->getObjects((WorldObject::Group) group);

	int n = gu->getObjectsNr((WorldObject::Group) group);

	DEBUGX("%i objects in layer %i, group %i",n,layer,group);

	// Schleife ueber alle Objekte
	// geprueft wird: Ebene, Schnitt mit der Flaeche
	for (int k=0;k< n;k++)
	{

		wo = arr[k];
		DEBUGX("testing obj %i layer %i",wo->getId(),wo->getLayer());

		s = wo->getShape();

		if ((wo->getLayer() & layer) && (wo->getGroup() & group))
		{
			DEBUGX("adding obj %i layer %i",wo->getId(), wo->getLayer());
			if ((shape ==0) || shape->intersects(*s))
			{

				if (wo != omit)
				{

					result->push_back(wo);

					if (empty_test)
						return true;
				}
			}
		}
	}

	return true;
}

bool Region::addObjectsOnLineFromGridunit(Line& line, Gridunit* gu, WorldObjectList* result, short layer, short group ,WorldObject* omit, bool empty_test )
{
	WorldObject* wo=0;
	Shape* s=0;

	std::vector<WorldObject*>& arr = gu->getObjects((WorldObject::Group) group);

	int n = gu->getObjectsNr((WorldObject::Group) group);

	DEBUGX("%i objects in layer %i, group %i",n,layer,group);
	// Schleife ueber alle Objekte
	// geprueft wird: Ebene, Schnitt mit der Linie
	for (int k=0;k< n;k++)
	{

		wo = arr[k];
		DEBUGX("testing %s %p",wo->getNameId().c_str(),wo->getNameId().c_str())
		DEBUGX("testing %p",wo);
		s = wo->getShape();

		if ((wo->getLayer() & layer) && (wo->getGroup() & group))
		{
			if (s->intersects(line))
			{
				if (wo != omit)
				{
					result->push_back(wo);

					if (empty_test)
						return true;
				}
			}
		}
	}

	return true;
}


bool Region::getObjectsInShape( Shape* shape,  WorldObjectList* result,short layer, short group, WorldObject* omit, bool empty_test)
{

	DEBUGX("shape %f %f %f",shape->m_center.m_x,shape->m_center.m_y,shape->m_radius);

	 // Wenn der Resultat Zeiger 0 ist: Fehler ausgeben
	if (result == 0)
	{
		ERRORMSG("no result list given");
		return false;
	}

	if (shape == 0)
	{
		ERRORMSG("no shape given");
		return false;
	}


	DEBUGX("layer %i group %i",layer,group);


	if (group == WorldObject::PLAYER)
	{
		// Wenn nur nach Spielern gesucht ist, dann nur die Liste der Spieler durchsuchen

		WorldObjectMap::iterator it;
		for (it = m_players.begin(); it != m_players.end(); ++it)
		{
 			if (shape->intersects(*(it->second->getShape()) ) && it->second != omit)
			{
				result->push_back(it->second);
			}
		}
	}
	else
	{
		int i,j;
		Gridunit* gu=0;
		// durchmustere alle 4*4 Felder die von dem Suchrechteck plus 4 Felder bedeckt werden

		// Maximale Ausdehnung der Flaeche in Richtung der Koordinatenaxen
		Vector ext = shape->getAxisExtent();
		Vector d1 = shape->m_center +ext;
		Vector d2 = shape->m_center -ext;


		int xmin = (int) floor(0.25*(d2.m_x-4));
		int ymin = (int) floor(0.25*(d2.m_y-4));
		int xmax = (int) floor(0.25*(d1.m_x+4));
		int ymax = (int) floor(0.25*(d1.m_y+4));
		// Pruefen ob die Suchanfrage nicht aus der Region herauslaeuft

		int is = MathHelper::Max (xmin,0);
		int ie = MathHelper::Min(xmax,m_dimx-1);
		int js = MathHelper::Max(ymin,0);
		int je = MathHelper::Min(ymax,m_dimy-1);

		DEBUGX("searching square (%i %i) (%i %i)",is,ie,js,je);

		// Alle 4x4 Felder durchmustern
		for (i = is;i<=ie;i++)
		{
			for (j=js;j<=je;j++)
			{

				DEBUGX("searching in Grid Tile %i %i",i,j);
				// Durchmustern der Listen im 4x4-Feld
				gu = &(m_data_grid)[i][j];

				// Totenebene
				if (group & WorldObject::DEAD)
				{
					DEBUGX("searching dead layer");

					addObjectsInShapeFromGridunit(shape, gu, result, layer,group & WorldObject::DEAD, omit, empty_test);
					if (!result->empty() && empty_test)
						return true;

				}

				// feste Objekte
				if (group & WorldObject::FIXED )
				{
					DEBUGX("searching fixed layer");

					addObjectsInShapeFromGridunit(shape, gu, result, layer,group & WorldObject::FIXED, omit, empty_test);
					if (!result->empty() && empty_test)
						return true;

				}

				// lebende Objekte
				if (group & WorldObject::CREATURE)
				{

					addObjectsInShapeFromGridunit(shape, gu, result, layer,group & WorldObject::CREATURE, omit, empty_test);
					if (!result->empty() && empty_test)
						return true;

				}

			}

		}
		
		// grosse Objekte durchmustern
		WorldObjectMap::iterator it;
		for (it = m_large_objects.begin(); it != m_large_objects.end(); ++it)
		{
			if (( it->second->getGroup() & group) && (it->second->getLayer() & layer))
			{
				if (shape->intersects(*(it->second->getShape())) && it->second != omit )
				{
					result->push_back(it->second);
					if (empty_test)
						return true;
				}
			}
		}
		
	}
	return true;
}


WorldObject* Region::getObjectAt(Vector pos, short layer, short group)
{
	Shape s;
	s.m_type = Shape::CIRCLE;
	s.m_radius =0;
	s.m_center = pos;

	WorldObjectList l;
	l.clear();
	getObjectsInShape ( &s, &l,layer,group );
	if (l.empty())
	{
		return 0;
	}
	else
	{
		return *(l.begin());
	}
}



void Region::getObjectsOnLine(Line& line,  WorldObjectList* result,short layer, short group , WorldObject* omit)
{
	Gridunit* gu=0;

	int xmin = (short) floor(0.25*(MathHelper::Min(line.m_start.m_x,line.m_end.m_x)-4));
	int ymin = (short) floor(0.25*(MathHelper::Min(line.m_start.m_y,line.m_end.m_y)-4));
	int xmax = (short) floor(0.25*(MathHelper::Max(line.m_start.m_x,line.m_end.m_x)+4));
	int ymax = (short) floor(0.25*(MathHelper::Max(line.m_start.m_y,line.m_end.m_y)+4));
	int i,j;
	float d;

	// Richtung der Linie
	Vector dir = line.getDirection();
	dir.normalize();
	Vector p;

	for (i = MathHelper::Max (xmin,0);i<=MathHelper::Min(xmax,m_dimx-1);i++)
	{
		for (j=MathHelper::Max(ymin,0);j<=MathHelper::Min(ymax,m_dimy-1);j++)
		{
			DEBUGX("searching in Grid Tile %i %i",i,j);

			// Herausfiltern jener Felder die zu weit von der Linie entfernt sind
			p.m_x = i*4.0+2;
			p.m_y = j*4.0+2;
			p -= line.m_start;

			p.normalPartTo(dir);

			d = p.getLength();
			if (d>32)
			{
				DEBUGX("aborted");
				continue;
			}

			// Durchmustern der Listen im 4x4-Feld
			gu = &(m_data_grid)[i][j];

			// Totenebene
			if (group & WorldObject::DEAD)
			{
				DEBUGX("searching dead layer");

				addObjectsOnLineFromGridunit(line, gu, result, layer,group & WorldObject::DEAD, omit);
			}

				// feste Objekte
			if (group & WorldObject::FIXED )
			{
				DEBUGX("searching fixed layer");


				addObjectsOnLineFromGridunit(line, gu, result, layer,group & WorldObject::FIXED, omit);
			}

				// lebende Objekte
			if (group & WorldObject::CREATURE)
			{


				addObjectsOnLineFromGridunit(line, gu, result, layer,group & WorldObject::CREATURE, omit);
			}


		}

	}
	
	WorldObjectMap::iterator it;
	for (it = m_large_objects.begin(); it != m_large_objects.end(); ++it)
	{
		if (( it->second->getGroup() & group) && (it->second->getLayer() & layer))
		{
			if ((it->second->getShape()->intersects(line)) && it->second != omit )
			{
				result->push_back(it->second);
			}
		}
	}
}


bool Region::insertObject(WorldObject* object, Vector pos, float angle )
{
	DEBUGX("try to insert %s at %f %f",object->getSubtype().c_str(), pos.m_x,pos.m_y);
	bool result = true;
	
	bool collision_test = object->checkInteractionFlag(WorldObject::COLLISION_DETECTION);
	
	object->setRegionId(m_id);

	 // Einfügen in den Binärbaum
	if (object->getState() != WorldObject::STATE_STATIC)
	{
		result &= (m_objects.insert(std::make_pair(object->getId(),object))).second;
		result &= (m_game_objects.insert(std::make_pair(object->getId(),object))).second;
	}
	else
	{
		result &= (m_static_objects.insert(std::make_pair(object->getId(),object))).second;
	}


	 // Wenn das Element bereits existiert ist die Antwort false
	if (result==false)
	{
		SW_DEBUG("Object with id %i already exists",object->getId());
		return result;
	}

	if (object->getType() == "PLAYER")
	{
		Player* pl = dynamic_cast<Player*>(object);
		DEBUGX("player entered Region");
		result &= (m_players.insert(std::make_pair(object->getId(),object))).second;
		
		if (m_revive_location != "")
		{
			pl->setRevivePosition(std::make_pair(getIdString(),m_revive_location));
		}
		
		Trigger* tr = new Trigger("enter_region");
		tr->addVariable("player", object->getId());
		insertTrigger(tr);

		// pruefen ob Party komplett in der Region
		Party* party = pl->getParty();
		std::set< int > & members = party->getMembers();
		std::set< int >::iterator it;
		bool complete = true;
		for (it = members.begin(); it != members.end(); ++it)
		{
			if (getObject(*it) == 0)
			{
				complete = false;
			}
		}
		
		if (complete)
		{
			tr = new Trigger("party_complete");
			insertTrigger(tr);
		}
	}
	else
	{
		// fuer Nicht Spieler NetEvent erzeugen, dass das Objekt erschaffen wurde
		
		NetEvent event;
		event.m_type = NetEvent::OBJECT_CREATED;
		event.m_id = object->getId();
		insertNetEvent(event);
		DEBUGX("insert object %i",event.m_id);
		
		if (object->getGroup() & WorldObject::CREATURE)
		{
			Trigger* tr = new Trigger("create_unit");
			tr->addVariable("unit", object->getId());
			insertTrigger(tr);
		}
	}

	object->getShape()->m_angle = angle;
	
	// Test auf Kollisionen und eventuelle Verschiebung
	if (collision_test)
	{
		getFreePlace(object->getShape(), object->getLayer(), pos);
	}

	 // Koordinaten setzen
	object->getShape()->m_center = pos;

	 // Einfuegen in das Grid
	int x_g = (int) floor(0.25*pos.m_x);
	int y_g = (int) floor(0.25*pos.m_y);
	object->getGridLocation()->m_grid_x = x_g;
	object->getGridLocation()->m_grid_y = y_g;


	 // Testen ob das Objekt in der Region liegt
	if (x_g<0 || y_g<0 || x_g>=m_dimx || y_g>=m_dimy)
	{
		DEBUGX("create Object at %f %f",pos.m_x, pos.m_y);
		return false;
	}
	else
	{
		if (object->getLayer() & WorldObject::LAYER_COLLISION)
		{
			// testen ob das Objekt zu den gesondert behandelten grossen Objekten gehoehrt
			if (object->isLarge())
			{
				DEBUGX("object %s is large",object->getName().c_str());
				m_large_objects.insert(std::make_pair(object->getId(),object));
				result = true;
			}
			else
			{
				Gridunit *gu = (m_data_grid.ind(x_g,y_g));

				result = gu->insertObject(object);
			}
		}

	}

	DEBUGX("object inserted %s %i at %f %f",object->getSubtype().c_str(), object->getId(), object->getShape()->m_center.m_x,object->getShape()->m_center.m_y);
	return result;
}

int Region::createObject(ObjectTemplateType generictype, Vector pos, float angle, float height, WorldObject::State state)
{
	// Umgebung erfahren
	EnvironmentName env = getEnvironment(pos);

	// genauen Subtyp ermitteln
	WorldObject::Subtype subtype = ObjectFactory::getObjectType(generictype, env);
	
	if (subtype == "notype")
	{
		// Objekt fuer die Umgebung explizit als nichtexistent gesetzt
		return 0;
	}
	
	if (subtype == "")
	{
		SW_DEBUG("no subtype found for generictype %s",generictype.c_str());
		return 0;
	}
	
	// Basistyp ermitteln
	WorldObject::Type type = ObjectFactory::getObjectBaseType(subtype);
	if (type == "NONE")
	{
		SW_DEBUG("no base type for subtype %s",subtype.c_str());
		return 0;
	}

	// Objekt erstellen
	WorldObject* object = ObjectFactory::createObject(type,subtype);

	if (object ==0)
	{
		SW_DEBUG("could not create object for generictype %s",generictype.c_str());
		return 0;
	}

	if (state != WorldObject::STATE_NONE)
	{
		object->setState(state,false);
	}
	
	if (state == WorldObject::STATE_AUTO)
	{
		if (object->getType() == "FIXED_OBJECT")
		{
			object->setState(WorldObject::STATE_INACTIVE,false);
		}
		else
		{
			object->setState(WorldObject::STATE_ACTIVE,false);
		}
	}
	
			
	// Objekt einfuegen
	object->setHeight(height);
	bool ret = insertObject(object,pos,angle);
	if (!ret)
	{
		SW_DEBUG("insertion of object %s failed",object->getNameId().c_str());
	}

	return object->getId();

}


void Region::createObjectGroup(ObjectGroupName templname, Vector position, float angle, std::string name,WorldObject::State state)
{
	Trigger* tr=0;
	
	// Template aus der Datenbank heraussuchen
	std::map<ObjectGroupName, ObjectGroup*>::iterator it;

	
	// Umgebung erfahren
	EnvironmentName env = getEnvironment(position);
	ObjectGroupName templnameneu = ObjectFactory::getObjectGroupType(templname,env);
	
	if (templnameneu == "notype")
	{
		// Objektgruppe fuer die Umgebung explizit als nichtexistent gesetzt
		return;
	}
	
	ObjectGroup* templ = ObjectFactory::getObjectGroup(templnameneu);
	
	if (templ != 0)
	{
		if (name != "" || !templ->getLocations()->empty())
		{
			tr = new Trigger("create_template");
			tr->addVariable("templname",templname);
			tr->addVariable("name",name);
			tr->addVariable("position",position);
			tr->addVariable("angle",angle);
			
			// Mittelpunkt eintragen
			std::string locname = name;
			locname += ":center";
			addLocation(locname,position);
			
			// Flaeche eintragen
			std::string areaname = name;
			areaname += ":area";
			addArea(areaname,*(templ->getShape()));
		}
		
		// Template wurde gefunden
		// Objekte platzieren
		std::list<ObjectGroup::GroupObject>::iterator gt;
		Vector pos;

		float oangle=0;
		for (gt = templ->getObjects()->begin(); gt != templ->getObjects()->end(); ++gt)
		{
			if (Random::random() < gt->m_probability)
			{

				pos = gt->m_center;
				pos.rotate(angle);
				pos += position;

				oangle = gt->m_angle;
				if (gt->m_prob_angle)
				{
					oangle = 2*3.14159*Random::random();
				}

				int id;
				// Objekte mit Name sollte man loeschen / modifizieren koennen
				// das wird erreicht, in dem der Status auf Inaktiv statt static gestellt wird
				WorldObject::State ostate = state;
				if (gt->m_name != "")
				{
					ostate = WorldObject::STATE_AUTO;
				}
				
				id = createObject(gt->m_type, pos, angle+oangle,gt->m_height,ostate);
				DEBUGX("inserting object %s at %f %f with id %i",gt->m_type.c_str(),pos.m_x, pos.m_y,id);
				
				if (gt->m_name != "")
				{
					setNamedId(gt->m_name,id);
					if (tr != 0)
					{
						tr->addVariable(gt->m_name,id);
					}
				}
			}
		}

		// Orte einfuegen
		std::map<LocationName, Vector>::iterator lt;
		for (lt = templ->getLocations()->begin(); lt != templ->getLocations()->end(); ++lt)
		{
			pos = lt->second;
			pos.rotate(angle);
			pos += position;

			std::string locname;
			if (name != "")
			{
				locname += name;
				locname += ":";
			}
			locname += lt->first;
			addLocation(locname,pos);
			if (tr != 0)
			{
				tr->addVariable(lt->first,pos);
			}
			DEBUGX("template location %s",locname.c_str());
		}
		
		std::map<AreaName, Shape>::iterator at;
		Shape sh;
		for (at = templ->getAreas().begin(); at != templ->getAreas().end(); ++at)
		{
			sh = at->second;
			sh.m_angle += angle;
			sh.m_center.rotate(angle);
			sh.m_center += position;
			
			std::string aname;
			if (name != "")
			{
				aname += name;
				aname += ":";
			}
			aname += at->first;
			
			addArea(aname,sh);
		}
		
		// Trigger der besagt dass das Template eingefuegt wurde
		if (tr !=0)
		{
			insertTrigger(tr);
		}
	}
	else
	{
		ERRORMSG("object group template %s not found",templname.c_str());
	}
}


void Region::createMonsterGroup(MonsterGroupName mgname, Vector position, float radius, std::list<int>* monsters)
{
	MonsterGroup * mgroup = ObjectFactory::getMonsterGroup(mgname);

	if (mgroup == 0)
	{
		SW_DEBUG("monster group %s not found",mgname.c_str());
		return;
	}

	std::list<MonsterGroup::SubGroup>::iterator mt;
	Vector pos;
	float dx,dy;
	
	for (mt = mgroup->m_monsters.begin(); mt != mgroup->m_monsters.end(); ++mt)
	{
		int count =0;
		for (int i=0; i< mt->m_number; i++)
		{
			pos = position;
			do
			{
				dx = -radius + Random::randf(radius*2);
				dy = -radius + Random::randf(radius*2);	
			}
			while (dx*dx+dy*dy > radius*radius);
			
			pos.m_x += dx;
			pos.m_y += dy;	

			if (Random::random() < mt->m_prob)
			{
				count ++;
				int id = createObject(mt->m_subtype, pos,2*3.14159*Random::random());
				DEBUGX("inserting monster %s at %f %f with id %i",mt->m_subtype.c_str(),pos.m_x, pos.m_y,id);
				
				if (monsters != 0)
				{
					monsters->push_back(id);
				}
			}
		}
		DEBUGX("monstergroup %s : %i x %s prob %f",mgname.c_str(), count, mt->m_subtype.c_str(), mt->m_prob);
		
	}
}


bool  Region::insertProjectile(Projectile* object, Vector pos)
{
	DEBUGX("projectile inserted: %s %i",object->getSubtype().c_str(), object->getId());
	m_projectiles.insert(std::make_pair(object->getId(),object));
	m_game_objects.insert(std::make_pair(object->getId(),object));
	object->getShape()->m_center = pos;
	object->setRegionId( m_id);

	// NetEvent erzeugen: neues Projektil in der Region
	NetEvent event;
	event.m_type = NetEvent::PROJECTILE_CREATED;
	event.m_id = object->getId();
	insertNetEvent(event);

	return true;
}

bool  Region::deleteObject (WorldObject* object)
{
	bool result = true;

	if (object == 0)
		return false;
	
	if (m_objects.count(object->getId()) == 0 && m_static_objects.count(object->getId()) == 0)
	{
		// Objekt nicht gefunden
		return false;
	}
	
	if (object->getType() != "PLAYER")
	{
		
		NetEvent event;
		event.m_type = NetEvent::OBJECT_DESTROYED;
		event.m_id = object->getId();
		insertNetEvent(event);
		
	}
	
	if (object->isCreature())
	{
		Creature* cr = dynamic_cast<Creature*>(object);
		if (cr != 0 && cr->getDialogue() != 0)
		{
			cr->getDialogue()->removeSpeaker(object->getId());
		}
	}
	
	 // Aus dem Binärbaum loeschen
	if (object->getState() != WorldObject::STATE_STATIC)
	{
		m_objects.erase(object->getId());
		m_game_objects.erase(object->getId());
	}
	else
	{
		m_static_objects.erase(object->getId());
	}

	if (object->getType() == "PLAYER")
	{
		DEBUGX("Player deleted");
		m_players.erase(object->getId());

		
		Trigger* tr = new Trigger("leave_region");
		tr->addVariable("player", object->getId());
		insertTrigger(tr);
	}
	
	if (object->isLarge())
	{
		if (m_large_objects.count(object->getId()) == 0)
		{
			result = false;
		}
		else
		{
			m_large_objects.erase(object->getId());
		}
	}
	else
	{
		// aus dem Grid loeschen
		int x = object->getGridLocation()->m_grid_x;
		int y = object->getGridLocation()->m_grid_y;
		DEBUGX("deleting object in grid tile %i %i",x,y);
	
		if (object->getLayer() & WorldObject::LAYER_COLLISION)
		{
			Gridunit *gu = (m_data_grid.ind(x,y));
			result = gu->deleteObject(object, object->getGridLocation()->m_index);
		}
	}

	return result;
}

bool Region::deleteObject(int id)
{
	WorldObject* wo = getObject(id);
	if (wo !=0)
	{
		return deleteObject(wo);
	}
	else
	{
		SW_DEBUG("no object with id %i",id);
	}
	return false;
}

bool Region::moveObject(WorldObject* object, Vector pos)
{
	bool result = true;
	// Wenn NULL Zeiger übergeben -> Fehler anzeigen
	if (object == 0)
		return false;
	
	if (object->getState() == WorldObject::STATE_STATIC)
	{
		ERRORMSG("Cant move static object %s",object->getNameId().c_str());
		return false;
	}
	
	// grosse Objekte werden einfach verschoben (da sie nicht in der Gridunit sind)
	if (object->isLarge())
	{
		object->getShape()->m_center = pos;
		return true;
	}
	
	if (object->getLayer() == WorldObject::LAYER_NOCOLLISION)
	{
		object->getShape()->m_center = pos;
		return true;
	}

	// Testen ob das Objekt innerhalb des 4*4 Grid in ein neues Feld kommt
	int x_old = object->getGridLocation()->m_grid_x;
	int y_old = object->getGridLocation()->m_grid_y;
	int x_new = (int) floor(0.25*pos.m_x);
	int y_new = (int) floor(0.25*pos.m_y);

	if (x_old == x_new && y_old == y_new)
	{
		object->getShape()->m_center = pos;
		return true;
	}
	// Testen ob das Objekt in der Region liegt
	if (x_new<0 || y_new<0 || x_new>=m_dimx || y_new>=m_dimy)
	{
		return false;
	}
	else
	{
		//DEBUG ("Moved object to [%d] x [%d] ; in region sized %d x %d", x_new, y_new, m_dimx, m_dimy);
		Gridunit *gu = &(m_data_grid)[x_old][y_old];
		result =gu->deleteObject(object, object->getGridLocation()->m_index);
		if (result == false)
		{
			ERRORMSG("failed to remove object %i from gridunit %i %i",object->getId(),x_old, y_old);
		}

		gu = &(m_data_grid)[x_new][y_new];
		result = gu->insertObject(object);
		if (result == false)
		{
			ERRORMSG("failed to insert object %i into gridunit %i %i",object->getId(),x_old, y_old);
		}


		object->getGridLocation()->m_grid_x=x_new;
		object->getGridLocation()->m_grid_y=y_new;

		object->getShape()->m_center = pos;
	}

	return result;
}


bool Region::changeObjectGroup(WorldObject* object,WorldObject::Group group )
{
	bool result = true;

	if (!object->isLarge())
	{
		int x = object->getGridLocation()->m_grid_x;
		int y = object->getGridLocation()->m_grid_y;
		DEBUGX("changing object in grid tile %i %i",x,y);
	
		Gridunit *gu = (m_data_grid.ind(x,y));
		result = gu->moveObject(object,group);
	}

	return result;

}

bool Region::changeObjectLayer(WorldObject* object,WorldObject::Layer layer)
{
	short oldlayer = object->getLayer();
	if (oldlayer == layer)
		return true;
	
	bool result = true;
	
	if (oldlayer == WorldObject::LAYER_NOCOLLISION)
	{
		// Einfuegen in das Grid
		Vector pos = object->getShape()->m_center;

	 	// Einfuegen in das Grid
		int x_g = (int) floor(0.25*pos.m_x);
		int y_g = (int) floor(0.25*pos.m_y);
		object->getGridLocation()->m_grid_x = x_g;
		object->getGridLocation()->m_grid_y = y_g;


	 	// Testen ob das Objekt in der Region liegt
		if (x_g<0 || y_g<0 || x_g>=m_dimx || y_g>=m_dimy)
		{
			DEBUGX("create Object at %f %f",pos.m_x, pos.m_y);
			return false;
		}
		else
		{
			if (object->isLarge())
			{
				DEBUGX("object %s is large",object->getName().c_str());
				m_large_objects.insert(std::make_pair(object->getId(),object));
				result = true;
			}
			else
			{
				Gridunit *gu = (m_data_grid.ind(x_g,y_g));

				result = gu->insertObject(object);
				DEBUGX("insert into grid %i %i",x_g,y_g);
			}
		}
	}
	
	if (layer == WorldObject::LAYER_NOCOLLISION)
	{
		if (object->isLarge())
		{
			if (m_large_objects.count(object->getId()) == 0)
			{
				result = false;
			}
			else
			{
				m_large_objects.erase(object->getId());
			}
		}
		else
		{
			// aus dem Grid loeschen
			int x = object->getGridLocation()->m_grid_x;
			int y = object->getGridLocation()->m_grid_y;
			DEBUGX("deleting object in grid tile %i %i",x,y);
			
			Gridunit *gu = (m_data_grid.ind(x,y));
			result = gu->deleteObject(object, object->getGridLocation()->m_index);
			
		}
	}
	
	return result;
}


void Region::deleteProjectile(Projectile* proj)
{
	if (proj ==0)
		return;
	
	int id = proj->getId();

	if (m_projectiles.count(id)!=0)
	{
		m_projectiles.erase(m_projectiles.find(id));
		DEBUGX("projectile deleted: %s %i",proj->getSubtype().c_str(), proj->getId());
	}
	m_game_objects.erase(id);
}

void Region::update(float time)
{
	DEBUGX("update region %i",getId());

	DEBUGX("\nUpdate aller WeltObjekte starten\n");
	//DEBUG("m_players %p",m_players);
	// Iterator zum durchmustern einer solchen Liste
	WorldObjectMap::iterator iter;
	WorldObject* object;
	ProjectileMap::iterator it3;

	// clear the list of sounds from the previous update
	for (std::list<PlayedSound*>::iterator it = m_played_sounds.begin(); it != m_played_sounds.end(); ++it)
	{
		delete *it;
	}
	m_played_sounds.clear();
	
	
	// Durchmustern aller WorldObjects
	for (iter =m_objects.begin(); iter!=m_objects.end(); )
	{
		object = iter->second;

		if (object->getDestroyed()==true)
		{
			// Objekte selbststaendig loeschen darf nur der Server
			if (World::getWorld()->isServer())
			{
				// nur Nichtspieler Objekte loeschen
				if (object->getType() != "PLAYER")
				{
					DEBUGX("Objekt gelöscht: %i \n",object->getId());
					
					++iter;
					object->destroy();
					bool succ = deleteObject(object);
					if (succ)
					{
						delete object;
						continue;
					}
				}
				else
					++iter;
			}
			else
			{
				++iter;
			}
		}
		else
		{
			++iter;
		}

	}
	
	for (iter =m_objects.begin(); iter!=m_objects.end(); ++iter)
	{
		object = iter->second;
		if (object->getDestroyed()!=true)
		{
			// Polymorpher Funktionsaufruf
			object->update(time);
		}
	}
	DEBUGX("Update aller WeltObjekte abgeschlossen\n\n");

	// alle Projektile updaten
	Projectile* pr =0;

	for (it3 = m_projectiles.begin(); it3 !=m_projectiles.end();)
	{
		pr = (it3->second);
		DEBUGX("projectile %i",pr->getId());
		if (pr->getDestroyed()==true)
		{
			// Projektile selbststaendig loeschen darf nur der Server
			if (World::getWorld()->isServer())
			{
				
				NetEvent event;
				event.m_type = NetEvent::PROJECTILE_DESTROYED;
				event.m_id = pr->getId();
				insertNetEvent(event);
				
				DEBUGX("deleting projectile %i",pr->getId());
				
				++it3;
				deleteProjectile(pr);
				delete pr;
				DEBUGX("loesche projektil");
			}
			else
			{
			 	++it3;
			}

		}
		else
		{
			pr->update(time);
			++it3;
		}
	}
	DEBUGX("update projektile abgeschlossen");

	// DropItems updaten
	DropItemMap::iterator it4;
	for (it4 = m_drop_items.begin(); it4 != m_drop_items.end(); ++it4)
	{
		it4->second->update(time);
	}

	if (World::getWorld()->isServer())
	{
		// NetEvents fuer geaenderte Objekte / Projektile erzeugen
		for (iter =m_objects.begin(); iter!=m_objects.end(); ++iter)
		{
			object = iter->second;

			// NetEvents durch Spieler werden global behandelt, daher hier nicht beruecksichtigen
			if (object->getType() == "PLAYER")
			{
				continue;
			}

			if (object->getNetEventMask() !=0)
			{
				DEBUGX("object %i has event mask %i",object->getId(), object->getNetEventMask());
				NetEvent event;
				event.m_type = NetEvent::OBJECT_STAT_CHANGED;
				event.m_data = object->getNetEventMask();
				event.m_id = object->getId();
				insertNetEvent(event);
				
				object->clearNetEventMask();
			}
		}

		for (it3 = m_projectiles.begin(); it3 !=m_projectiles.end();++it3)
		{
			pr = (it3->second);
			if (pr->getNetEventMask() !=0)
			{
				NetEvent event;
				event.m_type = NetEvent::PROJECTILE_STAT_CHANGED;
				event.m_data = pr->getNetEventMask();
				event.m_id = pr->getId();
				insertNetEvent(event);
				
				pr->clearNetEventMask();
			}
		}
	}

	// pruefen ob ein Spieler die Region verlassen hat
	bool del = false;
	for (iter = m_players.begin(); iter != m_players.end(); )
	{
		// Schleife ueber die Ausgaenge
		std::list<RegionExit>::iterator eit;
		for (eit = m_exits.begin(); eit != m_exits.end(); ++eit)
		{
			if (iter->second->getShape()->intersects(eit->m_shape))
			{
				// Spieler befindet sich im Ausgang
				// ID der neuen Region
				int id = World::getWorld()->getRegionId(eit->m_destination_region);

				WorldObjectMap::iterator iter2 = iter;
				++iter;
				del = true;

				// Spieler aus der Region entfernen
				WorldObject* pl = iter2->second;
				deleteObject(pl);

				// Spieler in die neue Region einfuegen
				World::getWorld()->insertPlayerIntoRegion(pl, id, eit->m_destination_location);

				break;
			}
		}


		if (del == false)
		{

			// Pruefen ob der Spieler getoetet wurde
			if (iter->second->getState() == WorldObject::STATE_DEAD)
			{
				// Lebenspunkte wieder fuellen etc
				Player* pl = static_cast<Player*>(iter->second);
				pl->revive();

				// Zielpunkt ermitteln
				RegionLocation regloc = pl->getRevivePosition();
				int id = World::getWorld()->getRegionId(regloc.first);

				if (id == getId())
				{
					DEBUGX("revive in current region");
					// Spieler bleibt in der aktuellen Region
					Vector pos = getLocation(regloc.second);
					getFreePlace(pl->getShape(), pl->getLayer(), pos, pl);

					pl->moveTo(pos);
				}
				else
				{
					DEBUGX("revive in other region %i ", id);

					// Spieler verlaesst die Region
					WorldObjectMap::iterator iter2 = iter;
					++iter;
					del = true;

					// Spieler aus der Region entfernen
					deleteObject(pl);

					// Spieler in die neue Region einfuegen
					World::getWorld()->insertPlayerIntoRegion(pl, id, regloc.second);
				}
			}

			if (del == false)
				++iter;
		}
		del = false;
	}
	
	// Spieler die die Region per Wegpunkt verlassen bearbeiten
	std::map<int,RegionLocation>::iterator wt;
	for (wt = m_teleport_players.begin(); wt != m_teleport_players.end(); ++wt)
	{
		Player* pl = static_cast<Player*>(getObject (wt->first));
		
		// Spieler aus der Region entfernen
		deleteObject(pl);

		// Spieler in die neue Region einfuegen
		int id = World::getWorld()->getRegionId(wt->second.first);
		World::getWorld()->insertPlayerIntoRegion(pl, id, wt->second.second);
	}
	m_teleport_players.clear();
	
	// Trigger & Events abarbeiten
	// Zeitverzoegerte Trigger
	std::list<std::pair<float, Trigger*> > ::iterator tit, tjt;
	for (tit = tjt = m_timed_trigger.begin(); tjt != m_timed_trigger.end(); )
	{
		tit = tjt;
		++tjt;
		
		tit->first -= time;
		if (tit->first<=0)
		{
			insertTrigger(tit->second);
			m_timed_trigger.erase(tit);
		}
	}
	
	// Schleife ueber die Trigger
	std::multimap<TriggerType, Event*>::iterator it, itend, jt;
	
	while (!m_triggers.empty())
	{
		EventSystem::setRegion(this);
		TriggerType type;
		type = m_triggers.front()->getType();
		
		DEBUGX("trigger: %s",type.c_str());
		
		// Schleife ueber die ausgeloesten Events
		it = m_events.lower_bound(type);
		itend = m_events.upper_bound(type);
		while (it != itend)
		{
			jt = it;
			++it;
			
			// vom Trigger definierte Variablen einfuegen
			EventSystem::doString((char*) m_triggers.front()->getLuaVariables().c_str());
			DEBUGX("lua code \n %s",m_triggers.front()->getLuaVariables().c_str());
			
			// ggf Dialog setzen
			if (m_triggers.front()->getDialogueId() != 0)
			{
				EventSystem::setDialogue( getDialogue( m_triggers.front()->getDialogueId() ) );
			}
				
			// Event ausfuehren
			bool ret = EventSystem::executeEvent(jt->second);
			
			if (ret)
			{
				DEBUGX("event on trigger: %s",type.c_str());
			}
			
			// einmalige Ereignisse loeschen, wenn erfolgreich ausgefuehrt
			if (jt->second->getOnce() &&  ret)
			{
				delete jt->second;
				m_events.erase(jt);
			}
			
		}
		
		// Objektbezogene Events ausloesen
		int id = m_triggers.front()->getObjectId();
		if (id != 0)
		{
			ScriptObject* so = dynamic_cast<ScriptObject*>(getObject(id));
			if (so != 0)
			{
				so->activateTrigger(m_triggers.front());
			}
		}
		
		delete m_triggers.front();
		m_triggers.pop_front();
	}
	
	// update der Dialoge
	if (World::getWorld()->isServer())
	{
		std::map<int, Dialogue*>::iterator it5;
		Dialogue* dia;
		for (it5 = m_dialogues.begin(); it5!= m_dialogues.end(); )
		{
			EventSystem::setDialogue(it5->second);
			
			it5->second->update(time);
			
			if (it5->second->isFinished())
			{
				dia = it5->second;
				++it5;
				deleteDialogue(dia);
			}
			else 
			{
				if (it5->second->getEventMaskRef() != 0)
				{
					DEBUGX("update dialogue %i",it5->second->getId());
					NetEvent event;
					event.m_type = NetEvent::DIALOGUE_STAT_CHANGED;
					event.m_data = it5->second->getEventMaskRef();
					event.m_id = it5->second->getId();
					insertNetEvent(event);
					it5->second->clearNetEventMask();
				}
				++it5;
			}
		}
		EventSystem::setDialogue(0);
	}
	
	m_camera.update(time);
	m_light.update(time);
	
	// Schadensanzeigen aktualisieren
	std::map<int,FloatingText*>::iterator dit;
	for (dit = m_floating_texts.begin(); dit != m_floating_texts.end(); )
	{
		dit->second->m_time -= time;
		if (dit->second->m_time <= 0)
		{
			delete dit->second;
			m_floating_texts.erase(dit++);
		}
		else
		{
			++dit;
		}
	}
}

void Region::getRegionData(CharConv* cv)
{
	// Dimension des Feldes angeben
	cv->toBuffer((short) m_dimx);
	cv->toBuffer((short) m_dimy);

	// Name der Region
	cv->toBuffer(m_name);

	cv->toBuffer(m_ground_material);
	

	// Anzahl der statischen Objekte eintragen
	DEBUGX("static objects: %i",m_static_objects.size());
	cv->toBuffer((short) m_static_objects.size());

	// statische Objekte in den Puffer eintragen
	WorldObjectMap::iterator it;
	for (it = m_static_objects.begin();it!=m_static_objects.end();++it)
	{
		(it->second)->toString(cv);
		DEBUGX("static object: %s",(it->second)->getNameId().c_str());
	}


	// Anzahl der nicht  statischen Objekte eintragen
	WorldObjectMap::iterator jt;
	short nr=0;
	for (jt = m_objects.begin();jt!=m_objects.end();++jt)
	{
		if (jt->second->getLayer() & WorldObject::LAYER_ALL)
			nr++;
	}
	DEBUGX("nonstatic objects: %i",nr);
	cv->toBuffer((short) nr);

	// nicht statische Objekte in den Puffer eintragen

	for (jt = m_objects.begin();jt!=m_objects.end();++jt)
	{
		if (jt->second->getLayer() & WorldObject::LAYER_ALL)
		{
			DEBUGX("write offset: %i",cv->writeBits());
			(jt->second)->toString(cv);

			DEBUGX("object: %s",(jt->second)->getNameId().c_str());
		}
	}

	// Anzahl der Projektile eintragen
	cv->toBuffer((short) m_projectiles.size());
	DEBUGX("projectiles: %i",m_projectiles.size());

	// Projektile in den Puffer eintragen
	ProjectileMap::iterator kt;
	for (kt = m_projectiles.begin(); kt != m_projectiles.end(); ++kt)
	{
		kt->second->toString(cv);
	}

	cv->toBuffer((short) m_drop_items.size());
	DEBUGX("dropped items: %i",m_drop_items.size());

	//  Items in den Puffer eintragen
	DropItemMap::iterator lt;
	for (lt = m_drop_items.begin(); lt != m_drop_items.end(); ++lt)
	{
		lt->second->toString(cv);
	}
	
	// Dialoge eintragen
	std::map<int, Dialogue*>::iterator dt;
	cv->toBuffer((short) m_dialogues.size());
	for (dt = m_dialogues.begin(); dt != m_dialogues.end(); ++dt)
	{
		dt->second->toString(cv);
	}
	
	// Musik
	writeMusicTracksToString(cv);
	
	// Cutscene modus
	cv->toBuffer(m_cutscene_mode);
	getCamera().toString(cv);
	
	// Licht
	m_light.toString(cv);
	
	// Untergrund
	cv->toBuffer(m_ground_material);
	
	
}


void Region::createObjectFromString(CharConv* cv, WorldObjectMap* players)
{
	WorldObject::Type type;
	WorldObject::Subtype subt;
	int id;

	WorldObject* obj;

	DEBUGX("read offset: %i",cv->readBits());

	cv->fromBuffer(type);
	cv->fromBuffer(subt);
	cv->fromBuffer(id);
	
	DEBUGX("read offset: %i",cv->readBits());

	DEBUGX("object %s id %i",subt.c_str(),id);

	// alle Objekte ausser den Spielern werden neu angelegt
	// die Spieler existieren schon
	if (type != "PLAYER")
	{
		obj = ObjectFactory::createObject(type, subt,id);
	}
	else
	{
		if (players->count(id) ==0)
		{
			ERRORMSG("player (%s) with id %i does not exist",subt.c_str(),id);
		}
		obj = (*players)[id];
	}

	if (obj==0)
	{
		ERRORMSG("failed to create Object %s",subt.c_str());
	}
	
	obj->fromString(cv);

	WorldObject* oldobj = getObject(obj->getId());
	if (oldobj != 0)
	{
		SW_DEBUG("Object %i already exists",oldobj->getId());
		oldobj->destroy();
		deleteObject(oldobj);
		delete oldobj;
	}
	insertObject(obj,obj->getShape()->m_center,obj->getShape()->m_angle);
}


void Region::createProjectileFromString(CharConv* cv)
{

	GameObject::Type type;
	GameObject::Subtype subt;
	int id;
	Projectile* proj;

	cv->fromBuffer(type);
	cv->fromBuffer(subt);
	cv->fromBuffer(id);

	DEBUGX("new projectile %s  id %i",subt.c_str(),id);

	proj = ObjectFactory::createProjectile(subt,id);

	proj->fromString(cv);

	Projectile* oldproj = getProjectile(proj->getId());
	if (oldproj != 0)
	{
		SW_DEBUG("Projectile %i already exists",oldproj->getId());
		deleteProjectile(oldproj);
		delete oldproj;
	}
	insertProjectile(proj,proj->getShape()->m_center);
}

void Region::createItemFromString(CharConv* cv)
{
	GameObject::Type type;
	GameObject::Subtype subt;
	int id;
	
	cv->fromBuffer(type);
	cv->fromBuffer(subt);
	cv->fromBuffer(id);
	
	DEBUGX("got Item %i %s %s",id,type.c_str(),subt.c_str());
	
	DropItem* di = new DropItem(id);
	di->fromString(cv);

	if (m_drop_items.count(id) >0)
	{
		DEBUGX("Item %i already exists",di->getId());
		deleteItem(id);
	}
	m_drop_items.insert(std::make_pair(id,di));
	m_drop_item_locations.insert(std::make_pair(di->getLocationId(),di));
	m_game_objects.insert(std::make_pair(di->getId(),di));
}

void Region::createDialogueFromString(CharConv* cv)
{
	int id;
	cv->fromBuffer(id);
	
	if (m_dialogues.count(id) >0)
	{
		SW_DEBUG("Dialogue %i already exists",id);
		deleteDialogue(m_dialogues[id]);
	}	
	
	Dialogue* dia = new Dialogue(this,"",id);
	dia->fromString(cv);
	insertDialogue(dia);
	
}

void Region::setRegionData(CharConv* cv,WorldObjectMap* players)
{	
	// Groesse der Region wird schon vorher eingelesen

	// Name der Region
	
	cv->fromBuffer(m_name);
	cv->fromBuffer(m_ground_material);


	// alle bisherigen statischen Objekte entfernen
	WorldObjectMap::iterator it, itnext;
	for (it = m_static_objects.begin();it!=m_static_objects.end();++it)
	{
		itnext = it;
		++itnext;
		
		it->second->destroy();
		deleteObject(it->second);
		delete it->second;
		
		it = itnext;
	}
	m_static_objects.clear();

	// alle bisherigen nichtstatischen Objekte entfernen
	// die SpielerObjekte bleiben erhalten, alle anderen werden geloescht
	WorldObjectMap::iterator jt, jtnext;
	for (jt = m_objects.begin();jt!=m_objects.end();)
	{
		jtnext = jt;
		++jtnext;
		if (jt->second->getType() != "PLAYER")
		{
			jt->second->destroy();
			deleteObject(jt->second);
			delete jt->second;
		}
		jt = jtnext;
	}
	m_objects.clear();
	m_players.clear();
	
	// DropItems loeschen
	DropItemMap::iterator k;
	for (k =  m_drop_items.begin(); k != m_drop_items.end(); ++k)
	{
		if (k->second->getItem() !=0)
		{
			delete k->second->getItem();
		}
		m_game_objects.erase(k->second->getId());
		delete k->second;
	}
	m_drop_items.clear();
	
	std::list<Trigger*>::iterator l;
	for (l = m_triggers.begin(); l != m_triggers.end(); ++l)
	{
		delete *l;
	}
	m_triggers.clear();

	// statische Objekte einlesen
	short nr_stat;
	cv->fromBuffer(nr_stat);
	DEBUGX("static objects: %i",nr_stat);

	for (int i=0; i<nr_stat;i++)
	{
		createObjectFromString(cv,players);
	}



	// neue Objekte einlesen
	short nr_nonstat;
	cv->fromBuffer(nr_nonstat);
	DEBUGX("nonstatic objects: %i",nr_nonstat);

	for (int i=0; i<nr_nonstat;i++)
	{
		createObjectFromString(cv,players);
	}


	// Anzahl der Projektile einlesen
	short nr_proj;
	cv->fromBuffer(nr_proj);
	DEBUGX("projectiles: %i",nr_proj);
	// Projektile einlesen
	for (int i=0; i<nr_proj;i++)
	{
		createProjectileFromString(cv);
	}

	// Anzahl Gegenstaende einlesen
	short nr_items;
	cv->fromBuffer(nr_items);
	DEBUGX("items: %i",nr_items);
	// Gegenstaende einlesen
	for (int i=0; i<nr_items;i++)
	{
		createItemFromString(cv);
	}
	
	// Dialoge einlesen
	short nr_dia;
	cv->fromBuffer(nr_dia);
	for (int i=0; i< nr_dia; i++)
	{
		createDialogueFromString(cv);
	}
	
	// Musik
	readMusicTracksFromString(cv);

	// Cutscene modus
	cv->fromBuffer(m_cutscene_mode);
	getCamera().fromString(cv);
	
	// Licht
	m_light.fromString(cv);
	
	// Untergrund
	cv->fromBuffer(m_ground_material);
	
}

void Region::getRegionCheckData(CharConv* cv)
{


	// Anzahl der nicht  statischen Objekte eintragen
	WorldObjectMap::iterator jt;
	short nr=0;
	for (jt = m_objects.begin();jt!=m_objects.end();++jt)
	{
		if (jt->second->getLayer() & WorldObject::LAYER_ALL)
			nr++;
	}
	DEBUGX("nonstatic objects: %i",nr);
	cv->toBuffer((short) nr);

	// nicht statische Objekte in den Puffer eintragen

	for (jt = m_objects.begin();jt!=m_objects.end();++jt)
	{
		if (jt->second->getLayer() & WorldObject::LAYER_ALL)
		{
			cv->toBuffer((jt->second)->getId());
			DEBUGX("object: %s",(jt->second)->getNameId().c_str());
		}
	}

	// Anzahl der Projektile eintragen
	cv->toBuffer((short) m_projectiles.size());
	DEBUGX("projectiles: %i",m_projectiles.size());

	// Projektile in den Puffer eintragen
	ProjectileMap::iterator kt;
	for (kt = m_projectiles.begin(); kt != m_projectiles.end(); ++kt)
	{
		cv->toBuffer((kt->second)->getId());
	}

	cv->toBuffer((short) m_drop_items.size());
	DEBUGX("dropped items: %i",m_drop_items.size());

	//  Items in den Puffer eintragen
	DropItemMap::iterator lt;
	for (lt = m_drop_items.begin(); lt != m_drop_items.end(); ++lt)
	{
		cv->toBuffer((lt->second)->getId());
	}
	
	// Cutscene modus
	cv->toBuffer(m_cutscene_mode);
}

void Region::checkRegionData(CharConv* cv)
{
	short nr;
	std::set<int> objects;
	int id;
	
	WorldObject* wo;
	Projectile* pr;
	DropItem* di;
	
	static std::set<int> objectstodelete;
	static std::set<int> objectsmissing;
	static std::set<int> projtodelete;
	static std::set<int> projmissing;
	static std::set<int> itemtodelete;
	static std::set<int> itemmissing;
	static std::set<int>::iterator setit;
	
	// Objekte die nach dem letzten Check fraglich waren loeschen
	// bzw neu anfordern
	WorldObjectList::iterator wit;
	WorldObjectMap::iterator it;
	
	// WorldObjects
	for (setit = objectstodelete.begin(); setit != objectstodelete.end(); ++setit)
	{
		wo = getObject((*setit));
		if (wo != 0)
		{
			WARNING("object %i does not exist at server",wo->getId());
			deleteObject(wo);
			if (wo->getType() != "PLAYER")
			{
				wo->destroy();
				delete wo;
			}
		}
	}
	for (setit = objectsmissing.begin(); setit != objectsmissing.end(); ++setit)
	{
		wo = getObject((*setit));
		if (wo ==0)
		{
			WARNING("object %i is missing",(*setit));
			// fehlende Daten zur Region anfordern
			PackageHeader header;
			header.m_content = PTYPE_C2S_DATA_REQUEST; 	// Data Request von Client zu Server
			header.m_number =1;

			ClientDataRequest datareq;
			datareq.m_data = ClientDataRequest::OBJECT;
			datareq.m_id = *setit;
			datareq.m_region_id = getId();

			NetworkPacket* msg = World::getWorld()->getNetwork()->createPacket();
			header.toString(msg);
			datareq.toString(msg);

			World::getWorld()->getNetwork()->pushSlotMessage(msg);
			World::getWorld()->getNetwork()->deallocatePacket(msg);
		}
	}
	
	// Projektile
	for (setit = projtodelete.begin(); setit != projtodelete.end(); ++setit)
	{
		pr = getProjectile(*setit);
		if (pr != 0)
		{
			WARNING("projectile %i does not exist at server",pr->getId());
			deleteProjectile(pr);
			delete pr;
		}
	}
	for (setit = projmissing.begin(); setit != projmissing.end(); ++setit)
	{
		pr = getProjectile(*setit);
		if (pr ==0)
		{
			WARNING("projectile %i is missing",(*setit));
			// fehlende Daten zur Region anfordern
			PackageHeader header;
			header.m_content = PTYPE_C2S_DATA_REQUEST; 	// Data Request von Client zu Server
			header.m_number =1;

			ClientDataRequest datareq;
			datareq.m_data = ClientDataRequest::PROJECTILE;
			datareq.m_id = *setit;
			datareq.m_region_id = getId();

			NetworkPacket* msg = World::getWorld()->getNetwork()->createPacket();
			
			header.toString(msg);
			datareq.toString(msg);

			World::getWorld()->getNetwork()->pushSlotMessage(msg);
			World::getWorld()->getNetwork()->deallocatePacket(msg);
		}
	}
	
	// DropItem
	for (setit = itemtodelete.begin(); setit != itemtodelete.end(); ++setit)
	{
		di = getDropItem(*setit);
		if (di != 0)
		{
			WARNING("dropitem %i does not exist at server",di->getId());
			deleteItem(di->getId(), true);
		}
	}
	for (setit = itemmissing.begin(); setit != itemmissing.end(); ++setit)
	{
		di = getDropItem(*setit);
		if (di ==0)
		{
			WARNING("dropitem %i is missing",(*setit));
			// fehlende Daten zur Region anfordern
			PackageHeader header;
			header.m_content = PTYPE_C2S_DATA_REQUEST; 	// Data Request von Client zu Server
			header.m_number =1;

			ClientDataRequest datareq;
			datareq.m_data = ClientDataRequest::ITEM;
			datareq.m_id = *setit;
			datareq.m_region_id = getId();

			NetworkPacket* msg = World::getWorld()->getNetwork()->createPacket();
			
			header.toString(msg);
			datareq.toString(msg);

			World::getWorld()->getNetwork()->pushSlotMessage(msg);	
			World::getWorld()->getNetwork()->deallocatePacket(msg);
		}
	}
	
	
	objectstodelete.clear();
	objectsmissing.clear();
	projtodelete.clear();
	projmissing.clear();
	itemtodelete.clear();
	itemmissing.clear();
	
	
	
	cv->fromBuffer(nr);
	DEBUGX("nonstatic Objects %i",nr);
	for (int i=0; i<nr; i++)
	{
		cv->fromBuffer(id);
		objects.insert(id);
		
		if (m_objects.count(id) ==0)
		{
			// Objekt fehlt beim Client
			objectsmissing.insert(id);
		}
	}
	
	for (it = m_objects.begin();it!=m_objects.end();++it)
	{
		wo = it->second;
		if (objects.count(wo->getId()) ==0)
		{
			// Objekt ist beim Client zu viel
			objectstodelete.insert(wo->getId());
		}
	}
	
	objects.clear();
	
	cv->fromBuffer(nr);
	ProjectileMap::iterator kt;
	
	DEBUGX("projectiles %i",nr);
	for (int i=0; i<nr; i++)
	{
		cv->fromBuffer(id);
		objects.insert(id);
		
		if (m_projectiles.count(id) ==0)
		{
			// Objekt fehlt beim Client
			projmissing.insert(id);
		}
	}
	
	for (kt = m_projectiles.begin(); kt!=m_projectiles.end();++kt)
	{
		pr = kt->second;
		if (objects.count(pr->getId()) ==0)
		{
			// Objekt ist beim Client zu viel
			projtodelete.insert(pr->getId());
		}
	}
	
	objects.clear();
	
	cv->fromBuffer(nr);
	DropItemMap::iterator lt;
	
	DEBUGX("dropitems %i",nr);
	for (int i=0; i<nr; i++)
	{
		cv->fromBuffer(id);
		objects.insert(id);
		
		if (m_drop_items.count(id) ==0)
		{
			// Objekt fehlt beim Client
			itemmissing.insert(id);
		}
	}
	
	for (lt = m_drop_items.begin(); lt!=m_drop_items.end();++lt)
	{
		di = lt->second;
		if (objects.count(di->getId()) ==0)
		{
			// Objekt ist beim Client zu viel
			itemtodelete.insert(di->getId());
		}
	}
}

void Region::writeMusicTracksToString(CharConv* cv)
{
	cv->toBuffer((short) m_music_tracks.size());
	std::list<MusicTrack>::iterator mt;
	for (mt = m_music_tracks.begin(); mt != m_music_tracks.end(); ++mt)
	{
		cv->toBuffer(*mt);
	}
}

void Region::readMusicTracksFromString(CharConv* cv)
{
	clearMusicTracks();
	
	short nr;
	cv->fromBuffer(nr);
	std::string music;
	for (int i=0; i<nr; i++)
	{
		cv->fromBuffer(music);
		addMusicTrack(music);
	}
}


bool  Region::dropItem(Item* item, Vector pos)
{
	DEBUGX("drop %s %i",item->m_subtype.c_str(), item->m_id);
	// Menge der bereits getesteten Felder
	std::set<int> tfields;

	// Schlange mit zu testenden Feldern
	std::queue<int> fields;

	// Position in 0.5 x 0.5 Feldern
	int sx = (int) (pos.m_x*2);
	int sy = (int) (pos.m_y*2);
	int i;
	fields.push(sx*10000+sy);

  	// true, solange alle Felder von festen Objekten blockiert sind
	bool fixblock = true;
	
	// Flaeche auf die das Item gedroppt wird
	Shape s;
	s.m_type= Shape::RECT;
	s.m_extent = Vector(0.5,0.5);


	WorldObjectList res;

	// eine Stelle suchen an der das Item fallen gelassen werden kann
	while (!fields.empty())
	{
		// erstes Feld aus der Queue nehmen
		i = fields.front();
		sx = i /10000;
		sy = i %10000;
		fields.pop();

		DEBUGX("testing field %i %i",sx,sy);

		// Testen ob dort keine festen Objekte sind
		s.m_center = Vector(sx*0.5, sy*0.5);


		res.clear();
		getObjectsInShape(&s,&res,WorldObject::LAYER_BASE,WorldObject::FIXED,0,true);

		if (!res.empty())
		{
			// Objekt im Weg
			res.clear();

			// wenn alle bisherigen Plaetze von festen Objekten blockiert warten trotzdem weitersuchen
			if (!fixblock)
				continue;
		}
		else
		{
			fixblock = false;
		}

		// Testen, ob dort nicht schon ein Item liegt
		if (!fixblock && m_drop_item_locations.find(i) == m_drop_item_locations.end())
		{
			DEBUGX("field is free");
			// Stelle ist frei
			// Item einfuegen
			DropItem* di = new DropItem(item);
			di->setPosition(Vector(sx/2.0f, sy/2.0f));
			DEBUGX("dropped item %i", sx*10000+sy);
			
			m_drop_items.insert(std::make_pair(di->getId(),di));
			m_drop_item_locations.insert(std::make_pair(i,di));
			m_game_objects.insert(std::make_pair(di->getId(),di));

			DEBUGX("items dropped at %f %f locID %i %p",sx/2.0f,sy/2.0f, di->getLocationId(),item);

			if (World::getWorld()->isServer())
			{
				
				NetEvent event;
				event.m_type = NetEvent::ITEM_DROPPED;
				event.m_id = di->getId();

				insertNetEvent(event);
				
			}

			return true;

		}
		else
		{
			// an der Stelle liegt bereits ein Item
			tfields.insert(i);

			// Nachbarfelder in die Liste der zu untersuchenden Felder einfuegen
			if (sx>0)
			{
				i = (sx-1)*10000+sy;
				if (tfields.find(i)==tfields.end())
				{
					fields.push(i);
				}
			}

			if (sy>0)
			{
				i = (sx)*10000+sy-1;
				if (tfields.find(i)==tfields.end())
				{
					fields.push(i);
				}
			}

			if (sx<m_dimx*8-1)
			{
				i = (sx+1)*10000+sy;
				if (tfields.find(i)==tfields.end())
				{
					fields.push(i);
				}
			}

			if (sy<m_dimy*8-1)
			{
				i = (sx)*10000+sy+1;
				if (tfields.find(i)==tfields.end())
				{
					fields.push(i);
				}
			}

		}

	}

	// keine Stelle gefunden um das Item zu droppen
	return false;

}

bool Region::dropItem(Item::Subtype subtype, Vector pos, int magic_power)
{
		
	Item::Type type = ItemFactory::getBaseType(subtype);
	if (type == Item::NOITEM)
	{
		return false;
	}
	
	Item* item= ItemFactory::createItem(type,subtype,0,magic_power);
	DEBUGX("drop item %s at %f %f %p",subtype.c_str(),pos.m_x,pos.m_y,item);
	
	if (item !=0)
	{
		return dropItem(item, pos);
	}
	
	return false;
}


Item*  Region::getItemAt(Vector pos)
{
	DropItemMap::iterator it;
	short sx = (int) (pos.m_x*2 + 0.5);
	short sy = (int) (pos.m_y*2 + 0.5);
	int id = sx*10000 + sy;

	it = m_drop_item_locations.find(id);
	if (it == m_drop_items.end())
	{
		return 0;
	}
	else
	{
		return it->second->getItem();
	}
}

Item* Region::getItem(int id)
{
	DropItemMap::iterator it;
	it = m_drop_items.find(id);
	if (it == m_drop_items.end())
	{
		return 0;
	}
	else
	{
		return it->second->getItem();
	}
}

DropItem* Region::getDropItem(int id)
{
	DropItemMap::iterator it;
	it = m_drop_items.find(id);
	if (it == m_drop_items.end())
	{
		return 0;
	}
	else
	{
		return it->second;
	}
}


bool Region::deleteItem(int id, bool delitem)
{

	DropItemMap::iterator it;
	DropItemMap::iterator it2;
	it = m_drop_items.find(id);
	if (it == m_drop_items.end())
	{
		return false;
	}
	else
	{
		// Item Wrapper loeschen
		int pos = it->second->getLocationId();
		it2 = m_drop_item_locations.find(pos);

		
		NetEvent event;
		event.m_type = NetEvent::ITEM_REMOVED;
		event.m_id = it->second->getId();
		insertNetEvent(event);

		if (delitem)
		{
			delete it->second->getItem();
		}
		delete (it->second);

		m_drop_items.erase(it);
		if (it2 != m_drop_item_locations.end())
		{
			m_drop_item_locations.erase(it2);
		}
		m_game_objects.erase(id);


		return true;
	}
}

EnvironmentName Region::getEnvironment(Vector pos)
{
	if (m_environments.empty())
	{
		ERRORMSG("no environments defined");
	}
	// Hoehe an der angegebenen Stelle
	float height = *(m_height.ind(int(pos.m_x/4),int(pos.m_y/4)));

	std::map<float, EnvironmentName>::iterator it;

	for (it = m_environments.begin(); it !=m_environments.end();++it)
	{
		if (height < it->first)
		{
			DEBUGX("environment %s", it->second.c_str());
			return it->second;
		}
	}

	return m_environments.rbegin()->second;
}

Vector Region::getLocation(LocationName name)
{
	std::map<LocationName, Vector>::iterator it;
	it = m_locations.find(name);

	if (it == m_locations.end())
	{
		WARNING("Location %s does not exist",name.c_str());
		return Vector(-1,-1);
	}

	return it->second;
}

Shape Region::getArea(AreaName name)
{
	std::map<AreaName, Shape>::iterator it;
	it = m_areas.find(name);

	if (it == m_areas.end())
	{
		return Shape();
	}

	return it->second;
}


void Region::addExit(RegionExit exit)
{
	exit.m_shape.m_center = getLocation(exit.m_exit_name);
	m_exits.push_back(exit);
}

void Region::insertTrigger(Trigger* trigger)
{
	m_triggers.push_back(trigger);
}

void Region::insertTimedTrigger(Trigger* trigger, float time)
{
	m_timed_trigger.push_back(std::make_pair(time,trigger));
}

void Region::addEvent(TriggerType trigger, Event* event)
{
	m_events.insert(std::make_pair(trigger,event));
}

void Region::deleteCopiedEvents()
{
	std::multimap<TriggerType, Event*>::iterator it,jt;
	for (it = m_events.begin(); it != m_events.end(); )
	{
		jt = it;
		++it;
		Event* ev = jt->second;
		if (ev->isCopy())
		{
			delete ev;
			m_events.erase(jt);
		}
	}
}

void Region::copyEventsFromRegionData(RegionData* rdata)
{
	std::multimap<TriggerType, Event*>::iterator it;
	Event* ev;
	for (it = rdata->m_events.begin(); it != rdata ->m_events.end(); ++it)
	{
		ev = new Event(*(it->second));
		addEvent(it->first,ev);
	}
}

void Region::setCutsceneMode(bool mode)
{
	NetEvent event;
	event.m_type = NetEvent::REGION_CUTSCENE;
	event.m_id = getId();
	event.m_data = (int) mode;
	insertNetEvent(event);
	
	m_cutscene_mode = mode;
}


Dialogue* Region::getDialogue(int id)
{
	std::map<int, Dialogue*>::iterator it;
	it = m_dialogues.find(id);
	
	if (it == m_dialogues.end())
		return 0;
	
	return it->second;
}

void Region::insertDialogue(Dialogue* dia)
{
	m_dialogues.insert(std::make_pair(dia->getId(),dia));
	
	if (World::getWorld()->isServer())
	{
		NetEvent event;
		event.m_type = NetEvent::DIALOGUE_CREATED;
		event.m_id = dia->getId();
		insertNetEvent(event);
	}
	DEBUGX("insert Dialogue %i",dia->getId());
}

void Region::deleteDialogue(Dialogue* dia)
{
	DEBUGX("delete Dialogue %i",dia->getId());
	std::map<int, Dialogue*>::iterator it;
	it = m_dialogues.find(dia->getId());
	if (it != m_dialogues.end())
	{
		m_dialogues.erase(it);
	}
	
	if (World::getWorld()->isServer())
	{
		NetEvent event;
		event.m_type = NetEvent::DIALOGUE_DESTROYED;
		event.m_id = dia->getId();
		insertNetEvent(event);
	}
	
	delete dia;
}

void Region::setNamedId(std::string name, int id)
{
	m_name_ids[name] = id;
}

int Region::getIdByName(std::string name)
{
	std::map<std::string,int>::iterator it;
	it = m_name_ids.find(name);
	if (it == m_name_ids.end())
		return 0;
	
	return it->second;
}

void Region::visualizeDamage(int number, Vector position, FloatingText::Size size)
{
	std::stringstream stream;
	stream << number;
	TranslatableString text(stream.str());
	createFloatingText(text, position, size);
}

void Region::createFloatingText(TranslatableString text,  Vector position, FloatingText::Size size, std::string colour, float time, float float_offset)
{
	static int id =0;
	id ++;
	FloatingText* dmgvis = new FloatingText;
	m_floating_texts[id] = dmgvis;
	dmgvis->m_text = text;
	dmgvis->m_position = position;
	dmgvis->m_size = size;
	dmgvis->m_maxtime = time;
	dmgvis->m_time = time;
	dmgvis->m_float_offset = float_offset;
	dmgvis->m_colour = colour;
	
	if (World::getWorld()->isServer())
	{
		NetEvent event;
		event.m_type = NetEvent::DAMAGE_VISUALIZER_CREATED;
		event.m_id = id;
		insertNetEvent(event);
	}
}

void Region::playSound(std::string soundname, Vector position, float volume , bool global)
{
	PlayedSound* sound = new PlayedSound;
	sound->m_soundname = soundname;
	sound->m_position = position;
	sound->m_volume = volume;
	sound->m_global = global;
	
	m_played_sounds.push_back(sound);
}

void Region::addMusicTrack(MusicTrack track)
{
	SW_DEBUG ("Region: adding music track: %s", track.c_str ());
	//
	MusicTrack copiedName = track;
	copiedName = SoundHelper::getNameWithPathForMusicTrack (copiedName);
	m_music_tracks.push_back(copiedName);
	
	// Now also affect the music player. Add the track to the playlist with the name of this region.
	SoundManager::getPtr ()->addPlaylistTrack (getName (), copiedName);

	NetEvent event;
	event.m_type = NetEvent::MUSIC_CHANGED;
	event.m_id = getId();
	insertNetEvent(event);
}

void Region::clearMusicTracks()
{
	m_music_tracks.clear();
	if (World::getWorld()->getLocalPlayer() !=0
		&& World::getWorld()->getLocalPlayer()->getRegion() == this)
	{
		//MusicManager::instance().stop();
		SoundManager::getPtr ()->getMusicPlayer ()->stop ();
	}
	
	NetEvent event;
	event.m_type = NetEvent::MUSIC_CHANGED;
	event.m_id = getId();
	insertNetEvent(event);
}
