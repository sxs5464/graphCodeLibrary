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
# fibonacci Makefile
#
#  Created on: Feb 15, 2015
#      Author: Sam Skalicky
# 
 
all:
	@echo "full - compile and produce executable to construct the full graph"
	@echo "stage - compile and produce executable to construct the compressed graph"

full:
	g++ -o fibonacci fibonacci.cpp ../../libGCL.a -I../.. -I../../full

stage:
	g++ -o fibonacci fibonacci.cpp ../../libGCL.a -I../.. -I../../stage

clean:
	rm -rf fibonacci
	rm -rf *.txt