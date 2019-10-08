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


#include "pathfind.h"

#define INF 100000000

PathfindInfo::~PathfindInfo()
{
	if (m_pot)
		delete m_pot;

	if (m_block)
		delete m_block;
}

SearchField::SearchField(int dim)
{

	m_dim = dim;
	// Heap ist zum Beginn leer
	m_heap_dimension=0;

	// Anzahl Felder = Kantenlänge^2
	int sqdim=dim*dim;

	// Feld für Array anlegen
	m_heap = new SearchFieldEntry*[sqdim];

	// Array mit den Suchfeldeintraegen
	m_array = new Matrix2d<SearchFieldEntry>(dim,dim) ;

	// Bei allen Suchfeldeintraegen die Koordinaten eintragen
	for (int i=0;i<m_dim;++i)
	{
		for (int j=0;j<m_dim;++j)
		{
			m_array->ind(i,j)->m_x =i;
			m_array->ind(i,j)->m_y =j;
		}
	}


}

SearchField::~SearchField()
{
	delete m_array;
	delete[] m_heap;
}

void SearchField::init(Matrix2d<float>* pot, Matrix2d<char>* block)
{
	m_pot = pot;
	m_block = block;
}

void SearchField::heapPush(SearchFieldEntry* fentry)
{

	// größe des Heap inkrementieren
	m_heap_dimension++;

	// Feld an letzte Position speichern
	m_heap[m_heap_dimension]=fentry;

	// speichern, wo im Heap sich das Feld befindet
	fentry->m_heapref=m_heap_dimension;

	//Feld korrekt einordnen
	reOrder(fentry);
}

SearchFieldEntry* SearchField::heapPop()
{
	// Wenn der Heap leer ist NULL ausgebe
	if (m_heap_dimension==0)
		return 0;

	// Erstes Element im Heap ausgeben
	SearchFieldEntry* result = m_heap[1];
	// Element wird aus dem Heap entfernt
	result->m_heapref=0;

	// Letztes Element an die Spitze des Heap setzen
	m_heap[1]=m_heap[m_heap_dimension];
	m_heap[1]->m_heapref=1;

	// Dimension dekrementieren
	m_heap_dimension--;

	// Ab hier: Element einsickern lassen
	int pos=-1,newpos=1;
	SearchFieldEntry* swap=0;
	while (pos != newpos)
	{
		// solange bis das Element sich nicht mehr bewegt
		pos=newpos;
		if (2*pos<m_heap_dimension)
		{
			// Knoten hat zwei Kinder
			// Wenn einer der Kindknoten einen kleineren Wert hat
			// Kindknoten mit den kleinsten Wert als neue Position auswaehlen
			if (m_heap[pos]->m_f > m_heap[2*pos]->m_f)
				newpos=2*pos;
			if (m_heap[newpos]->m_f > m_heap[2*pos+1]->m_f)
				newpos=2*pos+1;
		}
		else if (2*pos==m_heap_dimension)
		{
			// Knoten hat ein Kind
			// Wenn Kindknoten kleineren Wert hat, Element weiter einsickern lassen
			if (m_heap[pos]->m_f > m_heap[2*pos]->m_f)
				newpos=2*pos;
		}

		if (newpos!=pos)
		{
			//Element einsickern lassen

			// Vertauschen mit dem ausgesuchen Kindknoten
			swap=m_heap[newpos];
			m_heap[newpos]=m_heap[pos];
			m_heap[pos]=swap;

			// aktualisieren der Indizee die in den Heap zeigen
			m_heap[pos]->m_heapref=pos;
			m_heap[newpos]->m_heapref=newpos;
		}

	}

	return result;
}

void SearchField::reOrder(SearchFieldEntry* fentry)
{
	// aktuelle Position des betrachteten Elements
	int i = fentry->m_heapref;
	SearchFieldEntry* swap;
	while	(i>1 && m_heap[i]->m_f <= m_heap[i/2]->m_f)
	{
		// Element hat einen kleineren Wert als der Vaterknoten

		// tauschen mit dem Vater
		swap=m_heap[i/2];
		m_heap[i/2]=m_heap[i];
		m_heap[i]=swap;

		// aktualisieren der Indizee die in den Heap zeigen
		m_heap[i]->m_heapref=i;
		m_heap[i/2]->m_heapref=i/2;
		i=i/2;
	}

}

SearchFieldEntry* SearchField::getSearchFieldEntry(int x, int y)
{

	// Wenn Koordinaten innerhalb des Array liegen, Zeiger in das Array zurückgeben
	if (x>= 0 && x<m_dim  && y>= 0 && y<m_dim )
	{
		SearchFieldEntry* ret =m_array->ind(x,y);
		return ret;

	}

	// Koordinaten ausserhalb angegeben, NULL Zeiger ausgeben
	return 0;
}

void SearchField::createPotential(int start_x, int start_y)
{
	// Felder initialisieren
	int i,j;
	/*for (i=0; i<sqdim;i++)
	{
		m_heap[i]=0;
	}*/
	memset(m_heap,0,sizeof(m_heap));

	for (i=0;i<m_dim;++i)
	{
		for (j=0;j<m_dim;++j)
		{
			m_array->ind(i,j)->m_heapref=0;
			m_array->ind(i,j)->m_state=0;

		}
	}

	//printf("initialized\n");
	const int adj[4][2] = {{0,1},{1,0},{0,-1},{-1,0}};

	SearchFieldEntry* fentry,*fentry_adj;
	fentry=getSearchFieldEntry(start_x,start_y);
	fentry->m_f =0;
	fentry->m_state =1;
	//printf("start: %i %i\n",start_x,start_y);

	heapPush(fentry);
	int x,y,xa,ya,s;
	float f,fa,p,po,t;



	while (m_heap_dimension>0)
	{
		fentry=heapPop();
		x= fentry->m_x;
		y=fentry->m_y;
		f=fentry->m_f;
		//printf("processing (%i %i) : %f\n",x,y,f);
		po = penalty[(int) *(m_block->ind(x,y))];

		for (i=0;i<4;++i)
		{
			xa = x+adj[i][0];
			ya = y+adj[i][1];

			fentry_adj = getSearchFieldEntry(xa, ya);
			if (!fentry_adj)
				continue;

			if (*(m_block->ind(xa,ya))=='X')
				continue;
			else
			{
				//p = penalty[*(m_block->ind(xa,ya))-'0'];
				p = penalty[(int) *(m_block->ind(xa,ya))];

			}



			fa = fentry_adj->m_f;

			s= fentry_adj->m_state;
			if (s ==0)
			{


				fentry_adj->m_f = f+p;
				fentry_adj->m_state =1;
				//printf("adding (%i %i) : %f\n",xa,ya,f+1);
				heapPush(fentry_adj);
			}
			else if (s==1)
			{
				//printf("neighbour (%i %i) : %f\n",xa,ya,fa);
				fa = fentry_adj->m_f;
				if (fa <= f+p)
				{
					//fentry_adj->m_f = f+p*DIAG_PRIO*(fa-f)/(p+(fa-f));
					t= p/(fa-f);
					fentry_adj->m_f = fa-1/(t*(t+1));
					reOrder(fentry_adj);
				}
				else
				{

					fentry_adj->m_f = f+p;
					reOrder(fentry_adj);
				}
			}

		}
		fentry->m_state =2;
		*(m_pot->ind(x,y)) = fentry->m_f;
	}

	/*
	for (i=0;i<m_dim;++i)
	{
		for (j=0;j<m_dim;++j)
		{
			printf("%2.2f ",*(m_pot->ind(j,m_dim-1-i)));
		}
		printf("\n");
	}
	*/

}

void SearchField::createGradient(Matrix2d<float[2]>* grad)
{
	const int adj[4][2] = {{0,1},{1,0},{0,-1},{-1,0}};
	const int diag[4][2] = {{1,1},{1,-1},{-1,-1},{-1,1}};
	int k,ia1,ja1,ia2,ja2,id,jd;
	bool b;
	float fa1,fa2,fd,f1,f2;
	float qmax=0,q;
	int i,j;
	float f;
	for (i=0;i<m_dim;++i)
	{
		for (j=0;j<m_dim;++j)
		{
			b=false;
			f=*(m_pot->ind(i,j));
			for (k=0;k<4;++k)
			{
				qmax =0;
				ia1 = i+adj[k][0];
				ja1 = j+adj[k][1];
				ia2 = i+adj[(k+1)%4][0];
				ja2 = j+adj[(k+1)%4][1];

				if (ia1 <0 || ia2<0 || ja1 <0 || ja2<0 || ia1>=m_dim || ia2>=m_dim || ja1>=m_dim || ja2>=m_dim)
				{
					continue;

				}


				id = i+diag[k][0];
				jd = j+diag[k][1];

				if (*(m_block->ind(ia1,ja1))=='X' || *(m_block->ind(ia2,ja2))=='X' || *(m_block->ind(id,jd))=='X')
				{
					continue;
				}

				fa1 = *(m_pot->ind(ia1,ja1));
				fa2 = *(m_pot->ind(ia2,ja2));
				fd = *(m_pot->ind(id,jd));


				if (fa1<f && fa2<f && fd<fa1 && fd < fa2)
				{
					f1 = (f-fa1)*adj[k][0]+(f-fa2)*adj[(k+1)%4][0];
					f2 = (f-fa1)*adj[k][1]+(f-fa2)*adj[(k+1)%4][1];

					q = f1*f1+f2*f2;
					if (q>qmax)
					{

						(*(grad->ind(i,j)))[0] = f1;
						(*(grad->ind(i,j)))[1] = f2;
						q=qmax;

					}
					b=true;
				}
			}

			if (!b)
			{
				for (k=0;k<4;++k)
				{
					ia1 = i+adj[k][0];
					ja1 = j+adj[k][1];

					if (ia1 <0 || ja1 <0 || ia1>=m_dim || ja1>=m_dim)
						continue;

					fa1 = *(m_pot->ind(ia1,ja1));
					if (fa1 <f && *(m_block->ind(ia1,ja1))!='X')
					{
						f1 = (f-fa1)*adj[k][0];
						f2 = (f-fa1)*adj[k][1];


						q = f1*f1+f2*f2;
						if (q>qmax)
						{
							(*(grad->ind(i,j)))[0] = f1;
							(*(grad->ind(i,j)))[1] = f2;
							qmax=q;
							b=true;
						}
					}
				}
			}

			if (!b)
			{
				(*(grad->ind(i,j)))[0] = 0;
				(*(grad->ind(i,j)))[1] = 0;
			}


		}
	}





}

 void SearchField::getGradient(PathfindInfo* pathinfo,Vector pos, Vector& dir )
{

	float sqs = pathinfo->m_base_size / pathinfo->m_quality;
	float l = 0.5*sqs * pathinfo->m_dim;
	
	if (fabs(pos.m_x-pathinfo->m_center.m_x) >l  || fabs(pos.m_y-pathinfo->m_center.m_y) >l)
	{
		dir = pathinfo->m_start -  pos;
		return;
	}
	
	Matrix2d<float>& pot = *(pathinfo->m_pot);
	Matrix2d<char>& block = *(pathinfo->m_block);
	int dim = pathinfo->m_dim;
	
	
	int x,y;
	x = (int) ((pos.m_x - pathinfo->m_center.m_x + l) / sqs);
	y = (int) ((pos.m_y - pathinfo->m_center.m_y + l) / sqs);
	
	const int adj[4][2] = {{0,1},{1,0},{0,-1},{-1,0}};
	const int diag[4][2] = {{1,1},{1,-1},{-1,-1},{-1,1}};

	int k,ia1,ja1,ia2,ja2,id,jd;
	bool b;
	float fa1,fa2,fd,f1,f2;
	float qmax=0,q;
	int i,j;
	float f;
	b=false;
	i =x;
	j=y;

	f=pot[i][j];

	if (block[i][j]=='X')
	{
		DEBUG("blockiertes Feld betreten");
		qmax = 1000000;
		for (k=0;k<4;++k)
		{
			ia1 = i+adj[k][0];
			ja1 = j+adj[k][1];
			if (ia1 <0 ||  ja1 <0 || ia1>=dim ||  ja1>=dim )
			{
				continue;
			}
			fa1 = pot[ia1][ja1];
			
			q = fabs(fa1);
			DEBUGX("coord %i %i val %f",ia1,ja1,q);
			if (q<qmax && q!=0)
			{

				qmax = q;
				dir.m_x = adj[k][0];
				dir.m_y = adj[k][1];
				b = true;
			}

		}
	}
	else
	{
		// debugging
		/*
		for (k=0;k<=20;k++)
		{
			for (ia1=0;ia1<=20;ia1++)
			{
				printf("%f ",*(m_pot->ind(k,ia1)));
			}
			printf("\n");
		}
		*/

		for (k=0;k<4;++k)
		{
			qmax =0;
			ia1 = i+adj[k][0];
			ja1 = j+adj[k][1];
			ia2 = i+adj[(k+1)%4][0];
			ja2 = j+adj[(k+1)%4][1];

			if (ia1 <0 || ia2<0 || ja1 <0 || ja2<0 || ia1>=dim || ia2>=dim || ja1>=dim || ja2>=dim)
			{
				continue;

			}


			id = i+diag[k][0];
			jd = j+diag[k][1];

			if (block[ia1][ja1]=='X' || block[ia2][ja2]=='X' || block[id][jd]=='X')
			{
				continue;
			}

			fa1 = pot[ia1][ja1];
			fa2 = pot[ia2][ja2];
			fd = pot[id][jd];


			if (fa1<f && fa2<f && fd<fa1 && fd < fa2)
			{
				f1 = (f-fa1)*adj[k][0]+(f-fa2)*adj[(k+1)%4][0];
				f2 = (f-fa1)*adj[k][1]+(f-fa2)*adj[(k+1)%4][1];

				q = f1*f1+f2*f2;
				if (q>qmax)
				{

					dir.m_x = f1;
					dir.m_y = f2;
					q=qmax;

				}
				b=true;
			}
		}

		if (!b)
		{
			for (k=0;k<4;++k)
			{
				ia1 = i+adj[k][0];
				ja1 = j+adj[k][1];

				if (ia1 <0 || ja1 <0 || ia1>=dim || ja1>=dim)
					continue;

				fa1 = pot[ia1][ja1];
				if (fa1 <f && block[ia1][ja1]!='X')
				{
					f1 = (f-fa1)*adj[k][0];
					f2 = (f-fa1)*adj[k][1];


					q = f1*f1+f2*f2;
					if (q>qmax)
					{
						dir.m_x = f1;
						dir.m_y = f2;
						qmax=q;
						b=true;
					}
				}
			}
		}
	}

	if (!b)
	{
		dir.m_x = 0;
		dir.m_y = 0;
	}

}


