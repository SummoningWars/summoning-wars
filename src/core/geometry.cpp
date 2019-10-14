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


#include "geometry.h"
#include "mathhelper.h"

float Line::getDistance(Vector point)
{
	// Richtungsvektor der Linie
	Vector dir = getDirection();
	
	// Vektor vom Linienstart zu P
	Vector a = point - m_start;
	Vector b = m_end - point;
	Vector c = point;
	
	if (a*dir <0)
	{
		c -= m_start;
	}
	else if( b*dir<0)
	{
		c -= m_end;
	}
	else
	{
		c -=  m_start;
		c.normalPartTo(dir);
	}
	
	return c.getLength();
}

bool Shape::intersects(Line& line)
{
	// Abstand der Linie zum Mittelpunkt
	float dist = line.getDistance(m_center);
	
	// Linie schneidet die Flaeche, wenn sie den Inkreis schneidet
	if (dist < getInnerRadius())
	{
		return true;
	}
	
	//  Linie schneidet die Flaeche nicht, wenn sie den Umkreis nicht
	if (dist >= getOuterRadius())
	{
		return false;
	}
	
	// Fall Kreis hier schon entschieden
	if (m_type == RECT)
	{
		// KOS Transformation, sodass das Rechteck zentriert / ausgerichtet ist
		Line tl(line.m_start-m_center,line.m_end-m_center);
		tl.m_start.rotate(-m_angle);
		tl.m_end.rotate(-m_angle);
		
		Vector dir = tl.getDirection();
		
		// testen ob Linie rechteck garantiert verfehlt
		if ((tl.m_start.m_x < -m_extent.m_x && tl.m_end.m_x < -m_extent.m_x) || 
		    (tl.m_start.m_x > m_extent.m_x && tl.m_end.m_x > m_extent.m_x) || 
		    (tl.m_start.m_y < -m_extent.m_y && tl.m_end.m_y < -m_extent.m_y) || 
		    (tl.m_start.m_y > m_extent.m_y && tl.m_end.m_y > m_extent.m_y))
		{
			return false;
		}
		
		// Kreuzprodukte bilden zwischen der Richtung und Vektor zu zwei gegenueberliegenden Ecken
		// Auswahl des Eckenpaares so, dass die Diagonale gewaehlt wird, die einen grossen Winkel zur Linienrichtung hat
		// Kreuzprodukt muss einmal positiv, einmal negativ sein

		float p1,p2;
		Vector c = m_extent;
		if (dir.m_x*dir.m_y>0)
		{
			c.m_y = -c.m_y;
		}
		
		p1 = dir.crossProduct(tl.m_start - c);
		p2 = dir.crossProduct(tl.m_start + c);
		
		if (p1*p2<0)
		{
			return true;
		}
		
	}
	
	return false;
}

bool Shape::intersects(Shape& s2)
{
	// zwei Flaechen ueberschneiden sich, wenn sich die Inkreise ueberschneiden
	if ((m_center-s2.m_center).getLength() < getInnerRadius()+s2.getInnerRadius())
	{
		return true;
	}
	
	// zwei Flachen ueberschneiden sich nicht, wenn sich die Umkreise nicht ueberschneiden
	if ((m_center-s2.m_center).getLength() >= getOuterRadius()+s2.getOuterRadius())
	{
		return false;
	}

	// Der Fall Kreis - Kreis ist jetzt schon mit Sicherheit entschieden
	
	if (m_type == Shape::RECT && s2.m_type == Shape::RECT)
	{
		// Fall: zwei Rechtecke
		DEBUGX("checking rect-rect");

		// Test per Separation Axis Theorem,
		// moegliche Achsen sind die Seiten der Rechtecke
		
		// Herstellen eines lokalen Koordinatensystems, in dem R1 zentriert und an den Achsen ausgerichtet ist
		
		// neuer Mittelpunkt und Rotationswinkel von R2
		Vector newcent = s2.m_center - m_center;
		newcent.rotate(-m_angle);
		float angle2 = s2.m_angle - m_angle;
		
		// Testen ob alle vier Ecken von R2 oberhalb, unterhalb, links oder Rechts von R1 liegen
		float ex = s2.m_extent.m_x;
		float ey = s2.m_extent.m_y;
		
		if (fabs(newcent.m_x) -m_extent.m_x >= fabs(ex*cos(angle2)) + fabs(ey * sin(angle2)))
		{
			return false;
		}
		if (fabs(newcent.m_y) -m_extent.m_y >= fabs(ey*cos(angle2)) + fabs(ex * sin(angle2)))
		{
			return false;
		}
		

		// Das selbe nochmal, jetzt R2 zentiert und an den Achsen ausgerichtet
		
		// neuer Mittelpunkt und Rotationswinkel von R1
		newcent = m_center - s2.m_center;
		newcent.rotate(-s2.m_angle);
		angle2 = m_angle - s2.m_angle;
		
		ex = m_extent.m_x;
		ey = m_extent.m_y;
		
		if (fabs(newcent.m_x) -s2.m_extent.m_x >= fabs(ex*cos(angle2)) + fabs(ey * sin(angle2)))
		{
			return false;
		}
		if (fabs(newcent.m_y) -s2.m_extent.m_y >= fabs(ey*cos(angle2)) + fabs(ex * sin(angle2)))
		{
			return false;
		}
		
		// keine separierende Achse gefunden
		return true;
	}
	else
	{
		// Pruefen Kreis gegen Rechteck
		DEBUGX("checking circle-rect");

		// Herstellen eines lokalen Koordinatensystems, in das Rechteck zentriert und an den Achsen ausgerichtet ist
		
		// neues Zentrum des Kreises
		Vector newcent;
		// Ausdehnung des Rechtecks
		Vector rext;
		float r;
		
		
		if (m_type == Shape::RECT)
		{
			newcent = s2.m_center - m_center;
			newcent.rotate(-m_angle);
			rext = m_extent;
			r = s2.m_radius;
		}
		else
		{
			newcent = m_center - s2.m_center;
			newcent.rotate(-s2.m_angle);
			rext =  s2.m_extent;
			r = m_radius;
		}
		
		// Testen, ob der Kreis sich durch eine der Koordinatenachsen vom Rechteck trennen laesst
		if (fabs(newcent.m_x) >= rext.m_x + r || fabs(newcent.m_y) >= rext.m_y + r)
		{
			return false;
		}
		
		// Testen ob Kreismittelpunkt direkt im Rechteck liegt
		if ( fabs(newcent.m_x) < rext.m_x || fabs(newcent.m_y) < rext.m_y)
		{
			return true;
		}
		
		
		// Kreis liegt auf dem Reckteck, wenn Abstand zu einer der Ecken kleiner als r
		float mx = MathHelper::Min (fabs(newcent.m_x+rext.m_x), fabs(newcent.m_x-rext.m_x));
		float my = MathHelper::Min (fabs(newcent.m_y+rext.m_y), fabs(newcent.m_y-rext.m_y));
		if (mx*mx + my*my < r*r)
		{
			return true;
		}
		else
		{
			return false;
		}

	}
	
	return false;
}

float Shape::getDistance(Shape& s2)
{
	if (m_type == Shape::CIRCLE && s2.m_type == Shape::CIRCLE)
	{
		// Fall: zwei Kreise
		return MathHelper::Max(0.0f,(m_center-s2.m_center).getLength() - m_radius - s2.m_radius);
	}
	else if (m_type == Shape::RECT && s2.m_type == Shape::RECT)
	{
		// Fall: zwei Rechtecke
		DEBUGX("checking rect-rect");
	
		
		float dmin = 1e20;
		
		// Herstellen eines lokalen Koordinatensystems, in dem R1 zentriert und an den Achsen ausgerichtet ist
		
		// neuer Mittelpunkt und Rotationswinkel von R2
		Vector newcent = s2.m_center - m_center;
		newcent.rotate(-m_angle);
		float angle2 = s2.m_angle - m_angle;
		
		float ex = s2.m_extent.m_x;
		float ey = s2.m_extent.m_y;
		
		int corner[4][2] = {{1,1},{1,-1},{-1,1},{-1,-1}};
		Vector c;
		
		// Schleife ueber alle Ecken von R2
		
		for (int i=0; i<4; i++)
		{
			c.m_x = newcent.m_x + ex*cos(angle2)*corner[i][0] - ey*sin(angle2)*corner[i][1];
			c.m_y = newcent.m_y + ex*sin(angle2)*corner[i][0] + ey*cos(angle2)*corner[i][1];
			
			if (fabs(c.m_x) < m_extent.m_x)
			{
				if (fabs(c.m_y) < m_extent.m_y)
				{
					return 0;
				}
				else
				{
					dmin = MathHelper::Min(dmin,float(fabs(c.m_y)-m_extent.m_y));
				}
			}
			else if (fabs(c.m_y) < m_extent.m_y)
			{
				dmin = MathHelper::Min(dmin,float(fabs(c.m_x)-m_extent.m_x));
			}
			else
			{
				c.m_x = fabs(c.m_x) - m_extent.m_x;
				c.m_y = fabs(c.m_y) - m_extent.m_y;
				dmin = MathHelper::Min(dmin,c.getLength());
			}
		}
		
		
		// dasselbe nochmal fuer R2
		newcent = m_center - s2.m_center;
		newcent.rotate(-s2.m_angle);
		angle2 = m_angle - s2.m_angle;
		
		ex = m_extent.m_x;
		ey = m_extent.m_y;
		
		// Schleife ueber alle Ecken von R1
		
		for (int i=0; i<4; i++)
		{
			c.m_x = newcent.m_x + ex*cos(angle2)*corner[i][0] - ey*sin(angle2)*corner[i][1];
			c.m_y = newcent.m_y + ex*sin(angle2)*corner[i][0] + ey*cos(angle2)*corner[i][1];
			
			if (fabs(c.m_x) < s2.m_extent.m_x)
			{
				if (fabs(c.m_y) < s2.m_extent.m_y)
				{
					return 0;
				}
				else
				{
					dmin = MathHelper::Min(dmin, float(fabs(c.m_y) - s2.m_extent.m_y));
				}
			}
			else if (fabs(c.m_y) < s2.m_extent.m_y)
			{
				dmin = MathHelper::Min(dmin,float(fabs(c.m_x)-s2.m_extent.m_x));
			}
			else
			{
				c.m_x = fabs(c.m_x) - s2.m_extent.m_x;
				c.m_y = fabs(c.m_y) - s2.m_extent.m_y;
				dmin = MathHelper::Min(dmin,c.getLength());
			}
		}
		
		
		
		return dmin;
	}
	else
	{
		// Pruefen Kreis gegen Rechteck
		DEBUGX("distance circle-rect");

		// Herstellen eines lokalen Koordinatensystems, in das Rechteck zentriert und an den Achsen ausgerichtet ist
		
		// neues Zentrum des Kreises
		Vector newcent;
		// Ausdehnung des Rechtecks
		Vector rext;
		float r;
		
		
		if (m_type == Shape::RECT)
		{
			newcent = s2.m_center - m_center;
			newcent.rotate(-m_angle);
			rext = m_extent;
			r = s2.m_radius;
		}
		else
		{
			newcent = m_center - s2.m_center;
			newcent.rotate(-s2.m_angle);
			rext =  s2.m_extent;
			r = m_radius;
		}
		

		if ( fabs(newcent.m_x) < rext.m_x || fabs(newcent.m_y) < rext.m_y)
		{
			// Abstand zur naechsten Kante
			return MathHelper::Max( MathHelper::Max(0.0f,fabs(newcent.m_x) - rext.m_x -r), MathHelper::Max(0.0f,fabs(newcent.m_y) - rext.m_y-r));
		}

		
		// Abstand zur naechsten Ecke
		float mx = MathHelper::Min (fabs(newcent.m_x+rext.m_x), fabs(newcent.m_x-rext.m_x));
		float my = MathHelper::Min (fabs(newcent.m_y+rext.m_y), fabs(newcent.m_y-rext.m_y));
		return sqrt(mx*mx + my*my) - r;
	}
	
	return 0;
}

Vector Shape::projectionOnBorder(Vector point)
{
	if (m_type == Shape::CIRCLE)
	{
		// Vector vom Zentrum zum Punkt
		Vector dir = point;
		dir -= m_center;
		
		// Punkt auf dem Kreis in dieser Richtung
		dir.normalize();
		dir *= m_radius;
		
		dir += m_center;
		return dir;
	}
	else
	{
		Vector proj;
		
		// Drehen des Bezugssystems, sodass das Rechteck ausgerichtet ist
		Vector locpos = point - m_center;
		locpos.rotate(-m_angle);
		
		// signalisiert Fall Projektion auf Kante parallel zur x-Achse oder y-Achse
		bool projx = false;
		bool projy = false;
		if (fabs(locpos.m_x) < m_extent.m_x)
			projx = true;
		if (fabs(locpos.m_y) < m_extent.m_y)
			projy = true;
		
		if (projx && projy)
		{
			// Entscheidung fuer die Kante, die naeher ist
			float diffx = MathHelper::Min(fabs(locpos.m_x-m_extent.m_x), fabs(locpos.m_x+m_extent.m_x));
			float diffy = MathHelper::Min(fabs(locpos.m_y-m_extent.m_y), fabs(locpos.m_y+m_extent.m_y));
			if (diffx < diffy)
				projx = false;
			else
				projy = false;
		}
		
		if (projx)
		{
			// Fall Projektion auf Kante parallel zur y-Achse
			proj.m_x = locpos.m_x;
			if (locpos.m_y >0)
				proj.m_y = m_extent.m_y;
			else
				proj.m_y = -m_extent.m_y;
		}
		else if (projy)
		{
			// Fall Projektion auf Kante parallel zur y-Achse
			proj.m_y = locpos.m_y;
			if (locpos.m_x >0)
				proj.m_x = m_extent.m_x;
			else
				proj.m_x = -m_extent.m_x;
		}
		else
		{
			// Projektion auf eine Ecke
			if (locpos.m_y >0)
				proj.m_y = m_extent.m_y;
			else
				proj.m_y = -m_extent.m_y;
			
			if (locpos.m_x >0)
				proj.m_x = m_extent.m_x;
			else
				proj.m_x = -m_extent.m_x;
		}
		
		// zurueckrechnen auf globale Position
		proj.rotate(m_angle);
		proj += m_center;
		
		return proj;
	}
}

