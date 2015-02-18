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
# graphViewerCMD Makefile
#
#  Created on: Feb 18, 2015
#      Author: Sam Skalicky
# 

.PHONY: all,compile,run,clean

all:
	@echo "compile - compile java program"
	@echo "run - run java program"
	@echo "clean - remove generated files"

compile:
	javac -d bin -classpath lib/jgraphx.jar -sourcepath src src/graphPanel.java

run:
	java -classpath bin:lib/jgraphx.jar graphPanel DFG.txt

clean:
	rm -rf bin/*
	rm -rf *.png