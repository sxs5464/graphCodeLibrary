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
# matrix Makefile
#
#  Created on: Feb 15, 2015
#      Author: Sam Skalicky
# 

all:
	@echo "matrix - compile and produce executable to construct the full graph using the Matrix as the base data structure"

matrix:
	g++ -o dummy dummyCode.cpp funcNames.cpp ../../libGCLmatrix.a -I../.. -I../../matrix

clean:
	rm -rf dummy
	rm -rf *.txt