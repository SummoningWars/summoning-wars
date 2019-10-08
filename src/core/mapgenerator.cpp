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

#include "mapgenerator.h"
#include "world.h"


bool operator<(WeightedLocation first, WeightedLocation second)
{
	return first.m_value< second.m_value;
}

TemplateMap::TemplateMap(int dimx, int dimy)
	: m_template_map(dimx,dimy) , m_template_index_map(dimx,dimy)
{
	m_dimx = dimx;
	m_dimy = dimy;
}

void TemplateMap::init(Matrix2d<char>* base_map)
{
	// Dimension der Basiskarte
	int bdimx = m_dimx/2;
	int bdimy = m_dimy/2;
	
	m_template_map.clear();
	m_template_index_map.clear();
	int val;
	
	// 8x8 Feld Informationen aus der Basiskarte in
	// 4x4 Karte uebertragen
	for (int i=0; i<bdimx; i++)
	{
		for (int j=0; j<bdimy; j++)
		{
			if ((*base_map)[i][j] <= 0)
			{
				val = -1;
			}
			else
			{
				val =0;
			}
			
			m_template_map[2*i][2*j] = val;
			m_template_map[2*i][2*j+1] = val;
			m_template_map[2*i+1][2*j+1] = val;
			m_template_map[2*i+1][2*j] = val;
		}
	}
	
	// unterer und linker Rand mit -1 fuellen
	for (int i=0; i<m_dimx; i++)
	{
		m_template_map[i][m_dimy-1] = -1;
	}
	for (int j=0; j<m_dimy; j++)
	{
		m_template_map[m_dimx-1][j] = -1;
	}
	
	// restliche Karte berechnen
	for (int i=m_dimx-2; i>=0; i--)
	{
		for (int j=m_dimy-2; j>=0; j--)
		{
			recalcMapElement(i,j);
		}
	}
}

void TemplateMap::recalcMapElement(int i,int j)
{
	int val;
	if (i >= m_dimx || j >= m_dimy)
		return;
	
	if (i== m_dimx-1 || j == m_dimy-1)
	{
		val = -1;	// Rand immer blockiert
	}
	else if (m_template_map[i][j] == -1)
	{
		val = -1;	// blockiert bleibt blockiert
	}
	else
	{
		// Minimum aus den Felden darunter, daneben, diagonal darunter; plus 1
		val = m_template_map[i+1][j];
		val = MathHelper::Min(val, m_template_map[i][j+1]);
		val = MathHelper::Min(val, m_template_map[i+1][j+1]);
		val ++;
		
		if (val == 0)
			val = 1;
	}
	
	if (val != -1)
	{
		setMapElement(i,j,val);
	}
}

void TemplateMap::setMapElement(int i, int j, int template_max_size)
{
	int oldval = m_template_map[i][j];
	
	if (oldval == template_max_size || oldval == -1)
		return;
	
	// aus Datenstrukturen entfernen falls alter Wert > 0
	if (oldval > 0)
	{
		// geloescht wird, indem das letzte Element des Vektors
		// ueber das zu loeschende kopiert wird
		int oldindex = m_template_index_map[i][j];
		int vecsize = m_template_places[oldval].size();
		if (oldindex != vecsize -1)
		{
			int lastx = m_template_places[oldval][vecsize-1].first;
			int lasty = m_template_places[oldval][vecsize-1].second;
			m_template_places[oldval][oldindex].first = lastx;
			m_template_places[oldval][oldindex].second = lasty;
			m_template_index_map[lastx][lasty] = oldindex;
		}
		
		m_template_places[oldval].resize(vecsize-1);
	}
	
	m_template_map[i][j] = template_max_size;
	
	// Eintragen des neuen Wertes in die Datenstrukturen
	if (template_max_size > 0)
	{
		int vecsize = m_template_places[template_max_size].size();
		m_template_places[template_max_size].resize(vecsize +1);
		m_template_places[template_max_size][vecsize].first = i;
		m_template_places[template_max_size][vecsize].second = j;
		m_template_index_map[i][j] = vecsize;
	}
	
}

bool TemplateMap::getTemplatePlace(Shape* shape, Vector & place)
{
	if (shape ==0)
	{
		return false;
	}

	bool success = false;

	// Groesse des Templates in 4x4 Feldern
	Vector ext = shape->getAxisExtent();
	int ex = int(ceil(ext.m_x/2));
	int ey = int(ceil(ext.m_y/2));
	
	int size = MathHelper::Max(ex,ey);
	DEBUGX("template size %i %i angle %f extent %f %f",ex,ey,shape->m_angle,shape->m_extent.m_x, shape->m_extent.m_y);
	std::map< int, std::vector< std::pair<int, int> > >::iterator mt, mt2;
	std::vector<int>::iterator st;
	
	int px,py;
	
	while (!success)
	{
		int nr;
		// Anzahl der in Frage kommenden Orte ermitteln
		// alle Orte aufsummieren mit Abstandsindex mindens size
		nr =0;
		mt2 = mt = m_template_places.lower_bound(size);

		while (mt != m_template_places.end())
		{
			nr += mt->second.size();
			++mt;
		}

		if (nr == 0)
		{
			// kein Platz fuer das Template vorhanden
			return false;
		}
		
		// Ort auswuerfeln und ermitteln
		nr = Random::randi(nr);
		mt = mt2;
		while (nr >= int(mt->second.size()))
		{
			nr -= mt->second.size();
			++mt;
		}
		
		px = mt->second[nr].first;
		py = mt->second[nr].second;
		
		// Flaeche die durch das Template ueberdeckt wird bestimmen
		int imin = px;
		int jmin = py;
		int imax = imin + ex -1;
		int jmax = jmin + ey -1;
		// Ort eines eventuellen Hindernisses
		int obi = -1, obj= -1;

		// Pruefen ob die Flaeche keine Hindernisse enthaelt
		for (int i = imin; i<= imax; i++)
		{
			for (int j= jmin; j<= jmax; j++)
			{
				if (m_template_map[i][j] <0)
				{
					obi = i;
					obj = j;
					DEBUGX("found obstacle at %i %i",i,j);
					break;
				}
			}
			if (obi != -1)
			{
				break;
			}
		}
		
		if (obi != -1)
		{
			// Hindernis gefunden

			// dieses Hindernis verhindert das platzieren der Templates in seiner Umgebung
			// in dieser Umgebung werden die Zahlen fuer den minimalen Abstand zum naechsten Hindernis aktualisiert
			imin = MathHelper::Max(0,obi - size+1);
			jmin = MathHelper::Max(0,obj - size+1);
			imax = obi;
			jmax = obj;

			// Schleife ueber die Umgebung des Hindernisses
			for (int i = imax; i>= imin; i--)
			{
				for (int j= jmax; j>= jmin; j--)
				{
					recalcMapElement(i,j);
				}
			}
		}
		else
		{
			// Erfolg
			// Orte die von dem Template ueberdeckt werden aus der Liste der freien Orte entfernen

			// Objekt auf der Template Karte eintragen
			for (int i = imin; i<= imax; i++)
			{
				for (int j= jmin; j<= jmax; j++)
				{
					setMapElement(i,j,-1);
				}
			}
			
			place.m_x = px*4+2*ex;
			place.m_y = py*4+2*ey;

			DEBUGX("found place %i %i",px,py);

			success = true;
		}
	}
	return true;
}

void TemplateMap::print()
{
	for (int i=0; i< m_dimx; i++)
	{
		for (int j=0; j<m_dimy; j++)
		{
			if (m_template_map[i][j]>0)
				std::cout << m_template_map[i][j] << " ";
			else
				std::cout << "# ";
		}
		std::cout << "\n";
	}
	std::cout << "\n";
	
	std::map< int, std::vector< std::pair<int, int> > >::iterator it;
	for (it = m_template_places.begin(); it != m_template_places.end(); ++it)
	{
		DEBUG("places of size %i : %i",it->first, it->second.size());
	}
}

Region* MapGenerator::createRegion(RegionData* rdata)
{
	// temporaere Daten fuer die Erzeugung der Region
	MapGenerator::MapData mdata;

	bool success = false;
	int counter =0;
	while (!success)
	{
		counter ++;
		if (counter > 10)
			return 0;
		
		// Speicher anfordern
		MapGenerator::createMapData(&mdata,rdata);
		

		// grundlegende Karte anfertigen
		if (rdata->m_region_template =="")
		{
			MapGenerator::createBaseMap(&mdata,rdata);
		}
		
		// Umgebungen in die Region einfuegen
		std::list<std::pair<float, EnvironmentName> >::iterator et;
		for (et = rdata->m_environments.begin(); et != rdata->m_environments.end(); ++et)
		{
			mdata.m_region->insertEnvironment(et->first,et->second);
		}
		
		// Umgebungskarte generieren
		createPerlinNoise(mdata.m_region->getHeight(), rdata->m_dimx, rdata->m_dimy,MathHelper::Min(rdata->m_dimx,rdata->m_dimy)/4 , 0.4,false);

		if (rdata->m_region_template =="")
		{
			// Objektgruppen platzieren
			success = MapGenerator::insertGroupTemplates(&mdata,rdata);
		}
		else
		{
			DEBUGX("region template %s",rdata->m_region_template.c_str());
			// Region besteht aus einer einzelnen Objektgruppe
			// Objektgruppe anhand des Namens suchen
			ObjectGroup* templ;
			templ = ObjectFactory::getObjectGroup(rdata->m_region_template);
			if (templ ==0)
			{
				ERRORMSG("unknown object group %s",rdata->m_region_template.c_str());
				return 0;
			}

			Vector pos(rdata->m_dimx*2, rdata->m_dimy*2);
			mdata.m_region->createObjectGroup(rdata->m_region_template,pos,0);
			success = true;
		}
		
		// Events kopieren
		mdata.m_region->copyEventsFromRegionData(rdata);
		
		mdata.m_region->setReviveLocation(rdata->m_revive_location);
		mdata.m_region->setGroundMaterial(rdata->m_ground_material);
		
		// Wenn der Versuch nicht erfolgreich war alles loeschen und von vorn beginnen
		if (!success)
		{
			DEBUG("not successful");

			delete mdata.m_base_map;
			delete mdata.m_region;
		}
	}

	if (rdata->m_region_template =="")
	{
		// Berandungen einfuegen
		MapGenerator::createBorder(&mdata,rdata);
	}
	
	// Ausgaenge erzeugen
	MapGenerator::createExits(&mdata,rdata);
	
	if (rdata->m_region_template =="")
	{
		// Monster einfuegen
		MapGenerator::insertSpawnpoints(&mdata,rdata);
	}

	// Speicher freigeben
	delete mdata.m_base_map;
	return mdata.m_region;
}

void MapGenerator::createMapData(MapData* mdata, RegionData* rdata)
{
	mdata->m_base_map = new Matrix2d<char>(rdata->m_dimx/2,rdata->m_dimy/2);
	mdata->m_base_map->clear();
	mdata->m_region = new Region(rdata->m_dimx,rdata->m_dimy,rdata->m_id,rdata->m_name,rdata);
	mdata->m_border.clear();
}

void MapGenerator::createBaseMap(MapData* mdata, RegionData* rdata)
{

	

	float size = rdata->m_area_percent;

	// Karte wird in 8x8 Felder erzeugt, Region rechnet aber in 4x4 Gridunits
	int dimx = rdata->m_dimx/2;
	int dimy = rdata->m_dimy/2;

	// temporaere Karte anlegen
	Matrix2d<float>* hmap = new Matrix2d<float>(dimx, dimy);

	hmap->clear();

	// Karte erzeugen mit Perlin Noise
	// erhaltenes Feld wird als Hoehenkarte interpretiert
	// alle Felder die unterhalb eines kritischen Niveaus liegen sind begehbar
	createPerlinNoise(hmap,dimx, dimy, rdata->m_granularity/2, rdata->m_complexity,true);


	// Delta zu Nachbarfeldern
	int nb[4][2] = {{-1,0},{1,0},{0,-1},{0,1}};
	int dnb[8][2] = {{-1,0},{1,0},{0,-1},{0,1},{-1,1},{1,1},{1,-1},{-1,-1}};

	// Suchen eine Niveaus das eine hinreichend grosse zusammenhaengende Flaeche erzeugt
	// Idee: alle Felder werden in einer PriorityQueue eingeordnet
	// alle Felder werden in einen UnionFind Baum eingeordnet
	// von kleinster Höhe beginnend werden die Felder jeweils mit dem tiefstliegenden Nachbarfeld verschmolzen

	std::priority_queue<WeightedLocation> fields_queue;
	WeightedLocation loc;
	for (int i=0; i< dimx; i++)
	{
		for (int j=0; j< dimy; j++)
		{
			loc.m_x = i;
			loc.m_y = j;
			loc.m_value = 1- (*hmap)[i][j];

			fields_queue.push(loc);
		}
	}

	UnionFindTree uftree(dimx*dimy);

	float height;
	int x,y,nbx, nby;

	// Solange *Wasserstand* erhoehen, bis eine genuegend grosse zusammenhaengende Flaeche gefunden wurde
	while (1)
	{
		// oberstes Feld aus der Queue nehmen
		loc = fields_queue.top();
		fields_queue.pop();

		height = 1-loc.m_value;
		x = loc.m_x;
		y = loc.m_y;

		// Schleife ueber die Nachbarfelder
		for (int i=0; i<4; i++)
		{
			nbx = x + nb[i][0];
			nby = y + nb[i][1];

			// Dimension pruefen
			if (nbx<0 || nby<0 || nbx >= dimx || nby >= dimy)
				continue;

			if ( (*hmap)[nbx][nby] <= height)
			{
				uftree.merge(nbx*dimy+nby,x*dimy+y);
			}
		}

		// Testen ob eine hinreichend große Flaeche entstanden ist
		if (uftree.getCardinality(x*dimy+y) >= dimx*dimy*size)
		{
			break;
		}

	}

	// kleiner Aufschlag um sehr enge Durchgaenge zu vermeiden
	height += 0.01;
	
	/*
	// Debugging
	DEBUG("height %f",height);
	for (int j=0; j< dimy; j++)
	{
		for (int i=0; i< dimx; i++)
		{
			if ((*hmap)[i][j]<height)
			{
				std::cout << " ";
			}
			else
			{
				std::cout << "X";
			}
		}
		std::cout << "\n";
	}
	*/
	
	// Floodfill um die Flaeche und deren Rand zu finden
	// die besuchten Felder werden in border mit 1 markiert
	// es werden nur Felder besucht, die unterhalb der gefundenen Hoehe liegen
	int cnt=0;
	std::queue<std::pair<int,int> > qu;
	std::pair<int,int> point;

	// Queue mit Felder die zum Rand gehoeren
	std::queue<std::pair<int,int> > borderqu;

	qu.push(std::make_pair(x,y));
	(*(mdata->m_base_map))[x][y]=1;

	// Solange die Queue nicht leer ist

	while (!qu.empty())
	{
		// Feld entnehmen
		point = qu.front();
		qu.pop();

		x = point.first;
		y = point.second;

		// Schleife ueber die Nachbarfelder
		for (int i=0; i<4; i++)
		{
			nbx = x + nb[i][0];
			nby = y + nb[i][1];

			// Dimension pruefen
			if (nbx<0 || nby<0 || nbx >= dimx || nby >= dimy)
				continue;

			// nur noch nicht besuchte Felder
			if ((*(mdata->m_base_map))[nbx][nby] != 0)
				continue;

			// Felder oberhalb der Obergrenze zum Rand hinzufuegen
			if ((*hmap)[nbx][nby] > height)
			{
				borderqu.push(std::make_pair(nbx,nby));
				(*(mdata->m_base_map))[nbx][nby]=-1;

				mdata->m_border.push_back(std::make_pair(nbx,nby));
				continue;
			}

			// Feld hinzufuegen
			qu.push(std::make_pair(nbx,nby));
			// als besucht markieren
			(*(mdata->m_base_map))[nbx][nby]=1;

			cnt++;
		}
	}
	
	//std::list<std::pair<int,int> >::iterator it;
	//for (
	
	
	// nochmal 3 Runden Floodfill um den Rand zu ermitteln

	// Marker einfuegen
	int markercnt =0;
	borderqu.push(std::make_pair(-1,-1));


	while (!borderqu.empty() && markercnt <3)
	{
		// Feld entnehmen
		point = borderqu.front();
		borderqu.pop();

		x = point.first;
		y = point.second;

		// Testen ob der Marker entnommen wurde
		if (x==-1)
		{
			markercnt ++;
			borderqu.push(std::make_pair(-1,-1));
			continue;
		}

		// Schleife ueber die Nachbarfelder
		for (int i=0; i<8; i++)
		{
			nbx = x + dnb[i][0];
			nby = y + dnb[i][1];

			// Dimension pruefen
			if (nbx<0 || nby<0 || nbx >= dimx || nby >= dimy)
				continue;

			// nur noch nicht besuchte Felder
			if ((*(mdata->m_base_map))[nbx][nby] != 0)
				continue;


			// Feld hinzufuegen
			borderqu.push(std::make_pair(nbx,nby));
			// als besucht markieren
			(*(mdata->m_base_map))[nbx][nby]=-1;
		}
	}
	

	delete hmap;
}

bool MapGenerator::insertGroupTemplates(MapData* mdata, RegionData* rdata)
{
	TemplateMap tmap(rdata->m_dimx, rdata->m_dimy);
	tmap.init(mdata->m_base_map);
	
	// obligatorische Objektgruppen einfuegen

	std::multimap<int, RegionData::NamedObjectGroup >::reverse_iterator it;
	Shape s;
	Vector pos;
	bool succ;
	LocationName locname, areaname;
	for (it = rdata->m_named_object_groups.rbegin(); it != rdata->m_named_object_groups.rend(); ++it)
	{
		// Objektgruppe anhand des Namens suchen
		s = ObjectFactory::getObjectGroupShape(it->second.m_group_name);
		if (s.m_center.m_x < 0)
		{
			ERRORMSG("unknown object group %s",it->second.m_group_name.c_str());
			continue;
		}

		// Ort fuer das Template suchen
		succ = tmap.getTemplatePlace(&s,pos);
		s.m_center = pos;

		if (succ == false)
		{
			// Obligatorisches Template konnte nicht platziert werden
			DEBUG("could not place template %s",it->second.m_group_name.c_str());
			/*
			int hdimx = rdata->m_dimx/2;
			int hdimy = rdata->m_dimy/2;
			for (int i=0; i<hdimx; i++)
			{
				for (int j=0; j< hdimy;j++)
				{
					if (*(mdata->m_base_map->ind(i,j)) != -1)
					{
						std::cout << "  ";
					}
					else
					{
						std::cout << "# ";
					}
				}
				std::cout << "\n";
			}
			*/
			return false;
		}

		// Objektgruppe einfuegen
		DEBUGX("placing group %s at %f %f",it->second.m_group_name.c_str(), pos.m_x, pos.m_y);
		mdata->m_region->createObjectGroup(it->second.m_group_name,pos,s.m_angle, it->second.m_name);
		insertBlockedArea(mdata,s);

		
	}
	
	if (rdata->m_has_waypoint)
	{
		ObjectGroup* templ;
	
		// Wegpunkt einfuegen
		templ = ObjectFactory::getObjectGroup("waypoint_templ");
		memcpy(&s , templ->getShape(), sizeof(Shape));
		succ = tmap.getTemplatePlace(&s,pos);
		s.m_center = pos;
		
		if (succ == false)
		{
			// Wegpunkt konnte nicht platziert werden
			DEBUG("could not place waypoint");
			return false;
		}
		mdata->m_region->createObjectGroup("waypoint_templ",pos,0);
		insertBlockedArea(mdata,s);
	}
	
	// fakultative Objektgruppen einfuegen
	
	std::multimap<int, RegionData::ObjectGroupSet >::reverse_iterator jt;
	for (jt = rdata->m_object_groups.rbegin(); jt != rdata->m_object_groups.rend(); ++jt)
	{
		s = ObjectFactory::getObjectGroupShape(jt->second.m_group_name);
		if (s.m_center.m_x < 0)
		{
			ERRORMSG("unknown object group %s",jt->second.m_group_name.c_str());
			continue;
		}

		DEBUGX("template %s size %f %f",jt->second.m_group_name.c_str(),s.m_extent.m_x, s.m_extent.m_y);
		float angle =0;
		for (int i=0; i< jt->second.m_number; i++)
		{
			if (Random::random() > jt->second.m_probability)
			{
				continue;
			}
			
			// Drehwinkel ermitteln
			// Kreise beliebig Rechtecke nur um 90°
			if (s.m_type == Shape::CIRCLE)
			{
				angle = 2*3.14159*Random::random();
			}
			else
			{
				int n = Random::randi(4);
				angle = 3.14159*(n*90.0)/180.0;
			}
			s.m_angle = angle;
			
			// Ort fuer das Template suchen
			succ = tmap.getTemplatePlace(&s,pos);
			s.m_center = pos;
			
			if (succ == false)
			{
				// Template konnte nicht platziert werden
				break;
			}

			// Objektgruppe einfuegen
			DEBUGX("placing group %s at %f %f",jt->second.m_group_name.c_str(), pos.m_x, pos.m_y);
			
			mdata->m_region->createObjectGroup(jt->second.m_group_name,pos,angle);
			if (! jt->second.m_decoration)
			{
				insertBlockedArea(mdata,s);
			}
		}
		
	}
	return true;
}


void MapGenerator::createBorder(MapData* mdata, RegionData* rdata)
{
	// Delta zu Nachbarfeldern
	int nb[4][2] = {{-1,0},{1,0},{0,-1},{0,1}};

	int hdimx = rdata->m_dimx/2;
	int hdimy = rdata->m_dimy/2;

	// Orte fuer den Ausgang in den vier Richtungen
	int exit[4][2];
	int exitcount[4];
	for (int i=0; i<4; i++)
	{
		exit[i][0] = nb[i][0]*-1000;
		exit[i][1] = nb[i][1]*-1000;
		exitcount[i] =1;
	}

	// Ausgaenge suchen
	for (int i=0; i<hdimx; i++)
	{
		for (int j=0; j< hdimy;j++)
		{

			int rnd;
			if (*(mdata->m_base_map->ind(i,j)) >=1 )
			{
				// Testen ob das Feld als Ausgang in Frage kommt
				// Also am noerdlichsten, westlichsten usw liegt
				for (int k=0; k<4; k++)
				{
					if (exit[k][0]*nb[k][0] + exit[k][1]*nb[k][1] < i*nb[k][0] + j*nb[k][1])
					{
						exit[k][0] =i;
						exit[k][1] =j;
						exitcount[k] =1;
					}
					
					if (exit[k][0]*nb[k][0] + exit[k][1]*nb[k][1] == i*nb[k][0] + j*nb[k][1])
					{
						rnd = Random::randi(exitcount[k]);
						if (rnd ==0)
						{
							exit[k][0] =i;
							exit[k][1] =j;
						}
						exitcount[k] ++;
					}
				}
			}

		}
	}

	// Ausgaenge platzieren
	for (int k=0; k<4; k++)
	{
		std::string dirname[4] = {"west","east","north","south"};
		std::string locname;
		if (rdata->m_exit_directions[k])
		{
			int i = exit[k][0];
			int j = exit[k][1];


			locname = "entry_";
			locname += dirname[k];
			mdata->m_region->addLocation(locname,Vector(i*8+4,j*8+4));
            *(mdata->m_base_map->ind(i,j)) = 2;

			i+= nb[k][0];
			j+= nb[k][1];
			locname = "exit_";
			locname += dirname[k];
			mdata->m_region->addLocation(locname,Vector(i*8+4,j*8+4));

			while (i>=0 && j>=0 && i< hdimx && j<hdimy)
			{
				*(mdata->m_base_map->ind(i,j)) = 2;
				i+= nb[k][0];
				j+= nb[k][1];
			}
		}
	}


	
	// fuer jedes markierte Feld werden die 4 Nachbarfelder angesehen
	// die 4 Informationen werden als 4bit Zahl interpretiert
	// blockierte Felder erhalten eine 0, freie eine 1
	//
	// es gibt 6 verschiedene Konstellationen fuer die Nachbarfelder
	// die 4bit Zahl dient als Index um die Konstellation und den Drehwinkel zu erhalten
	//
	// Aufbau der Bitmaske:
	// 
	//  D
	// A*B
	//  C
	// => Bitmaske ABCD
	
	// Liste mit den 6 verschiedenen Moeglichenkeiten welche Nachbarfelder besetzt sind
	// Form der 6 Template (+ frei, # blockiert):
	//
	// ?+?   ?+?   ?+?   ?+?   ?#?   ?+?
	// ###   ##+   ###   ##+   ###   +#+
	// ?#?   ?#?   ?+?   ?+?   ?#?   ?+?
	std::string borders[6] = {"$border(side)","$border(corner)","$border(twoside)","$border(twocorner)","$border(filled)","$border(single_block)"};

	// Maske dafuer, welche Situation vorliegt
	int bmask[16]={4,0,0,2,0,1,1,3,0,1,1,3,2,3,3,5};

	// Maske fuer die Rotationswinkel in Schritten von 90 Grad
	int rotmask[16] = {0,0,2,0,3,0,3,0,1,1,2,2,1,1,3,0};
	int mask =0;
	int nbi, nbj;

	// Abstaende der Diagonalen
	int diag[4][2] ={{-1,-1},{-1,1},{1,1},{1,-1}};
	
	// Fuer die Diagonalen wird eine aehnliche Bitmaske erstellt:
	// // eine 1 wird gesetzt, wenn das Feld selbst frei ist, die beiden Felder daneben aber besetzt
	//
	// B C
	//  *
	// A D
	// => Bitmaske ABCD
	
	// Liste mit den verschiedenen Moeglichenkeiten welche Nachbarfelder besetzt sind
	// Form der Template (+ frei, # blockiert):
	//
	// ###   ###   ##+   +##   +#+   +#+
	// ###   ###   ###   ###   ###   ###
	// ###   ##+   ##+   ##+   ##+   +#+
	
	std::string smallcorners[8] = {"","(one_smallcorner)","(two_smallcorner)","(diag_smallcorner)","(three_smallcorner)","(four_smallcorner)"};
		
	int diagbmask[16]=    {0,1,1,2,1,3,2,4, 1,2,3,4,2,4,4,5};
	int diagrotmask[16] = {0,0,1,0,2,0,1,0, 3,3,3,3,2,2,1,0};
	
	int diagi, diagj;
	int dmask;
	
	ObjectGroupName templ,diagtempl;
	float angle;

	bool skip;

	
	for (int j=0; j< hdimy;j++)
	{
		for (int i=0; i<hdimx; i++)
		{
			skip = false;
			if (*(mdata->m_base_map->ind(i,j)) != -1)
			{
				//std::cout << "  ";
				continue;
			}

			mask =0;
			dmask =0;
			
			// Bitmasken aufbauen
			for (int k=0; k<4; k++)
			{
				nbi = i + nb[k][0];
				nbj = j + nb[k][1];

				mask *=2;
				dmask  *= 2;
				
				if ( nbi>=0 && nbj>=0 && nbi<hdimx && nbj<hdimy)
				{
					if (*(mdata->m_base_map->ind(nbi,nbj)) >= 1)
					{
						mask +=1;
					}

				// Wenn eines der Nachbarfelder im *leeren Raum* liegt
				// dann keine Objekte setzen
					if (*(mdata->m_base_map->ind(nbi,nbj)) == 0)
					{
						skip = true;
					}
				}

			
				
				// Bitmaske fuer Diagonalen
				diagi = i + diag[k][0];
				diagj = j + diag[k][1];
				
				if ( diagi>=0 && diagj>=0 && diagi<hdimx && diagj<hdimy)
				{
					if (*(mdata->m_base_map->ind(diagi,diagj)) >= 1 &&
						*(mdata->m_base_map->ind(diagi,j)) == -1 &&
						*(mdata->m_base_map->ind(i,diagj)) == -1)
					{
						dmask +=1;
					}
				}

			}
			
			//std::cout << bmask[dmask] <<" ";
			if (skip)
				continue;

			// Template und dessen Winkel bestimmen
			templ = borders[bmask[mask]];
			angle = rotmask[mask] *PI/2;

			diagtempl = smallcorners[diagbmask[dmask]];
			float diagangle = diagrotmask[dmask] *PI/2;
			
			// gemeinsames Template aus Innenecken und Rand herstellen
			if (templ == "$border(filled)" && diagtempl !="")
			{
				templ ="$border";
				templ += diagtempl;
				angle = diagangle;
			}
			else if (templ == "$border(side)" && diagtempl == "(one_smallcorner)")
			{
				if (angle == diagangle)
				{
					templ = "$border(side)(right_smallcorner)";
				}
				else
				{
					templ = "$border(side)(left_smallcorner)";
				}
			}
			else
			{
				templ += diagtempl;
			}
			
			DEBUGX("placing type (%i %i) %s",i,j,templ.c_str());
			
			mdata->m_region->createObjectGroup(templ,Vector(i*8+4,j*8+4),angle);
		}
		//std::cout << "\n";
	}



}

void MapGenerator::insertSpawnpoints(MapData* mdata, RegionData* rdata)
{
	// Punkte an denen Spawnpoints möglich sind
	std::vector< std::pair<int,int> > points;

	int hdimx = rdata->m_dimx/2;
	int hdimy = rdata->m_dimy/2;

	// moegliche Orte fuer Spawnpoints ermitteln
	for (int i=0; i<hdimx; i++)
	{
		for (int j=0; j< hdimy;j++)
		{
			// nur erreichbare Felder
			if (*(mdata->m_base_map->ind(i,j)) !=1)
			{
				continue;
			}

			points.push_back(std::make_pair(i,j));
		}
	}


	// Spawnpoints platzieren
	std::list<RegionData::SpawnGroup>::iterator st;
	WorldObject* wo;
	bool stop = false;
	for (st = rdata->m_monsters.begin(); st != rdata->m_monsters.end() &&!stop; ++st)
	{
		// aufhoeren, wenn keine Orte mehr vorhanden
		
		DEBUGX("%s x %i",st->m_monsters.c_str(),st->m_number);
		for (int i=0; i< st->m_number; i++)
		{
			int r = Random::randi(points.size());
			wo = new Spawnpoint(st->m_monsters);

			mdata->m_region->insertObject(wo,Vector(points[r].first*8+4, points[r].second*8+4));

			DEBUGX("placing spawnpoint for %s at %i %i",st->m_monsters.c_str(), points[r].first*8+4, points[r].second*8+4);
			
			points[r] = points.back();
			points.resize(points.size() -1);
			if (points.size() ==0)
			{
				stop = true;
				break;
			}
		}
	}
}


void MapGenerator::createExits(MapData* mdata, RegionData* rdata)
{
	int dimx = rdata->m_dimx/2;
	int dimy = rdata->m_dimy/2;
	
	std::list<RegionExit>::iterator it;
	Vector pos;
	for (it = rdata->m_exits.begin(); it != rdata->m_exits.end(); ++it)
	{
		mdata->m_region->addExit(*it);

		// Bei den Ausgaengen keine Monster
		if (rdata->m_region_template =="")
		{
			pos = mdata->m_region->getLocation(it->m_exit_name);
			int cx = int(pos.m_x/8);
			int cy = int(pos.m_y/8);
			for (int i=MathHelper::Max(0,cx-2); i<MathHelper::Min(dimx,cx+2); i++)
			{
				for (int j=MathHelper::Max(0,cy-2); j<MathHelper::Min(dimy,cy+2); j++)
				{
					if (*(mdata->m_base_map->ind(i,j))==1)
					{
						*(mdata->m_base_map->ind(i,j)) = 2;
					}
				}
			}
		}

	}
	
	if (rdata->m_has_waypoint)
	{
		Vector pos = mdata->m_region->getLocation("WaypointLoc");
		WorldObject* wo =	 new Waypoint();
		mdata->m_region->insertObject(wo,pos);
		mdata->m_region->getWaypointLocation() = pos;
		
		//mdata->m_region->insertObject(wo,pos);
		DEBUGX("waypoint at %f %f %i",pos.m_x, pos.m_y,wo->getId());
		
		// Bei den Ausgaengen keine Monster
		if (rdata->m_region_template =="")
		{
			int cx = int(pos.m_x/8);
			int cy = int(pos.m_y/8);
			for (int i=MathHelper::Max(0,cx-2); i<MathHelper::Min(dimx,cx+2); i++)
			{
				for (int j=MathHelper::Max(0,cy-2); j<MathHelper::Min(dimy,cy+2); j++)
				{
					if (*(mdata->m_base_map->ind(i,j))==1)
					{
						*(mdata->m_base_map->ind(i,j)) = 2;
					}
				}
			}
		}
	}
}

void MapGenerator::insertBlockedArea(MapData* mdata, Shape s)
{
	// Groesse des Templates in 4x4 Feldern
	Vector ext = s.getAxisExtent();
	Vector emin = s.m_center - ext;
	Vector emax = s.m_center + ext;
	int imin = int(emin.m_x / 8);
	int jmin = int(emin.m_y / 8);
	int imax = int( ceil(emax.m_x / 8));
	int jmax = int( ceil(emax.m_y / 8));
	
	for (int i=imin; i< imax; i++)
	{
		for (int j=jmin; j<jmax; j++)
		{
			if (*(mdata->m_base_map->ind(i,j))==1)
			{
				*(mdata->m_base_map->ind(i,j)) = 2;
			}
		}
	}
}


void MapGenerator::createPerlinNoise(Matrix2d<float> *data, int dimx, int dimy,int startfreq, float persistance, bool bounds)
{
	Matrix2d<float>* tmp= new Matrix2d<float>(dimx+1, dimy+1);
	tmp->clear();
	float* weight = new float[MathHelper::Min(dimx,dimy)];
	float ampl =1;
	int dx,dy;
	float invdist;
	
	if (startfreq > dimx || startfreq > dimy)
	{
		startfreq = MathHelper::Min(dimx,dimy);
	}
	
	DEBUGX("Perlin noise dimx %i dimy %i",dimx,dimy);
	
	// Perlin Noise Algorithmus
	// Aenderung hier: Schleife geht ueber den Abstand zwischen den Stuetzstellen
	for (int dist = startfreq; dist>0 ;dist/=2)
	{
		// Dimension des Zufallszahlenfeldes fuer diesen Abstand
		dx = 2+(dimx-2)/dist;
		dy = 2+(dimy-2)/dist;
		
		DEBUGX("distance %i  tmp array %i %i",dist,dx,dy);
		
		// anlegen der Zufallszahlen fuer die aktuelle Frequenz
		for (int i=0;i<=dx;i++)
		{
			for (int j=0;j<=dy;j++)
			{
				*(tmp->ind(i,j)) = Random::random()* ampl;

			}

		}

		// Interpolation (nur fuer Abstand >1)
		if (dist>1)
		{
			invdist = 1.0f/dist;
	
			for (int k=0;k<dist;k++)
			{
				weight[k] = k*invdist;
			}
	
			
			
			for (int i=0;i<dx;i++)
			{
				for (int j=0;j<dy;j++)
				{
					for (int k=0;k<dist;k++)
					{
						for (int l=0;l<dist;l++)
						{
							if (i*dist+k< dimx && j*dist+l< dimy)
							{
								*(data->ind(i*dist+k,j*dist+l)) += (*(tmp->ind(i,j))*(1-weight[k])*(1-weight[l])
										+ *(tmp->ind(i+1,j))*weight[k]*(1-weight[l])
										+ *(tmp->ind(i+1,j+1))*weight[k]*weight[l]
										+ *(tmp->ind(i,j+1))*(1-weight[k])*weight[l]) ;
							}
						}
					}
				}
			}
		}
		else
		{
			for (int i=0;i<dimx;i++)
			{
				for (int j=0;j<dimy;j++)
				{
					*(data->ind(i,j)) +=*(tmp->ind(i,j));
				}
			}
		}

		ampl *= persistance;
		/*
		if (bounds)
		{
		
			std::cout.width(6);
			std::cout << "\n";
			for (int j=0;j<dy;j++)
			{
				for (int i=0;i<dx;i++)
				{
				
					std::cout << *(tmp->ind(i,j))<<" ";
				}
				std::cout << "\n";
			}
		}
		if (bounds)
		{
		
			std::cout.width(6);
			std::cout << "\n";
			for (int j=0;j<dimy;j++)
			{
				for (int i=0;i<dimx;i++)
				{
					std::cout << *(data->ind(i,j))<<" ";
				}
				std::cout << "\n";
			}
		}
		*/
	}

	// Maximum und Minimum suchen
	float fmax =0,fmin = 1000000;
	for (int i=0;i<dimx;i++)
	{
		for (int j=0;j<dimy;j++)
		{
			if (*(data->ind(i,j)) > fmax)
			{
				fmax = *(data->ind(i,j));
			}
			if (*(data->ind(i,j)) < fmin)
			{
				fmin = *(data->ind(i,j));
			}


		}
	}

	// normieren auf [0..1]
	for (int i=0;i<dimx;i++)
	{
		for (int j=0;j<dimy;j++)
		{
			*(data->ind(i,j)) -= fmin;
			if (fmax-fmin !=0)
			{
				*(data->ind(i,j)) /= (fmax-fmin);
			}
		}
	}
	
	
	// Raender anlegen
	if (bounds)
	{
		float bnd = MathHelper::Min(5.0f, MathHelper::Min(dimx/3.0f,dimy/3.0f));
		float dist;
		for (int i=0;i<dimx;i++)
		{
			for (int j=0;j<dimy;j++)
			{
				dist = MathHelper::Min(MathHelper::Min(i,dimx-i-1),MathHelper::Min(j,dimy-j-1));
				if (dist<bnd)
				{
					*(data->ind(i,j)) = *(data->ind(i,j))*(dist)/bnd + (bnd-dist)/bnd;
				}
			}
		}
	}
	
	/*
	//Debugging
	
	std::cout << "\n";
	for (int j=0;j<dimy;j++)
	{
		for (int i=0;i<dimx;i++)
		{
			std::cout << *(data->ind(i,j))<<" ";
		}
		std::cout << "\n";
	}
	*/
	delete[] weight;
	delete tmp;
}



