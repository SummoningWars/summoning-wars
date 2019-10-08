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

#ifndef GEOMETRY_H
#define GEOMETRY_H

#ifndef PI
#define PI 3.14159
#endif

#include <math.h>
#include <algorithm>
#include "debug.h"
#include "mathhelper.h"
/**
 * \struct Vector
 * \brief zweidimensionaler Vektor
 */
struct Vector
{
	/**
	 * \fn Vector(float x=0, float y=0)
	 * \brief Erzeugt einen neuen Vektor
	 * \param x x-Komponente
	 * \param y y-Komponente
	 */
	Vector(float x=0, float y=0)
	{
		m_x = x;
		m_y = y;
	}
	
			
	/**
	 * \var float m_x
	 * \brief x-Komponente
	 **/
	float m_x;
	
	/**
	 * \var float m_y
	 * \brief y-Komponente
	 **/
	float m_y;
	
	/**
	 * \fn float getLength()
	 * \brief Gibt die Laenge des Vektors aus
	 */
	float getLength()
	{
		return sqrt(m_x*m_x + m_y*m_y);
	}
	
	/**
	 * \fn void normalize()
	 * \brief Normiert den Vektor
	 */
	void normalize()
	{
		float l = getLength();
		if (l!=0)
			operator*=(1/l);
	}
	
	/**
	 * \fn void operator+=(Vector other)
	 * \brief Addiert den zweiten Vektor zum ersten
	 * \param other zweiter Operand
	 */
	void operator+=(Vector other)
	{
		m_x += other.m_x;
		m_y += other.m_y;
		
	}
	
	/**
	 * \fn void operator-=(Vector other)
	 * \brief Addiert den zweiten Vektor zum ersten
	 * \param other zweiter Operand
	 */
	void operator-=(Vector other)
	{
		m_x -= other.m_x;
		m_y -= other.m_y;
		
	}
	
	/**
	 * \fn Vector operator+(Vector other)
	 * \brief Gibt die Summe der beiden Vectoren aus
	 * \param other zweiter Operand
	 */
	Vector operator+(Vector other)
	{
		return Vector(m_x+other.m_x, m_y + other.m_y);
	}
	
	/**
	 * \fn Vector operator-(Vector other)
	 * \brief Gibt die Differenz der beiden Vectoren aus
	 * \param other zweiter Operand
	 */
	Vector operator-(Vector other)
	{
		return Vector(m_x-other.m_x, m_y - other.m_y);
	}
	
	/**
	 * \fn float operator*(Vector other)
	 * \brief Berechnet das Skalarprodukt der beiden Vektoren
	 * \param other zweiter Operand
	 */
	float operator*(Vector other)
	{
		return m_x*other.m_x + m_y*other.m_y;
	}
	
	/**
	 * \fn Vector operator*(float mul)
	 * \brief Gibt ein Vielfaches des Vektors aus
	 * \param mul Faktor
	 */
	Vector operator*(float mul)
	{
		return Vector(m_x*mul, m_y*mul);
	}
	
	/**
	 * \fn Vector operator/(float mul)
	 * \brief Gibt ein Bruchteil des Vektors aus
	 * \param mul Faktor
	 */
	Vector operator/(float mul)
	{
		return Vector(m_x/mul, m_y/mul);
	}
	
	/**
	 * \fn void operator*=(float mul)
	 * \brief Skaliert den Vektor
	 * \param mul Skalierungsfaktor
	 */
	void operator*=(float mul)
	{
		m_x *= mul;
		m_y *= mul;
	 	
	}
	
	/**
	 * \fn void operator=(Vector other)
	 * \brief Kopieroperator
	 * \param other zweiter Operand
	 */
	void operator=(Vector other)
	{
		m_x = other.m_x;
		m_y = other.m_y;
		
	}
	
	/**
	 * \fn float crossProduct(Vector other)
	 * \brief Bildet das Kreuzprodukt
	 * \param other zweiter Operand
	 */
	float crossProduct(Vector other)
	{
		return m_x*other.m_y - m_y*other.m_x;
	}
	
	/**
	 * \fn float angle()
	 * \brief Gibt Winkel zur x-Achse aus
	 */
	float angle()
	{
		return atan2(m_y,m_x);
	}
	
	/**
	 * \fn float angle(Vector other)
	 * \brief Gibt den Winkel zwischen den beiden Vektoren aus
	 * \param other zweiter Operand
	 */
	float angle(Vector other)
	{
		return acos((*this)*other /(getLength() * other.getLength()));
	}
	
	/**
	 * \fn void projectOn(Vector other)
	 * \brief Bildet die Projektion auf den angegebenen Vektor
	 * \param other Vektor auf den projiziert wird
	 */
	void projectOn(Vector other)
	{
		float mul = (*this)*other/(other.m_x*other.m_x + other.m_y*other.m_y);
		m_x = other.m_x*mul;
		m_y = other.m_y*mul;
		
	}
	
	/**
	 * \fn void normalPartTo(Vector other)
	 * \brief Bildet von einem Vektor den Teil der senkrecht zum angegebenen zweiten Vektor ist
	 * \param other Vektor 
	 */
	void normalPartTo(Vector other)
	{
		float mul = (*this)*other/(other.m_x*other.m_x + other.m_y*other.m_y);
		m_x -= other.m_x*mul;
		m_y -= other.m_y*mul;
	}
	
	float distanceTo(Vector point)
	{
		return sqrt((point.m_x -m_x)*(point.m_x -m_x) + (point.m_y -m_y)*(point.m_y -m_y));
	}
	
	/**
	 * \fn void rotate(float angle)
	 * \brief rotiert den Vektor um den angegebenen Winkel
	 * \param angle Rotationswinkel
	 */
	void rotate(float angle)
	{
		float x = m_x*cos(angle) - m_y*sin(angle);
		float y = m_x*sin(angle) + m_y*cos(angle);
		m_x = x;
		m_y = y;
	}

};



/**
 * \struct Line
 * \brief Linie im 2D Raum
 */
struct Line
{
	/**
	 * \var Vector m_start
	 * \brief Startpunkt der Linie
	 */
	Vector m_start;
	
	/**
	 * \var Vector m_end
	 * \brief Endpunkt der Linie
	 */
	Vector m_end;
	
	/**
	 * \fn Line()
	 * \brief Konstruktor
	 */
	Line()
	{
		
	}
	
	/**
	 * \fn Line(Vector start, Vector end)
	 * \brief Konstruktor
	 * \param start Startpunkt
	 * \param end Endpunkt
	 */
	Line(Vector start, Vector end)
	: m_start(start) , m_end(end)
	{
	}
	
	/**
	 * \fn Vector getDirection()
	 * \brief Gibt die Richtung der Linie aus
	 */
	Vector getDirection()
	{
		return m_end - m_start;
	}
	
	/**
	 * \fn float getLength()
	 * \brief Gibt die Lange der Linie aus
	 */
	float getLength()
	{
		return getDirection().getLength();
	}
	
	/**
	 * \fn float getDistance(Vector point)
	 * \brief Gibt den Abstand des Punktes zur Gerade aus
	 * \param point Punkt
	 */
	float getDistance(Vector point);
	
};

/**
 * \struct Shape
 * \brief Geometrische Form, moeglich sind Rechteck und Kreis
 */
struct Shape
{
	
	/**
	 * \enum ShapeType
	 * \brief Typ der geometrischen Form
	 */
	enum ShapeType
	{
		RECT=1,
		CIRCLE=2
	};
	
	/**
	 * \fn Shape()
	 * \brief Konstruktor
	 */
	Shape()
	{
		m_type = CIRCLE;
		m_radius=0;
		m_angle=0;
	}
	
	
	/**
	 * \var Vector m_center
	 * \brief Mittelpunkt der Figur
	 */
	Vector m_center;


	/**
	 * \var m_type
	 * \brief Form des Objektes (kreis / rechteckfoermig)
	 */
	ShapeType m_type;

	/**
	 * \var Vector m_extent
	 * \brief Ausdehnung des Objektes (nur bei Rechteckigen Objekten verwendet)
	 */
	Vector m_extent;
	
	/**
	 * \var m_radius
	 * \brief Radius des Objektes ( nur bei kreisfoermigen Objekten verwendet)
	 */
	float m_radius;
	
	/**
	 * \var m_angle
	 * \brief Winkel um den das Objekt gegenueber dem Koordinatensystem gedreht ist
	 */
	float m_angle;
	
	/**
	 * \fn bool intersects(Shape& s2)
	 * \brief Gibt true aus, wenn die beiden Flaechen sich ueberschneiden
	 * \param s2 zweite Flaeche
	 */
	bool intersects(Shape& s2);
	
	/**
	 * \fn bool intersects(Line& line)
	 * \brief Gibt true aus, wenn die Linie die Flaeche schneidet
	 * \param line Linie
	 */
	bool intersects(struct Line& line);
	
	/**
	 * \fn Vector projectionOnBorder(Vector point)
	 * \brief Bildet die Projektion eines Punktes auf den Rand
	 * \param point projizierter Punkt
	 */
	Vector projectionOnBorder(Vector point);
	
	/**
	 * \fn float distance(Shape& s2)
	 * \brief Gibt die Distanz zwischen den beiden Flaechen aus
	 * \param s2 zweite Flaeche
	 */
	
	float getDistance(Shape& s2);
	
	/**
	 * \fn float getInnerRadius()
	 * \brief Gibt den Innkreisradius aus
	 */
	float getInnerRadius()
	{
		if (m_type == RECT)
			return (MathHelper::Min(fabs(m_extent.m_x),fabs(m_extent.m_y)));
		else
			return m_radius;
	}
	
	/**
	 * \fn float getOuterRadius()
	 * \brief Gibt den Innkreisradius aus
	 */
	float getOuterRadius()
	{
		if (m_type == RECT)
			return m_extent.getLength();
		else
			return m_radius;
	}
	
	/**
	 * \fn Vector getAxisExtent()
	 * \brief Gibt die Ausdehnung in die Achsenrichtungen aus
	 */
	Vector getAxisExtent()
	{
		if (m_type == RECT)
		{
			return (Vector(fabs(m_extent.m_x*cos(m_angle)) + fabs(m_extent.m_y*sin(m_angle)) , fabs(m_extent.m_y*cos(m_angle)) + fabs(m_extent.m_x*sin(m_angle))));
		}
		
		return Vector(m_radius,m_radius);
		
	}
	
	/**
	 * \fn Shape& operator=(Shape other)
	 * \brief Zuweisungsoperator
	 */
	Shape& operator=(Shape other)
	{
		m_center = other.m_center;
		m_type = other.m_type;
		m_radius = other.m_radius;
		m_extent = other.m_extent;
		m_angle = other.m_angle;
		
		return *this;
	}
};


#endif

