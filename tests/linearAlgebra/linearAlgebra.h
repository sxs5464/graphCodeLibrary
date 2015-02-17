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
 * LinearAlgebra.h
 *
 *  Created on: Apr 28, 2013
 *      Author: Sam Skalicky
 */
#include <vector>
#include <math.h>

#include "Data.h"

template <class T>
class LinearAlgebra {
public:
	typedef vector<T> array;
	typedef vector<array> matrix;

	/*
	 * This object holds the diagonal elements and lower triangular elements for cholesky decomposition
	 */
	struct cholResult {
	public:
		array diag;
		matrix lower;

		cholResult(long unsigned size) : diag(size) {
			//initialize result matrix
			for(unsigned i=0; i<size; i++) {
				array tmp(i);
				lower.push_back(tmp);
			}
		}
	};

	/*
	 * This function performs a reduction operation on a given array in the most parallel form,
	 * by adding pairs of elements in the array simultaneously, and then all pairs of results
	 * of the first pairs, etc...
	 */
	static T reduction(array *a1) {
		array prev;
		for(unsigned i=0; i<a1->size(); i++)
			prev.push_back(a1->at(i));

		int start;

		if(prev.size() % 2 == 1)
			start = prev.size()/2 + 1;
		else
			start = prev.size()/2;

		for(int j=start; j>0; j/=2) {

			//check for non-even split
			if(prev.size() % 2 == 1) {

				array m2(prev.size()/2+1);
				//put last odd indexed value at the end of the array
				m2[prev.size()/2] = prev[prev.size()-1];
				//continue with summation
				for(unsigned i=0; i<prev.size()-1; i+=2) {
					m2[i/2] = prev[i] + prev[i+1];
				}

				//in the next iteration, we'll check again for even split
				prev = m2;

			}
			else {

				array m2(prev.size()/2);
				for(unsigned i=0; i<prev.size(); i+=2) {
					m2[i/2] = prev[i] + prev[i+1];
				}
				prev = m2;
			}
			//printf("------End of Iteration-------\n");
		}

		return prev[0];
	}

	/*
		 * This function performs a reduction operation on a given array in the most parallel form,
		 * by adding pairs of elements in the array simultaneously, and then all pairs of results
		 * of the first pairs, etc... This version subtracts all the components instead of adding
		 */
		static T reductionNeg(array *a1) {
			array prev;
			for(unsigned i=0; i<a1->size(); i++)
				prev.push_back(a1->at(i));

			int start;

			if(prev.size() % 2 == 1)
				start = prev.size()/2 + 1;
			else
				start = prev.size()/2;

			for(int j=start; j>0; j/=2) {

				//check for non-even split
				if(prev.size() % 2 == 1) {

					array m2(prev.size()/2+1);
					//put last odd indexed value at the end of the array
					m2[prev.size()/2] = prev[prev.size()-1];
					//continue with summation
					for(unsigned i=0; i<prev.size()-1; i+=2) {
						m2[i/2] = prev[i] - prev[i+1];
					}

					//in the next iteration, we'll check again for even split
					prev = m2;

				}
				else {

					array m2(prev.size()/2);
					for(unsigned i=0; i<prev.size(); i+=2) {
						m2[i/2] = prev[i] - prev[i+1];
					}
					prev = m2;
				}
				//printf("------End of Iteration-------\n");
			}

			return prev[0];
		}

	/*
	 * This function performs a dot product in the most parallel way by performing all
	 * multiplications in parallel and then using the parallel reduction by summing
	 * the resulting products
	 */
	static T dotProduct(array *a1,array *a2) {
		T result;
		array m1(a1->size());

		//loop through all multiplies
		for(unsigned i=0; i<a1->size(); i++) {

			m1[i] = a1->at(i) * a2->at(i);
		}

		//loop through and sum all the results of multiplies
		result = reduction(&m1);

		return result;
	}

	/*
	 * This function performs a dot product in the most parallel way by performing all
	 * multiplications in parallel and then using the parallel reduction by summing
	 * the resulting products. Assumes that
	 * the entire vectors are NOT able to be stored in memory, and so
	 * each block is read in as needed
	 */
	static T dotProductRead(int a1size, int a2size) {
		array a1(a1size);
		array a2(a2size);

		T result;
		array m1(a1size);

		//loop through all multiplies
		for(unsigned i=0; i<a1size; i++) {

			m1[i] = a1.at(i) * a2.at(i);
		}

		//loop through and sum all the results of multiplies
		result = reduction(&m1);

		return result;
	}

	/*
	 * This function performs a dot product using a block-based approach.
	 * data size must be a power of 2 & at least size 4. Assumes that
	 * the entire vectors are able to be stored in memory
	 */
	static T dotProductBlocked(array *a1,array *a2) {
		/* These sizes have already been predetermined to be the best sizes
		 * to reduce the amount of data stored in memory.
		 * index:0 = data size 4
		 * index:1 = data size 8...
		 */
		int blockSizes[12] = {2,2,2,4,4,8,8,16,16,32,32,64};

		/* calculate index into block size array based on array length
		 * by finding the closest power of 2 (rounding up, ceiling)
		 */
		int index = ceil(log(a1->size())/log(2))-2;

		int blockSize = blockSizes[index];

		//stores final result
		T result;
		//stores intermediate blocked results for final summation later
		array tmp;

		typename array::iterator a1start = a1->begin();
		typename array::iterator a1stop = a1->begin() + blockSize;

		typename array::iterator a2start = a2->begin();
		typename array::iterator a2stop = a2->begin() + blockSize;

		//perform blocked dot products
		for(unsigned i=0; i<a1->size()/blockSize; i++) {
			//create blocks
			array b1(a1start,a1stop);
			array b2(a2start,a2stop);

			//perform full dot product on these blocks
			tmp.push_back(dotProduct(&b1,&b2));

			/* after completing the dot product above, the variable info will
			 * have been modified (variables that have been read into memory etc.
			 * and since the blocks we created were copies and not references,
			 * the data in the arrays a1 & a2 were not updated. If we dont do this
			 * update, then the graph will appear to have more memory accesses
			 * since we wont be able to track memory accesses that were already
			 * completed.
			 *
			 * I am currently using this method to reduce the amount of data in
			 * actual memory during graph generation. However, if actual block-
			 * based approach is needed (and additional memory accesses ok)
			 * this this next code should be commented out in order to assume
			 * that each block is processed independently and so some data
			 * elements will be accessed repeatedly
			 */
			for(unsigned j=0; j<blockSize; j++) {
				a1->at(j) = b1[j];
				a2->at(j) = b2[j];
			}

			//update iterators
			a1start = a1stop;
			a1stop = a1stop+blockSize;
			a2start = a2stop;
			a2stop = a2stop+blockSize;
		}

		result = reduction(&tmp);

		return result;
	}

	/*
	 * This function performs a dot product using a block-based approach.
	 * data size must be a power of 2 & at least size 4. Assumes that
	 * the entire vectors are NOT able to be stored in memory, and so
	 * each block is read in as needed
	 */
	static T dotProductBlockedRead(int a1size,int a2size) {
		/* These sizes have already been predetermined to be the best sizes
		 * to reduce the amount of data stored in memory.
		 * index:0 = data size 4
		 * index:1 = data size 8...
		 */
		unsigned blockSizes[12] = {2,2,2,4,4,8,8,16,16,32,32,64};

		/* calculate index into block size array based on array length
		 * by finding the closest power of 2 (rounding up, ceiling)
		 */
		unsigned index = ceil(log(a1size)/log(2))-2;

		unsigned blockSize = blockSizes[index];

		//stores final result
		T result;
		//stores intermediate blocked results for final summation later
		array tmp;

		//perform blocked dot products
		for(unsigned i=0; i<a1size/blockSize; i++) {
			//create blocks
			array b1(blockSize);
			array b2(blockSize);

			//perform full dot product on these blocks
			tmp.push_back(dotProduct(&b1,&b2));
		}

		result = reduction(&tmp);

		return result;
	}

	/*
	 * This function performs a dot product computation on 2 arrays
	 * by computing the multiplications and then adding them so as to
	 * keep as few items in memory as possible at any one time
	 */
	static T dotProductMinimal(array *a1,array *a2) {
		array m1;
		unsigned arrayIndex=0;

		unsigned iterations;
		//check if the size is a power of 2
		if((a1->size() != 0) && !(a1->size() & (a1->size() - 1))) {
			iterations = a1->size()*2-1;
		}
		else {
			//get next power of 2 of the size
			unsigned power = pow(2,ceil(log(a1->size())/log(2)));
			iterations = power*2-1;
		}

		unsigned pairCount = 0;
		unsigned pair = getPair(pairCount++);
		vector<int> seq = expand(pair);
		unsigned seqIndex = 0;

		//loop through all multiplies
		for(unsigned i=0; i<iterations; i++) {
			if(seq[seqIndex] == 1 && arrayIndex >= a1->size()) {
				break;
			}
			else if(seq[seqIndex++] == 1) {
				//array size increases so do another multiplication
				m1.push_back(a1->at(arrayIndex) * a2->at(arrayIndex));
				arrayIndex++;
			}
			else {
				//array size decreases so do an addition of the latest values
				T tmp1 = m1.back();
				m1.pop_back();
				T tmp2 = m1.back();
				m1.pop_back();
				m1.push_back(tmp1+tmp2);
			}

			//check and see if we need the next pair
			if(seqIndex == seq.size()) {
				pair = getPair(pairCount++);
				seq = expand(pair);
				seqIndex = 0;
			}
		}

		/*
		 * if there is more than 1 value left in the array, its due to the size
		 * not being a power of 2, so now we just do a normal reduction on the
		 * remaining values
		 */
		if(m1.size() > 1) {
			m1[0] = reduction(&m1);
		}

		return m1[0];
	}

	/*
	 * This function performs a dot product computation on 2 arrays
	 * by computing the multiplications and then adding them so as to
	 * keep as few items in memory as possible at any one time
	 *
	 * This function takes two matrices as input and allows for dot product
	 * calculation on either rows or columns of either matrix.
	 *
	 * For example, lets say we have 2 lower triangular matrices a1 & a2
	 * and we want to multiply the 2nd column of a1 by the 2nd row of a2,
	 * so out inputs to the function would be:
	 *
	 * 	    | X     |
	 * a1 = | X X   |
	 *      | X X X |
	 *
	 * rc1 = 'c' or 'C'
	 * a1index = 1 (zero based indexing, so col 1 is index 0, col 2 is index 1)
	 * a1start = 1 (since the matrix is lower triangular, we want to start at index 1)
	 *
	 * rc2 = 'r' or 'R'
	 * a2index = 1
	 * a1start = 0
	 *
	 * size = 2
	 */
	static T dotProductMinimalRC(matrix *a1,char rc1, unsigned a1index, unsigned a1start, matrix *a2, char rc2, unsigned a2index, unsigned a2start, unsigned size) {
		array m1;
		long long unsigned arrayIndex=0;

		long long unsigned iterations;

		//check if the size is a power of 2
		if((size != 0) && !(size & (size - 1))) {
			iterations = size*2-1;
		}
		else {
			//get next power of 2 of the size
			long long unsigned power = pow(2,ceil(log(size)/log(2)));
			iterations = power*2-1;
		}

		long long unsigned pairCount = 0;
		long long unsigned pair = getPair(pairCount++);
		vector<int> seq = expand(pair);
		unsigned seqIndex = 0;

		//loop through all multiplies
		for(long long unsigned i=0; i<iterations; i++) {
			if(seq[seqIndex] == 1 && arrayIndex >= size) {
				break;
			}
			else if(seq[seqIndex++] == 1) {
				T *tmp1,*tmp2;
				//array size increases so do another multiplication
				if(rc1 == 'r' || rc1 == 'R')
					tmp1 = &(a1->at(a1index)[a1start+arrayIndex]);
				else
					tmp1 = &(a1->at(a1start+arrayIndex)[a1index]);

				if(rc2 == 'r' || rc2 == 'R')
					tmp2 = &(a2->at(a2index)[a2start+arrayIndex]);
				else
					tmp2 = &(a2->at(a2start+arrayIndex)[a2index]);

				m1.push_back((*tmp1) * *(tmp2));
				arrayIndex++;
			}
			else {
				//array size decreases so do an addition of the latest values
				T tmp1 = m1.back();
				m1.pop_back();
				T tmp2 = m1.back();
				m1.pop_back();
				m1.push_back(tmp1+tmp2);
			}

			//check and see if we need the next pair
			if(seqIndex == seq.size()) {
				pair = getPair(pairCount++);
				seq = expand(pair);
				seqIndex = 0;
			}
		}

		/*
		 * if there is more than 1 value left in the array, its due to the size
		 * not being a power of 2, so now we just do a normal reduction on the
		 * remaining values
		 */
		if(m1.size() > 1) {
			m1[0] = reduction(&m1);
		}

		T result(m1[0]);
		return result;
	}

	/*
		 * This function performs a dot product computation on 2 arrays
		 * by computing the multiplications and then adding them so as to
		 * keep as few items in memory as possible at any one time. This
		 * version subtracts all the results of the multiplications
		 *
		 * This function takes two matrices as input and allows for dot product
		 * calculation on either rows or columns of either matrix.
		 *
		 * For example, lets say we have 2 lower triangular matrices a1 & a2
		 * and we want to multiply the 2nd column of a1 by the 2nd row of a2,
		 * so out inputs to the function would be:
		 *
		 * 	    | X     |
		 * a1 = | X X   |
		 *      | X X X |
		 *
		 * rc1 = 'c' or 'C'
		 * a1index = 1 (zero based indexing, so col 1 is index 0, col 2 is index 1)
		 * a1start = 1 (since the matrix is lower triangular, we want to start at index 1)
		 *
		 * rc2 = 'r' or 'R'
		 * a2index = 1
		 * a1start = 0
		 *
		 * size = 2
		 */
		static T dotProductMinimalRCNeg(matrix *a1,char rc1, unsigned a1index, unsigned a1start, matrix *a2, char rc2, unsigned a2index, unsigned a2start, unsigned size) {
			array m1;
			long long unsigned arrayIndex=0;

			long long unsigned iterations;

			//check if the size is a power of 2
			if((size != 0) && !(size & (size - 1))) {
				iterations = size*2-1;
			}
			else {
				//get next power of 2 of the size
				long long unsigned power = pow(2,ceil(log(size)/log(2)));
				iterations = power*2-1;
			}

			long long unsigned pairCount = 0;
			long long unsigned pair = getPair(pairCount++);
			vector<int> seq = expand(pair);
			unsigned seqIndex = 0;

			bool first = true;

			//loop through all multiplies
			for(long long unsigned i=0; i<iterations; i++) {
				if(seq[seqIndex] == 1 && arrayIndex >= size) {
					break;
				}
				else if(seq[seqIndex++] == 1) {
					T *tmp1,*tmp2;
					//array size increases so do another multiplication
					if(rc1 == 'r' || rc1 == 'R')
						tmp1 = &(a1->at(a1index)[a1start+arrayIndex]);
					else
						tmp1 = &(a1->at(a1start+arrayIndex)[a1index]);

					if(rc2 == 'r' || rc2 == 'R')
						tmp2 = &(a2->at(a2index)[a2start+arrayIndex]);
					else
						tmp2 = &(a2->at(a2start+arrayIndex)[a2index]);

					T tmp3 = ((*tmp1) * (*tmp2));
					//if this is the first multiply, subtract the result from zero
					if(first) {
						first = false;
						T zero(0);
						tmp3 = zero - tmp3;
					}
					m1.push_back(tmp3);
					arrayIndex++;
				}
				else {
					//array size decreases so do an addition of the latest values
					T tmp1 = m1.back();
					m1.pop_back();
					T tmp2 = m1.back();
					m1.pop_back();
					m1.push_back(tmp1-tmp2);
				}

				//check and see if we need the next pair
				if(seqIndex == seq.size()) {
					pair = getPair(pairCount++);
					seq = expand(pair);
					seqIndex = 0;
				}
			}

			/*
			 * if there is more than 1 value left in the array, its due to the size
			 * not being a power of 2, so now we just do a normal reduction on the
			 * remaining values
			 */
			if(m1.size() > 1) {
				m1[0] = reductionNeg(&m1);
			}

			T result(m1[0]);
			return result;
		}

	static T dotProductMinimalRC(matrix *a1,char rc1, unsigned a1index, unsigned a1start, array *a2, unsigned a2start, unsigned size) {
			array m1;
			long long unsigned arrayIndex=0;

			long long unsigned iterations;

			//check if the size is a power of 2
			if((size != 0) && !(size & (size - 1))) {
				iterations = size*2-1;
			}
			else {
				//get next power of 2 of the size
				long long unsigned power = pow(2,ceil(log(size)/log(2)));
				iterations = power*2-1;
			}

			long long unsigned pairCount = 0;
			long long unsigned pair = getPair(pairCount++);
			vector<int> seq = expand(pair);
			unsigned seqIndex = 0;

			//loop through all multiplies
			for(long long unsigned i=0; i<iterations; i++) {
				if(seq[seqIndex] == 1 && arrayIndex >= size) {
					break;
				}
				else if(seq[seqIndex++] == 1) {
					T *tmp1,*tmp2;
					//array size increases so do another multiplication
					if(rc1 == 'r' || rc1 == 'R')
						tmp1 = &(a1->at(a1index)[a1start+arrayIndex]);
					else
						tmp1 = &(a1->at(a1start+arrayIndex)[a1index]);

					tmp2 = &(a2->at(arrayIndex));

					m1.push_back((*tmp1) * *(tmp2));
					arrayIndex++;
				}
				else {
					//array size decreases so do an addition of the latest values
					T tmp1 = m1.back();
					m1.pop_back();
					T tmp2 = m1.back();
					m1.pop_back();
					m1.push_back(tmp1+tmp2);
				}

				//check and see if we need the next pair
				if(seqIndex == seq.size()) {
					pair = getPair(pairCount++);
					seq = expand(pair);
					seqIndex = 0;
				}
			}

			/*
			 * if there is more than 1 value left in the array, its due to the size
			 * not being a power of 2, so now we just do a normal reduction on the
			 * remaining values
			 */
			if(m1.size() > 1) {
				m1[0] = reduction(&m1);
			}

			T result(m1[0]);
			return result;
		}

	/*
	 * This function performs a matrix-vector multiply computation by performing
	 * parallel dot product operations for each row in the matrix with the vector
	 */
	static array matrixVectorMultiply(matrix *m1, array *a1) {
		array result(m1->at(0).size());

		for(unsigned i=0; i<m1->size(); i++) {
			result[i] = dotProduct(&(m1->at(i)),a1);
		}

		return result;
	}

	/*
	 * This function performs a matrix-vector multiply computation by performing
	 * parallel dot product operations for each row in the matrix with the vector
	 * using the blocked dot product method
	 */
	static array matrixVectorMultiplyBlocked(matrix *m1, array *a1) {
		array result(m1->at(0).size());

		for(unsigned i=0; i<m1->size(); i++) {
			result[i] = dotProductBlocked(&(m1->at(i)),a1);
		}

		return result;
	}

	/*
	 * This function performs a matrix-vector multiply computation by performing
	 * parallel dot product operations for each row in the matrix with the vector
	 * using the blocked dot product method. Assumes that the entire vectors are
	 * NOT able to be stored in memory, and so each block is read in as needed
	 */
	static array matrixVectorMultiplyBlockedRead(int m1size, int a1size) {
		array result(m1size);

		array a1(a1size);

		for(unsigned i=0; i<m1size; i++) {
			/* each row in the matrix needs to be read in, and is then discarded
			 * since there is no more use for that data anymore
			 */
			array m1row(a1size);

			result[i] = dotProductBlocked(&m1row,&a1);
		}

		return result;
	}

	/*
	 * This function performs parallel matrix-matrix multiply computation by
	 * performing parallel dot product operations for each row in matrix 1 and
	 * each row in matrix 2 (assuming matrix 2 has already been transposed)
	 */
	static matrix matrixMatrixMultiply(matrix *m1, matrix *m2) {
		matrix result;

		//loop through all rows of m1
		for(unsigned i=0; i<m1->size(); i++) {
			//add new vector to result matrix
			array tmp(m2->size());
			result.push_back(tmp);

			//loop through all cols of m2 (assuming m2 has already been transposed)
			for(unsigned j=0; j<m2->size(); j++) {
				cout << "-------Before " << j+1 << "th dot product-------" << endl;
				Data<int>::printStats();
				cout << "---------------------------------------" << endl;

				//perform dot product
				result[i][j] = dotProduct(&(m1->at(i)),&(m2->at(j)));

				cout << "-------After " << j+1 << "th dot product-------" << endl;
				Data<int>::printStats();
				cout << "---------------------------------------" << endl;
			}
		}

		return result;
	}

	/*
	 * This function adds all elements of one matrix to the other in parallel
	 */
	static matrix matrixAdd(matrix *m1, matrix *m2) {
		matrix result;

		//loop through all the rows
		for(unsigned i=0; i<m1->size(); i++) {
			array tmp(m2->size());
			result.push_back(tmp);

			//loop through all the cols
			for(unsigned j=0; j<m2->size(); j++) {
				result[i][j] = (m1->at(i))[j] + (m1->at(i))[j];
			}
		}

		return result;
	}

	/*
	 * This function performs matrix-matrix on two matrices of the given size
	 * in a block based fashion using the block size given. It assumes that
	 * the individual blocks are read in as needed, and resulting blocks are
	 * written to memory separately. So no entire matrix (inputs or output)
	 * are every stored in memory.
	 *
	 * The matrix sizes are assumed to be a multiple of the blockSize
	 */
	static void matrixMatrixBlockMultiply(unsigned long m1size, unsigned long m2size, unsigned long blockSize) {


		//loop through all row blocks of m1 & col blocks of m2
		for(unsigned i=0; i<m1size/blockSize; i++) {
			//perform individual matrix multiplications on
		}
	}

	/*
	 * This function performs matrix-matrix on two matrices of the given size
	 * in a minimal way such that only the specific values needed for the
	 * current work are read in from memory.
	 */
	static void matrixMatrixMinimalMultiply(unsigned long m1size, unsigned long m2size, unsigned long blockSize) {

	}

	/*
	 * This function performs cholesky decomposition by keeping the diagonal and
	 * lower triangular elements separate to minimize divide operations
	 */
	static cholResult choleskyDecomposition(matrix *m1) {
		cholResult result(m1->size());

		//set first iteration results
		result.diag.push_back(m1->at(0)[0]);

		//loop through each row (except first)
		for(unsigned i = 1; i<m1->size(); i++) {
			//calculate Z's
			array Z(i);

			for(unsigned j=0; j<Z.size(); j++) {
				//calculate multiplies Lji * Zi
				if(j > 0) {

					array Zmult(j);
					for(unsigned k=0; k<j; k++) {
						Zmult[k] = result.lower[j][k] * Z[k];
					}

					//sum the multiplies
					T sum = reduction(&Zmult);

					//calculate Z value
					Z[j] = m1->at(i)[j] - sum;

				}
				else
					Z[j] = m1->at(i)[j];
			}

			//calculate R's
			array R(i);
			for(unsigned j=0; j<R.size(); j++) {
				R[j] = Z[j]/result.diag[j];
				result.lower[i][j] = R[j];
			}

			//calculate diag
			//calculate sum of Ri^2 * Di
			//calculate multiplies
			array Rmult(i);
			for(unsigned j=0; j<i; j++) {
				Rmult[j] = R[j]*R[j]*result.diag[j];
			}

			T sum = reduction(&Rmult);

			result.diag[i] = m1->at(i)[i] - sum;

		}

		return result;
	}



	/*
	 * This function performs cholesky decomposition by keeping the diagonal and
	 * lower triangular elements separate to minimize divide operations. This is the
	 * FPGA version of the algorithm
	 */
	static cholResult choleskyDecompositionDiv(matrix *m1) {
		cholResult result(m1->size());

		//loop over the number of rows in the matrix
		for(unsigned i=0; i<m1->size(); i++) {
			//create array to hold Z values for this row
			array Z(i);

			if(i > 0) {	//dont loop over columns for the first row, just do diagonal at the end
				array squared(i);
				//loop over the columns in the row
				for(unsigned j=0; j<i; j++) {
					//cant do dot product on zero items, it'll crash!
					if(j > 0) {
						//calculate the dot product of j'th row of R's * Z's
						T res = dotProductMinimalRC(&(result.lower),'r', j, 0, &Z, 0, j);

						Z[j] = m1->at(i)[j] - res;
						result.lower[i][j] = Z[j] / result.diag[j];
					}
					else {
						Z[j] = m1->at(i)[j];
						result.lower[i][j] = Z[j] / result.diag[j];
					}
					squared[j] = result.lower[i][j]*result.lower[i][j];
				}

				//calculate the dot product of the i'th row of R's * diag's
				T res = dotProductMinimal(&squared, &(result.diag));

				//diagonal
				result.diag[i] = m1->at(i)[i] - res;
			}
			else {
				result.diag[i] = m1->at(i)[i];
			}

		}

		return result;
	}

	/*
		 * This function performs cholesky decomposition by keeping the diagonal and
		 * lower triangular elements separate to minimize divide operations. This is the
		 * FPGA version of the algorithm with dot product calculations removed to follow
		 * the pipeline processing order.
		 */
	static cholResult choleskyDecompositionDivAdd(matrix *m1) {
		cholResult result(m1->size());

		//loop over the number of rows in the matrix
		for(unsigned i=0; i<m1->size(); i++) {
			//create array to hold Z values for this row
			array Z(i);

			if(i > 0) {	//dont loop over columns for the first row, just do diagonal at the end

				array squared(i);
				//loop over the columns in the row
				for(unsigned j=0; j<i; j++) {
					//cant do dot product on zero items, it'll crash!
					if(j > 0) {
						//calculate the dot product of j'th row of R's * Z's
						//T res = dotProductMinimalRC(&(result.lower),'r', j, 0, &Z, 0, j);
						T res;
						for(unsigned k=0; k<j; k++) {
							if(k==0)
								res = result.lower[j][k] * Z[k];
							else {
								T tmp = result.lower[j][k] * Z[k];
								res = res + tmp;
							}
						}

						Z[j] = m1->at(i)[j] - res;
						result.lower[i][j] = Z[j] / result.diag[j];
					}
					else {
						Z[j] = m1->at(i)[j];
						result.lower[i][j] = Z[j] / result.diag[j];
					}
					squared[j] = result.lower[i][j]*result.lower[i][j];
				}

				//calculate the dot product of the i'th row of R's * diag's
				//T res = dotProductMinimal(&squared, &(result.diag));
				T res;
				for(unsigned j=0; j<squared.size() && j<result.diag.size(); j++) {
					if(j==0)
						res = squared[j] * result.diag[j];
					else {
						T tmp = (squared[j] * result.diag[j]);
						res = res + tmp;
					}
				}

				//diagonal
				result.diag[i] = m1->at(i)[i] - res;
			}
			else {
				result.diag[i] = m1->at(i)[i];
			}

		}

		return result;
	}

	/*
	 * This function performs cholesky decomposition by keeping the diagonal and
	 * lower triangular elements separate to minimize divide operations
	 */
	static cholResult choleskyDecompositionBlockedRead(int size) {
		cholResult result(size);

		//set first iteration results
		//result.diag.push_back(m1->at(0)[0]);

		//loop through each row (except first)
		for(int i = 1; i<size; i++) {
			//read the current row of data required (will only need i elements from the current row)
			array m1row(i+1);

			//calculate Z's
			array Z;

			for(int j=0; j<i; j++) {
				if(j > 0) {
					T sum = dotProductMinimal(&(result.lower[j]),&Z);

					//calculate Z value
					Z.push_back(m1row[j] - sum);
				}
				else {
					Z.push_back(m1row[j]);
				}
			}

			for(int j=0; j<i; j++) {
				//calculate R's
				//array R(i);
				T tmp = Z[j]/result.diag[j];
				result.lower[i][j] = tmp;

				//calculate diag
				//calculate sum of Ri^2 * Di
				//calculate multiplies
				//array Rmult(i);
				Z[j] = tmp*tmp*result.diag[j];
			}

			T sum = reduction(&Z);

			result.diag[i] = m1row[i] - sum;
		}

		return result;
	}

	/*
	 * This function performs matrix inversion on triangular matrices
	 */
	static matrix matrixInverse(matrix *m1) {
		matrix result;

		//initialize result matrix
		for(unsigned i=0; i<m1->size(); i++) {
			array tmp(m1->size());
			result.push_back(tmp);
		}

		for(unsigned i=0; i<m1->size(); i++) {
			for(unsigned j=0; j<m1->size(); j++) {
				if(i>j) {
					//result.get(i,j) = 0;
				}
				else if(i == j && i == 0) {
					T tmp(1);
					result[i][j] = tmp / m1->at(j)[j];
				}
				else if(i < j) {
					//calculate multiplies
					array mult(j);
					for(unsigned m=0; m<j; m++) {
						mult[m] = result[i][m] * m1->at(m)[j];
					}

					//summation
					T sum = reduction(&mult);

					if(i == 0) {
						T tmp(1);
						result[j][j] = tmp / m1->at(j)[j];
						result[i][j] = sum * result[j][j];
					}
					else {
						result[i][j] = sum * result[j][j];
					}
				}
			}
		}

		return result;
	}



	/*
	 * This function performs matrix inversion on triangular matrices. This version
	 * performs more divisions of the diagonal elements thus removing some dependencies
	 * on the inverse of the diagonal elements.
	 */
	static matrix matrixInverseDiv(matrix *m1) {
		matrix result;

		//initialize matrices
		for(unsigned i=0; i<m1->size(); i++) {
			array tmp(m1->size());
			result.push_back(tmp);
		}

		for(unsigned j=0; j<m1->size(); j++) {
			array temp(j);

			for(unsigned i=0; i<j; i++) {
				/*
				for(unsigned k=0; k<=i; k++) {
					temp[k]+=m1->at(i)[j] * result[k][i];
				}
				*/
				temp[i] = dotProductMinimalRC(m1,'c', j, 0, &result, 'r', i, i, j-i);
			}



			T one(1);
			result[j][j] = one / m1->at(j)[j];

			for(unsigned k=0; k<temp.size(); k++) {
				result[k][j] = -temp[k] / m1->at(j)[j];
			}
		}

		return result;
	}

	/*
		 * This function performs matrix inversion on triangular matrices. This version
		 * performs more divisions of the diagonal elements thus removing some dependencies
		 * on the inverse of the diagonal elements. This version uses subtractions instead of additions
		 */
		static matrix matrixInverseDivNeg(matrix *m1) {
			matrix result;

			//initialize matrices
			for(unsigned i=0; i<m1->size(); i++) {
				array tmp(m1->size());
				result.push_back(tmp);
			}

			for(unsigned j=0; j<m1->size(); j++) {
				array temp(j);

				for(unsigned i=0; i<j; i++) {

//					for(unsigned k=0; k<=i; k++) {
//						temp[k]+=m1->at(i)[j] * result[k][i];
//					}

					temp[i] = dotProductMinimalRCNeg(m1,'c', j, 0, &result, 'r', i, i, j-i);
				}



				T one(1);
				result[j][j] = one / m1->at(j)[j];

				for(unsigned k=0; k<temp.size(); k++) {
					result[k][j] = temp[k] / m1->at(j)[j];
				}
			}

			return result;
		}

	/*
	 * This function performs matrix inversion on triangular matrices
	 */
	static matrix matrixInverseBlockedRead(unsigned size) {
		matrix result;
		matrix m1;

		//initialize matrices
		for(unsigned i=0; i<size; i++) {
			array tmp(i+1);
			result.push_back(tmp);
			array tmp1(i+1);
			m1.push_back(tmp1);
		}

		cout << "allocated" << endl;

		for(unsigned i=0; i<size; i++) {
			for(unsigned j=((i==0)?i:(i+1)); j<size; j++) {
				if(i == j && i == 0) {
					T tmp(1);
					result[j][i] = tmp / m1[j][j];
				}
				else if(i < j) {

					/*
						//calculate multiplies
						array mult(j);
						for(unsigned m=i; m<j; m++) {
							cout << "result[" << m << "][" << i << "], m1[" << j << "][" << m << "]" <<endl;
							mult[m] = result[m][i] * m1[j][m];
						}

						//summation
						T sum = reduction(&mult);
					 */

					T sum = dotProductMinimalRC(&result,'c',i,i,&m1,'r',j,i,(j-i));

					if(i == 0) {
						T tmp(1);
						result[j][j] = tmp / m1[j][j];
						result[j][i] = sum * result[j][j];
					}
					else {
						result[j][i] = sum * result[j][j];
					}

				}
			}
			cout << i << " / " << size << endl;
		}

		return result;
	}

private:
	/*
	 * Returns the index of the first occurrence of a
	 * particular pair of numbers
	 */
	static int first(int i) {
		return pow(2,i-1)-1;
	}
	/*
	 * Returns the i'th pair of numbers
	 * (first number is always 2, so we only return 2nd)
	 */
	static int getPair(int i) {
		bool done = false;
		int cnt = 1;

		while(!done) {
			/*
			 * Take the current index i and account for offset
			 * using the first function. Then determine if this
			 * number is a multiple of cnt. If it is, we've
			 * found the result, otherwise check the next one
			 */
			if((i-first(cnt))%(int)pow(2,cnt) == 0)
				done = true;
			else
				cnt++;
		}

		return cnt;
	}

	/*
	 * Expands the given pair of numbers into a
	 * binary sequence where the first 2 bits are 1
	 * and the remaining are 0
	 */
	static vector<int> expand(int pair) {
		vector<int> seq(2+pair);
		//first two are always 1
		seq[0] = 1;
		seq[1] = 1;

		//all others are 0
		for(int i=0; i<pair; i++) {
			seq[2+i] = 0;
		}

		return seq;
	}
};


