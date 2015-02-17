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
 * Graph.cpp
 *
 *  Created on: Apr 14, 2013
 *      Author: Sam Skalicky
 */

#include "Graph.h"

long long unsigned opCount = 0;

int Types::Empty = 0;
int Types::Add = 1;
int Types::Mult = 2;
int Types::Div = 3;
int Types::Sub = 4;
int Types::Mod = 5;

int Types::Scale = 8;
int Types::eMult = 9;
int Types::Square = 10;

int Types::user = 20;

int Types::memOffset = 100;
int Types::Mem0 = 0;
int Types::Mem1 = memOffset;
int Types::Mem2 = memOffset*2;
int Types::Mem3 = memOffset*3;
int Types::Mem4 = memOffset*4;

int Types::getMemType(int val) {
	if(val >= Mem4)
		return Mem4/memOffset;
	else if(val >= Mem3)
		return Mem3/memOffset;
	else if(val >= Mem2)
		return Mem2/memOffset;
	else if(val >= Mem1)
		return Mem1/memOffset;
	else
		return 0;
}

int Types::getOpType(int val) {
	int tmp = val % Mem1;
	if(tmp == Empty)
		return Empty;
	else if(tmp == Add)
		return Add;
	else if(tmp == Mult)
		return Mult;
	else if(tmp == Div)
		return Div;
	else if(tmp == Sub)
		return Sub;
	else if(tmp == Mod)
		return Mod;
		
	else if(tmp == user)
		return user;
		
	else
		return -1;
}

int Types::setMemType(int val) {
	return memOffset * val;
}
