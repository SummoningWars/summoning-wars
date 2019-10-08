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

#include "unionfindtree.h"

UnionFindTree::UnionFindTree(int dim)
{
	m_data = new int[dim];
	m_cardinality = new int[dim];
	m_dim = dim;
	
	for (int i=0; i<dim; i++)
	{
		m_data[i] = -1;
		m_cardinality[i] = 1;
	}
}

UnionFindTree::~UnionFindTree()
{
	delete[] m_data;
	delete[] m_cardinality;
}

int UnionFindTree::find(int index)
{
	// Weg vom Knoten bis zu Wurzel
	std::vector<int> path;
	path.reserve(10);
	
	int parent = m_data[index];
	int result = index;
	
	// Hinaufsteigen bis zu Wurzel
	while (parent != -1)
	{
		path.push_back(result);
		
		result = parent;
		parent = m_data[result];
	}
	
	// Pfadkompression
	std::vector<int>::iterator it;
	for (it = path.begin() ; it != path.end(); ++it)
	{
		m_data[*it] = result;
	}
	
	return result;
}

int UnionFindTree::getCardinality(int index)
{
	int root = find(index);
	return m_cardinality[root];
}

void UnionFindTree::merge(int index1, int index2)
{
	// Wurzeln der beiden Teilbaeume
	int root1 = find(index1);
	int root2 = find(index2);
	
	// nur vereinigen, wenn die Elemente in verschiedenen Mengen sind
	if (root1==root2)
		return;
	
	// Haenge den kleineren Teilbaum an den groesseren
	if (m_cardinality[root1] >m_cardinality[root2])
	{
		m_cardinality[root1] += m_cardinality[root2];
		m_data[root2] = root1;
	}
	else
	{
		m_cardinality[root2] += m_cardinality[root1];
		m_data[root1] = root2;
	}
	
}

void UnionFindTree::getElements(int index, std::list<int>& result)
{
	// Wurzel der Menge
	int root = find(index);
	
	// komplete Menge durchsuchen
	for (int i=0; i<m_dim; i++)
	{
		if (find(i) == root)
		{
			result.push_back(i);
		}
	}
}


/*
int main()
{
	UnionFindTree uf(10);
	
	char c=' ';
	int i1, i2;
	while (c!='x')
	{
		std::cout << "\nu: union  f: find  p: print  c: cardinality  x: exit\n";
		std::cin >> c;
		if (c=='u')
		{
			std::cin >> i1 >> i2;
			uf.merge(i1,i2);
		}
		else if (c=='f')
		{
			std::cin >> i1;
			std::cout << "\n root: "<<uf.find(i1)<<"\n";
		}
		else if (c=='c')
		{
			std::cin >> i1;
			std::cout << "\n cardinality: "<<uf.getCardinality(i1)<<"\n";
		}
		else if (c=='p')
		{
			uf.print();
		}
		else if (c=='s')
		{
			std::cin >> i1;
			std::list<int> res;
			uf.getElements(i1,res);
			std::list<int>::iterator it;
			for (it = res.begin(); it != res.end(); ++it)
			{
				std::cout << std::setw(3)<<*it;
			}
		}
		
	}
	
	return 0;
}
*/

