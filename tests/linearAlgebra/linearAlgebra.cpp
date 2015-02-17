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
 * LinearAlgebra.cpp
 *
 *  Created on: Apr 28, 2013
 *      Author: Sam Skalicky
 */

#include "Data.h"
#include "linearAlgebra.h"

int main() {

  Data<int> result = LinearAlgebra<Data<int> >::dotProductRead(4,4);

  Data<int>::printResult();
  Data<int>::writeResult("out");

  return 0;
}
