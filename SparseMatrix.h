/*
 * This file is part of the GraphCodeLibrary.
 * 
 * GraphCodeLibrary is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * GraphCodeLibrary is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with GraphCodeLibrary.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * SparseMatrix.h
 *
 *  Created on: Mar 5, 2013
 *      Author: Sam Skalicky
 */

#include <vector>
#include <set>
#include <list>
#include <string>

#include <stdio.h>

#include "SparseSet.h"

#ifndef _SPARSEMATRIX
#define _SPARSEMATRIX

using namespace std;

struct SparseSetCompareCol {
	bool operator() (const SparseSet *o1, const SparseSet *o2) const {
		if(o1->j < o2->j)
			return true;
		else if(o1->j == o2->j) {
			if(o1->i < o2->i)
				return true;
			else if(o1->i == o2->i) {
				return false;
			}
			else
				return false;
		}
		else
			return false;
	}
};

struct SparseSetCompareRow {
	bool operator() (const SparseSet o1, const SparseSet o2) const {
		if(o1.i < o2.i)
			return true;
		else if(o1.i == o2.i) {
			if(o1.j < o2.j)
				return true;
			else if(o1.j == o2.j) {
				return false;
			}
			else
				return false;
		}
		else
			return false;
	}
};

class SparseMatrix {
public:
	unsigned long long r,c;
	set<SparseSet,SparseSetCompareRow, allocator<SparseSet> > data_row;
	string filename;
	static bool debug;

	SparseMatrix();
	~SparseMatrix();
	void setNew(long i, long j, int val);
	void setData(long i, long j, int val);
	int get(long i, long j);
	void displayFull();
	string toString();
};

#endif
