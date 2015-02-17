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
 * Matrix.h
 *
 *  Created on: Apr 15, 2013
 *      Author: Sam Skalicky
 */

#include <iostream>

using namespace std;

template <class T>
class Matrix {
public:
	long unsigned row;
	long unsigned col;
	static bool debug;

	Matrix() {
	  ID = count++;
	  row = 0;
	  col = 0;
		if(debug) printf("Created empty Matrix #%ld\n",ID);
	}

	Matrix(long unsigned r, long unsigned c) {
		ID = count++;
		row = r;
		col = c;

		for(int i=0; i<row; i++) {
		  data.push_back(vector<T>());
		  for(int j=0; j<col; j++)  {
		    data[i].push_back(0);
		  }
		}

		if(debug) printf("Created Matrix with zeros #%ld\n",ID);
	}

	Matrix(T* dat, long unsigned r, unsigned long c) {
		ID = count++;
		row = r;
		col = c;
	
		for(int i=0; i<row; i++) {
		  data.push_back(vector<T>());
	
		  for(int j=0; j<col; j++)  {
		    data[i].push_back(dat[i*row+j]);
		  }
		}
		if(debug) printf("Created Matrix with data #%ld\n",ID);
	}

	Matrix(Matrix& oth) {
	  ID = count++;
	  row = oth.row;
	  col = oth.col;

	  if(oth.data.size() > 0) {
		for(int i=0; i<row; i++) {
		  data.push_back(vector<T>());
		  for(int j=0; j<col; j++)  {
		    data[i].push_back(oth.data[i][j]);
		  }
		}
		if(debug) printf("Created copied Matrix with data #%ld\n",ID);
	  }
	}

	Matrix(const Matrix& oth) {
	  ID = count++;
	  row = oth.row;
	  col = oth.col;

	  if(oth.data.size() > 0) {
		for(int i=0; i<row; i++) {
		  data.push_back(vector<T>());
		  for(int j=0; j<col; j++)  {
		    data[i].push_back(oth.data[i][j]);
		  }
		}
		if(debug) printf("Created copied Matrix with data #%ld\n",ID);
	  }
	}

	~Matrix() {
		if(debug) printf("Attempting to Destroy Matrix #%ld\n",ID);
		if(debug) printf("Destroyed Matrix #%ld\n",ID);
	}

	T& get(const int i, const int j) {
		return data[i][j];
	}

	vector<T> operator[](const int index) {
		if(index >= 0 && (unsigned)index < row) {
		  vector<T> tmp = data[index];
		  return tmp;
		}
		else {
		  cerr << "Index out of bounds! Index: " << index << ", Length: " << row << endl;
		  return NULL;
		}
	}

	void setSize(long unsigned  rows, long unsigned cols) {
	  for(int i=row; i<rows; i++) {
	    data.push_back(vector<T>());
	    for(int j=col; j<cols; j++)  {
	      data[i].push_back(0);
	    }
	  }
	  row = rows;
	  col = cols;
	}

	void addRow() {
	  data.push_back(vector<T>());
	  row++;
	}

	void addValue(T val) {
	  if(row == 0) {
	    data.push_back(vector<T>());
	    row++;
	  }

	  data[data.size()-1].insert((data[data.size()-1]).begin(),val);
	  col++;
	}

	int numRows() {
	  return row;
	}

	int numCols() {
	  return col;
	}

private:
	vector<vector<T> > data;
	long unsigned ID;
	static long unsigned count;
};

template <class T>
long unsigned Matrix<T>::count = 0;

template <class T>
bool Matrix<T>::debug = false;
