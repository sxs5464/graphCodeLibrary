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
 * fibonacci.cpp
 *
 *  Created on: Apr 15, 2013
 *      Author: Sam Skalicky
 */


#include "Data.h"

Data<unsigned long> fib1 ( Data<unsigned long> i ) {
	Data<unsigned long> one(1);
	Data<unsigned long> two(2);

	if ( i < two )
		return i;

	one = fib1 ( i - one );
	two = fib1 ( i - two );

	Data<unsigned long> result = one + two;

	return result;
}

int main() {
	Data<unsigned long> num(4);

	fib1(num);

	Data<unsigned long>::printResult();
	Data<unsigned long>::writeResult("test");
}
