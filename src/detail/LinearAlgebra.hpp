/*******************************************************************************
 * This file is part of IMTAphy
 * _____________________________________________________________________________
 *
 * Copyright (C) 2010
 * Institute of Communication Networks (LKN)
 * Department of Electrical Engineering and Information Technology (EE & IT)
 * Technische Universitaet Muenchen
 * Arcisstr. 21
 * 80333 Muenchen - Germany
 * http://www.lkn.ei.tum.de/~jan/imtaphy/index.html
 * 
 * _____________________________________________________________________________
 *
 *   IMTAphy is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   IMTAphy is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with IMTAphy.  If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/


#ifndef IMTAPHY_DETAIL_LINEARALGEBRA_HPP
#define IMTAPHY_DETAIL_LINEARALGEBRA_HPP

#include <vector>
#include <WNS/Assure.hpp>
#include <WNS/Singleton.hpp>
// For opt versions disable range checking in the multi_array wrapper
// WNS_NDEBUG means: assures are disabled
#ifdef  WNS_NDEBUG
#define BOOST_DISABLE_ASSERTS
#endif
#include <boost/multi_array.hpp>


//#ifdef MKL
// from the MKL user guide:
#define MKL_Complex8 std::complex<float>
#define MKL_Complex16 std::complex<double>
#include <mkl_vml_functions.h>
#include <mkl_service.h>
#include <mkl_cblas.h>
#include <mkl_vml_defines.h>
#include <mkl_lapacke.h>
#include <mkl.h>

#include <boost/random.hpp>
#include <boost/smart_ptr.hpp>

namespace imtaphy { namespace detail {

    // should only be used with float, double, std::complex<float> and std::complex<double>
    // is it possible to restrict the template to these types?
    template <typename T>
    class MKLMatrix :
        public boost::multi_array_ref<T, 2>
    {                
    public:
        MKLMatrix() :
         boost::multi_array_ref<T, 2>(static_cast<T*>(mkl_malloc(sizeof(T) * 1, 32)), boost::extents[1][1])
        {
            assure(0, "Do not use the default constructor");
        }
        inline
        MKLMatrix(unsigned int rows_, unsigned int columns_) :
            boost::multi_array_ref<T, 2>(static_cast<T*>(mkl_malloc(sizeof(T) * rows_ * columns_, 32)), boost::extents[rows_][columns_]),
            external(false)
        {
            assure(rows_, "Number of rows must be positive");
            assure(columns_, "Number of columns must be positive");
            
            assure(this->data(), "MKL malloc failed");
        }

        inline
        MKLMatrix(unsigned int rows_, unsigned int columns_, T* location) :
            boost::multi_array_ref<T, 2>(location, boost::extents[rows_][columns_]),
            external(true)
        {
            assure(rows_, "Number of rows must be positive");
            assure(columns_, "Number of columns must be positive");
            assure(this->data(), "Cannot point to null");
        }

        inline
        ~MKLMatrix()
        {
            if (!external)
            {
                assure(this->data(), "Trying to free null pointer");
                mkl_free(this->data());
            }
        }
        
        inline
        MKLMatrix(const MKLMatrix& copy) :
            boost::multi_array_ref<T, 2>(static_cast<T*>(mkl_malloc(sizeof(T) * copy.getRows() * copy.getColumns(), 32)), boost::extents[copy.getRows()][copy.getColumns()]),
            external(false)
        {
            memcpy(this->data(), copy.data(), sizeof(T)* copy.getRows() * copy.getColumns());
        }

        inline unsigned int getRows() const {return this->shape()[0];}
        inline unsigned int getColumns() const {return this->shape()[1];}
        inline T* getLocation() const {return const_cast<T*>(this->data());}

    private:
        bool external;
    };
    
    typedef MKLMatrix<std::complex<float> > ComplexFloatMatrix;
    typedef MKLMatrix<std::complex<double> > ComplexDoubleMatrix;
    typedef MKLMatrix<float> FloatMatrix;
    typedef MKLMatrix<double> DoubleMatrix;
    
    typedef boost::shared_ptr<ComplexFloatMatrix> ComplexFloatMatrixPtr;
    typedef boost::shared_ptr<ComplexDoubleMatrix> ComplexDoubleMatrixPtr;
    typedef boost::shared_ptr<FloatMatrix> FloatMatrixPtr;
    typedef boost::shared_ptr<DoubleMatrix> DoubleMatrixPtr;
    
        
    template <typename T>
    class Identity
    {
        public:
            Identity()
            {
                eye.resize(32);
                
                for (int n = 1; n < 32; n++)
                {
                    eye[n] = boost::shared_ptr<MKLMatrix<T> >(new MKLMatrix<T>(n, n));
                    for (int i = 0; i < n; i++)
                        for (int j = 0; j < n; j++)
                        {
                            if (i == j)
                                (*eye[n])[i][j] = static_cast<T>(1);
                            else
                                (*eye[n])[i][j] = static_cast<T>(0);
                        }
                }
            }
            boost::shared_ptr<MKLMatrix<T> > get(unsigned int n) {return eye[n];}
        private:
            std::vector<boost::shared_ptr<MKLMatrix<T> > > eye;
    };
    typedef wns::SingletonHolder<Identity<std::complex<float> > > TheComplexFloatIdentities;
    typedef wns::SingletonHolder<Identity<std::complex<double> > > TheComplexDoubleIdentities;
    typedef wns::SingletonHolder<Identity<float > > TheFloatIdentities;
    typedef wns::SingletonHolder<Identity<double > > TheDoubleIdentities;
    
    template <typename T>
    class MKLgemm // The ?gemm routines perform a matrix-matrix operation with general matrices. The operation is defined as C := alpha*op(A)*op(B) + beta*C
    {
    public:
        void execute(const CBLAS_ORDER Order,
                        const CBLAS_TRANSPOSE TransA,
                        const CBLAS_TRANSPOSE TransB,
                        const int M,
                        const int N,
                        const int K,
                        const void *alpha,
                        const void *A,
                        const int lda,
                        const void *B,
                        const int ldb,
                        const void *beta,
                        void *C,
                        const int ldc);
    };
    
            
    
    template <typename PRECISION>
    class MKLpotrf
    {
        public:
            lapack_int execute( int matrix_order, 
                                char uplo,           // CHARACTER*1. Must be 'U' or 'L'.
                                lapack_int n,        // INTEGER. The order of matrix A; n ≥ 0.
                                std::complex<PRECISION>* a,       // REAL for spotrf
                                                     // DOUBLE PRECISION for dpotrf
                                                     //       COMPLEX for cpotrf
                                                     //       DOUBLE COMPLEX for zpotrf.
                                lapack_int lda       // leading dimension of A -> n
                              );
    };
    
    template <> inline lapack_int MKLpotrf<float>::execute( int matrix_order, 
                                                            char uplo,         
                                                            lapack_int n,      
                                                            std::complex<float>* a,     
                                                            lapack_int lda       
                                                           )
    {
        return LAPACKE_cpotrf(matrix_order, uplo, n, a, lda);
    }

    template <> inline lapack_int MKLpotrf<double>::execute( int matrix_order, 
                                                            char uplo,         
                                                            lapack_int n,      
                                                            std::complex<double>* a,     
                                                            lapack_int lda       
                                                           )
    {
        return LAPACKE_zpotrf(matrix_order, uplo, n, a, lda);
    }

    template <typename PRECISION>
    class MKLpotri
    {
        public:
            lapack_int execute( int matrix_order, 
                                char uplo,           // CHARACTER*1. Must be 'U' or 'L'.
                                lapack_int n,        // INTEGER. The order of matrix A; n ≥ 0.
                                std::complex<PRECISION>* a,       // REAL for spotrf
                                                     // DOUBLE PRECISION for dpotrf
                                                     //       COMPLEX for cpotrf
                                                     //       DOUBLE COMPLEX for zpotrf.
                                lapack_int lda       // leading dimension of A -> n
                              );
    };
    
    template <> inline lapack_int MKLpotri<float>::execute( int matrix_order, 
                                                            char uplo,         
                                                            lapack_int n,      
                                                            std::complex<float>* a,     
                                                            lapack_int lda       
                                                           )
    {
        return LAPACKE_cpotri(matrix_order, uplo, n, a, lda);
    }

    template <> inline lapack_int MKLpotri<double>::execute( int matrix_order, 
                                                            char uplo,         
                                                            lapack_int n,      
                                                            std::complex<double>* a,     
                                                            lapack_int lda       
                                                           )
    {
        return LAPACKE_zpotri(matrix_order, uplo, n, a, lda);
    }

    template <typename PRECISION>
    class MKLgelss
    {
        public:
            lapack_int execute(int matrix_order, // LAPACK_ROW_MAJOR or LAPACK_COL_MAJOR
                        lapack_int m,     // 
                        lapack_int n,     //
                        lapack_int nrhs,  // for pinv, should be same as m
                        std::complex<PRECISION>* a, // A is input m-by-n matrix
                        lapack_int lda,           // The first dimension of a; at least max(1, m) -> m
                        std::complex<PRECISION>* b, // Input: for pinv, this is a m-by-m identity matrix
                                                    // Output: Overwritten by the n-by-nrhs solution matrix X.
                                                    //         If mâ¥n and rank = n, the residual sum-of-squares for the 
                                                    //         solution in the i-th column is given by the sum of squares 
                                                    //         of modulus of elements n+1:m in that column.
                        lapack_int ldb,           // The first dimension of b; must be at least max(1, m, n) -> ???
                        PRECISION* s,                // output array to hold singular values
                        lapack_int* rank );       // output: The effective rank of A, that is, the number of singular 
                                                    //         values which are greater than rcond *s(1).
                                                // Return value info: If info = 0, the execution is successful.
                                                //                    If info = -i, the i-th parameter had an illegal value.
                                                //                    If info = i, then the algorithm for computing the SVD failed to converge; i indicates the number 
                                                //                    of off-diagonal elements of an intermediate bidiagonal form which did not converge to zero.
    };
    
                         
    template <> inline lapack_int MKLgelss<float>::execute(int matrix_order, 
                        lapack_int m,      
                        lapack_int n,     
                        lapack_int nrhs,  
                        std::complex<float>* a, 
                        lapack_int lda,           
                        std::complex<float>* b, 
                        lapack_int ldb,           
                        float* s,                
                        lapack_int* rank )
                {
                    return LAPACKE_cgelss(matrix_order, m, n, nrhs, a, lda, b, ldb, s, 1.1921e-07, rank);
                }
    template <> inline lapack_int MKLgelss<double>::execute(int matrix_order, 
                        lapack_int m,      
                        lapack_int n,     
                        lapack_int nrhs,  
                        std::complex<double>* a, 
                        lapack_int lda,           
                        std::complex<double>* b, 
                        lapack_int ldb,           
                        double* s,                
                        lapack_int* rank )
                {
                    return LAPACKE_zgelss(matrix_order, m, n, nrhs, a, lda, b, ldb, s, 2.2204e-16, rank);
                }

    
    template <typename PRECISION>
    MKL_INT pseudoInverse(MKLMatrix<std::complex<PRECISION> >&  A,
                       MKLMatrix<std::complex<PRECISION> >& inverse)
    {
        assure(A.getColumns() == inverse.getRows(), "Number of columns of A must be equal to number of rows of inverse");
        assure(A.getRows() == inverse.getColumns(), "Number of rows of A must be equal to number of columns of inverse");
        
        MKLMatrix<std::complex<PRECISION> > tmpA(A.getRows(), A.getColumns());
        
        memcpy(tmpA.getLocation(), A.getLocation(), sizeof(std::complex<PRECISION>) * A.getRows() * A.getColumns());
        
        int maxDim = std::max(A.getRows(), A.getColumns());
        int minDim = std::min(A.getRows(), A.getColumns());

        MKLMatrix<std::complex<PRECISION> > identity(maxDim, maxDim);
        memcpy(identity.getLocation(), 
               wns::SingletonHolder<Identity<std::complex<PRECISION> > >::Instance().get(maxDim)->getLocation(), 
               sizeof(std::complex<PRECISION>) * maxDim * maxDim);
        
        PRECISION* singularValues = static_cast<PRECISION*>(mkl_malloc(sizeof(PRECISION) * maxDim, 32));
        
        lapack_int rank;
        
        // is the inline going to work in this case with the MKLgless object?
        MKLgelss<PRECISION> mklgelss;
        lapack_int status = mklgelss.execute(  
                A.getColumns() <= A.getRows() ? LAPACK_ROW_MAJOR : LAPACK_COL_MAJOR, // how matrices are stored in memory
                maxDim,     // m
                minDim,     // n
                maxDim,  // nrhs for pinv, should be same as m
                tmpA.getLocation(), // A is input m-by-n matrix
                A.getColumns() <= A.getRows() ? minDim : maxDim, // lda, depends on storage mode
                identity.getLocation(),  // Input: for pinv, this is a n-by-m identity matrix
                                          // Output: Overwritten by the n-by-nrhs solution matrix X.
                maxDim,                   // ldb: The first dimension of b; must be at least max(1, m, n)
                singularValues,           // output array to hold singular values
                &rank );                  // output: The effective rank of A, that is, the number of singular 
                                          //         values which are greater than rcond *s(1).
        
        if (A.getColumns() <= A.getRows())
            memcpy(inverse.getLocation(), identity.getLocation(), sizeof(std::complex<PRECISION>) * A.getRows() * A.getColumns());
        else
        { // in this case, result is stored in COLUMN MAJOR mode a.k.a. transposed, so reverse while copying
            int leadingDimension = maxDim;
            int pos = 0;
            for(unsigned int i = 0;  i < A.getColumns(); i++)
            {
                for (unsigned int j=0; j < A.getRows(); j++)
                {
                    inverse[i][j] = identity.getLocation()[pos + j];
                }
                pos += leadingDimension;
            }
        }

        mkl_free(singularValues);
        
        if (status != 0)
            std::cout << "Problem with pinv, status=" << status << "\n";
        
        return rank;
    };
    
    template <typename PRECISION>
    void matrixAddAtoB(MKLMatrix<std::complex<PRECISION> >& A,
                       MKLMatrix<std::complex<PRECISION> >& B)
    {
        assure(A.getRows() == B.getRows(), "Must have same number of rows");
        assure(A.getColumns() == B.getColumns(), "Must have same number of columns");
        
        unsigned int size = A.getRows() * A.getColumns();
        for (unsigned int i = 0; i < size; i++)
            B.getLocation()[i] += A.getLocation()[i];
    }
    

    template <typename PRECISION>
    void scaleMatrixA(MKLMatrix<std::complex<PRECISION> >& A, PRECISION c)
    {
        unsigned int size = A.getRows() * A.getColumns();
        for (unsigned int i = 0; i < size; i++)
            A.getLocation()[i] *= c;
    }

    

    template <typename PRECISION>
    void matrixMultiplyCequalsAB(MKLMatrix<std::complex<PRECISION> >& C,
                                 MKLMatrix<std::complex<PRECISION> >& A,
                                 MKLMatrix<std::complex<PRECISION> >& B)
    {
        assure(C.getColumns() == B.getColumns(), "Result matrix C must have as many columns as second multiplicand B");
        assure(C.getRows() == A.getRows(), "Result matrix C must have as many rows as first multiplicand A");
        assure(A.getColumns() == B.getRows(), "First multiplicand must have as many columns as second multiplicand has rows");

        unsigned int m = A.getRows();      // rows of result C
        unsigned int n = B.getColumns();   // columns of result C
        unsigned int p = A.getColumns();   // joint dimension
        
        // C is m-by-n, A is m-by-p, and B is p-by-n, all matrices contain complex entries
        // C = A*B

        // when multiplying 4x4 or bigger matrices, MKL tends to be faster
        // but for smaller matrices the MKL function call overhead is too big
        if (m*n*p < 64)
        {

            // xPos points to the row in matrix X
            unsigned int i, j, k, cPos = 0, aPos = 0, bPos = 0;
            std::complex<PRECISION> tmp;

            for (i = 0; i < m; i++)      // over the rows of C
                {
                    for (j = 0; j < n; j++)  // over the columns of C
                    {
                        tmp = std::complex<PRECISION>(0.0, 0.0);
                        bPos = 0;
                        for (k = 0; k < p; k++)
                        {
                            tmp += A.getLocation()[aPos + k] * B.getLocation()[bPos + j];
                            bPos += n;
                        }
                        C.getLocation()[cPos + j] = tmp;
                    }
                    cPos += n;
                    aPos += p;
                }
        }
        else
        {
            MKLgemm<PRECISION> mklgemm;

            std::complex<PRECISION> one = std::complex<PRECISION>(1.0, 0.0);
            std::complex<PRECISION> zero = std::complex<PRECISION>(0.0, 0.0);
            
            // ?gemm computes C = alpha * A * B + beta * C
            mklgemm.execute(
                            CblasRowMajor,
                            CblasNoTrans,
                            CblasNoTrans,
                            m,
                            n,
                            p,
                            &one, // alpha
                            A.getLocation(),
                            p, // lda: leading dimension of A (columns of A)
                            B.getLocation(),
                            n, // ldb: leading dimension of B (columns of B)
                            &zero, // beta
                            C.getLocation(),
                            n // ldc: leading dimension of C (columns of C)
                            );
        }
    }


    // this is a special case where we don't want ComplexMatrices to be passed as the matrix
    // A (namely for computing the effective channel)
    template <typename PRECISION>
    void matrixMultiplyCequalsAB(MKLMatrix<std::complex<PRECISION> >& C,
                                std::complex<PRECISION>* A,
                                MKLMatrix<std::complex<PRECISION> >& B)
    {
        assure(C.getColumns() == B.getColumns(), "Result matrix C must have as many columns as second multiplicand B");

        unsigned int m = C.getRows();      // rows of result C
        unsigned int n = B.getColumns();   // columns of result C
        unsigned int p = B.getRows();   // joint dimension

        // C is m-by-n, A is m-by-p, and B is p-by-n, all matrices contain complex entries
        // C = A*B

        // when multiplying 4x4 or bigger matrices, MKL tends to be faster
        // but for smaller matrices the MKL function call overhead is too big
        if (m*n*p < 64)
        {

            // xPos points to the row in matrix X
            unsigned int i, j, k, cPos = 0, aPos = 0, bPos = 0;
            std::complex<PRECISION> tmp;

            for (i = 0; i < m; i++)      // over the rows of C
                    {
                        for (j = 0; j < n; j++)  // over the columns of C
                        {
                            tmp = std::complex<PRECISION>(0.0, 0.0);
                            bPos = 0;
                            for (k = 0; k < p; k++)
                            {
                                tmp += A[aPos + k] * B.getLocation()[bPos + j];
                                bPos += n;
                            }
                            C.getLocation()[cPos + j] = tmp;
                        }
                        cPos += n;
                        aPos += p;
                    }
        }
        else
        {
            MKLgemm<PRECISION> mklgemm;

            std::complex<PRECISION> one = std::complex<PRECISION>(1.0, 0.0);
            std::complex<PRECISION> zero = std::complex<PRECISION>(0.0, 0.0);

            // ?gemm computes C = alpha * A * B + beta * C
            mklgemm.execute(
                CblasRowMajor,
                CblasNoTrans,
                CblasNoTrans,
                m,
                n,
                p,
                &one, // alpha
                A,
                p, // lda: leading dimension of A (columns of A)
                B.getLocation(),
                n, // ldb: leading dimension of B (columns of B)
                &zero, // beta
                C.getLocation(),
                n // ldc: leading dimension of C (columns of C)
            );
        }
    }



    template <typename PRECISION>
    void matrixMultiplyCequalsNormOfAB(MKLMatrix<PRECISION>&  C,
                                       MKLMatrix<std::complex<PRECISION> >& A,
                                       MKLMatrix<std::complex<PRECISION> >& B)
    {
        // C is m-by-n, A is m-by-p, and B is p-by-n, A and B are complex matrices, C is real
        // The function multiplies the complex matrices A and B and stores the real-valued norm (abs^2) of each
        // matrix entry in the result matrix C. In Matlab notation this would be: C = abs(A*B).^2
        
        assure(C.getColumns() == B.getColumns(), "Result matrix C must have as many columns as second multiplicand B");
        assure(C.getRows() == A.getRows(), "Result matrix C must have as many rows as first multiplicand A");
        assure(A.getColumns() == B.getRows(), "First multiplicand must have as many columns as second multiplicand has rows");
        
        unsigned int m = A.getRows();      // rows of result C
        unsigned int n = B.getColumns();   // columns of result C
        unsigned int p = A.getColumns();   // joint dimension
    
        // xPos points to the row in matrix X
        unsigned int i, j, k, cPos = 0, aPos = 0, bPos = 0;
        std::complex<PRECISION> tmp;

        for (i = 0; i < m; i++)      // over the rows of C
            {
                for (j = 0; j < n; j++)  // over the columns of C
                {
                    tmp = std::complex<PRECISION>(0.0, 0.0);
                    bPos = 0;
                    for (k = 0; k < p; k++)
                    {
                        tmp += A.getLocation()[aPos + k] * B.getLocation()[bPos + j];
                        bPos += n;
                    }
                    C.getLocation()[cPos + j] = norm(tmp);
                }
                cPos += n;
                aPos += p;
            }
    }


    // we cannot do hermitian in place because we can not change dimensions of matrix after construction
    template <typename PRECISION>
    boost::shared_ptr<MKLMatrix<std::complex<PRECISION> > > matrixHermitian(MKLMatrix<std::complex<PRECISION> >& A)
    {
        boost::shared_ptr<MKLMatrix<std::complex<PRECISION> > > result(new MKLMatrix<std::complex<PRECISION> >(A.getColumns(), A.getRows()));
            
        matrixHermitian<PRECISION>(A, *result);

        return result;
    }

    template <typename T>
    void displayMatrix(MKLMatrix<T>& A)
    {
        // Matrix output in Matlab compatible format

#pragma omp critical
{
        std::cout << "[";
        for (unsigned int i = 0; i < A.getRows(); i++)
        {
            for (unsigned int j = 0; j < A.getColumns(); j++)
                std::cout << A[i][j] << "\t\t";
            std::cout << ";...\n";
        }
        std::cout << "]\n";
}
    }

    template <typename PRECISION>
    void matrixHermitian(MKLMatrix<std::complex<PRECISION> >& A,
                         MKLMatrix<std::complex<PRECISION> >& Ahermitian)
    {
        assure(A.getColumns() == Ahermitian.getRows(), "result matrix must have as many rows as input has columns");
        assure(A.getRows() == Ahermitian.getColumns(), "result matrix must have as many columns as input has rows");

        unsigned int m = A.getRows();
        unsigned int n = A.getColumns();
        
        unsigned int size = m*n - 1;
        
        Ahermitian.getLocation()[0] = conj(A.getLocation()[0]);
        
        if (!size) // we operate on a 1x1 "matrix"
            return;
        
        unsigned int swap = 0;
        for(unsigned int i = 1; i < size; i++)
        {
            swap = (swap + n) % size;
            Ahermitian.getLocation()[i] = conj(A.getLocation()[swap]);
        }
        Ahermitian.getLocation()[size] = conj(A.getLocation()[size]);
    }
    
    // compute the inverse of a m-by-m square matrix. If not invertible, compute pseudoinverse
    template <typename PRECISION>
    void matrixInverse(MKLMatrix<std::complex<PRECISION> >& A,
                       MKLMatrix<std::complex<PRECISION> >& inverse)
    {
        assure(A.getRows() == A.getColumns(), "Input must be square");
        assure(inverse.getRows() == inverse.getColumns(), "Inverse will have to be square");
        assure(A.getRows() == inverse.getRows(), "A and inverse must be of identical size");

        unsigned int m = A.getRows();
        
        if (m == 1)
        {
            inverse.getLocation()[0] = std::complex<PRECISION>(1.0, 0.0) / A.getLocation()[0];
            return;
        }
        
        if (m == 2)
        {
            // For a 2x2 matrix
            // a b
            // c d
            // the inverse exists for det=(ad - bc) != 0
            // and will be (1/det) *  d -b
            //                       -c  a

            std::complex<PRECISION> det = A.getLocation()[0]*A.getLocation()[3] - A.getLocation()[1]*A.getLocation()[2];


            PRECISION eps = 1e-30; // float should go to 1e-38 and double to 1e-308 let's be conservative. if it's smaller (or actually 0) we let MKL take care of it
            if (((det.real() < -eps) || (det.real() > eps)) || ((det.imag() < -eps) || (det.imag() > eps)))
            {
                std::complex<PRECISION> div = std::complex<PRECISION>(1.0, 0.0) / det;
                inverse.getLocation()[0] = A.getLocation()[3] * div;
                inverse.getLocation()[1] = -A.getLocation()[1] * div;
                inverse.getLocation()[2] = -A.getLocation()[2] * div;
                inverse.getLocation()[3] = A.getLocation()[0] * div;
                
                return;
            }
            else
                pseudoInverse<PRECISION>(A, inverse);
        }
        else // matrix bigger than 2x2
        {
            memcpy(inverse.getLocation(), A.getLocation(), sizeof(std::complex<PRECISION>) * m*m);
            
            // for our use cases, most matrices will be pos. def. hermitian so try to treat it like one
            MKLpotrf<PRECISION> potrf;
            lapack_int status = potrf.execute( LAPACK_ROW_MAJOR,
                                               'U',
                                               m,
                                               inverse.getLocation(),
                                               m);
            
            if (status == 0)
            {
                MKLpotri<PRECISION> potri;
                status = potri.execute(LAPACK_ROW_MAJOR,
                                       'U',
                                       m,
                                       inverse.getLocation(),
                                       m);
                if (status == 0)
                {
                    // call succeeded, the result is now in the upper triangle only, so mirror to lower part
                    unsigned int posUpper = 0;
                    unsigned int posLower = 0;
                    
                    for (unsigned int i = 0; i < m; i++)
                    {
                        posLower = posUpper + m;
                        for (unsigned int j = i + 1; j < m; j++) // only consider upper triangle when taking transpose
                            {
                                inverse.getLocation()[posLower + i] = conj(inverse.getLocation()[posUpper + j]);
                                posLower += m;
                            }
                            posUpper += m;
                    }
                    
                    
                    return;
                }
                else if (status > 0) // not a pos.def. hermitian matrix
                        pseudoInverse<PRECISION>(A, inverse);
                        else
                            assure(status >= 0, "Illegal value for potri");
            }
            else if (status > 0) // not a pos.def. hermitian matrix
                    pseudoInverse<PRECISION>(A, inverse);
            else
                assure(status >= 0, "Illegal value for potrf");
        }
    }
    template <typename PRECISION>
    bool matrixSVD(MKLMatrix<std::complex<PRECISION> >& A,
                   MKLMatrix<PRECISION >& SVector)
    {
        /* CGESVD prototype */
        /********************************************************************//*
            *        extern void cgesvd( char* jobu, char* jobvt, int* m, int* n, fcomplex* a,
            *                int* lda, float* s, fcomplex* u, int* ldu, fcomplex* vt, int* ldvt,
            *                fcomplex* work, int* lwork, float* rwork, int* info );
            *        ********************************************************************/
            // for detailed example please have a look at the following link
            // http://software.intel.com/sites/products/documentation/hpc/mkl/lapack/mkl_lapack_examples/cgesvd_ex.c.htm/
            
        MKL_INT m = A.getRows();
        MKL_INT n = A.getColumns();
        
        MKL_INT lda = m; // should be m according to the manual
        MKL_INT ldu = m;
        MKL_INT ldvt = n;
        int minDim = m;
        
        // First we need to take the hermitian of matrix A, because the cgesvd() function reads the matrix in column-major mode.
        // another reason is that the SVD function returns V^H and not V. So taking hermitian of A before calling the function
        // will return V instead of V^H.
        MKLMatrix<std::complex<float> > tempA(A.getColumns(), A.getRows());
        
        matrixHermitian<PRECISION>(A, tempA);
        
        //we need this tempU because CGESVD() function returns U^H
        MKLMatrix<std::complex<float> > U(m, m);
        MKLMatrix<std::complex<float> > V(n, n);
        
        // According to MKL docs:
        // lwork >= 2*min(m, n) + max(m, n) (for complex flavors).
        // For good performance, lwork must generally be larger.
        MKL_INT lwork = (2*m + n) * 10; // big enough!?
        std::complex<PRECISION>* work = static_cast<std::complex<float>*>(mkl_malloc(sizeof(std::complex<float>) * lwork, 32 ));
        
        // Workspace array, DIMENSION at least max(1, 5*min(m, n)). Used in complex flavors only.
        PRECISION* rwork = static_cast<float*>(mkl_malloc(sizeof(float) * 10 * 5 * minDim, 32)); // how dows cgesvd know about this size?
        MKL_INT info = 0;
        
        // CGESVD computes the singular value decomposition (SVD) of a complex M-by-N matrix A, optionally
        // computing the left and/or right singular vectors. The SVD is written
        // A = U * SIGMA * conjugate-transpose(V)
        
        // check whether it makes sense to query the workspace size first. we might save this time by
        // providing a big-enough workspace size in the first place
        
        cgesvd( "None",     // Specifies options for computing all or part of the matrix U
                "None",      // Specifies options for computing all or part of the matrix V**H
                &m,         // The number of rows of the input matrix A.  M >= 0
                &n,         // The number of columns of the input matrix A.  N >= 0
                tempA.getLocation(),
                // On  entry,  the  M-by-N  matrix A.  On exit, if JOBU = 'O',
                // A is overwritten with the first min(m,n) columns of U (the left singular vectors,
                // stored columnwise); if JOBVT = 'O', A is overwritten with the first min(m,n) rows of
                // V**H (the right singular vectors, stored rowwise); if JOBU != 'O' and JOBVT !=. 'O',
                // the contents of A are destroyed.
                &lda,       // The leading dimension of the array A.  LDA >= max(1,M).
                SVector.getLocation(),          // The singular values of A, sorted so that S(i) >= S(i+1).
                U.getLocation(),
                // (LDU,M)  if  JOBU = 'A' or (LDU,min(M,N)) if JOBU = 'S'.  If JOBU = 'A', U contains the
                // M-by-M unitary matrix U; if JOBU = 'S', U contains the first min(m,n) columns
                // of U (the left singular vectors, stored columnwise); if JOBU = 'N' or 'O', U is not referenced.
                &ldu,       // The leading dimension of the array U.  LDU >= 1; if JOBU = 'S' or 'A', LDU >= M.
                V.getLocation(),
                // If JOBVT = 'A', VT contains the N-by-N unitary matrix V**H; if JOBVT = 'S', VT contains the
                // first min(m,n) rows of V**H (the right singular vectors, stored rowwise);
                // if JOBVT = 'N' or 'O', VT is not referenced.
                &ldvt,      // The leading dimension of the array VT.  LDVT >= 1; if JOBVT = 'A', LDVT >= N;
                // if JOBVT = 'S', LDVT >= min(M,N).
                work,       // (workspace/output) COMPLEX array, dimension (MAX(1,LWORK))
                // On exit, if INFO = 0, WORK(1) returns the optimal LWORK.
                &lwork,     // (input) INTEGER
                // The  dimension  of  the  array WORK.  LWORK >=  MAX(1,2*MIN(M,N)+MAX(M,N)).  For good performance,
                // LWORK should generally be larger.  If LWORK = -1, then a workspace
                // query is assumed; the routine only calculates the optimal size of the WORK array, returns this
                // value as the first entry of the  WORK  array,  and  no  error  message
                // related to LWORK is issued by XERBLA.
                rwork,      // (workspace) REAL array, dimension (5*min(M,N))
                // On  exit,  if  INFO  > 0, RWORK(1:MIN(M,N)-1) contains the unconverged superdiagonal elements of
                // an upper bidiagonal matrix B whose diagonal is in S (not necessarily
                // sorted).  B satisfies A = U * B * VT, so it has the same singular values as A, and singular
                // vectors related by U and VT.
                &info       // (output) INTEGER
                // = 0:  successful exit.
                // < 0:  if INFO = -i, the i-th argument had an illegal value.
                // > 0:  if CBDSQR did not converge, INFO specifies how many superdiagonals of an intermediate
                //       bidiagonal form B did not converge to zero. See the description of  RWORK above for details.
        );
        
        MKL_free(work);
        MKL_free(rwork);
        
        if (info > 0)
        {
            std::cout << "The algorithm for computing SVD failed to converge\n";
            return false;
        }

        return true;
        
        
        
    }
    template <typename PRECISION>
    bool matrixSVD(MKLMatrix<std::complex<PRECISION> >& A,
                   MKLMatrix<PRECISION >& SVector,
                   MKLMatrix<std::complex<PRECISION> >& V)
    {
        /* CGESVD prototype */
        /********************************************************************//*
            *        extern void cgesvd( char* jobu, char* jobvt, int* m, int* n, fcomplex* a,
            *                int* lda, float* s, fcomplex* u, int* ldu, fcomplex* vt, int* ldvt,
            *                fcomplex* work, int* lwork, float* rwork, int* info );
            *        ********************************************************************/
            // for detailed example please have a look at the following link
            // http://software.intel.com/sites/products/documentation/hpc/mkl/lapack/mkl_lapack_examples/cgesvd_ex.c.htm/
            
        MKL_INT m = A.getRows();
        MKL_INT n = A.getColumns();
        
        MKL_INT lda = m; // should be m according to the manual
        MKL_INT ldu = m;
        MKL_INT ldvt = n;
        int minDim = m;
        
        // First we need to take the hermitian of matrix A, because the cgesvd() function reads the matrix in column-major mode.
        // another reason is that the SVD function returns V^H and not V. So taking hermitian of A before calling the function
        // will return V instead of V^H.
        MKLMatrix<std::complex<float> > tempA(A.getColumns(), A.getRows());
        
        matrixHermitian<PRECISION>(A, tempA);
        
        //we need this tempU because CGESVD() function returns U^H
        MKLMatrix<std::complex<float> > U(m, m);
        
        // According to MKL docs:
        // lwork >= 2*min(m, n) + max(m, n) (for complex flavors).
        // For good performance, lwork must generally be larger.
        MKL_INT lwork = (2*m + n) * 10; // big enough!?
        std::complex<PRECISION>* work = static_cast<std::complex<float>*>(mkl_malloc(sizeof(std::complex<float>) * lwork, 32 ));
        
        // Workspace array, DIMENSION at least max(1, 5*min(m, n)). Used in complex flavors only.
        PRECISION* rwork = static_cast<float*>(mkl_malloc(sizeof(float) * 10 * 5 * minDim, 32)); // how dows cgesvd know about this size?
        MKL_INT info = 0;
        
        // CGESVD computes the singular value decomposition (SVD) of a complex M-by-N matrix A, optionally
        // computing the left and/or right singular vectors. The SVD is written
        // A = U * SIGMA * conjugate-transpose(V)
        
        // check whether it makes sense to query the workspace size first. we might save this time by
        // providing a big-enough workspace size in the first place
        
        cgesvd( "None",     // Specifies options for computing all or part of the matrix U
                "All",      // Specifies options for computing all or part of the matrix V**H
                &m,         // The number of rows of the input matrix A.  M >= 0
                &n,         // The number of columns of the input matrix A.  N >= 0
                tempA.getLocation(),
                // On  entry,  the  M-by-N  matrix A.  On exit, if JOBU = 'O',
                // A is overwritten with the first min(m,n) columns of U (the left singular vectors,
                // stored columnwise); if JOBVT = 'O', A is overwritten with the first min(m,n) rows of
                // V**H (the right singular vectors, stored rowwise); if JOBU != 'O' and JOBVT !=. 'O',
                // the contents of A are destroyed.
                &lda,       // The leading dimension of the array A.  LDA >= max(1,M).
                SVector.getLocation(),          // The singular values of A, sorted so that S(i) >= S(i+1).
                U.getLocation(),
                // (LDU,M)  if  JOBU = 'A' or (LDU,min(M,N)) if JOBU = 'S'.  If JOBU = 'A', U contains the
                // M-by-M unitary matrix U; if JOBU = 'S', U contains the first min(m,n) columns
                // of U (the left singular vectors, stored columnwise); if JOBU = 'N' or 'O', U is not referenced.
                &ldu,       // The leading dimension of the array U.  LDU >= 1; if JOBU = 'S' or 'A', LDU >= M.
                V.getLocation(),
                // If JOBVT = 'A', VT contains the N-by-N unitary matrix V**H; if JOBVT = 'S', VT contains the
                // first min(m,n) rows of V**H (the right singular vectors, stored rowwise);
                // if JOBVT = 'N' or 'O', VT is not referenced.
                &ldvt,      // The leading dimension of the array VT.  LDVT >= 1; if JOBVT = 'A', LDVT >= N;
                // if JOBVT = 'S', LDVT >= min(M,N).
                work,       // (workspace/output) COMPLEX array, dimension (MAX(1,LWORK))
                // On exit, if INFO = 0, WORK(1) returns the optimal LWORK.
                &lwork,     // (input) INTEGER
                // The  dimension  of  the  array WORK.  LWORK >=  MAX(1,2*MIN(M,N)+MAX(M,N)).  For good performance,
                // LWORK should generally be larger.  If LWORK = -1, then a workspace
                // query is assumed; the routine only calculates the optimal size of the WORK array, returns this
                // value as the first entry of the  WORK  array,  and  no  error  message
                // related to LWORK is issued by XERBLA.
                rwork,      // (workspace) REAL array, dimension (5*min(M,N))
                // On  exit,  if  INFO  > 0, RWORK(1:MIN(M,N)-1) contains the unconverged superdiagonal elements of
                // an upper bidiagonal matrix B whose diagonal is in S (not necessarily
                // sorted).  B satisfies A = U * B * VT, so it has the same singular values as A, and singular
                // vectors related by U and VT.
                &info       // (output) INTEGER
                // = 0:  successful exit.
                // < 0:  if INFO = -i, the i-th argument had an illegal value.
                // > 0:  if CBDSQR did not converge, INFO specifies how many superdiagonals of an intermediate
                //       bidiagonal form B did not converge to zero. See the description of  RWORK above for details.
        );
        
        MKL_free(work);
        MKL_free(rwork);
        
        if (info > 0)
        {
            std::cout << "The algorithm for computing SVD failed to converge\n";
            return false;
        }

        return true;
        
        

    }
    template <typename PRECISION>
    bool matrixSVD(MKLMatrix<std::complex<PRECISION> >& A,
                   MKLMatrix<std::complex<PRECISION> >& U,
                   MKLMatrix< PRECISION >& SVector,
                   MKLMatrix<std::complex<PRECISION> >& V)
    {
        /* CGESVD prototype */
        /********************************************************************//*
            *        extern void cgesvd( char* jobu, char* jobvt, int* m, int* n, fcomplex* a,
            *                int* lda, float* s, fcomplex* u, int* ldu, fcomplex* vt, int* ldvt,
            *                fcomplex* work, int* lwork, float* rwork, int* info );
            *        ********************************************************************/
            // for detailed example please have a look at the following link
            // http://software.intel.com/sites/products/documentation/hpc/mkl/lapack/mkl_lapack_examples/cgesvd_ex.c.htm/
            
        MKL_INT m = A.getRows();
        MKL_INT n = A.getColumns();
        
        MKL_INT lda = m; // should be m according to the manual
        MKL_INT ldu = m;
        MKL_INT ldvt = n;
        int minDim = m;
        
        // First we need to take the hermitian of matrix A, because the cgesvd() function reads the matrix in column-major mode.
        // another reason is that the SVD function returns V^H and not V. So taking hermitian of A before calling the function
        // will return V instead of V^H.
        MKLMatrix<std::complex<float> > tempA(A.getColumns(), A.getRows());
        
        matrixHermitian<PRECISION>(A, tempA);
        
        //we need this tempU because CGESVD() function returns U^H
        MKLMatrix<std::complex<float> > tempU(m, m);
        
        // According to MKL docs:
        // lwork >= 2*min(m, n) + max(m, n) (for complex flavors).
        // For good performance, lwork must generally be larger.
        MKL_INT lwork = (2*m + n) * 10; // big enough!?
        std::complex<PRECISION>* work = static_cast<std::complex<float>*>(mkl_malloc(sizeof(std::complex<float>) * lwork, 32 ));
        
        // Workspace array, DIMENSION at least max(1, 5*min(m, n)). Used in complex flavors only.
        PRECISION* rwork = static_cast<float*>(mkl_malloc(sizeof(float) * 10 * 5 * minDim, 32)); // how dows cgesvd know about this size?
        MKL_INT info = 0;
        
        // CGESVD computes the singular value decomposition (SVD) of a complex M-by-N matrix A, optionally
        // computing the left and/or right singular vectors. The SVD is written
        // A = U * SIGMA * conjugate-transpose(V)
        
        // check whether it makes sense to query the workspace size first. we might save this time by
        // providing a big-enough workspace size in the first place
        
        cgesvd( "All",     // Specifies options for computing all or part of the matrix U
                "All",      // Specifies options for computing all or part of the matrix V**H
                &m,         // The number of rows of the input matrix A.  M >= 0
                &n,         // The number of columns of the input matrix A.  N >= 0
                tempA.getLocation(),
                // On  entry,  the  M-by-N  matrix A.  On exit, if JOBU = 'O',
                // A is overwritten with the first min(m,n) columns of U (the left singular vectors,
                // stored columnwise); if JOBVT = 'O', A is overwritten with the first min(m,n) rows of
                // V**H (the right singular vectors, stored rowwise); if JOBU != 'O' and JOBVT !=. 'O',
                // the contents of A are destroyed.
                &lda,       // The leading dimension of the array A.  LDA >= max(1,M).
                SVector.getLocation(),          // The singular values of A, sorted so that S(i) >= S(i+1).
                tempU.getLocation(),
                // (LDU,M)  if  JOBU = 'A' or (LDU,min(M,N)) if JOBU = 'S'.  If JOBU = 'A', U contains the
                // M-by-M unitary matrix U; if JOBU = 'S', U contains the first min(m,n) columns
                // of U (the left singular vectors, stored columnwise); if JOBU = 'N' or 'O', U is not referenced.
                &ldu,       // The leading dimension of the array U.  LDU >= 1; if JOBU = 'S' or 'A', LDU >= M.
                V.getLocation(),
                // If JOBVT = 'A', VT contains the N-by-N unitary matrix V**H; if JOBVT = 'S', VT contains the
                // first min(m,n) rows of V**H (the right singular vectors, stored rowwise);
                // if JOBVT = 'N' or 'O', VT is not referenced.
                &ldvt,      // The leading dimension of the array VT.  LDVT >= 1; if JOBVT = 'A', LDVT >= N;
                // if JOBVT = 'S', LDVT >= min(M,N).
                work,       // (workspace/output) COMPLEX array, dimension (MAX(1,LWORK))
                // On exit, if INFO = 0, WORK(1) returns the optimal LWORK.
                &lwork,     // (input) INTEGER
                // The  dimension  of  the  array WORK.  LWORK >=  MAX(1,2*MIN(M,N)+MAX(M,N)).  For good performance,
                // LWORK should generally be larger.  If LWORK = -1, then a workspace
                // query is assumed; the routine only calculates the optimal size of the WORK array, returns this
                // value as the first entry of the  WORK  array,  and  no  error  message
                // related to LWORK is issued by XERBLA.
                rwork,      // (workspace) REAL array, dimension (5*min(M,N))
                // On  exit,  if  INFO  > 0, RWORK(1:MIN(M,N)-1) contains the unconverged superdiagonal elements of
                // an upper bidiagonal matrix B whose diagonal is in S (not necessarily
                // sorted).  B satisfies A = U * B * VT, so it has the same singular values as A, and singular
                // vectors related by U and VT.
                &info       // (output) INTEGER
                // = 0:  successful exit.
                // < 0:  if INFO = -i, the i-th argument had an illegal value.
                // > 0:  if CBDSQR did not converge, INFO specifies how many superdiagonals of an intermediate
                //       bidiagonal form B did not converge to zero. See the description of  RWORK above for details.
        );
        
        MKL_free(work);
        MKL_free(rwork);
        
        matrixHermitian<PRECISION>(tempU, U);
        if (info > 0)
        {
            std::cout << "The algorithm for computing SVD failed to converge\n";
            return false;
        }

        return true;
        


    }
    
    template <typename PRECISION>
    void matrixMultiplyCequalsAAhermitian(MKLMatrix<std::complex<PRECISION> >& C,
                                          MKLMatrix<std::complex<PRECISION> >& A)
    {
        assure(C.getRows() == C.getColumns(), "Resulting matrix C will be square");
        assure(C.getRows() == A.getRows(), "Resulting matrix C will have as many rows as input A");

        unsigned int m = A.getRows();
        unsigned int n = A.getColumns();
        // C = A * A^H

        // xPos points to the row in matrix X
        unsigned int i, j, k, cPos = 0, aPos = 0, bPos = 0, cTransPos;
        std::complex<PRECISION> tmp;

        for (i = 0; i < m; i++)      // over the rows of C
            {
                bPos = 0;
                cTransPos = cPos;
                bPos = aPos;
                for (j = i; j < m; j++)  // over the columns of C
                {
                    tmp = std::complex<PRECISION>(0.0, 0.0);
                    for (k = 0; k < n; k++)
                    {
                        tmp += A.getLocation()[aPos + k] * conj(A.getLocation()[bPos + k]);
                    }

                    C.getLocation()[cPos + j] = tmp; // C[i][j]
                    C.getLocation()[cTransPos + i] = conj(tmp); // C[j][i] for the diagonal we overwrite but that does not make a difference here

                    bPos += n;
                    cTransPos +=m;
                }
                cPos += m;
                aPos += n;
            }
    }


    template <typename PRECISION>
    void matrixMultiplyCequalsAlphaSquareTimesAAhermitianPlusC(MKLMatrix<std::complex<PRECISION> >& C,
                                                               MKLMatrix<std::complex<PRECISION> >&  A,
                                                               PRECISION alphaSquare)
    {
        assure(C.getRows() == C.getColumns(), "Resulting matrix C will be square");
        assure(C.getRows() == A.getRows(), "Resulting matrix C will have as many rows as input A");
        
        unsigned int m = A.getRows();
        unsigned int n = A.getColumns();
        
        // C = A * A^H

        // xPos points to the row in matrix X
        unsigned int i, j, k, cPos = 0, aPos = 0, bPos = 0, cTransPos;
        std::complex<PRECISION> tmp;

        for (i = 0; i < m; i++)      // over the rows of C
            {
                bPos = 0;
                cTransPos = cPos;
                bPos = aPos;
                for (j = i; j < m; j++)  // over the columns of C
                {
                    tmp = std::complex<PRECISION>(0.0, 0.0);
                    for (k = 0; k < n; k++)
                    {
                        tmp += A.getLocation()[aPos + k] * conj(A.getLocation()[bPos + k]);
                    }
                    tmp *= alphaSquare;

                    C.getLocation()[cPos + j] += tmp; // C[i][j]

                    if (i != j)
                    {
                        C.getLocation()[cTransPos + i] += conj(tmp); // C[j][i]
                    }
                    
                    bPos += n;
                    cTransPos +=m;
                }
                cPos += m;
                aPos += n;
            }
    }


    template <typename PRECISION>
    inline
    void matrixMultiplyCequalsAhermitianA(MKLMatrix<std::complex<PRECISION> >&  C,
                                          MKLMatrix<std::complex<PRECISION> >&  A)
    {
        assure(C.getRows() == C.getColumns(), "Resulting matrix C will be square");
        assure(C.getRows() == A.getColumns(), "Resulting matrix C will have as many rows as input A has columns");
        
        unsigned int m = A.getRows();
        unsigned int n = A.getColumns();
        // C = A^H * A with A being m-by-n and the result thus being n-by-n

        // xPos points to the row in matrix X
        unsigned int i, j, k, cPos = 0, aPos = 0, bPos = 0, cTransPos = 0;
        std::complex<PRECISION> tmp;

        for (i = 0; i < n; i++)      // over the rows of C and the rows of "A, i.e. AH"
            {

                cTransPos = cPos;
                for (j = i; j < n; j++)  // over the columns of C and the columns of "B, i.e. A"
                {
                    bPos = 0;
                    tmp = std::complex<PRECISION>(0.0, 0.0);
                    for (k = 0; k < m; k++) // over columns of "A" and rows of "B"
                    {
                        tmp += conj(A.getLocation()[bPos + i]) * A.getLocation()[bPos + j];
                        bPos += n;
                    }

                    C.getLocation()[cPos + j] = tmp; // C[i][j]
                    C.getLocation()[cTransPos + i] = conj(tmp); // C[j][i] for the diagonal we overwrite but that does not make a difference here


                    cTransPos +=n;
                }
                aPos += m;
                cPos += n;
            }
    }
    
    bool findNullVectors(MKLMatrix<std::complex<float> >&  A,
                         MKLMatrix<std::complex<float> >&  nullA);
                         
    float findSubspaceAngle(MKLMatrix<std::complex<float> >&  A,
                         MKLMatrix<std::complex<float> >&  B);
        // append a matrix to another matrix by adding rows
    
    inline
    bool appendRowsToMatrix(MKLMatrix<std::complex<float> >&   A, 
                            MKLMatrix<std::complex<float> >&  append, 
                            unsigned int startAppendfromRow)
    {
        unsigned int m = A.getRows();
        unsigned int n = A.getColumns();
        unsigned int appendRows = append.getRows();
        unsigned int appendCols = append.getColumns();
        if (m == 0 || n == 0 || appendRows == 0 || appendCols == 0)
        {
            std::cout << "Error: Append function called on a matrix with no space in appendee or \n";
            return false;
            
        }
        if (startAppendfromRow >= m)
        {
            std::cout << "Error: Cannot append more rows to the matrix. Matrix full\n";
            return false;
        }
        if (appendCols != n)
        {
            std::cout << "Error: appendRows() must be called with matrices having equal columns\n";
            return false;
        }

        for (unsigned int row = 0; row < appendRows; row++)
            for (unsigned int col = 0; col < appendCols; col++)
            {
                A[startAppendfromRow + row][col] = append[row][col];
            }
        return true;
    }

    // append a matrix to another matrix by adding rows
    template <typename T>
    void copyMatrixToSubmatrix(MKLMatrix<T>&  A,      // target matrix
                               MKLMatrix<T>&  source,  // source matrix
                               unsigned int startCopyFromRow,
                               unsigned int startCopyFromColumn,
                               unsigned int copyNumRows,
                               unsigned int copyNumCols)
    {
        assure(copyNumCols, "Cannot copy 0 columns");
        assure(copyNumRows, "Cannot copy 0 rows");
        assure(A.getRows() >= startCopyFromRow, "Trying to copy to nonexisting rows");
        assure(A.getColumns() >= startCopyFromColumn, "Trying to copy to nonexisting columns");
        assure(A.getRows() >= startCopyFromRow + copyNumRows, "Trying to copy too many rows, out of bounds");
        assure(A.getColumns() >= startCopyFromColumn + copyNumCols, "Trying to copy too many columns, out of bounds");
        
        for (unsigned int row = 0; row < copyNumRows; row++)
            for (unsigned int col = 0; col < copyNumCols; col++)
                A[startCopyFromRow + row][startCopyFromColumn + col] = source[row][col];
    }

    // append a matrix to another matrix by adding rows
    template <typename T>
    void extractSubMatrix(MKLMatrix<T>&  target,      // target matrix
                          MKLMatrix<T>&  source,  // source matrix
                          unsigned int startCopyFromRow,
                          unsigned int startCopyFromColumn,
                          unsigned int copyNumRows,
                          unsigned int copyNumCols)
    {
        assure(copyNumCols, "Cannot copy 0 columns");
        assure(copyNumRows, "Cannot copy 0 rows");
        assure(target.getRows() == copyNumRows, "Target matrix must have exactly copyNumRows rows");
        assure(target.getColumns() == copyNumCols, "Target matrix must have exactly copyNumCols columns");

        assure(source.getRows() >  startCopyFromRow + copyNumRows - 1, "Trying to copy out-of-bound row");
        assure(source.getColumns() >  startCopyFromColumn + copyNumCols - 1, "Trying to copy out-of-bound column");
        
        for (unsigned int row = 0; row <  copyNumRows; row++)
            for (unsigned int col = 0; col < copyNumCols; col++)
                target[row][col] = source[startCopyFromRow + row][startCopyFromColumn + col];
    }

    template <typename PRECISION>
    PRECISION matrixNorm(MKLMatrix<std::complex<PRECISION> >&  A)
    {
        // Calculate the frobenius norm of the matrix
        PRECISION result = 0.0;
        unsigned int m = A.getRows();      // rows of result A
        unsigned int n = A.getColumns();   // columns of A
    
        for (unsigned int j = 0; j < m*n; j++)      
        {
            result += norm(A.getLocation()[j]);
        }
        return sqrt(result);
    }


    template <typename PRECISION>
    PRECISION matrixNormSquared(MKLMatrix<std::complex<PRECISION> >&  A)
    {
        // Calculate the frobenius norm of the matrix
        PRECISION result = 0.0;
        unsigned int m = A.getRows();      // rows of result A
        unsigned int n = A.getColumns();   // columns of A
    
        for (unsigned int j = 0; j < m*n; j++)      
        {
            result += norm(A.getLocation()[j]);
        }
        return result;
    }
    
    template <typename PRECISION>
    void applyWhiteGaussianNoiseToA(MKLMatrix<std::complex<PRECISION> >&  A, PRECISION perElementPower)
    {
        static boost::normal_distribution<PRECISION> normal(0.0, 1.0 / sqrt(2.0));
        static boost::variate_generator<boost::mt19937&, 
                                    boost::normal_distribution<PRECISION> > normalDistribution(*wns::simulator::getRNG(), normal);
        assure(perElementPower > 0, "Need positive power for error matrix elements");
        
        PRECISION scaleRandom = sqrt(perElementPower); 
        
        unsigned int num = A.getColumns() * A.getRows();
        for (unsigned int j = 0; j < num; j++)      
        {
            A.getLocation()[j] = A.getLocation()[j] + scaleRandom * std::complex<PRECISION>(normalDistribution(), normalDistribution());
        }
    }
    
    template <typename PRECISION>
    void fillWhiteGaussianNoise(MKLMatrix<std::complex<PRECISION> >&  A, PRECISION perElementPower)
    {
        static boost::normal_distribution<PRECISION> normal(0.0, 1.0 / sqrt(2.0));
        static boost::variate_generator<boost::mt19937&, 
                                    boost::normal_distribution<PRECISION> > normalDistribution(*wns::simulator::getRNG(), normal);
        assure(perElementPower > 0, "Need positive power for error matrix elements");
        
        PRECISION scaleRandom = sqrt(perElementPower); 
        
        unsigned int num = A.getColumns() * A.getRows();
        for (unsigned int j = 0; j < num; j++)      
        {
            A.getLocation()[j] = scaleRandom * std::complex<PRECISION>(normalDistribution(), normalDistribution());
        }
    }
    
    

    template <typename PRECISION>
    std::complex<PRECISION> trace(MKLMatrix<std::complex<PRECISION> >& A)
    {
        assure(A.getRows() == A.getColumns(), "Matrix must be square");
        
        std::complex<PRECISION> result = 0;
        
        for (unsigned int i = 0; i < A.getColumns(); i++)
        {
            result += A[i][i];
        }
        
        return result;
    }

    template <typename PRECISION>
    std::complex<PRECISION> dotProductOfAColumnAndConjugateBColumn(MKLMatrix<std::complex<PRECISION> >& A, unsigned int columnA,
                                                                   MKLMatrix<std::complex<PRECISION> >& B, unsigned int columnB)
    {
        assure(A.getRows() == B.getRows(), "Need identical number of rows");
        assure(columnA < A.getColumns(), "Column index A out of bounds");
        assure(columnB < B.getColumns(), "Column index B out of bounds");
        
        std::complex<PRECISION> dotProduct = std::complex<PRECISION>(0, 0);

        for (unsigned int i = 0; i < A.getRows(); i++)
        {
            dotProduct += A[i][columnA] * std::conj(B[i][columnB]);
        }
        
        return dotProduct;
    }
    
    template <typename PRECISION>
    void matrixCisAminusB(MKLMatrix<std::complex<PRECISION> >& A,
                          MKLMatrix<std::complex<PRECISION> >& B,
                          MKLMatrix<std::complex<PRECISION> >& C)
    {
        assure(A.getRows() == B.getRows(), "Must have same number of rows");
        assure(A.getColumns() == B.getColumns(), "Must have same number of columns");
        assure(A.getRows() == C.getRows(), "Must have same number of rows");
        assure(A.getColumns() == C.getColumns(), "Must have same number of columns");
        
        unsigned int size = A.getRows() * A.getColumns();
        for (unsigned int i = 0; i < size; i++)
            C.getLocation()[i] = A.getLocation()[i] - B.getLocation()[i];
    }
    
    template <typename PRECISION>
    bool log2DetOfPosDefHermitianMatrix(MKLMatrix<std::complex<PRECISION> >& A, double& logDet)
    {
        // a hermitian matrix always has a real determinant and hence a real logarithm
        // in the hard-coded implementations for sizes 1-3 we don't check for real determinants
        // to avoid precision issues
        
       double log2Inverse = 1.0 / log(2.0);
       double determinant;
       
       assure(A.getRows() == A.getColumns(), "Matrix must be square");
       
       switch(A.getRows())
       {
        case 0:
            logDet = 0.0; // that's what Matlab computes for an empty matrix
            return true;
            break;
        case 1:
            logDet = log(std::real(A[0][0])) * log2Inverse;
            return true;
            break;
        case 2:
            determinant = std::real(A[0][0]*A[1][1] - A[0][1]* A[1][0]);
            logDet = log(determinant) * log2Inverse;
            return true;
            break;
        case 3:
            determinant = std::real( A[0][0]*A[1][1]*A[2][2] + A[0][1]*A[1][2]*A[2][0] + A[0][2]*A[1][0]*A[2][1]
                                    -A[0][2]*A[1][1]*A[2][0] - A[0][1]*A[1][0]*A[2][2] - A[0][0]*A[1][2]*A[2][1]);
            logDet = log(determinant) * log2Inverse;
            return true;
            break;
        default:
            MKLMatrix<std::complex<PRECISION> > temp(A);
            
            // Computes the Cholesky factorization of a symmetric (Hermitian) positive-definite matrix.
            MKLpotrf<PRECISION> potrf;
            lapack_int status = potrf.execute( LAPACK_ROW_MAJOR,
                                               'U',
                                                A.getRows(),
                                                temp.getLocation(),
                                                A.getRows());
            if (status == 0)
            {
                logDet = 0.0;
                for (unsigned int i = 0; i < A.getRows(); i++)
                {
                    // the cholesky decomposition yields temp^H temp = A, so we have to square the diagonal elements
                    // for computing the determinant we would have to multiply, but as we compute log det directly,
                    // we simply sum
                    logDet += 2.0 * log(std::real(temp[i][i])) * log2Inverse; 
                }
                return true;
            }
            else
                return false; // not a pos. def. matrix
       }
       return false; // should not get here
    }

    
    template <typename PRECISION>
    void performCholeskyFactorization(MKLMatrix<std::complex<PRECISION> >&  A, MKLMatrix<std::complex<PRECISION> >&  L)
    {
        assure(A.getColumns() == L.getColumns(), "A and L must have equal number of columns");
        assure(A.getRows() == L.getRows(), "A and L must have equal number of rows");
        assure(A.getRows() == A.getColumns(), "Matrices must be square");

        memcpy(L.getLocation(), A.getLocation(), sizeof(std::complex<PRECISION>) * A.getRows() * A.getColumns());
        
        imtaphy::detail::MKLpotrf<float> potrf;
        lapack_int status = potrf.execute( LAPACK_ROW_MAJOR,
                                          'L', // lower triangular: A = L*L^H
                                           L.getRows(),
                                           L.getLocation(),
                                           L.getRows());

        
        // the routine just overwrites the lower diagonal part with the result
        // so we will have to clean out the rest:
        for (unsigned int i = 0; i < L.getRows(); i++)
            for (unsigned int j = i + 1; j < L.getColumns(); j++)
                L[i][j] = 0;
    }

    
    template <typename PRECISION>
    void generateLowerTriangularBartlettDecompositionMatrix(MKLMatrix<std::complex<PRECISION> >&  A, unsigned int numberOfSamples)
    {
        // see TR 36.829 Section 8.2 and R1-111031
        // MS Bartlett. On the theory of statistical regression. Proceedings of the Royal Society of Edinburgh, 53:260-283, 1933
        //
        // W. B. Smith and R. R. Hocking. Algorithm AS 53: Wishart variate generator. 
        // Journal of the Royal Statistical Society. Series C (Applied Statistics), 21(3):341-345, 1972
        // 
        // The decomposition factors a covariance matrix R_est ~ Wishart(R_perf, M) following a Wishart distribution 
        // with M (number of samples) degrees of freedom and covariance R_perf into:
        // R_est = QAA^HQ^H with Q being a Cholesky factor of R_perf
        // A has to be square with the dimensions N_rx times N_rx (receive antennas)
        // 
        assure(A.getRows() == A.getColumns(), "Needs to be square");
        assure(A.getRows() > 0, "Cannot work on empty matrix");
        assure(numberOfSamples >= A.getRows(), "We need at least as many samples as the matrix has dimensions");
        
        // As a first step, we fill the lower triangular matrix with circular symmetric complex standard normal random numbers
        // and the upper triangular with zeros
        
        // why was this static again? does it need to be static further down?
        static boost::normal_distribution<PRECISION> normal(0.0, 1.0 / sqrt(2.0));
        static boost::variate_generator<boost::mt19937&, 
                                        boost::normal_distribution<PRECISION> > normalDistribution(*wns::simulator::getRNG(), normal);
        
        for (unsigned int i = 0; i < A.getRows(); i++)
        {
            // random numbers on the lower left part
            for (unsigned int j = 0; j < i; j++)      
            {
                A[i][j] = std::complex<PRECISION>(normalDistribution(), normalDistribution());
            }
            // zeros on the upper right
            for (unsigned int j = i+1; j < A.getColumns(); j++)
            {
                A[i][j] = std::complex<PRECISION>(0, 0);
            }
        }
        
        // now, we need to fill the diagonal
        // the i-th diagonal element should follow a \chi^2(2*(numberOfSamples - i + 1)) distribution (see Bartlett/Smith)
        // note that this assumes counting from 1, but we count from 0
        // 
        // Wikipedia (http://en.wikipedia.org/wiki/Chi-squared_distribution#Relation_to_other_distributions):
        // "The chi-squared distribution X ~ \chi^2(k) is a special case of the gamma distribution, in that X ~ \Gamma(k/2, 2) 
        // (using the shape parameterization of the gamma distribution) where k is an integer."
        // But note that boost does not support the scale paramter, http://www.johndcook.com/cpp_TR1_random.html#chisquared
        // " A chi squared distribution with v degrees of freedom is the same as a gamma distribution with shape parameter v/2 a
        // and scale parameter 2. Since the C++ TR1 implementation only directly supports gamma distributions with scale parameter 1, 
        // generate a sample from a gamma distribution with shape v/2 and unit scale then multiply the sample by 2.
        
        for (unsigned int i = 0; i < A.getColumns(); i++)
        {
            // we should generate \chi^2(2*(M - i + 1)) but with counting from 0 we have \chi^2(2*(M-i))
            // or \Gamma(2/2 * (M-i), 2) or 2*\Gamma(M-i,1)
            boost::gamma_distribution<PRECISION> gammaDist(numberOfSamples - i);
            boost::variate_generator<boost::mt19937&, // the reference is important to use the same generator without resetting
                                     boost::gamma_distribution<PRECISION> > Gamma(*wns::simulator::getRNG(), gammaDist);
            // here we would need to multiply Gamma() with 2 (for the scale parameter) 
            //and then divide by 2 for the sqrt(c/2) diagonal entry, so it's just the sqrt
            A[i][i] = sqrt(Gamma());
        }
    }
    } // detail
} // imtaphy


//#endif // MKL
#endif 
