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
#include <vector>
#include <map>

#include "Graph.h"
#include "Matrix.h"
#include "SparseMatrix.h"
#include "SparseSet.h"

using namespace std;

#ifndef _DATA_
#define _DATA_

extern long long unsigned maxNodes;
extern long long unsigned currentNodes;
extern bool maxOverflow;
extern bool currentOverflow;

extern long long unsigned opCount;

/* User Provided function names map */
extern std::map<std::string, int> create_map();

class Data;

typedef struct multiple_d {
    Data *ptr;
    unsigned char qty;
} multiple;

class Data {
public:
	bool calculated;
	bool read;
	long long unsigned node;
	static bool debug;
	Matrix<double> mat;
	string name;

	Data() {
		calculated = false;
		read = false;
		node = 0;
		ID = count++;
		if(debug) printf("Created Data #%llu\n",ID);
	}
	
	Data(Data& oth) {
		calculated = oth.calculated;
		read = oth.read;
		node = 0;
		ID = count++;
		mat = Matrix<double>(oth.mat);
		name = oth.name;
		if(debug) printf("Created Data #%llu from copy of %llu with name: %s\n",ID,oth.ID,name.c_str());
	}

	Data(const Data& oth) {
		calculated = oth.calculated;
		read = oth.read;
		node = 0;
		ID = count++;
		mat = Matrix<double>(oth.mat);
		name = oth.name;
		if(debug) printf("Created Data #%llu from copy of %llu with name: %s\n",ID,oth.ID,name.c_str());
	}
	
	Data(double data) {
		calculated = false;
		read = true;
		node = 0;
		ID = count++;
		mat.addValue(data);
		if(debug) printf("Created Data #%llu with value\n",ID);
	}
	
	Data(double data, string n) {
		calculated = false;
		read = true;
		node = 0;
		ID = count++;
		mat.addValue(data);
		name = n;
		if(debug) printf("Created Data #%llu with value and name: %s\n",ID,name.c_str());
	}

	Data(double* data, int rows, int cols) {
		calculated = false;
		read = true;
		node = 0;
		ID = count++;
		mat = Matrix<double>(data,rows,cols);
		if(debug) printf("Created Data #%llu with value\n",ID);
	}
	
	Data(double* data, int rows, int cols, string n) {
		calculated = false;
		read = true;
		node = 0;
		ID = count++;
		mat = Matrix<double>(data,rows,cols);
		name = n;
		if(debug) printf("Created Data #%llu with value and name: %s\n",ID,name.c_str());
	}

	~Data() {
		if(debug) printf("Attempting to Destroy Data #%llu\n",ID);
		if(debug) printf("Destroyed Data #%llu\n",ID);
	}

	Data operator-() {
		Data oth(*this);

		return oth;
	}
	
	/*
	 * const is needed for intermediary object values
	 * Example:
	 *		c = a + b 
	 * 		is equivalent to tmp = a + b; c = tmp;
	 * So the compiler will treat the tmp variable as const during the assignment
	 * and it will not exist after the assignment (memory will be freed)
	 */
	Data& operator=(const Data &d1) {
		if(debug) printf("Data #%llu = Data #%llu\n",ID,d1.ID);

		read = d1.read;
		calculated = d1.calculated;
		node = d1.node;
		name = d1.name;

		return *this;
	}

	Data& operator=(Data &d1) {
		if(debug) printf("Data #%llu = Data #%llu\n",ID,d1.ID);

		read = d1.read;
		calculated = d1.calculated;
		node = d1.node;
        name = d1.name;

		return *this;
	}

	Data operator+(int increment) {
	  Data oth(*this);
	  oth.calculated = true;

	  Data d1(increment);
	  d1.read = true;
	  d1.calculated = false;

	  if(debug) printf("Data #%llu = Data #%llu + Data #%llu\n",oth.ID,ID,d1.ID);
	  
	  twoOperand(oth,d1);

	  //set operation type
	  matrix.setData(oth.node,oth.node,Types::Add + matrix.get(oth.node,oth.node));
	  
	  return oth;
	}

	Data operator+(Data &d1) {
		Data oth(*this);
		oth.calculated = true;

		if(debug) printf("Data #%llu = Data #%llu + Data #%llu\n",oth.ID,ID,d1.ID);

		twoOperand(oth,d1);

		//set operation type
		matrix.setData(oth.node,oth.node,Types::Add + matrix.get(oth.node,oth.node));

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

		return *this;
	}

	Data operator-(Data &d1) {
		Data oth(*this);
		oth.calculated = true;

		if(debug) printf("Data #%llu = Data #%llu - Data #%llu\n",oth.ID,ID,d1.ID);

		twoOperand(oth,d1);

		//set operation type
		matrix.setData(oth.node,oth.node,Types::Sub + matrix.get(oth.node,oth.node));

		return oth;
	}

	Data& operator-=(Data &d1) {
		if(debug) printf("Data #%llu -= Data #%llu\n",ID,d1.ID);
		calculated = true;

		oneOperand(d1);

		//set operation type
		matrix.setData(node,node,Types::Sub + matrix.get(node,node));

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

		return oth;
	}

	Data& operator*=(Data &d1) {
		if(debug) printf("Data #%llu *= Data #%llu\n",ID,d1.ID);
		calculated = true;

		oneOperand(d1);

		//set operation type
		matrix.setData(node,node,Types::Mult + matrix.get(node,node));

		return *this;
	}

	Data operator/(Data &d1) {
		Data oth(*this);
		oth.calculated = true;

		if(debug) printf("Data #%llu = Data #%llu / Data #%llu\n",oth.ID,ID,d1.ID);

		twoOperand(oth,d1);

		//set operation type
		matrix.setData(oth.node,oth.node,Types::Div + matrix.get(oth.node,oth.node));
	
		return oth;
	}

	Data& operator/=( Data &d1) {
		if(debug) printf("Data #%llu /= Data #%llu\n",ID,d1.ID);
		calculated = true;

		oneOperand(d1);

		//set operation type
		matrix.setData(node,node,Types::Div + matrix.get(node,node));

		return *this;
	}

	Data operator%(Data &d1) {
		Data oth(*this);
		oth.calculated = true;

		if(debug) printf("Data #%llu = Data #%llu %% Data #%llu\n",oth.ID,ID,d1.ID);

		twoOperand(oth,d1);

		//set operation type
		matrix.setData(oth.node,oth.node,Types::Mod + matrix.get(oth.node,oth.node));

		return oth;
	}
	
    static void func(multiple *inputs, multiple *outputs, const char *fname) {
        printf("function: %s with %d inputs and %d outputs\n",fname,inputs->qty,outputs->qty);
        multipleIOs(inputs, outputs, fname);
    }

	bool operator<( Data &d1) {
		return false;
	}

	bool operator<=( Data &d1) {
		return false;
	}

	bool operator>( Data &d1) {
		return false;
	}

	bool operator>=( Data &d1) {
		return false;
	}

	bool operator==( Data &d1) {
		return false;
	}

	bool operator!=( Data &d1) {
		return false;
	}

	string str() {
		stringstream ss("");
		ss << "Data#";
		ss << ID;
		ss << " node: " << node;
		return ss.str();
	}

	static void printMatrix() {
		for(long unsigned i=0; i<matrix.r; i++) {
			for(long unsigned j=0; j<matrix.c; j++) {
				printf("%d ",matrix.get(i,j));
			}
			printf("\n");
		}
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
	
	static void printInputData() {
		for(long unsigned i=0; i<inputData.size(); i++) {
			for(long unsigned j=0; j<inputData[i].size(); j++) {
				printf("%s ",inputData[i][j].c_str());
			}
			printf("\n");
		}
	}
	
	static void writeInputData(string filename) {
		ofstream myfile;
		myfile.open (filename.c_str());

		for(long unsigned i=0; i<inputData.size(); i++) {
			for(long unsigned j=0; j<inputData[i].size(); j++) {
				myfile << inputData[i][j] << " ";
			}
			myfile << "\n";
		}
		myfile.close();
	}

private:
	long long unsigned ID;
	static long long unsigned count;
	static SparseMatrix matrix;
	static vector<vector<string> > inputData;
    static map<string,int> funcNames;

	void oneOperand(Data &d1) {

		//initialize number of memory accesses
		int mem = 0;

		//set node number
		long long unsigned tmpNode = opCount++;
		if(debug) printf("Opcount: %llu\n",opCount);

		//check first operand
		if(!calculated) {	//this variable has just been created (ie. memory access)
			if(!read) {	//variable has not already been read into cache (ie. memory not already accessed)
				if(debug) printf("Adding memory access (Data#%llu) for Op#%llu\n",ID,tmpNode);
				mem++;

				read = true;
			}			
			else
				if(debug) printf("1 Data#%llu already accessed for Op#%llu\n",ID,d1.node);
		}
		else {	//this variable is the result of some other operation (ie. previous Op)
			if(debug) printf("1 Creating edge between Op#%llu and Op#%llu\n",node,tmpNode);
			matrix.setNew(node,tmpNode,1);
		}

		//check second operand
		if(!d1.calculated) {	//this variable has just been created (ie. memory access)
			if(!d1.read) {	//variable has not already been read into cache (ie. memory not already accessed)
				if(debug) printf("Adding memory access (Data#%llu) for Op#%llu\n",d1.ID,tmpNode);
				mem++;

				d1.read = true;
			}
			else
				if(debug) printf("2 Data#%llu already accessed for Op#%llu\n",ID,d1.node);
		}
		else if(d1.node != tmpNode && d1.node != 0) {	//this variable is the result of some other operation (ie. previous Op)
			if(debug) printf("2 Creating edge between Op#%llu and Op#%llu\n",d1.node,tmpNode);
			matrix.setNew(d1.node,tmpNode,1);
		}

		//set memory accesses
		matrix.setNew(tmpNode,tmpNode,Types::setMemType(mem));
		
		inputData.push_back(vector<string>());
		if(d1.name.length() == 0) {
			//this variable is not from input data (has no name)
			stringstream ss;
			ss << d1.node;
			inputData[inputData.size()-1].push_back(ss.str());
		}
		else {
			inputData[inputData.size()-1].push_back(d1.name);
		}

		//set this variable as a calculated variable (ie. not a memory access, has dependencies)
		calculated = true;

		//set the op node number
		d1.node = tmpNode;

		//cout << "Created node: " << node << endl;
	}

	void twoOperand(Data &oth, Data &d1) {
		//set this variable as a calculated variable (ie. not a memory access, has dependencies)
		oth.calculated = true;

		//initialize number of memory accesses
		int mem = 0;

		//set node number
		oth.node = opCount++;
		if(debug) printf("Opcount: %llu\n",opCount);

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
		
		//add names to inputData
		inputData.push_back(vector<string>());
		if(name.length() == 0) {
			//this variable is not from input data (has no name)
			stringstream ss;
			ss << node;
			inputData[inputData.size()-1].push_back(ss.str());
		}
		else {
			inputData[inputData.size()-1].push_back(name);
		}
		
		if(d1.name.length() == 0) {
			//this variable is not from input data (has no name)
			stringstream ss;
			ss << d1.node;
			inputData[inputData.size()-1].push_back(ss.str());
		}
		else {
			inputData[inputData.size()-1].push_back(d1.name);
		}

		//cout << "Created node: " << oth.node << endl;
	}
    
    static void multipleIOs(multiple* inputs, multiple *outputs, const char *fname) {
        
		//initialize number of memory accesses
		int mem = 0;
        
		//set node number
		long long unsigned tmpNode = opCount++;
		if(debug) printf("Opcount: %llu\n",opCount);
        
        //update input file
        inputData.push_back(vector<string>());
        
		//check input operands
        int i;
        for(i=0; i<inputs->qty; i++) {
            if(!inputs->ptr[i].calculated) {	//this variable has just been created (ie. memory access)
                if(!inputs->ptr[i].read) {	//variable has not already been read into cache (ie. memory not already accessed)
                    if(debug) printf("Adding memory access (Data#%llu) for Op#%llu\n",inputs->ptr[i].ID,tmpNode);
                    mem++;
                    
                    inputs->ptr[i].read = true;
                }
                else
                    if(debug) printf("1 Data#%llu already accessed for Op#%llu\n",inputs->ptr[i].ID,inputs->ptr[i].node);
            }
            else {	//this variable is the result of some other operation (ie. previous Op)
                if(debug) printf("1 Creating edge between Op#%llu and Op#%llu\n",inputs->ptr[i].node,tmpNode);
                matrix.setNew(inputs->ptr[i].node,tmpNode,1);
            }
            
            //add input to input file
            if(inputs->ptr[i].name.length() == 0) {
                //this variable is not from input data (has no name)
                stringstream ss;
                ss << inputs->ptr[i].node;
                inputData[inputData.size()-1].push_back(ss.str());
            }
            else {
                inputData[inputData.size()-1].push_back(inputs->ptr[i].name);
            }
        }
        
		//set memory accesses & operation type
		matrix.setNew(tmpNode,tmpNode,Types::setMemType(mem) + (Types::user + funcNames[fname]));
        
        //setup output operands
        for(i=0; i<outputs->qty; i++) {
            //set this variable as a calculated variable (ie. not a memory access, has dependencies)
            outputs->ptr[i].calculated = true;
            
            //set the op node number
            outputs->ptr[i].node = tmpNode;
        }
        
		//cout << "Created node: " << node << endl;
	}
};

SparseMatrix Data::matrix;

vector<vector<string> > Data::inputData;

long long unsigned Data::count = 0;

bool Data::debug = false;

map<string,int> Data::funcNames = create_map();

#endif

