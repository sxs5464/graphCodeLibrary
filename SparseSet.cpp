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
 * SparseSet.cpp
 *
 *  Created on: Mar 5, 2013
 *      Author: Sam Skalicky
 */

#include <string>
#include <sstream>
#include "SparseSet.h"

using namespace std;

SparseSet::SparseSet(long i_pos, long j_pos, int val) {
	i = i_pos;
	j = j_pos;
	data = val;
}

bool SparseSet::operator<(const SparseSet &oth) const {
	if(i < oth.i)
		return true;
	else if(i == oth.i) {
		if(j < oth.j)
			return true;
		else if(j == oth.j) {
			return false;
		}
		else
			return false;
	}
	else
		return false;
}

string SparseSet::toString() const {
	stringstream ss;
	ss << "Set{" << i << "," << j << "]=" << data;
	return ss.str();
}
