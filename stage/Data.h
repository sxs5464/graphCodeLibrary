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
 *  Created on: May 6, 2013
 *      Author: Sam Skalicky
 */
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <vector>
#include <map>

#include "Graph.h"

using namespace std;

#ifndef _Data_
#define _Data_

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

		value = new T;

		//increment global node counters
		if(!currentOverflow)
			currentNodes++;

		if(!maxOverflow && currentNodes > maxNodes)
			maxNodes = currentNodes;
	}

	Data(T val) {
		calculated = false;
		read = true;
		node = 0;

		value = new T;
		*value = val;

		//increment global node counters
		if(!currentOverflow)
			currentNodes++;

		if(!maxOverflow && currentNodes > maxNodes)
			maxNodes = currentNodes;
	}

	Data(const Data &oth) {
		calculated = oth.calculated;
		read = oth.read;
		node = oth.node;

		value = new T;
		*value = *(oth.value);

		//increment global node counters
		if(!currentOverflow)
			currentNodes++;

		if(!maxOverflow && currentNodes > maxNodes)
			maxNodes = currentNodes;
	}

	~Data() {
		delete(value);

		//decrement global node counters
		if(!currentOverflow)
			currentNodes--;
	}

	Data operator-() {
		Data oth(*this);

		*oth.value = -(*(value));

		return oth;
	}

	Data& operator=(Data &d1) {

		read = d1.read;
		calculated = d1.calculated;

		node = d1.node;

		value = new T;
		*value = *(d1.value);

		return *this;
	}

	Data& operator=(const Data &d1) {

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

		twoOperand(oth,d1);

		*oth.value = *(value) + *(d1.value);

		return oth;
	}

	/*
	 * This function is used for adding the results of intermediate calculations
	 */
	Data& operator+=(Data d1) {
		calculated = true;
		oneOperand(d1);

		*value += *(d1.value);

		return *this;
	}

	/*
	 * This function is used for adding another defined variable
	 */
	Data& operator+=(Data &d1) {
		calculated = true;

		oneOperand(d1);

		*value += *(d1.value);

		return *this;
	}

	Data operator-(Data &d1) {
		Data oth(*this);
		oth.calculated = true;

		twoOperand(oth,d1);

		*oth.value = *(value) - *(d1.value);

		return oth;
	}

	Data& operator-=(const Data &d1) {
		calculated = true;

		oneOperand(d1);

		*value -= *(d1.value);

		return *this;
	}

	Data operator*(Data &d1) {
		Data oth(*this);
		oth.calculated = true;

		twoOperand(oth,d1);

		*oth.value = *(value) * *(d1.value);

		return oth;
	}

	Data& operator*=(const Data &d1) {
		calculated = true;

		oneOperand(d1);

		*value *= *(d1.value);

		return *this;
	}

	Data operator/(Data &d1) {
		Data oth(*this);
		oth.calculated = true;

		twoOperand(oth,d1);

		if(*d1.value != 0)
			*(oth.value) = *(value) / *(d1.value);

		return oth;
	}

	Data& operator/=(const Data &d1) {
		calculated = true;

		oneOperand(d1);

		*value /= *(d1.value);

		return *this;
	}

	Data operator%(Data &d1) {
		Data oth(*this);
		oth.calculated = true;

		twoOperand(oth,d1);

		*oth.value = *(value) % *(d1.value);

		return oth;
	}

	Data& operator%=(const Data &d1) {
		calculated = true;

		oneOperand(d1);

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
		ss << "DataStage#";
		ss << " node: " << node << " value: " << *value;
		return ss.str();
	}

	static void writeOpStages(string filename) {
		ofstream myfile;
		myfile.open (filename.c_str());

		for(unsigned i=0; i<opStages.size(); i++) {
			myfile << opStages[i] << "\n";
		}
		myfile.close();
	}

	static void writeMemStages(string filename) {
		ofstream myfile;
		myfile.open (filename.c_str());

		for(unsigned i=0; i<memStages.size(); i++) {
			myfile << memStages[i] << "\n";
		}
		myfile.close();
	}

	static void writeResult(string filename) {
	  string op = filename + "_op.txt";
	  string mem = filename + "_mem.txt";
	  writeOpStages(op);
	  writeMemStages(mem);
	}

	static void printOpStages() {
		printf("OpStages:\n");
		for(unsigned i=0; i<opStages.size(); i++) {
			printf("Stage(%u) = %llu\n",i,opStages[i]);
		}
	}

	static void printMemStages() {
		printf("MemStages:\n");
		for(unsigned i=0; i<memStages.size(); i++) {
			printf("Stage(%u) = %llu\n",i,memStages[i]);
		}
	}

	static void printResult() {
	  printOpStages();
	  printMemStages();
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

private:
	T *value;
	static long long unsigned count;
	static vector<long long unsigned> opStages;
	static vector<long long unsigned> memStages;
	void oneOperand(Data &d1) {

		//initialize number of memory accesses
		int mem = 0;

		//index of this node
		long unsigned index = 0;

		//check first operand
		if(!calculated) {	//this variable has just been created (ie. memory access)
			if(!read) {	//variable has not already been read into cache (ie. memory not already accessed)
				mem++;

				read = true;
			}
		}
		else {	//this variable is the result of some other operation (ie. previous Op)
			index = node+1;
		}

		//check second operand
		if(!d1.calculated) {	//this variable has just been created (ie. memory access)
			if(!d1.read) {	//variable has not already been read into cache (ie. memory not already accessed)
				mem++;

				d1.read = true;
			}
		}
		else {	//this variable is the result of some other operation (ie. previous Op)
			if(index <= d1.node)
				index = d1.node+1;
		}

		//set this variable as a calculated variable (ie. not a memory access, has dependencies)
		calculated = true;

		//set op
		//store current nodes stage
		node = index;

		//make sure we're not adding to a new stage
		while(opStages.size() <= index){opStages.push_back(0);}

		//add the node to the stage
		opStages[index]++;

		//make sure we're not adding to a new stage
		while(memStages.size() <= index){memStages.push_back(0);}

		//set memory accesses
		memStages[index]+=mem;
	}

	void twoOperand(Data &oth, Data &d1) {
		//set this variable as a calculated variable (ie. not a memory access, has dependencies)
		oth.calculated = true;

		//initialize number of memory accesses
		int mem = 0;

		//index of this node
		long long unsigned index = 0;

		//check first operand
		if(!calculated) {	//this variable has just been created (ie. memory access)
			if(!read) {	//variable has not already been read into cache (ie. memory not already accessed)
				mem++;

				read = true;
			}
		}
		else {	//this variable is the result of some other operation (ie. previous Op)
			index = node+1;
		}

		//check second operand
		if(!d1.calculated) {	//this variable has just been created (ie. memory access)
			if(!d1.read) {	//variable has not already been read into cache (ie. memory not already accessed)
				mem++;

				d1.read = true;
			}
		}
		else {	//this variable is the result of some other operation (ie. previous Op)
			if(index <= d1.node)
				index = d1.node+1;
		}

		//set op
		//store current nodes stage
		oth.node = index;

		//make sure we're not adding to a new stage
		while(opStages.size() <= index){opStages.push_back(0);}

		//add the node to the stage
		opStages[index]++;

		//make sure we're not adding to a new stage
		while(memStages.size() <= index){memStages.push_back(0);}

		//set memory accesses
		memStages[index]+=mem;
	}
};

template <class T>
vector<long long unsigned> Data<T>::opStages;

template <class T>
vector<long long unsigned> Data<T>::memStages;

template <class T>
bool Data<T>::debug = false;

#endif

