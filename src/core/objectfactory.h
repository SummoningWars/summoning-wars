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

#ifndef OBJECTFACTORY_H
#define OBJECTFACTORY_H



#include <map>
#include "fixedbase.h"
#include "monsterbase.h"
#include "playerbase.h"
#include "projectilebase.h"
#include "treasurebase.h"
#include <tinyxml.h>
#include <string>
#include <list>
#include "event.h"



class World;



/**
 * \struct ScriptObjectData
 * \brief Struktur mit Daten von ScriptObjekten
 */
struct ScriptObjectData
{
	/**
	 * \var FixedObjectData m_fixed_data
	 * \brief Basisdaten analog zu FixedObject
	 */
	FixedObjectData m_fixed_data;
	
	/**
	 * \var std::string m_render_info
	 * \brief Name der RenderInfo Daten fuer das Objekt
	 */
	std::string m_render_info;
	
	/**
	 * \var std::multimap<TriggerType, Event*> m_events
	 * \brief Fuer dieses Objekt registrierte Events
	 */
	std::multimap<TriggerType, Event*> m_events;
	
	/**
	 * \var char m_interaction_flags
	 * \brief Flags, die die Interaktion des Objekts mit der Umgebung bestimmen
	 */
	char m_interaction_flags;
	
	/**
	 * \fn ~ScriptObjectData()
	 * \brief Destruktor
	 */
	~ScriptObjectData()
	{
		std::multimap<TriggerType, Event*>::iterator it;
		for (it = m_events.begin(); it != m_events.end(); ++it)
		{
			delete it->second;
		}
	}
};

/**
 * Bezeichnung fuer eine Umgebung
 */
typedef std::string EnvironmentName;

/**
 * Liste von Typbezeichnungen fuer Objekte
 */
typedef std::list<GameObject::Subtype> GameObjectTypeList;
typedef std::list<GameObject::Subtype> WorldObjectTypeList;

/**
 * Name fuer ein Objekt in einem Template
 * kann entweder ein Objektname sein oder ein generischer Name
 * generische Name beginnen mit $
 */
typedef std::string ObjectTemplateType;

/**
 * \class ObjectTemplate
 * \brief Platzhalter fuer ein Objekt. Das Objekt das daraus tatsaechlich erzeugt wird ist von der Umgebung abhaengig
 */
class ObjectTemplate
{
	private:
		/**
	 * \var std::map<EnvironmentName, WorldObjectTypeList > m_env_objects
	 * \brief Gibt fuer jede Umgebung eine Liste von Objekten an, die den Platzhalter ersetzen
		 */
		std::map<EnvironmentName, WorldObjectTypeList > m_env_objects;
		
	
	public:
		
		/**
		 * \var GameObject::Type m_type
		 * \brief Basistyp des Objekts
		 */
		GameObject::Type m_type;
		
		/**
		 * \var EnvironmentName m_default_environment
		 * \brief Umgebung, auf die zurueckgegriffen wird, wenn keine Informationen zu einer Umgebung vorliegen
		 */
		EnvironmentName m_default_environment;
		
		/**
		 * \fn void addObject(EnvironmentName env, GameObject::Subtype object)
		 * \brief Fuegt fuer die Umgebung ein Objekt hinzu
		 * \param env Name der Umgebung
		 * \param object Typ des Objekts
		 */
		void addObject(EnvironmentName env, GameObject::Subtype object)
		{
			m_env_objects[env].push_back(object);
		}
		
		/**
		 * \fn void addEnvironment(EnvironmentName env)
		 * \brief Fuegt eine Umgebung ein
		 * \param env Name der Umgebung
		 */
		void addEnvironment(EnvironmentName env)
		{
			m_env_objects[env];
		}
		
		/**
		 * \fn GameObject::Subtype getObject(EnvironmentName env)
		 * \brief Gibt fuer eine gegebene Umgebung ein passendes Objekt aus
		 * \param env Umgebung
		 */
		GameObject::Subtype getObject(EnvironmentName env);
};


/**
 * Name einer Gruppe von Objekten
 */
typedef std::string ObjectGroupName;

/**
 * Name eines Templates einer Gruppe von Objekten
 */
typedef std::string ObjectGroupTemplateName;

/**
 * \class ObjectGroupTemplate
 * \brief Platzhalter fuer eine Gruppe von Objekten. Die Objektgruppe die daraus tatsaechlich erzeugt wird ist von der Umgebung abhaengig
 */
class ObjectGroupTemplate
{
	private:
		/**
		 * \var std::map<EnvironmentName, std::list<ObjectGroupName > > m_env_object_groups
		 * \brief Gibt fuer jede Umgebung eine Liste von Objektgruppen an, die den Platzhalter ersetzen
		 */
		std::map<EnvironmentName, std::list<ObjectGroupName > > m_env_object_groups;
		
	
	public:
		
		/**
		 * \var Shape m_shape
		 * \brief Grundlegende Form des Templates. Sollte mit der Form aller ersetztenden Gruppen uebereinstimmen
		 */
		Shape m_shape;
		
		/**
		 * \var EnvironmentName m_default_environment
		 * \brief Umgebung, auf die zurueckgegriffen wird, wenn keine Informationen zu einer Umgebung vorliegen
		 */
		EnvironmentName m_default_environment;
		
		/**
		 * \fn void addObjectGroup(EnvironmentName env, ObjectGroupName object)
		 * \brief Fuegt fuer die Umgebung ein Objekt hinzu
		 * \param env Name der Umgebung
		 * \param object Typ des Objekts
		 */
		void addObjectGroup(EnvironmentName env, ObjectGroupName object)
		{
			m_env_object_groups[env].push_back(object);
		}
		
		/**
		 * \fn void addEnvironment(EnvironmentName env)
		 * \brief Fuegt eine Umgebung ein
		 * \param env Name der Umgebung
		 */
		void addEnvironment(EnvironmentName env)
		{
			m_env_object_groups[env];
		}
		
		/**
		 * \fn ObjectGroupName getObjectGroup(EnvironmentName env)
		 * \brief Gibt fuer eine gegebene Umgebung eine passende Objektgruppe aus
		 * \param env Umgebung
		 */
		ObjectGroupName getObjectGroup(EnvironmentName env);
};



/**
 * Name eines Ortes
 */
typedef std::string LocationName;

/**
 * Name eines Gebietes
 */
typedef std::string AreaName;

/**
 * \class ObjectGroup
 * \brief Gruppe von Objekten in einer vorgegebenen Anordnung
 */
class ObjectGroup
{
	public:
		/**
	 * \struct GroupObject
	 * \brief Objekt das Bestandteil der Gruppe ist
		 */
		struct GroupObject
		{
			/**
			 * \var ObjectTemplateType m_type
			 * \brief Typ des Objektes. Beginnt mit $ fuer generische Objekte
			 */
			ObjectTemplateType m_type;
			
			/**
			 * \var std::string m_name
			 * \brief Name des Objektes. Der Name wird nur registriert, wenn er nicht leer ist
			 */
			std::string m_name;
			
			/**
			 * \var Vector m_center
			 * \brief Ortsvektor des Mittelpunktes relativ zum Mittelpunkt des Templates
			 */
			Vector m_center;
			
			/**
			 * \var float m_angle
			 * \brief Drehwinkel
			 */
			float m_angle;
			
			/**
			 * \var float m_probability
			 * \brief Chance, dass das Objekt eingesetzt wird
			 */
			float m_probability;
			
			/**
			 * \var bool m_prob_angle
			 * \brief Wenn auf true gesetzt, wird der winkel zufaellig gesetzt
			 */
			bool m_prob_angle;
			
			/**
			 * \var float m_height
			 * \brief Hoehe, in der das Objekt platziert wird
			 */
			float m_height;
		};
	
		/**
		 * \fn Shape* getShape()
		 * \brief Gibt die Form aus
		 */
		Shape* getShape()
		{
			return &m_shape;
		}
		
		/**
		 * \fn std::list<GroupObject>* getObjects()
		 * \brief Gibt die Objekte aus
		 */
		std::list<GroupObject>* getObjects()
		{
			return &m_objects;
		}
		
		/**
		 * \fn std::map<std::string, Vector>* getLocations()
		 * \brief Gibt die Orte aus
		 */
		std::map<std::string, Vector>* getLocations()
		{
			return &m_locations;
		}
		
		/**
		 * \fn void addLocation(LocationName name, Vector pos)
		 * \brief Fuegt einen benannten Ort in das Template ein
		 * \param name Name des Ortes
		 * \param pos Position relativ zum Mittelpunkt
		 */
		void addLocation(LocationName name, Vector pos)
		{
			m_locations[name] = pos;
		}
		
		/**
		 * \fn std::map<AreaName, Shape>& getAreas()
		 * \brief Gibt die Struktur mit den Gebieten aus
		 */
		std::map<AreaName, Shape>& getAreas()
		{
			return m_areas;
		}
		
		/**
		 * \fn void addArea(AreaName name, Shape area)
		 * \brief Fuegt eine neue Flaeche ein
		 */
		void addArea(AreaName name, Shape area)
		{
			m_areas.insert(std::make_pair(name,area));	
		}
		
		/**
		 * \fn void addWaypoint(Vector pos)
		 * \brief Fuegt einen Wegpunkt in das Template ein
		 * \param pos Position
		 */
		void addWaypoint(Vector pos)
		{
			m_waypoint = pos;
		}
		
		/**
		 * \fn void addObject(GroupObject gobj)
		 * \brief Fuegt ein Objekt dem Template hinzu
		 * \param gobj Objekt
		 */
		void addObject(GroupObject gobj)
		{
			m_objects.push_back(gobj);
		}
		
		/**
		 * \fn void addObject(ObjectTemplateType objtype, Vector pos, float angle=0, float probability = 1.0)
		 * \brief Fuegt ein neues Objekt in die Gruppe ein
		 * \param objtype Typ des eingefuegten Objekts
		 * \param pos Position relativ zum Mittelpunkt
		 * \param angle Drehwinkel
		 * \param m_probability Wahrscheinlichkeit, dass das Objekt vertreten ist
		 */
		void addObject(ObjectTemplateType objtype, Vector pos, float angle=0, float probability =1.0);
	
	private:
		/**
	 * \var Shape m_shape
	 * \brief Form der Gruppe
		 */
		Shape m_shape;
		
		/**
		 * \var Vector m_waypoint
		 * \brief der Punkt an dem das Template an das Wegenetz angebunden wird
		 */
		Vector m_waypoint;
		
		/**
		 * \var std::list<GroupObject> m_objects
		 * \brief Liste der Objekte in der Gruppe
		 */
		std::list<GroupObject> m_objects;
		
		/**
		 * \var std::map<LocationName, Vector> m_locations
		 * \brief Orte mit Namen
		 */
		std::map<LocationName, Vector> m_locations;
		
		/**
		 * \var std::map<AreaName, Shape> m_areas
		 * \brief Orte mit Namen
		 */
		std::map<AreaName, Shape> m_areas;
};




/**
 * \class ObjectFactory
 * \brief Erzeugt Objekte vom Typ WorldObject
 */
class ObjectFactory
{
	public:
		
	/**
	 * \fn static WorldObject* createObject(GameObject::Type type, GameObject::Subtype subtype, int id=0)
	 * \brief Erzeugt ein Objekt des angefordertens Typs/Subtyps
	 * \param type Typ des Objektes
	 * \param subtype Subtyp des Objektes
	 * \param id ID des Objektes. Wenn id==0 wird eine neue ID generiert
	 */
	static WorldObject* createObject(GameObject::Type type, GameObject::Subtype subtype, int id=0);
	
	/**
	 * \fn static Projectile* createProjectile(GameObject::Subtype subtype, int id=0)
	 * \brief erzeugt ein Projektil des geforderten Subtypes
	 * \param subtype Subtype des Objekts
	 * \param id ID des Objektes. Wenn id==0 wird eine neue ID generiert
	 */
	static Projectile* createProjectile(GameObject::Subtype subtype, int id=0);
	
	/**
	 * \fn static GameObject::Subtype getObjectType(ObjectTemplateType generictype, EnvironmentName env)
	 * \brief erzeugt aus einem ObjektTemplate einen Objekttyp passend zu seiner Umgebung
	 * \param generictype generischer Typ des Objekts
	 * \param env Umgebung
	 */
	static GameObject::Subtype getObjectType(ObjectTemplateType generictype, EnvironmentName env);
	
	/**
	 * \fn static ObjectGroupName getObjectType(ObjectGroupTemplateName generictype, EnvironmentName env)
	 * \brief erzeugt aus einem ObjektGroupTemplate eine Objektgruppe passend zu seiner Umgebung
	 * \param generictype generischer Typ
	 * \param env Umgebung
	 */
	static ObjectGroupName getObjectGroupType(ObjectGroupTemplateName generictype, EnvironmentName env);
	
	/**
	 * \fn static Shape getObjectGroupShape(ObjectGroupTemplateName generictype)
	 * \brief Gibt die Groesse einer Objektgruppe aus
	 * \param generictype generischer Typ
	 */
	static Shape getObjectGroupShape(ObjectGroupTemplateName generictype);
	
	/**
	 * \fn static ObjectGroup* getObjectGroup(ObjectGroupName name)
	 * \brief sucht zu dem betreffenden Name das passende Template heraus
	 * \param name Name der Objektgruppe
	 */
	static ObjectGroup* getObjectGroup(ObjectGroupName name);
	
	/**
	 * \fn static MonsterGroup* getMonsterGroup(MonsterGroupName name)
	 * \brief Gibt die Monstergruppe zu dem angegebenen Name aus
	 */
	static MonsterGroup* getMonsterGroup(MonsterGroupName name);
	
	/**
	 * \fn static GameObject::Type getObjectBaseType(GameObject::Subtype subtype)
	 * \brief Gibt den Basistyp des Objekts aus
	 * \param subtype Subtyp des Objekts
	 */
	static GameObject::Type getObjectBaseType(GameObject::Subtype subtype);
	
	/**
	 * \brief Gibt den Name eines Objektes aus
	 */
	static TranslatableString getObjectName(GameObject::Subtype subtype);
	
	/**
	 * \fn static PlayerBasicData* getPlayerData(GameObject::Subtype subtype)
	 * \brief Gibt zu dem angegebenen Spielertyp die Daten fuer die Initialisierung aus. Gibt 0 aus, wenn der Spielertyp nicht registriert ist.
	 * \param subtype Spielertyp
	 */
	static PlayerBasicData* getPlayerData(GameObject::Subtype subtype);
	
	/**
	 * \fn static ProjectileBasicData* getProjectileData(GameObject::Subtype subtype)
	 * \brief Gibt zu angegebenen Typ die Daten eines Projectils aus
	 * \param subtype Subtype des Projektils
	 */
	static ProjectileBasicData* getProjectileData(GameObject::Subtype subtype);
	
	/**
	 * \fn static registerMonster(GameObject::Subtype subtype, MonsterBasicData* data)
	 * \brief Registriert die Daten fuer ein bestimmten Monstertyp
	 * \param subtype Subtyp des Monsters
	 * \param data Daten des Monsters
	 * \param silent_replace if set to true, the data is replaced without error message.
	 */
	static void registerMonster(GameObject::Subtype subtype, MonsterBasicData* data,  bool silent_replace = false);
	
	/**
	 * \fn static registerPlayer(GameObject::Subtype subtype, PlayerBasicData* data)
	 * \brief Registriert die Daten fuer ein bestimmten Spielertyp
	 * \param subtype Subtyp der Spielerklasse
	 * \param data Daten der Spielerklasse
	 */
	static void registerPlayer(GameObject::Subtype subtype, PlayerBasicData* data);
	
	/**
	 * \brief Registriert die Daten fuer ein bestimmten Monstertyp
	 * \param subtype Subtyp des Objektes
	 * \param data Daten des festen Objekts
	 * \param silent_replace if set to true, the data is replaced without error message.
	 */
	static void registerFixedObject(GameObject::Subtype subtype, FixedObjectData* data,  bool silent_replace = false);
	
	/**
	 * \fn static void registerScriptObject(GameObject::Subtype subtype, ScriptObjectData* data)
	 * \brief Registriert Daten fuer einen Typ scriptbare Objekte
	 * \param subtype Subtyp des Objektes
	 * \param data Daten des  Objekts
	 */
	static void registerScriptObject(GameObject::Subtype subtype, ScriptObjectData* data);
	
	/**
	 * \fn static void registerProjectile(GameObject::Subtype subtype, ProjectileBasicData* data)
	 * \brief Registriert die Daten fuer ein Projectil
	 * \param subtype Subtyp des Objektes
	 * \param data Daten des festen Projectils
	 */
	static void registerProjectile(GameObject::Subtype subtype, ProjectileBasicData* data);
	
	/**
	 * \fn static void registerTreasure(GameObject::Subtype subtype, TreasureBasicData* data)
	 * \brief Registriert die Daten fuer ein Projectil
	 * \param subtype Subtyp des Objektes
	 * \param data Daten des festen Projectils
	 */
	static void registerTreasure(GameObject::Subtype subtype, TreasureBasicData* data);
	
	/**
	 * \fn static registerObjectTemplate(ObjectTemplateType type, ObjectTemplate* templ)
	 * \brief Registriert ein neues Objekttemplate
	 * \param type Typ des Templates
	 * \param templ die Daten
	 */
	static void registerObjectTemplate(ObjectTemplateType type, ObjectTemplate* templ);
	
	
	/**
	 * \fn static registerObjectGroup(ObjectGroupName name, ObjectGroup* data)
	 * \brief Registriert ein neues Template fuer eine Objektgruppe
	 * \param name der Name des Templates
	 * \param data die Daten
	 */
	static void registerObjectGroup(ObjectGroupName name, ObjectGroup* data);
	
	/**
	 * \fn static registerObjectGroupTemplate(ObjectGroupTemplateName type, ObjectGroupTemplate* templ)
	 * \brief Registriert ein neues ObjektGrouptemplate
	 * \param type Typ des Templates
	 * \param templ die Daten
	 */
	static void registerObjectGroupTemplate(ObjectGroupTemplateName type, ObjectGroupTemplate* templ);
	
	/**
	 * \fn static void registerMonsterGroup(MonsterGroupName name, MonsterGroup data)
	 * \brief Registriert eine Gruppe von Monstern
	 * \param name Name der Gruppe
	 * \param group Daten
	 */
	static void registerMonsterGroup(MonsterGroupName name, MonsterGroup* data);
	
	/**
	 * \fn static void registerEmotionSet(std::string name, EmotionSet* set)
	 * \brief Registriert ein neues EmotionSet
	 * \param name Name
	 * \param set Daten
	 */
	static void registerEmotionSet(std::string name, EmotionSet* set);
	
	/**
	 * \fn static void registerPlayerLook(GameObject::Subtype subtype, PlayerLook look)
	 * \brief Registiert ein fuer einen Spielertyp zulaessiges Aussehen
	 * \param subtype Spielertyp
	 * \param look Aussehen
	 * \param male Gibt Geschlecht an
	 */
	static void registerPlayerLook(GameObject::Subtype subtype, PlayerLook look);
	
	/**
	 * \fn static EmotionSet* getEmotionSet(std::string name)
	 * \brief Gibt das Emotionset mit dem angegebenen Name aus
	 * \param name Name
	 */
	static EmotionSet* getEmotionSet(std::string name);
	
	/**
	 * \fn static init()
	 * \brief registriert die Daten der Monster
	 */
	static void init();
	
	/**
	 * \brief Clears up the data loaded by the World constructor
	 * \param bitmask Specifies which data to delete, see also \ref World::DataGroups
	 */
	static void cleanup(int bitmask = 0xfffffff);
	
	/**
	 * \brief Loescht die Daten fuer Monster, FixedObject, Projectile, Scriptobject
	 * \param bitmask Specifies which data to delete, see also \ref World::DataGroups
	 */
	static void cleanupObjectData(int bitmask = 0xfffffff);
	
	/**
	 * \fn static std::map<GameObject::Subtype, PlayerBasicData*>& getPlayerData()
	 * \brief Gibt Daten zu allen registrierten Spielern aus
	 */
	static std::map<GameObject::Subtype, PlayerBasicData*>& getPlayerData()
	{
		return m_player_data;
	}
	
	/**
	 * \fn static void getPlayerLooks(GameObject::Subtype subtype, std::list< std::pair<bool, PlayerLook> > &looks)
	 * \brief Gibt fuer eine gegebene Spielerklasse alle Moeglichkeiten des Aussehens aus
	 * \param subtype Spielerklasse
	 * \param looks Ausgabe: Vektor mit allen Varianten
	 */
	static void getPlayerLooks(GameObject::Subtype subtype, std::list< PlayerLook > &looks);
	
	/**
	 * \fn static PlayerLook* getPlayerLook(GameObject::Subtype subtype, std::string name)
	 * \brief Gibt zu dem angegebenen Spielertyp und Lookname die Informationen zum Aussehen aus
	 */
	static PlayerLook* getPlayerLook(GameObject::Subtype subtype, std::string name);
	
	/**
	 * \brief returns the base data for a FixedObject
	 * \param subtype type of the object
	 */
	static FixedObjectData* getFixedObjectData(GameObject::Subtype subtype);
	
	/**
	 * \brief returns the data structure with all registered fixedObjects
	 */
	static const std::map<GameObject::Subtype, FixedObjectData*>& getAllFixedObjectData()
	{
		return m_fixed_object_data;
	}
	
	/**
	 * \brief returns the base data for a FixedObject
	 * \param subtype type of the object
	 */
	static MonsterBasicData* getMonsterData(GameObject::Subtype subtype);
	
	/**
	 * \brief returns the data structure with all registered fixedObjects
	 */
	static const std::map<GameObject::Subtype, MonsterBasicData*>& getAllMonsterData()
	{
		return m_monster_data;
	}
	
	private:	
	/**
	 * \var static std::map<GameObject::Subtype, MonsterBasicData*> m_monster_data
	 * \brief Basisdaten zu den Monstern
	 */
	static std::map<GameObject::Subtype, MonsterBasicData*> m_monster_data;
	
	/**
	 * \var static std::map<GameObject::Subtype, FixedObjectData*> m_fixed_object_data
	 * \brief Basisdaten zu den festen Objekten
	 */
	static std::map<GameObject::Subtype, FixedObjectData*> m_fixed_object_data;
	
	/**
	 * \var static std::map<GameObject::Subtype, ScriptObjectData*> m_script_object_data
	 * \brief Basisdaten zu ScriptObjekten
	 */
	static std::map<GameObject::Subtype, ScriptObjectData*> m_script_object_data;
	
	/**
	 * \var static std::map<GameObject::Subtype, ProjectileBasicData*> m_projectile_data
	 * \brief Basisdaten zu Geschossen
	 */
	static std::map<GameObject::Subtype, ProjectileBasicData*> m_projectile_data;
	
	/**
	 * \var static std::map<GameObject::Subtype, TreasureBasicData*> m_treasure_data
	 * \brief Basisdaten zu Schaetzen
	*/
	static std::map<GameObject::Subtype, TreasureBasicData*> m_treasure_data;
	
	/**
	 * \var static std::map<GameObject::Subtype, PlayerBasicData*> m_player_data
	 * \brief Basisdaten zu Spielerklassen
	 */
	static std::map<GameObject::Subtype, PlayerBasicData*> m_player_data;
	
	/**
	 * \var static std::map<ObjectTemplateType, ObjectTemplate*> m_object_templates
	 * \brief Datenbank fuer die Objekttemplates, indexiert nach Typ
	 */
	static std::map<ObjectTemplateType, ObjectTemplate*> m_object_templates;
	
	/**
	 * \var static std::map<ObjectGroupTemplateName, ObjectGroupTemplate*> m_object_group_templates
	 * \brief Datenbank fuer die Objekttemplates, indexiert nach Typ
	 */
	static std::map<ObjectGroupTemplateName, ObjectGroupTemplate*> m_object_group_templates;
	
	/**
	 * \var static std::map<ObjectGroupName, ObjectGroup*> m_object_groups
	 * \brief Datenbank fuer die Objektgruppen indexiert nach Name
	 */
	static std::map<ObjectGroupName, ObjectGroup*> m_object_groups;
	
	/**
	 * \var static std::map< MonsterGroupName, MonsterGroup*>  m_monster_groups
	 * \brief Liste von Monstern die von Spawnpoints erzeugt werden
	 */
	static std::map< MonsterGroupName, MonsterGroup*>  m_monster_groups;
	
	/**
	 * \var static std::map<GameObject::Subtype, GameObject::Type> m_object_types
	 * \brief Typen der Objekte
	 */
	static std::map<GameObject::Subtype, GameObject::Type> m_object_types;
	
	
	/**
	 * \var static std::map<std::string, EmotionSet*> m_emotion_sets
	 * \brief Alle Saetze von Emotionsbildern
	 */
	static std::map<std::string, EmotionSet*> m_emotion_sets;
	
	/**
	 * \var static std::multimap< GameObject::Subtype,  PlayerLook > m_player_look
	 * \brief Speichert, fuer welche Spielerklassen welches Aussehen erlaubt ist
	 */
	static std::multimap< GameObject::Subtype, PlayerLook > m_player_look;
	
};



#endif

