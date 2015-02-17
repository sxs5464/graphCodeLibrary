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
 * SparseMatrix.cpp
 *
 *  Created on: Mar 5, 2013
 *      Author: Sam Skalicky
 */

#include <iostream>
#include <vector>
#include <set>
#include <string>
#include <sstream>

#include <time.h>
#include <sys/time.h>
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>

#include "SparseSet.h"
#include "SparseMatrix.h"

using namespace std;

bool SparseMatrix::debug = false;

SparseMatrix::SparseMatrix() {//: row_cnts(r), col_cnts(c){
	r = 0;
	c = 0;

	if(debug) cout << "Creating SparseMatrix..." << endl;

	if(debug) cout << "Created SparseMatrix: " << r << "x" << c << endl;

	debug = true;
}

SparseMatrix::~SparseMatrix() {

}

void SparseMatrix::setData(long i, long j, int val) {
	SparseSet sset(i,j,val);

	//check and see if it exists yet
	set<SparseSet,SparseSetCompareRow, allocator<SparseSet> >::iterator oth = data_row.find(sset);
	if(oth == data_row.end()) {
		//it doesnt exist yet, add it
		data_row.insert(sset);
	}
	else {
		//it already exists, modify it
		oth->data = val;
	}
}

void SparseMatrix::setNew(long i, long j, int val) {
	SparseSet sset(i,j,val);

	if(i >= r)
		r = i+1;

	if(j >= c)
		c = j+1;

	//if(debug) cout << "------Adding new element:" << sset.toString() << "------" << endl;
	//put in row-major set
	data_row.insert(sset);
}


int SparseMatrix::get(long i, long j) {
	SparseSet s(i,j,0);
	set<SparseSet,SparseSetCompareRow, allocator<SparseSet> >::iterator oth = data_row.find(s);

	if(oth == data_row.end()) {
		return 0;
	}
	else {
		return (*oth).data;
	}
}

void SparseMatrix::displayFull() {
	for(unsigned long long i=0; i<r; i++) {
		for(unsigned long long j=0; j<c; j++) {
			int val = get(i,j);
			cout << val << " ";
		}
		cout << endl;
	}
}

string SparseMatrix::toString() {
	set<SparseSet>::iterator it;
	stringstream ss;
	ss << "Sparse Matrix " << r << "x" << c << ": ";
	for(it=data_row.begin(); it!=data_row.end(); it++) {
		ss << it->toString() << ", ";
	}

	return ss.str();
}
