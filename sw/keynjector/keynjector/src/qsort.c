/*
* Copyright 2014 Jonas Hagmar
* 
* This file is part of keynjector.
* 
* keynjector is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 2 of the License, or
* (at your option) any later version.
* 
* keynjector is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with keynjector.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "qsort.h"

static void quicksort_internal(uint32_t *x, int first, int last) {
	int pivot,j,temp,i;

	if(first<last){
		pivot=first;
		i=first;
		j=last;

		while(i<j){
			while(x[i]<=x[pivot]&&i<last)
			i++;
			while(x[j]>x[pivot])
			j--;
			if(i<j){
				temp=x[i];
				x[i]=x[j];
				x[j]=temp;
			}
		}

		temp=x[pivot];
		x[pivot]=x[j];
		x[j]=temp;
		quicksort_internal(x,first,j-1);
		quicksort_internal(x,j+1,last);

	}
}

void quicksort(uint32_t *x, int size) {
	quicksort_internal(x, 0, size-1);
}
