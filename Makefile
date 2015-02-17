# This file is part of the GraphCodeLibrary.
# 
# GraphCodeLibrary is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# GraphCodeLibrary is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
# 
# You should have received a copy of the GNU Lesser General Public License
# along with GraphCodeLibrary.  If not, see <http://www.gnu.org/licenses/>.
#
# GraphCodeLibrary Makefile
#
#  Created on: Feb 15, 2015
#      Author: Sam Skalicky
# 

.PHONY: all full stage matrix clean

all:
	@echo "full - compile and produce executable to construct the full graph"
	@echo "stage - compile and produce executable to construct the compressed graph"
	@echo "all - compile and produce executable to construct the full graph using a Matrix as the base class"

full:
	g++ -c SparseMatrix.cpp SparseSet.cpp Graph.cpp full/Data.cpp -I. -Ifull
	ar -cvq libGCLfull.a SparseMatrix.o SparseSet.o Graph.o Data.o
	rm -rf SparseMatrix.o
	rm -rf SparseSet.o
	rm -rf Graph.o
	rm -rf Data.o

stage:
	g++ -c SparseMatrix.cpp SparseSet.cpp Graph.cpp stage/Data.cpp -I. -Istage
	ar -cvq libGCLstage.a SparseMatrix.o SparseSet.o Graph.o Data.o
	rm -rf SparseMatrix.o
	rm -rf SparseSet.o
	rm -rf Graph.o
	rm -rf Data.o

matrix:
	g++ -c SparseMatrix.cpp SparseSet.cpp Graph.cpp -I. -Imatrix
	ar -cvq libGCLmatrix.a SparseMatrix.o SparseSet.o Graph.o
	rm -rf SparseMatrix.o
	rm -rf SparseSet.o
	rm -rf Graph.o

clean:
	rm -rf libGCLfull.a
	rm -rf libGCLstage.a
	rm -rf libGCLmatrix.a
	rm -rf SparseMatrix.o
	rm -rf SparseSet.o
	rm -rf Graph.o
	rm -rf Data.o