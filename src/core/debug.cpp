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

#include "debug.h"

void hexwrite(char* data,int len)
{
	unsigned char* ucp;
	char* cp;
	char a,b;
	int i=0;
	for (cp= data;cp <data+len;cp++)
	{
		ucp = (unsigned char*) cp;
		a= (*ucp)/16;
		b= (*ucp)%16;
		if (a>9)
			printf("%c",a-10+'a');
		else
			printf("%c",a+'0');

		if (b>9)
			printf("%c",b-10+'a');
		else
			printf("%c",b+'0');
			
		printf(" ");
		i++;
		if (i%4==0)
		{
			printf("| ");
		}
	
	}
	printf("\n");
}

