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
 * dummyCode.cpp
 *
 *  Created by: Heterogeneous Matlab Compiler
 *      Author: Sam Skalicky
 */

#include <string>
#include <map>

std::map<std::string, int> create_map() {
    std::map<std::string,int> m;
    
    m["svd"] = 0;
    m["VecSum"] = 1;
    m["MatVecSub"] = 2;
    m["RMS"] = 3;
    m["min"] = 4;
    
    return m;
}
