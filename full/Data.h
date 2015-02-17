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
 * Data.h
 *
 *  Created on: Apr 12, 2013
 *      Author: Sam Skalicky
 */

#include <iostream>
#include <string>
#include <fstream>
#include <set>
#include <sstream>

#include "Graph.h"
#include "SparseMatrix.h"
#include "SparseSet.h"

using namespace std;

#ifndef _DATA_
#define _DATA_

extern long long unsigned maxNodes;
extern long long unsigned currentNodes;
extern bool maxOverflow;
extern bool currentOverflow;

template <class T>
class Data {
public:
	bool calculated;
	bool read;
	long long unsigned node;
	static bool debug;

	Data() {
		calculated = false;
		read = false;
		node = 0;
		ID = count++;
		value = new T;
		if(debug) printf("Created Data #%llu\n",ID);
	}

	Data(T val) {
		calculated = false;
		read = true;
		node = 0;
		ID = count++;
		value = new T;
		*value = val;
		if(debug) printf("Created Data #%llu with value\n",ID);
	}

	Data(const Data &oth) {
		calculated = oth.calculated;
		read = oth.read;
		node = oth.node;
		ID = count++;
		value = new T;
		*value = *(oth.value);
		if(debug) printf("Created copy Data #%llu from Data #%llu\n",ID,oth.ID);
	}

	~Data() {
		if(debug) printf("Attempting to Destroy Data #%llu\n",ID);
		delete(value);
		if(debug) printf("Destroyed Data #%llu\n",ID);
	}

	void setData(Data &oth) {
		calculated = oth.calculated;
		read = oth.read;
		node = oth.node;
		ID = count++;
		value = new T;
		*value = *(oth.value);
	}

	Data operator-() {
		Data oth(*this);

		*oth.value = -(*(value));

		return oth;
	}

	Data& operator=(Data &d1) {
		if(debug) printf("Data #%llu = Data #%llu\n",ID,d1.ID);

		read = d1.read;
		calculated = d1.calculated;
		node = d1.node;

		value = new T;
		*value = *(d1.value);

		return *this;
	}

	Data& operator=(const Data &d1) {
		if(debug) printf("Data #%llu = Data #%llu\n",ID,d1.ID);

		read = d1.read;
		calculated = d1.calculated;
		node = d1.node;

		value = new T;
		*value = *(d1.value);

		return *this;
	}

	Data operator+(Data &d1) {
		Data oth(*this);
		oth.calculated = true;

		if(debug) printf("Data #%llu = Data #%llu + Data #%llu\n",oth.ID,ID,d1.ID);

		twoOperand(oth,d1);

		//set operation type
		matrix.setData(oth.node,oth.node,Types::Add + matrix.get(oth.node,oth.node));

		*oth.value = *(value) + *(d1.value);

		return oth;
	}

	/*
	 * This function is used for adding the results of intermediate calculations
	 */
	Data& operator+=(Data d1) {
		if(debug) printf("Data #%llu += Data #%llu\n",ID,d1.ID);
		calculated = true;
		oneOperand(d1);

		//set operation type
		matrix.setData(node,node,Types::Add + matrix.get(node,node));

		*value += *(d1.value);

		return *this;
	}

	/*
	 * This function is used for adding another defined variable
	 */
	Data& operator+=(Data &d1) {
		if(debug) printf("Data #%llu += Data #%llu\n",ID,d1.ID);
		calculated = true;

		oneOperand(d1);

		//set operation type
		matrix.setData(node,node,Types::Add + matrix.get(node,node));

		*value += *(d1.value);

		return *this;
	}

	Data operator-(Data &d1) {
		Data oth(*this);
		oth.calculated = true;

		if(debug) printf("Data #%llu = Data #%llu - Data #%llu\n",oth.ID,ID,d1.ID);

		twoOperand(oth,d1);

		//set operation type
		matrix.setData(oth.node,oth.node,Types::Sub + matrix.get(oth.node,oth.node));

		*oth.value = *(value) - *(d1.value);

		return oth;
	}

	Data& operator-=(Data &d1) {
		if(debug) printf("Data #%llu -= Data #%llu\n",ID,d1.ID);
		calculated = true;

		oneOperand(d1);

		//set operation type
		matrix.setData(node,node,Types::Sub + matrix.get(node,node));

		*value -= *(d1.value);

		return *this;
	}

	Data operator*(Data &d1) {
		Data oth(*this);
		oth.calculated = true;

		if(debug) printf("Data #%llu = Data #%llu * Data #%llu\n",oth.ID,ID,d1.ID);

		twoOperand(oth,d1);

		//set operation type
		int val = Types::Mult + matrix.get(oth.node,oth.node);
		if(debug) printf("Node info: %d\n",val);
		matrix.setData(oth.node,oth.node,val);

		*oth.value = *(value) * *(d1.value);

		return oth;
	}

	Data& operator*=(const Data &d1) {
		if(debug) printf("Data #%llu *= Data #%llu\n",ID,d1.ID);
		calculated = true;

		oneOperand(d1);

		//set operation type
		matrix.setData(node,node,Types::Mult + matrix.get(node,node));

		*value *= *(d1.value);

		return *this;
	}

	Data operator/(Data &d1) {
		Data oth(*this);
		oth.calculated = true;

		if(debug) printf("Data #%llu = Data #%llu / Data #%llu\n",oth.ID,ID,d1.ID);

		twoOperand(oth,d1);

		//set operation type
		matrix.setData(oth.node,oth.node,Types::Div + matrix.get(oth.node,oth.node));
		if(*d1.value != 0)
			*(oth.value) = *(value) / *(d1.value);

		return oth;
	}

	Data& operator/=(const Data &d1) {
		if(debug) printf("Data #%llu /= Data #%llu\n",ID,d1.ID);
		calculated = true;

		oneOperand(d1);

		//set operation type
		matrix.setData(node,node,Types::Div + matrix.get(node,node));

		*value /= *(d1.value);

		return *this;
	}

	Data operator%(Data &d1) {
		Data oth(*this);
		oth.calculated = true;

		if(debug) printf("Data #%llu = Data #%llu % Data #%llu\n",oth.ID,ID,d1.ID);

		twoOperand(oth,d1);

		//set operation type
		matrix.setData(oth.node,oth.node,Types::Mod + matrix.get(oth.node,oth.node));

		*oth.value = *(value) % *(d1.value);

		return oth;
	}

	Data& operator%=(const Data &d1) {
		if(debug) printf("Data #%llu + Data #%llu\n",ID,d1.ID);
		calculated = true;

		oneOperand(d1);

		//set operation type
		matrix.setData(node,node,Types::Mod + matrix.get(node,node));

		*value %= *(d1.value);

		return this;
	}

	bool operator<(const Data &d1) {
		return *value < *d1.value;
	}

	bool operator<=(const Data &d1) {
		return *value <= *d1.value;
	}

	bool operator>(const Data &d1) {
		return *value > *d1.value;
	}

	bool operator>=(const Data &d1) {
		return *value >= *d1.value;
	}

	bool operator==(const Data &d1) {
		return *value == *d1.value;
	}

	bool operator!=(const Data &d1) {
		return *value != *d1.value;
	}

	string str() {
		stringstream ss("");
		ss << "Data#";
		ss << ID;
		ss << " node: " << node << " value: " << *value;
		return ss.str();
	}

	static void printResult() {
		for(long unsigned i=0; i<matrix.r; i++) {
			for(long unsigned j=0; j<matrix.c; j++) {
				printf("%d ",matrix.get(i,j));
			}
			printf("\n");
		}
	}

	static void printStats() {
		if(currentOverflow)
			cout << "Current Nodes: OVERFLOW!" << endl;
		else
			cout << "Current Nodes: " << currentNodes << endl;

		if(maxOverflow)
			cout << "Max Nodes: OVERFLOW!" << endl;
		else
			cout << "Max Nodes: " << maxNodes << endl;
	}

	static void writeSparseMatrix(string filename) {
		ofstream myfile;
		myfile.open (filename.c_str());
		//write size
		myfile << opCount << "\n";
		set<SparseSet,SparseSetCompareRow, allocator<SparseSet> >::iterator it;

		for(it=matrix.data_row.begin(); it != matrix.data_row.end(); it++) {
			myfile << it->i << " " << it->j << " " << it->data << "\n";
		}

		myfile.close();
	}

	static void writeMatrix(string filename) {
		ofstream myfile;
		myfile.open (filename.c_str());

		for(long unsigned i=0; i<opCount; i++) {
			for(long unsigned j=0; j<opCount; j++) {
				myfile << matrix.get(i,j) << " ";
			}
			myfile << "\n";
		}
		myfile.close();
	}

	static void writeResult(string filename) {
	  string file = filename + ".txt";
	  writeMatrix(file);
	}

private:
	T *value;
	long long unsigned ID;
	static long long unsigned count;
	static SparseMatrix matrix;

	void oneOperand(Data &d1) {

		//initialize number of memory accesses
		int mem = 0;

		//set node number
		long unsigned tmpNode = opCount++;

		//check first operand
		if(!calculated) {	//this variable has just been created (ie. memory access)
			if(!read) {	//variable has not already been read into cache (ie. memory not already accessed)
				if(debug) printf("Adding memory access (Data#%llu) for Op#%llu\n",ID,tmpNode);
				mem++;

				read = true;
			}
		}
		else {	//this variable is the result of some other operation (ie. previous Op)
			if(debug) printf("Creating edge between Op#%llu and Op#%llu\n",node,tmpNode);
			matrix.setNew(node,tmpNode,1);
		}

		//check second operand
		if(!d1.calculated) {	//this variable has just been created (ie. memory access)
			if(!d1.read) {	//variable has not already been read into cache (ie. memory not already accessed)
				if(debug) printf("Adding memory access (Data#%llu) for Op#%llu\n",d1.ID,tmpNode);
				mem++;

				d1.read = true;
			}
		}
		else {	//this variable is the result of some other operation (ie. previous Op)
			if(debug) printf("Creating edge between Op#%llu and Op#%llu\n",d1.node,tmpNode);
			matrix.setNew(d1.node,tmpNode,1);
		}

		//set memory accesses
		matrix.setNew(tmpNode,tmpNode,Types::setMemType(mem));

		//set this variable as a calculated variable (ie. not a memory access, has dependencies)
		calculated = true;

		//set the op node number
		node = tmpNode;

		//cout << "Created node: " << node << endl;
	}

	void twoOperand(Data &oth, Data &d1) {
		//set this variable as a calculated variable (ie. not a memory access, has dependencies)
		oth.calculated = true;

		//initialize number of memory accesses
		int mem = 0;

		//set node number
		oth.node = opCount++;

		//check first operand
		if(!calculated) {	//this variable has just been created (ie. memory access)
			if(!read) {	//variable has not already been read into cache (ie. memory not already accessed)
				if(debug) printf("Adding memory access (Data#%llu) for Op#%llu\n",ID,oth.node);
				mem++;

				read = true;
			}
			else
				if(debug) printf("Data#%llu already accessed for Op#%llu\n",ID,oth.node);
		}
		else {	//this variable is the result of some other operation (ie. previous Op)
			if(debug) printf("Creating edge between Op#%llu and Op#%llu\n",node,oth.node);
			matrix.setNew(node,oth.node,1);
		}

		//check second operand
		if(!d1.calculated) {	//this variable has just been created (ie. memory access)
			if(!d1.read) {	//variable has not already been read into cache (ie. memory not already accessed)
				if(debug) printf("Adding memory access (Data#%llu) for Op#%llu\n",d1.ID,oth.node);
				mem++;

				d1.read = true;
			}
			else
				if(debug) printf("Data#%llu already accessed for Op#%llu\n",d1.ID,oth.node);
		}
		else {	//this variable is the result of some other operation (ie. previous Op)
			if(debug) printf("Creating edge between Op#%llu and Op#%llu\n",d1.node,oth.node);
			matrix.setNew(d1.node,oth.node,1);
		}

		//set memory accesses
		matrix.setNew(oth.node,oth.node,Types::setMemType(mem));

		//cout << "Created node: " << oth.node << endl;
	}
};

template <class T>
SparseMatrix Data<T>::matrix;

template <class T>
long long unsigned Data<T>::count = 0;

template <class T>
bool Data<T>::debug = true;

#endif

