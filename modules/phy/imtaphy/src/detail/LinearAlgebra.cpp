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

#include <IMTAPHY/detail/LinearAlgebra.hpp>

namespace imtaphy { namespace detail {

    // ?gemm matrix-matrix multiplication routines
    template <> void MKLgemm<float>::execute(const CBLAS_ORDER Order, const CBLAS_TRANSPOSE TransA,
                                            const CBLAS_TRANSPOSE TransB, const int M, const int N,
                                            const int K, const void *alpha, const void *A, const int lda,
                                            const void *B, const int ldb, const void *beta, void *C, const int ldc)
    {   // cblas_cgemm works on MKL_COMPLEX8, i.e., std::complex<float> values
    cblas_cgemm(Order, TransA, TransB, M, N, K, alpha, A, lda, B, ldb, beta, C, ldc);
    }
    // ?gemm matrix-matrix multiplication routines
    template <> void MKLgemm<double>::execute(const CBLAS_ORDER Order, const CBLAS_TRANSPOSE TransA,
                                            const  CBLAS_TRANSPOSE TransB, const int M, const int N,
                                            const int K, const void *alpha, const void *A, const int lda,
                                            const void *B, const int ldb, const void *beta, void *C, const int ldc)
    {   // cblas_zgemm works on MKL_COMPLEX16, i.e., std::complex<double> values
    cblas_zgemm(Order, TransA, TransB, M, N, K, alpha, A, lda, B, ldb, beta, C, ldc);
    }
    template class MKLgemm<float>;
    template class MKLgemm<double>;

}}

    
    bool
    imtaphy::detail::findNullVectors(imtaphy::detail::MKLMatrix<std::complex<float> >& A, imtaphy::detail::MKLMatrix<std::complex<float> >& nullA)
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
         int sizeNull = n - m; // size of null space
         
         // TODO: implement this in a clean way
         if ( m >= n )
         {
             std::cout << "Error: Cannot find the null space of a square or tall matrix\n";
             return false; // null vectors do not exists
             // Assumption: all the rows are linearly independent. (if rows < cols and vice versa)
             // If the special case of linearly dependent rows needs to be considered, then one needs to find the rank
             // first before calling the findNullVectors function because one needs to allocate space depending on rank.
         }
         
         // First we need to take the hermitian of matrix A, because the cgesvd() function reads the matrix in column-major mode.
         // another reason is that the SVD function returns V^H and not V. So taking hermitian of A before calling the function
         // will return V instead of V^H.
         MKLMatrix<std::complex<float> > tempA(A.getColumns(), A.getRows());
         
         matrixHermitian<float>(A, tempA);
         
         // allocate the U, S, V matrices
         MKLMatrix<std::complex<float> > U(m, m);
         
         float* S = static_cast<float*>(mkl_malloc(sizeof(float) * minDim, 32));
         
         MKLMatrix<std::complex<float> > V(n, n);
         
         // According to MKL docs:
         // lwork >= 2*min(m, n) + max(m, n) (for complex flavors).
         // For good performance, lwork must generally be larger.
         MKL_INT lwork = (2*m + n) * 10; // big enough!?
         std::complex<float>* work = static_cast<std::complex<float>*>(mkl_malloc(sizeof(std::complex<float>) * lwork, 32 ));
         
         // Workspace array, DIMENSION at least max(1, 5*min(m, n)). Used in complex flavors only.
         float* rwork = static_cast<float*>(mkl_malloc(sizeof(float) * 10 * 5 * minDim, 32)); // how dows cgesvd know about this size?
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
                 S,          // The singular values of A, sorted so that S(i) >= S(i+1).
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
         
         MKL_free(S);
         if (info > 0)
         {
             std::cout << "The algorithm for computing SVD failed to converge\n";
             return false;
         }
         
         
         for (int r = 0; r < n; r++)
             for (int c = 0; c < sizeNull; c++)
                 nullA[r][c] = V.getLocation()[r*n + m + c];
             
             return true;
         
         
         // EXTRA NOTES FOR POSSIBLE OPTIMIZATIONS AT A LATER STAGE
             /*The cost of the SVD approach is several times higher than computing the
              *        null space by reduction, but it should be acceptable whenever reliability is important. It is also
              *        possible to compute the null space by the QR decomposition, with the numerical stability and the cost both
              *        being between those of the SVD and the reduction approaches. The computation of a null space basis using the QR
              *        decomposition is explained in more detail below.
              *
              *        Let A be a mxn matrix with m < n. Using the QR factorization of , we can find a matrix such that
              *        A^T.P = Q.R = [Q_1 Q_2].R
              *        where P is a permutation matrix, Q is nxn and R is nxm. Matrix  is nxm and consists of the first m columns of Q.
              *        Matrix Q_2 is nx(n-m) and is made up of Q 's last n-m columns. Since A*Q_2 = 0, the columns of Q_2 span the null space of A.
              *
              *        Source: http://en.wikipedia.org/wiki/Kernel_(matrix)
              *
              */
    }
    float
    imtaphy::detail::findSubspaceAngle(imtaphy::detail::MKLMatrix<std::complex<float> >& A, imtaphy::detail::MKLMatrix<std::complex<float> >& B)
    {
        unsigned int m1 = A.getRows();
        unsigned int n1 = A.getColumns();
        unsigned int m2 = B.getRows();
        unsigned int n2 = B.getColumns();
        float angle = 0.0;
        
        assure ( m1 == m2, "findSubspaceAngle: Matrices must have equal rows");
        assure ( n1 == n2, "findSubspaceAngle: Matrices must have equal columns");
        unsigned int maxDim = std::max(m1,n1);
        
        // we might implement a rank function and find the rank of the matrices before doing the next step.
        // Assumption: miminum dimension == rank
        // if this assumption is not true, the null space will be included in the signal space and the 
        // computation of angle will be wrong
        unsigned int rank = std::min(m1,n1);
        
        
        // prepare the matrices
        ComplexFloatMatrix A_basis(maxDim, rank);
        ComplexFloatMatrix B_basis(maxDim, rank);
        FloatMatrix S(rank, 1);
        ComplexFloatMatrix V(n1, n1);
        if (m1 < n1) 
        {
            matrixSVD<float>(A, S, V);
            for (unsigned int r=0; r<A_basis.getRows(); r++)
                for (unsigned int c=0; c<A_basis.getColumns(); c++)
                {
                    A_basis[r][c] = V[r][c];
                }
            matrixSVD<float>(B, S, V);
            for (unsigned int r=0; r<B_basis.getRows(); r++)
                for (unsigned int c=0; c<B_basis.getColumns(); c++)
                {
                    B_basis[r][c] = V[r][c];
                }
        }
        else if (m1 > n1) 
        {
            ComplexFloatMatrix U(m1, m1);
            matrixSVD<float>(A, U, S, V);
            for (unsigned int r=0; r<A_basis.getRows(); r++)
                for (unsigned int c=0; c<A_basis.getColumns(); c++)
                {
                    A_basis[r][c] = U[r][c];
                }
            matrixSVD<float>(B, U, S, V);
            for (unsigned int r=0; r<B_basis.getRows(); r++)
                for (unsigned int c=0; c<B_basis.getColumns(); c++)
                {
                    B_basis[r][c] = U[r][c];
                }
        }
        else if (m1 == n1)
        {
            std::cout << "Warning: not possible to find angle between full spaces\n";
            angle = 0.0;
            return angle;
        
        }
        ComplexFloatMatrix A_basisHermitian(rank, maxDim);
        matrixHermitian<float>(A_basis, A_basisHermitian);
        ComplexFloatMatrix AhermitianB(rank, rank);
        matrixMultiplyCequalsAB<float>(AhermitianB, A_basisHermitian, B_basis);
        matrixSVD<float>(AhermitianB, S);
        angle = acos(((S.getLocation())[rank-1])); // rank-1 because of C++ indexing problem. 
        return angle;
        
    }
