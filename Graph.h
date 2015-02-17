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
 * Graph.h
 *
 *  Created on: Apr 14, 2013
 *      Author: Sam Skalicky
 */


#ifndef _GRAPH_
#define _GRAPH_

extern long long unsigned opCount;

class Types {
public:
	static int Empty;
	static int Add;
	static int Mult;
	static int Div;
	static int Sub;
	static int Mod;
	
	static int Scale;
	static int eMult;
	static int Square;
    
    static int user;

	static int getMemType(int val);
	static int getOpType(int val);
	static int setMemType(int val);

private:
	static int memOffset;
	static int Mem0;
	static int Mem1;
	static int Mem2;
	static int Mem3;
	static int Mem4;
};


#endif


