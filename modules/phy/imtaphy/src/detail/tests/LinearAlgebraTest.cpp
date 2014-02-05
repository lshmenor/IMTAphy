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

#include <WNS/CppUnit.hpp>
#include <cppunit/extensions/HelperMacros.h>

// By default, the array access methods operator() and operator[] perform range checking. If a supplied index is out of the range defined for an array, an assertion will abort the program. To disable range checking (for performance reasons in production releases), define the BOOST_DISABLE_ASSERTS preprocessor macro prior to including multi_array.hpp in an application.

// For opt versions disable range checking in the multi_array wrapper
// WNS_NDEBUG means: assures are disabled
#ifdef  WNS_NDEBUG
#define BOOST_DISABLE_ASSERTS
#endif
#include <boost/multi_array.hpp>

#include <itpp/itbase.h>
#include <algorithm>
#include <iostream>



#include <IMTAPHY/detail/LinearAlgebra.hpp>


// #ifndef lapack_complex_float
// #define lapack_complex_float   MKL_Complex8
// #endif
// 
// #ifndef lapack_complex_double
// #define lapack_complex_double   MKL_Complex16
// #endif

//#define PRECISION double
#include <boost/graph/graph_concepts.hpp>

namespace imtaphy { namespace detail { namespace tests {
                class LinearAlgebraTest :
            public CppUnit::TestFixture
                {
                
                CPPUNIT_TEST_SUITE( LinearAlgebraTest );
                    CPPUNIT_TEST( testPseudoInverse44 );
                    CPPUNIT_TEST( testPseudoInverse24 );
                    CPPUNIT_TEST( testPseudoInverse42 );
                    CPPUNIT_TEST( testPseudoInverse24float );
                    CPPUNIT_TEST( testPseudoInverse42float );
                    CPPUNIT_TEST( testMatrixMultiplication );
                    CPPUNIT_TEST( testMatrixMultiplicationfloat );
                    CPPUNIT_TEST( testAAHermitian );
                    CPPUNIT_TEST( testAHermitianA );
                    CPPUNIT_TEST( testInverses );
                    CPPUNIT_TEST( testMatrixMultiplyCequalsNormOfAB );
                    CPPUNIT_TEST( testNullVectors );
                    CPPUNIT_TEST( testMKLMatrix );
                    CPPUNIT_TEST( testMatrixMultiplyCequalsAlphaSquareTimesAAhermitianPlusC );
                    CPPUNIT_TEST( testNewMultiplication );
                    CPPUNIT_TEST( testSVD );
                    CPPUNIT_TEST( testSubspaceAngle );
                    CPPUNIT_TEST( testChannelError );
//                    CPPUNIT_TEST( testLogDet );
//                    CPPUNIT_TEST( testBartlettDecomposition );
                CPPUNIT_TEST_SUITE_END();

                public:

                    void setUp();
                    void tearDown();
                    void testMatrixMultiplication();
                    void testMatrixMultiplicationfloat();
                    void testPseudoInverse44();
                    void testPseudoInverse24();
                    void testPseudoInverse42();
                    void testPseudoInverse24float();
                    void testPseudoInverse42float();
                    void testAAHermitian();
                    void testAHermitianA();
                    void testInverses();
                    void testMatrixMultiplyCequalsNormOfAB();
                    void testNullVectors();
                    void testMKLMatrix();
                    void testNewMultiplication();
                    void testMatrixMultiplyCequalsAlphaSquareTimesAAhermitianPlusC();
                    void testSVD();
                    void testSubspaceAngle();
                    void testChannelError();
                    void testLogDet();
                    void testBartlettDecomposition();
                private:
                    
                };
    
                 // N7imtaphy6detail5tests17LinearAlgebraTestE::testPseudoInverse
                 CPPUNIT_TEST_SUITE_REGISTRATION( LinearAlgebraTest );


                void
                LinearAlgebraTest::setUp()
                {       
    
                }

                void 
                LinearAlgebraTest::tearDown()
                {
    
                }

                void
                LinearAlgebraTest::testPseudoInverse44()
                {
                    ComplexDoubleMatrix A44double(4, 4);
                    ComplexDoubleMatrix I44double(4, 4);
                    ComplexDoubleMatrix Ainv44double(4, 4);
                                        
                    I44double[0][0] = 1; I44double[0][1] = 0; I44double[0][2] = 0; I44double[0][3] = 0;
                    I44double[1][0] = 0; I44double[1][1] = 1; I44double[1][2] = 0; I44double[1][3] = 0;
                    I44double[2][0] = 0; I44double[2][1] = 0; I44double[2][2] = 1; I44double[2][3] = 0;
                    I44double[3][0] = 0; I44double[3][1] = 0; I44double[3][2] = 0; I44double[3][3] = 1;

                    A44double[0][0] = 16; A44double[0][1] = 2; A44double[0][2] = 3; A44double[0][3] = 13;
                    A44double[1][0] = 5; A44double[1][1] = 11; A44double[1][2] = 10; A44double[1][3] = 8;
                    A44double[2][0] = 9; A44double[2][1] = 7; A44double[2][2] = 6; A44double[2][3] = 12;
                    A44double[3][0] = 4; A44double[3][1] = 14; A44double[3][2] = 15; A44double[3][3] = 1;

                    // add some imaginary part to it
                    for (int i = 0; i < 4*4; i++)
                        A44double.getLocation()[i] = std::complex<double>(A44double.getLocation()[i].real(), A44double.getLocation()[i].real() / 2.0);

                    imtaphy::detail::pseudoInverse<double>(A44double, Ainv44double);
                    
                    itpp::cmat itpp44 = "0.0809-0.0404i  -0.0591+0.0296i  -0.0491+0.0246i   0.0509-0.0254i;\
                                        -0.0291+0.0146i   0.0309-0.0154i   0.0209-0.0104i   0.0009-0.0004i;\
                                        0.0109-0.0054i  -0.0091+0.0046i  -0.0191+0.0096i   0.0409-0.0204i;\
                                       -0.0391+0.0196i   0.0609-0.0304i   0.0709-0.0354i  -0.0691+0.0346i";
                    
                    for (int i = 0; i < 4; i++)
                        for (int j = 0; j < 4; j++)
                        {
                            CPPUNIT_ASSERT_DOUBLES_EQUAL(itpp44(i,j).real(), Ainv44double[i][j].real(), 1e-04);
                            CPPUNIT_ASSERT_DOUBLES_EQUAL(itpp44(i,j).imag(), Ainv44double[i][j].imag(), 1e-04);

                        }
                }
                
              
                void
                LinearAlgebraTest::testPseudoInverse24()
                {
                    int m = 2;
                    int n = 4;

                    ComplexDoubleMatrix A24double(m, n);
                    ComplexDoubleMatrix I24double(m, n);
                    ComplexDoubleMatrix Ainv42double(n, m);
                    
                    // for matlab use the following commands
                    // >> magic(4)
                    // A24 = ans(1:2,:) + i/2*ans(1:2,:)
                    A24double[0][0] = 16; A24double[0][1] = 2; A24double[0][2] = 3; A24double[0][3] = 13;
                    A24double[1][0] = 5; A24double[1][1] = 11; A24double[1][2] = 10; A24double[1][3] = 8;
                    // add some imaginary part to it
                    for (int i = 0; i < m*n; i++)
                        A24double.getLocation()[i] = std::complex<double>(A24double.getLocation()[i].real(), A24double.getLocation()[i].real() / 2.0);

                    imtaphy::detail::pseudoInverse<double>(A24double, Ainv42double);

                    itpp::cmat Inv42("0.037760351630788-0.018880175815394i -0.015843364467309+0.007921682233655i;\
                                     -0.019739273762549+0.009869636881275i  0.043414414864392-0.021707207432196i;\
                                     -0.014285000749213+0.007142500374607i  0.036681484441337-0.018340742220668i;\
                                      0.021397532590780-0.010698766295390i  0.004355426801858-0.002177713400929i");
                                    
                    for (int i = 0; i < n; i++)
                        for (int j = 0; j < m; j++)
                        {
                            CPPUNIT_ASSERT_DOUBLES_EQUAL(Inv42(i,j).real(), Ainv42double[i][j].real(), 1e-04);
                            CPPUNIT_ASSERT_DOUBLES_EQUAL(Inv42(i,j).imag(), Ainv42double[i][j].imag(), 1e-04);

                        }
                }
                
                void
                LinearAlgebraTest::testPseudoInverse42()
                {
                    int m = 4;
                    int n = 2;

                    ComplexDoubleMatrix A42double(m, n);
                    ComplexDoubleMatrix I42double(m, n);
                    ComplexDoubleMatrix Ainv24double(n, m);
                    
                    // for matlab use the following commands
                    // >> magic(4)
                    // A42 = ans(:,1:2) + i/2*ans(:,1:2)
                    A42double[0][0] = 16; A42double[0][1] = 2;
                    A42double[1][0] = 5; A42double[1][1] = 11;
                    A42double[2][0] = 9; A42double[2][1] = 7;
                    A42double[3][0] = 4; A42double[3][1] = 14;
                    // add some imaginary part to it
                    for (int i = 0; i < m*n; i++)
                        A42double.getLocation()[i] = std::complex<double>(A42double.getLocation()[i].real(), A42double.getLocation()[i].real() / 2.0);
                    
                    imtaphy::detail::pseudoInverse<double>(A42double, Ainv24double);
                    
                    itpp::cmat Inv24= "0.045229101658729-0.022614550829364i -0.003415996058466+0.001707998029233i  0.015503366726885-0.007751683363442i -0.011528986697323+0.005764493348662i;\
                                    -0.020857283626211+0.010428641813106i  0.025685662670389-0.012842831335195i  0.006503530957464-0.003251765478732i  0.036689111512564-0.018344555756282i";
                    for (int i = 0; i < n; i++)
                        for (int j = 0; j < m; j++)
                        {
                            CPPUNIT_ASSERT_DOUBLES_EQUAL(Inv24(i,j).real(), Ainv24double[i][j].real(), 1e-04);
                            CPPUNIT_ASSERT_DOUBLES_EQUAL(Inv24(i,j).imag(), Ainv24double[i][j].imag(), 1e-04);

                        }
                }

                void
                LinearAlgebraTest::testPseudoInverse24float()
                {
                    int m = 2;
                    int n = 4;

                    ComplexFloatMatrix A24float(m, n);
                    ComplexFloatMatrix I24float(m, n);
                    ComplexFloatMatrix Ainv42float(n, m);
                    
                    // for matlab use the following commands
                    // >> A = [1 2 3 4; 5 6 7 8]
                    A24float[0][0] = 1; A24float[0][1] = 2; A24float[0][2] = 3; A24float[0][3] = 4;
                    A24float[1][0] = 5; A24float[1][1] = 6; A24float[1][2] = 7; A24float[1][3] = 8;

                    imtaphy::detail::pseudoInverse<float>(A24float, Ainv42float);

                    // >> pinv(A)    
                    itpp::cmat Inv42("-0.5500    0.2500;\
                                      -0.2250    0.1250;\
                                       0.1000    0.0000;\
                                       0.4250   -0.1250");
                    
                                    
                    for (int i = 0; i < n; i++)
                        for (int j = 0; j < m; j++)
                        {
                            CPPUNIT_ASSERT_DOUBLES_EQUAL(Inv42(i,j).real(), Ainv42float[i][j].real(), 1e-04);
                            CPPUNIT_ASSERT_DOUBLES_EQUAL(Inv42(i,j).imag(), Ainv42float[i][j].imag(), 1e-04);
                        }
                }
                
                void
                LinearAlgebraTest::testPseudoInverse42float()
                {
                    int m = 4;
                    int n = 2;

                    ComplexFloatMatrix A42float(m, n);
                    ComplexFloatMatrix I42float(m, n);
                    ComplexFloatMatrix Ainv24float(n, m);

                    // for matlab use the following commands
                    // >> A = [1 2; 3 4; 5 6; 7 8]
                    A42float[0][0] = 1; A42float[0][1] = 2;
                    A42float[1][0] = 3; A42float[1][1] = 4;
                    A42float[2][0] = 5; A42float[2][1] = 6;
                    A42float[3][0] = 7; A42float[3][1] = 8;
                    
                    imtaphy::detail::pseudoInverse<float>(A42float, Ainv24float);

                    // >> pinv(A)    
                    itpp::cmat Inv24("-1.0000   -0.5000    0.0000    0.5000;\
                                      0.8500    0.4500    0.0500   -0.3500");
                    
                    for (int i = 0; i < n; i++)
                        for (int j = 0; j < m; j++)
                        {
                            CPPUNIT_ASSERT_DOUBLES_EQUAL(Inv24(i,j).real(), Ainv24float[i][j].real(), 1e-04);
                            CPPUNIT_ASSERT_DOUBLES_EQUAL(Inv24(i,j).imag(), Ainv24float[i][j].imag(), 1e-04);

                        }
                }


                void
                LinearAlgebraTest::testMatrixMultiplication()
                {
                    std::complex<double> one = std::complex<double>(1.0, 0.0);
                    std::complex<double> zero = std::complex<double>(0.0, 0.0);
                    
                    // create several matrices A (m-by-k) and B (k-by-n) for different m, n, k and
                    // check whether our routine gives the same results as the MKL gemm routine
                    
                    for (int m = 1; m < 7; m++)
                       for (int n = 1; n < 7; n++)
                            for (int k = 1; k < 7; k++)
                            {
                                ComplexDoubleMatrix A(m, k);
                                ComplexDoubleMatrix B(k, n);
                                ComplexDoubleMatrix C1(m, n);
                                ComplexDoubleMatrix C2(m, n);
                                
                                for (int i = 0; i < m * k; i++)
                                    A.getLocation()[i] = std::complex<double>(itpp::randn(), itpp::randn());
                                for (int i = 0; i < k * n; i++)
                                    B.getLocation()[i] = std::complex<double>(itpp::randn(), itpp::randn());
                                                     
                                // lda, ldb, and ldc are the sizes of the leading dimension. When storing in row major mode,
                                // and not transposed, this is the number of *columns* because that is the size of each row
                                // The comments in the MKL documentation talk about the *rows* instead but this is probably
                                // because the primary focus is Fortran which uses column-major mode (as Matlab).
                                
                                cblas_zgemm(CblasRowMajor, // const enum CBLAS_ORDER Order, matrix storage mode
                                            CblasNoTrans, // CBLAS_TRANSPOSE TransA: (no) transpose mode for matrix A
                                            CblasNoTrans,  
                                            m,    // M 
                                            n,    // N:
                                            k,    // K: 
                                            &one, // alpha = (1,0), i.e. no scaling 
                                            A.getLocation(),    //*A pointer to matrix A (MxK matrix)
                                            k,    // lda: (see above)
                                            B.getLocation(),    // *B pointer to matrix B (KxN matrix)
                                            n,    // ldb: (see above) 
                                            &zero,// *beta 0 because C should not be added and does not need to be zeroed then
                                            C1.getLocation(),   // *C pointer to matrix C = \alpha A * B + \beta C   (MxN matrix )
                                            n     // ldc: (see above)
                                        ); 
                                
                                matrixMultiplyCequalsAB<double>(C2, A, B);
                                
                                // check computations give identical results:
                                for (int i = 0; i < m ; i++)
                                    for (int j = 0; j < n; j++)
                                        
                                    {
                                        CPPUNIT_ASSERT_DOUBLES_EQUAL(C1[i][j].real(), C2[i][j].real(), 1e-06);
                                        CPPUNIT_ASSERT_DOUBLES_EQUAL(C1[i][j].imag(), C2[i][j].imag(), 1e-06);
                                    }
                            }
                }
                    

                void
                LinearAlgebraTest::testMatrixMultiplicationfloat()
                {
                    std::complex<float> one = std::complex<float>(1.0, 0.0);
                    std::complex<float> zero = std::complex<float>(0.0, 0.0);
                    
                    // create several matrices A (m-by-k) and B (k-by-n) for different m, n, k and
                    // check whether our routine gives the same results as the MKL gemm routine
                    
                    for (int m = 1; m < 7; m++)
                       for (int n = 1; n < 7; n++)
                            for (int k = 1; k < 7; k++)
                            {
                                ComplexFloatMatrix A(m, k);
                                ComplexFloatMatrix B(k, n);
                                ComplexFloatMatrix C1(m, n);
                                ComplexFloatMatrix C2(m, n);
                                
                                
                                for (int i = 0; i < m * k; i++)
                                    A.getLocation()[i] = std::complex<float>(itpp::randn(), itpp::randn());
                                for (int i = 0; i < k * n; i++)
                                    B.getLocation()[i] = std::complex<float>(itpp::randn(), itpp::randn());
                                
                                // std::cout << "Now doing " << m << "x" << k << " * " << k << "x" << n <<"\n";
                     
                                // lda, ldb, and ldc are the sizes of the leading dimension. When storing in row major mode,
                                // and not transposed, this is the number of *columns* because that is the size of each row
                                // The comments in the MKL documentation talk about the *rows* instead but this is probably
                                // because the primary focus is Fortran which uses column-major mode (as Matlab).
                                
                                cblas_cgemm(CblasRowMajor, // const enum CBLAS_ORDER Order, matrix storage mode
                                            CblasNoTrans, // CBLAS_TRANSPOSE TransA: (no) transpose mode for matrix A
                                            CblasNoTrans,  
                                            m,    // M 
                                            n,    // N: 
                                            k,    // K: 
                                            &one, // alpha = (1,0), i.e. no scaling 
                                            A.getLocation(),    //*A pointer to matrix A (MxK matrix)
                                            k,    // lda: (see above)
                                            B.getLocation(),    // *B pointer to matrix B (KxN matrix)
                                            n,    // ldb: (see above) 
                                            &zero,// *beta 0 because C should not be added and does not need to be zeroed then
                                            C1.getLocation(),   // *C pointer to matrix C = \alpha A * B + \beta C   (MxN matrix )
                                            n     // ldc: (see above)
                                        ); 
                                
                                matrixMultiplyCequalsAB<float>(C2, A, B);

                                // check computations give identical results:
                                for (int i = 0; i < m ; i++)
                                    for (int j = 0; j < n; j++)
                                        
                                    {
                                        CPPUNIT_ASSERT_DOUBLES_EQUAL(C1[i][j].real(), C2[i][j].real(), 1e-05);
                                        CPPUNIT_ASSERT_DOUBLES_EQUAL(C1[i][j].imag(), C2[i][j].imag(), 1e-05);
                                    }
                            }
                }
                
                void
                LinearAlgebraTest::testMatrixMultiplyCequalsNormOfAB()
                {
                    // create several matrices A (m-by-k) and B (k-by-n) for different m, n, k and
                    // check whether our routine gives the same results as the MKL gemm routine
                    
                    for (int m = 1; m < 7; m++)
                       for (int n = 1; n < 7; n++)
                            for (int k = 1; k < 7; k++)
                            {
                                ComplexFloatMatrix A(m, k);
                                ComplexFloatMatrix B(k, n);
                                ComplexFloatMatrix C1(m, n);
                                FloatMatrix C2(m, n);
                                
                                for (int i = 0; i < m; i++)
                                    for (int j = 0; j < k; j++)
                                        A[i][j] = std::complex<float>(itpp::randn(), itpp::randn());
                                for (int i = 0; i < k; i++)
                                    for (int j = 0; j < n; j++)
                                        B[i][j] = std::complex<float>(itpp::randn(), itpp::randn());
                                
                                // std::cout << "Now doing " << m << "x" << k << " * " << k << "x" << n <<"\n";
                     
                                matrixMultiplyCequalsAB<float>(C1, A, B);
                                matrixMultiplyCequalsNormOfAB<float>(C2, A, B);
                                
                                // check computations give identical results:
                                for (int i = 0; i < m; i++)
                                    for (int j = 0; j < n; j++)
                                    {
                                        CPPUNIT_ASSERT_DOUBLES_EQUAL(norm(C1[i][j]), C2[i][j], 1e-04);
                                    }
                            }
                }
                
                void
                LinearAlgebraTest::testAAHermitian()
                {
                   for (int m = 1; m < 7; m++)
                       for (int n = 1; n < 7; n++)
                       {
                            ComplexFloatMatrix A(m, n);
                            ComplexFloatMatrix Ahermitian(n, m);
                            ComplexFloatMatrix C1(m, m);
                            ComplexFloatMatrix C2(m, m);
                            
                            for (int i = 0; i < m * n; i++)
                                A.getLocation()[i] = std::complex<float>(itpp::randn(), itpp::randn());

                            // compute using special function
                            matrixMultiplyCequalsAAhermitian(C1, A);

                            // compute by first taking hermitian of A and then calling ordinary matrix multiplication
                            matrixHermitian<float>(A, Ahermitian);

                            matrixMultiplyCequalsAB<float>(C2, A, Ahermitian);

                            // check computations give identical results:
                            for (int i = 0; i < m ; i++)
                                for (int j = 0; j < m; j++)
                                    
                                {
                                    CPPUNIT_ASSERT_DOUBLES_EQUAL(C1[i][j].real(), C2[i][j].real(), 1e-05);
                                    CPPUNIT_ASSERT_DOUBLES_EQUAL(C1[i][j].imag(), C2[i][j].imag(), 1e-05);
                                }
                                
                        }
                }

                void
                LinearAlgebraTest::testAHermitianA()
                {
                   for (int m = 1; m < 7; m++)
                       for (int n = 1; n < 7; n++)
                       {
                            ComplexFloatMatrix A(m, n);
                            ComplexFloatMatrix Ahermitian(n, m);
                            ComplexFloatMatrix C1(n, n);
                            ComplexFloatMatrix C2(n, n);
                           
                                                      
                            for (int i = 0; i < m * n; i++)
                                A.getLocation()[i] = std::complex<float>(itpp::randn(), itpp::randn());

                            // compute using special function
                            matrixMultiplyCequalsAhermitianA<float>(C1, A);

                            // compute by first taking hermitian of A and then calling ordinary matrix multiplication
                            matrixHermitian<float>(A, Ahermitian);

                            matrixMultiplyCequalsAB<float>(C2, Ahermitian, A);

                            // check computations give identical results:
                            for (int i = 0; i < n ; i++)
                                for (int j = 0; j < n; j++)
                                    
                                {
                                    CPPUNIT_ASSERT_DOUBLES_EQUAL(C1[i][j].real(), C2[i][j].real(), 1e-05);
                                    CPPUNIT_ASSERT_DOUBLES_EQUAL(C1[i][j].imag(), C2[i][j].imag(), 1e-05);
                                }
                        }
                }


                void
                LinearAlgebraTest::testInverses()
                {
                   for (int m = 1; m < 7; m++)
                   {
                        ComplexDoubleMatrix A(m, m);
                        ComplexDoubleMatrix AAhermitian(m, m);
                        ComplexDoubleMatrix inverse(m, m);
                        ComplexDoubleMatrix C(m, m);
                       
                        for (int i = 0; i < m * m; i++)
                            A.getLocation()[i] = std::complex<double>(itpp::randn(), itpp::randn());

                        
                        // first test general matrices
                        matrixInverse<double>(A, inverse);
                        
                        
                        matrixMultiplyCequalsAB<double>(C, A, inverse);

                        
                        for (int i = 0; i < m; i++)
                            for (int j = 0; j < m; j++)
                            {
                                if (i == j)
                                {
                                    CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, C[i][j].real(), 1e-04);
                                    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, C[i][j].imag(), 1e-04);
                                }
                                else
                                {
                                    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, C[i][j].real(), 1e-04);
                                    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, C[i][j].imag(), 1e-04);
                                }
                            }
                          
                        // now do it for a positive definite matrices A*A^H
                        matrixMultiplyCequalsAAhermitian<double>(AAhermitian, A);
                        
                        matrixInverse<double>(AAhermitian, inverse);
                        
                        matrixMultiplyCequalsAB<double>(C, AAhermitian, inverse);

                        for (int i = 0; i < m; i++)
                            for (int j = 0; j < m; j++)
                            {
                                if (i == j)
                                {
                                    CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, C[i][j].real(), 1e-04);
                                    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, C[i][j].imag(), 1e-04);
                                }
                                else
                                {
                                    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, C[i][j].real(), 1e-04);
                                    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, C[i][j].imag(), 1e-04);

                                }
                            }

                        ComplexFloatMatrix Afloat(m, m);
                        ComplexFloatMatrix AAhermitianFloat(m, m);
                        ComplexFloatMatrix inverseFloat(m, m);
                        ComplexFloatMatrix Cfloat(m, m);
                        
                        for (int i = 0; i < m * m; i++)
                            Afloat.getLocation()[i] = std::complex<double>(itpp::randn(), itpp::randn());
                        
                        matrixMultiplyCequalsAAhermitian<float>(AAhermitianFloat, Afloat);
                        
                        matrixInverse<float>(AAhermitianFloat, inverseFloat);
                        matrixMultiplyCequalsAB<float>(Cfloat, AAhermitianFloat, inverseFloat);
                        
                        for (int i = 0; i < m; i++)
                            for (int j = 0; j < m; j++)
                            {
                                if (i == j)
                                {
                                    CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, Cfloat[i][j].real(), 1e-04);
                                    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, Cfloat[i][j].imag(), 1e-04);
                                }
                                else
                                {
                                    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, Cfloat[i][j].real(), 1e-04);
                                    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, Cfloat[i][j].imag(), 1e-04);
                                }
                            }
                   }
                }
                
                void
                LinearAlgebraTest::testNullVectors()
                {
                    int m = 2;
                    int n = 4;

                    ComplexFloatMatrix A(m, n);
                                        
                    A[0][0] = 1; A[0][1] = 2; A[0][2] = std::complex<float>(3., 1.);A[0][3] = std::complex<float>(3., 6.);
                    A[1][0] = 4; A[1][1] = std::complex<float>(5., 2.); A[1][2] = 6; A[1][3] = std::complex<float>(2., 1.);
                    
                    if (n > m)
                    {
                        ComplexFloatMatrix nullA(n, n-m);
                        
                        bool success = findNullVectors(A, nullA);

                        CPPUNIT_ASSERT(success);
                        
                        // Now test the result

                        ComplexFloatMatrix C(m, n-m);
                       
                        imtaphy::detail::matrixMultiplyCequalsAB<float>(C, A, nullA);

                        for (int i = 0; i < m; i++)
                            for (int j = 0; j < n-m; j++)
                        {
                            CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, C[i][j].real(), 1e-4);
                            CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, C[i][j].imag(), 1e-4);
                        }
                    }
                }

                void LinearAlgebraTest::testMKLMatrix()
                {   
                    ComplexDoubleMatrix A(2, 2);
                    ComplexDoubleMatrix B(4, 4);

/*                    std::cout << "A is " << A.getColumns() << "-by-" << A.getRows() << " at location " << A.getLocation() << "\n";
                    std::cout << "B is " << B.getColumns() << "-by-" << B.getRows() << " at location " << B.getLocation() << "\n";*/
                    
                    A[0][0] = 1;
                    A[0][1] = 2;
                    A[1][0] = 3;
                    A[1][1] = 4;

//                     std::cout << "\n";
//                     displayMatrix(A);
//                     std::cout << "before: \n";
//                     displayMatrix(B);

                    copyMatrixToSubmatrix<std::complex<double> >(B,
                                                                 A,
                                                                 1,
                                                                 1,
                                                                 2,
                                                                 2);
/*                    std::cout << "after: \n";
                    displayMatrix(B);
                    std::cout << "A is " << A.getColumns() << "-by-" << A.getRows() << " at location " << A.getLocation() << "\n";
                    std::cout << "B is " << B.getColumns() << "-by-" << B.getRows() << " at location " << B.getLocation() << "\n";*/
                    
                    
                };

                void LinearAlgebraTest::testNewMultiplication()
                {
                    
                    for (unsigned int p = 1; p < 10; p++)
                        for (unsigned int k = 1; k < 10; k++)
                            for (unsigned int m = 1; m < 10; m++)
                            {
                                ComplexFloatMatrix C1(k, m);
                                ComplexFloatMatrix C2(k, m);
                                
                                for (unsigned int n = 0; n < 10; n++)
                                {
                                    
                                    ComplexFloatMatrix A(k, p);
                                    
                                    for (unsigned int i = 0; i < k; i++)
                                        for (unsigned int j = 0; j < p; j++)
                                            A[i][j] = std::complex<double>(itpp::randn(), itpp::randn());
                                        
                                        ComplexFloatMatrix B(p, m);
                                    
                                    for (unsigned int i = 0; i < p; i++)
                                        for (unsigned int j = 0; j < m; j++)
                                            B[i][j] = std::complex<double>(itpp::randn(), itpp::randn());
                                        
                                        
                                        
                                    matrixMultiplyCequalsAB(C1, A, B);

                                    // that code unfortunately gone...
                                    /*matrixMultiplyCequalsAB(C2->getLocation(), A->getLocation(), B->getLocation(), k, m, p);
                                    
                                    for (int i = 0; i < k; i++)
                                        for (int j = 0; j < m; j++)
                                        {
                                            CPPUNIT_ASSERT_DOUBLES_EQUAL((*C1)[i][j].imag(), (*C2)[i][j].imag(), 1e-05);
                                            CPPUNIT_ASSERT_DOUBLES_EQUAL((*C1)[i][j].real(), (*C2)[i][j].real(), 1e-05);
                                        }
                                    */    
                                        
                                        
                                }
                            }
                };

                void LinearAlgebraTest::testMatrixMultiplyCequalsAlphaSquareTimesAAhermitianPlusC()
                {
                    for (int m = 1; m < 7; m++)
                    {
                        for (int n = 1; n < 7; n++)
                        {
                            ComplexFloatMatrix A(m, n);
                            ComplexFloatMatrix C1(m, m);
                            ComplexFloatMatrix C2(m, m);

                            
                            for (int i = 0; i < m; i++)
                                for (int j = 0; j < n; j++)
                                    A[i][j] = std::complex<float>(itpp::randn(), itpp::randn());

                            for (int i = 0; i < m; i++)
                                for (int j = 0; j < m; j++)
                                {
                                    C1[i][j] = std::complex<float>(itpp::randn(), itpp::randn());
                                    C2[i][j] = C1[i][j];
                                }

                            float alphaSquare = 4.2;
                                
                            imtaphy::detail::matrixMultiplyCequalsAlphaSquareTimesAAhermitianPlusC(C1, A, alphaSquare);

                            ComplexFloatMatrix Ahermitian(n,m);
                            ComplexFloatMatrix AAhermitian(m,m);

                            imtaphy::detail::matrixHermitian(A, Ahermitian);
                            imtaphy::detail::matrixMultiplyCequalsAB(AAhermitian, A, Ahermitian);
                            imtaphy::detail::scaleMatrixA(AAhermitian, alphaSquare);
                            imtaphy::detail::matrixAddAtoB(AAhermitian, C2);
                            
                            // check computations give identical results:
                            for (int i = 0; i < m ; i++)
                                for (int j = 0; j < m; j++)
                                    
                                {
                                    CPPUNIT_ASSERT_DOUBLES_EQUAL(C1[i][j].real(), C2[i][j].real(), 1e-04);
                                    CPPUNIT_ASSERT_DOUBLES_EQUAL(C1[i][j].imag(), C2[i][j].imag(), 1e-04);
                                }
                            
                        }
                    }
                };
                
                void LinearAlgebraTest::testSVD()
                
                {
                    int m = 2,  n = 2;
                    int minDim = std::min(m, n);
                    ComplexFloatMatrix A(m, n);
                    
                    A[0][0] = std::complex<float>(1., 2.);
                    A[0][1] = std::complex<float>(2.5, -8.);
                    A[1][0] = std::complex<float>(3., 1.5);
                    A[1][1] = std::complex<float>(4., -3.);
                    
                    
                    ComplexFloatMatrix U(m, m);
                    FloatMatrix S(minDim, 1);
                    ComplexFloatMatrix V(n, n);
                    
                    if (!(imtaphy::detail::matrixSVD<float>(A, S)))
                    {
                        assure(0, "matrixSVD function failed to find SVD");
                        return;
                    }
                    
                    
                    // The following reference values are taken from MATLAB by using [U,S,V] = svd(A)
                    // save2x2MatrixBoostFormat() was used to convert MATLAB matrix to Boost matrix format
                    ComplexFloatMatrix matlabU(m, m);
                    FloatMatrix matlabS(minDim, 1);
                    ComplexFloatMatrix matlabV(n, n);
                    matlabS[0][0] = 10.224373127654927;
                    matlabS[1][0] = 2.638597003425835;

                    
//                    std::cout << "Testing SVD (singular values only)\n";
                    for (int i = 0; i < minDim ; i++)
                        CPPUNIT_ASSERT_DOUBLES_EQUAL(matlabS[i][0], S[i][0], 1e-05);
                    
                   
                    if (!(imtaphy::detail::matrixSVD<float>(A, S, V)))
                    {
                        assure(0, "matrixSVD function failed to find SVD");
                        return;
                    }
                    

                    matlabV[0][0] = std::complex<float>( 0.308521847258076, -0.000000000000000);
                    matlabV[0][1] = std::complex<float>( 0.951217256868516, -0.000000000000000);
                    matlabV[1][0] = std::complex<float>( -0.199307685016742, 0.930102530078126);
                    matlabV[1][1] = std::complex<float>( 0.064644301509551, -0.301673407044569);

                    
                    // Testing SVD full
//                    std::cout << "Testing SVD (S and V only)\n";
                    for (int i = 0; i < m ; i++)
                        for (int j = 0; j < m; j++)
                            
                        {
                            CPPUNIT_ASSERT_DOUBLES_EQUAL(matlabV[i][j].real(), V[i][j].real(), 1e-05);
                            CPPUNIT_ASSERT_DOUBLES_EQUAL(matlabV[i][j].imag(), V[i][j].imag(), 1e-05);
                        }
                    for (int i = 0; i < minDim ; i++)
                        CPPUNIT_ASSERT_DOUBLES_EQUAL(matlabS[i][0], S[i][0], 1e-05);
                    
                   
                    
                    
                    
                    
                    if (!(imtaphy::detail::matrixSVD<float>(A, U, S, V)))
                    {
                        assure(0, "matrixSVD function failed to find SVD");
                        return;
                    }
                    
                    
                    matlabU[0][0] = std::complex<float>( 0.709194860634390, 0.443720259736447);
                    matlabU[0][1] = std::complex<float>( -0.492898022708876, 0.239178845132402);
                    matlabU[1][0] = std::complex<float>( 0.285459299607062, 0.467619470314308);
                    matlabU[1][1] = std::complex<float>( 0.836508475013162, 0.009929273989863);

                    
                    // Testing SVD full
  //                  std::cout << "Testing SVD full\n";
                    for (int i = 0; i < m ; i++)
                        for (int j = 0; j < m; j++)
                            
                        {
                            CPPUNIT_ASSERT_DOUBLES_EQUAL(matlabU[i][j].real(), U[i][j].real(), 1e-05);
                            CPPUNIT_ASSERT_DOUBLES_EQUAL(matlabU[i][j].imag(), U[i][j].imag(), 1e-05);
                        }
                    for (int i = 0; i < m ; i++)
                        for (int j = 0; j < m; j++)
                            
                        {
                            CPPUNIT_ASSERT_DOUBLES_EQUAL(matlabV[i][j].real(), V[i][j].real(), 1e-05);
                            CPPUNIT_ASSERT_DOUBLES_EQUAL(matlabV[i][j].imag(), V[i][j].imag(), 1e-05);
                        }
                    for (int i = 0; i < minDim ; i++)
                        CPPUNIT_ASSERT_DOUBLES_EQUAL(matlabS[i][0], S[i][0], 1e-05);
                    
                   
                    
                    return;
                    
                }
                void 
                LinearAlgebraTest::testSubspaceAngle()
                {
                    int m = 2,  n = 3;
                    ComplexFloatMatrix A(m, n);
                    ComplexFloatMatrix B(m, n);
                    float expectedAngle = 1.088839323235918; // from Matlab
                    
                    
                    A[0][0] = std::complex<float>( 2.000000000000000, -3.000000000000000);
                    A[0][1] = std::complex<float>( 2.000000000000000, -1.000000000000000);
                    A[0][2] = std::complex<float>( 4.000000000000000, 7.000000000000000);
                    A[1][0] = std::complex<float>( 2.000000000000000, 7.000000000000000);
                    A[1][1] = std::complex<float>( 3.000000000000000, -4.000000000000000);
                    A[1][2] = std::complex<float>( 5.000000000000000, 10.000000000000000);
                    
                    B[0][0] = std::complex<float>( 1.000000000000000, -2.000000000000000);
                    B[0][1] = std::complex<float>( 6.000000000000000, -1.000000000000000);
                    B[0][2] = std::complex<float>( 3.000000000000000, -7.000000000000000);
                    B[1][0] = std::complex<float>( 2.000000000000000, 5.000000000000000);
                    B[1][1] = std::complex<float>( 3.000000000000000, 4.000000000000000);
                    B[1][2] = std::complex<float>( 5.000000000000000, -6.000000000000000);
                    
                    float angle = findSubspaceAngle(A, B);
                    CPPUNIT_ASSERT_DOUBLES_EQUAL(expectedAngle, angle, 1e-05);
                    
                    // Now test that the angle between hermitians should also be the same
                    // Besides that, this test is also needed because we need to test that the 
                    // angles routine should work on flat and tall matrices both.
                    ComplexFloatMatrix Ahermitian(n, m);
                    ComplexFloatMatrix Bhermitian(n, m);
                    Ahermitian[0][0] = std::complex<float>( 2.000000000000000, 3.000000000000000);
                    Ahermitian[0][1] = std::complex<float>( 2.000000000000000, -7.000000000000000);
                    Ahermitian[1][0] = std::complex<float>( 2.000000000000000, 1.000000000000000);
                    Ahermitian[1][1] = std::complex<float>( 3.000000000000000, 4.000000000000000);
                    Ahermitian[2][0] = std::complex<float>( 4.000000000000000, -7.000000000000000);
                    Ahermitian[2][1] = std::complex<float>( 5.000000000000000, -10.000000000000000);

                    Bhermitian[0][0] = std::complex<float>( 1.000000000000000, 2.000000000000000);
                    Bhermitian[0][1] = std::complex<float>( 2.000000000000000, -5.000000000000000);
                    Bhermitian[1][0] = std::complex<float>( 6.000000000000000, 1.000000000000000);
                    Bhermitian[1][1] = std::complex<float>( 3.000000000000000, -4.000000000000000);
                    Bhermitian[2][0] = std::complex<float>( 3.000000000000000, 7.000000000000000);
                    Bhermitian[2][1] = std::complex<float>( 5.000000000000000, 6.000000000000000);
                    
                    angle = findSubspaceAngle(Ahermitian, Bhermitian);
                    CPPUNIT_ASSERT_DOUBLES_EQUAL(expectedAngle, angle, 1e-05);

                }
                
                void 
                LinearAlgebraTest::testChannelError()
                {
                    ComplexFloatMatrix A(2,2);
                    A[0][0] = std::complex<float>(0, 0);
                    A[0][1] = std::complex<float>(0, 0);
                    A[1][0] = std::complex<float>(0, 0);
                    A[1][1] = std::complex<float>(0, 0);
                    
                    float perElementPower = 1.234;
 
//                     itpp::Real_Timer timer;
//                     timer.tic();

                    unsigned int num = 100000;
                    for (unsigned int i = 0; i < num; i++)
                    {
                        applyWhiteGaussianNoiseToA(A, perElementPower);
                    }
                    
                    
//                      timer.toc();
//                      std::cout << "boost approach " << timer.get_time() << " seconds\n";
                    
                    scaleMatrixA(A, static_cast<float>(1.0)/static_cast<float>(num));
//                    std::cout << "Scaled matrix has total power= " << matrixNormSquared(A) << "\n";
//                    displayMatrix(A);

                    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, A[0][0].real(), 1e-02);
                    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, A[0][0].imag(), 1e-02);
                    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, A[0][1].real(), 1e-02);
                    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, A[0][1].imag(), 1e-02);
                    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, A[1][0].real(), 1e-02);
                    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, A[1][0].imag(), 1e-02);
                    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, A[1][1].real(), 1e-02);
                    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, A[1][1].imag(), 1e-02);
                    
                }
                
                void 
                LinearAlgebraTest::testLogDet()
                {
                    for (int n = 1; n < 7; n++)
                    {
                        ComplexFloatMatrix A(n, n);
                        ComplexFloatMatrix C(n, n);
                                                    
                        for (int i = 0; i < n * n; i++)
                            A.getLocation()[i] = std::complex<float>(itpp::randn(), itpp::randn());

                        // generate pos. def. hermitian matrix
                        matrixMultiplyCequalsAhermitianA<float>(C, A);

                        double logDet;
                        bool success = imtaphy::detail::log2DetOfPosDefHermitianMatrix(C, logDet);
                        
                        CPPUNIT_ASSERT(success);
                        
                        std::cout << "log2(det(C)) = " << logDet << " for matrix C=";
                        imtaphy::detail::displayMatrix(C);
                    }
                }
                
                void LinearAlgebraTest::testBartlettDecomposition()
                {
                    for (int n = 1; n < 7; n++)
                    {
                        ComplexFloatMatrix A(n, n);
                        ComplexFloatMatrix W(n, n);
                        
                        for (unsigned int m = n; m < 100; m++)
                        {
                            imtaphy::detail::generateLowerTriangularBartlettDecompositionMatrix(A, m);
                            imtaphy::detail::matrixMultiplyCequalsAAhermitian(W, A);
                            imtaphy::detail::scaleMatrixA(W, 1.0f / static_cast<float>(m));
                            
                            std::cout << "m=" << m << ":\n";
                            imtaphy::detail::displayMatrix(W);
                            std::cout << "\n";
                            
                        }
                    }
                    
                    for (unsigned int M = 5; M < 200; M += 20)
                    {
                        ComplexFloatMatrix random(4,4);
                        imtaphy::detail::fillWhiteGaussianNoise(random, 1.0f);

                        for (unsigned int round = 0; round < 3; round++)
                        {
                            ComplexFloatMatrix cov(4,4);
                            
                            imtaphy::detail::matrixMultiplyCequalsAAhermitian(cov, random);

                            ComplexFloatMatrix choleskyFactor(cov.getRows(), cov.getColumns());
                            imtaphy::detail::performCholeskyFactorization(cov, choleskyFactor);
                                                            
                            ComplexFloatMatrix A(4,4);
                            imtaphy::detail::generateLowerTriangularBartlettDecompositionMatrix(A, M);
                            
                            std::cout << "This is how the matrix A looks:\n";
                            displayMatrix(A);
                            
                            ComplexFloatMatrix product(4,4);
                            
                            matrixMultiplyCequalsAB(product, choleskyFactor, A);
                            
                            ComplexFloatMatrix estimate(4,4);
                            matrixMultiplyCequalsAAhermitian(estimate, product);
                            scaleMatrixA(estimate, 1.0f / static_cast<float>(M));
                            
                            std::cout << "\nPerfect covariance M=" << M << " round " << round << ":\n";
                            displayMatrix(cov);
                            std::cout << "Estimated covariance M=" << M << " round " << round << ":\n";
                            displayMatrix(estimate);
                            
                        }                        
                        
                    }                                
        }

}}}


