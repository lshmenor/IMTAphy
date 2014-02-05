/*******************************************************************************
 * This file is part of openWNS (open Wireless Network Simulator)
 * _____________________________________________________________________________
 *
 * Copyright (C) 2004-2007
 * Chair of Communication Networks (ComNets)
 * Kopernikusstr. 16, D-52074 Aachen, Germany
 * phone: ++49-241-80-27910,
 * fax: ++49-241-80-22242
 * email: info@openwns.org
 * www: http://www.openwns.org
 * _____________________________________________________________________________
 *
 * openWNS is free software; you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License version 2 as published by the
 * Free Software Foundation;
 *
 * openWNS is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/

#include <WNS/container/FastList.hpp>
#include <WNS/container/FastListEnabler.hpp>
#include <WNS/TestFixture.hpp>
#include <WNS/SmartPtr.hpp>
#include <WNS/Assure.hpp>

namespace wns { namespace container { namespace tests {
    /**
     * @brief Test for FastList<T>
     * @author Marc Schinnenburg <marc@schinnenburg.net>
     */
    class FastListTest :
        public wns::TestFixture
    {
        // begin example "wns.container.FastListEnabler.example"
        // Note how FLTest is enabled to
        // support FastLists by deriving from FastListEnabler
        class FLTest :
            public FastListEnabler<FLTest*>
        {
        public:
            explicit
            FLTest(int id) :
                FastListEnabler<FLTest*>(),
                Id(id)
            {
            }

            int
            getId() const
            {
                return Id;
            }

        private:
            int Id;
        };
        // end example

        class FLTest2 :
            virtual public RefCountable,
            public FastListEnabler< SmartPtr<FLTest2> >
        {
        public:
            explicit
            FLTest2(int id) :
                FastListEnabler< SmartPtr<FLTest2> >(),
                Id(id)
            {
            }

            int
            getId() const
            {
                return Id;
            }
        private:
            int Id;
        };

        class SFLTest :
            public SingleFastListEnabler<SFLTest*>
        {
        public:
            explicit
            SFLTest(int id) :
                SingleFastListEnabler<SFLTest*>(),
                Id(id)
            {
            }

            int
            getId() const
            {
                return Id;
            }
        private:
            int Id;
        };

        CPPUNIT_TEST_SUITE( FastListTest );
        CPPUNIT_TEST( insert );
        CPPUNIT_TEST( pushBack );
        CPPUNIT_TEST( size );
        CPPUNIT_TEST( erase );
        CPPUNIT_TEST( clear );
        CPPUNIT_TEST( remove );
        CPPUNIT_TEST( contains );
        CPPUNIT_TEST( iterators );
        CPPUNIT_TEST( copy );
        CPPUNIT_TEST( copySingleFastList );
        CPPUNIT_TEST( assignment );
        CPPUNIT_TEST( assignmentSingleFastList );
        // FastListEnabler is NonCopyable -> the following tests are therefore
        // disabled. If this should change, the tests need to enabled and the
        // test cases need to be added
        // CPPUNIT_TEST( copyElement );
        // CPPUNIT_TEST( copySingleFastListElement );
        // CPPUNIT_TEST( assignmentElement );
        // CPPUNIT_TEST( assignmentSingelFastListElement );
        CPPUNIT_TEST( front );
        CPPUNIT_TEST( removeFromAllLists );
        CPPUNIT_TEST( destructAndRemove );
        CPPUNIT_TEST_SUITE_END();

        typedef FastList<FLTest*>::iterator iterator;
        // begin example "wns.container.FastListDefinition.example"
        FastList<FLTest*> f;
        // end example
        FLTest* flt1;
        FLTest* flt2;

        typedef FastList<SmartPtr<FLTest2> >::iterator iterator2;
        FastList<SmartPtr<FLTest2> > bf;
        SmartPtr<FLTest2> bflt1;
        SmartPtr<FLTest2> bflt2;

        typedef FastList<SFLTest*>::iterator iterator3;
        SFLTest* sflt1;
        SFLTest* sflt2;
        FastList<SFLTest*> sf;

    public:

        void
        prepare()
        {
            f = FastList<FLTest*>();
            // begin example "wns.container.FastListPushBack.example"
            flt1 = new FLTest(1);
            f.push_front(flt1);
            // end example

            flt2 = new FLTest(2);
            f.push_front(flt2);

            bf = FastList<SmartPtr<FLTest2> >();
            bflt1 = SmartPtr<FLTest2>(new FLTest2(1));
            bf.push_front(bflt1);
            bflt2 = SmartPtr<FLTest2>(new FLTest2(2));
            bf.push_front(bflt2);

            sf = FastList<SFLTest*>();
            sflt1 = new SFLTest(1);
            sf.push_front(sflt1);
            sflt2 = new SFLTest(2);
            sf.push_front(sflt2);

        }

        void
        cleanup()
        {
            f.clear();
            bf.clear();
            sf.clear();
            delete flt1;
            delete flt2;
            bflt1 = SmartPtr<FLTest2>();
            bflt2 = SmartPtr<FLTest2>();
            delete sflt1;
            delete sflt2;
        }

        void
        insert()
        {
            iterator itr;
            itr = f.begin();
            CPPUNIT_ASSERT_EQUAL( 2, itr->getId() );
            CPPUNIT_ASSERT_EQUAL( 1, (++itr)->getId() );

            iterator2 bitr;
            bitr = bf.begin();
            CPPUNIT_ASSERT_EQUAL( 2, bitr->getId() );
            CPPUNIT_ASSERT_EQUAL( 1, (++bitr)->getId() );

            iterator3 sitr;
            sitr = sf.begin();
            CPPUNIT_ASSERT_EQUAL( 2, sitr->getId() );
            CPPUNIT_ASSERT_EQUAL( 1, (++sitr)->getId() );
        }

        void
        pushBack()
        {
            f.clear();
            f.push_back(flt1);
            f.push_back(flt2);

            iterator itr = f.begin();
            CPPUNIT_ASSERT_EQUAL( 1, itr->getId() );
            CPPUNIT_ASSERT_EQUAL( 2, (++itr)->getId() );

            sf.clear();
            sf.push_back(sflt1);
            sf.push_back(sflt2);

            iterator3 sitr = sf.begin();
            CPPUNIT_ASSERT_EQUAL( 1, sitr->getId() );
            CPPUNIT_ASSERT_EQUAL( 2, (++sitr)->getId() );
        }

        void
        size()
        {
            CPPUNIT_ASSERT_EQUAL( 2, f.size() );

            CPPUNIT_ASSERT_EQUAL( 2, bf.size() );

            CPPUNIT_ASSERT_EQUAL( 2, sf.size() );
        }

        void
        erase()
        {
            iterator itr = f.begin();
            f.erase(itr);
            itr = f.begin();
            CPPUNIT_ASSERT_EQUAL( 1, f.size() );
            CPPUNIT_ASSERT_EQUAL( 1, itr->getId() );
            CPPUNIT_ASSERT( ++itr==f.end() );

            iterator2 bitr = bf.begin();
            bf.erase(bitr);
            bitr = bf.begin();
            CPPUNIT_ASSERT_EQUAL( 1, bf.size() );
            CPPUNIT_ASSERT_EQUAL( 1, bitr->getId() );
            CPPUNIT_ASSERT( ++bitr==bf.end() );

            iterator3 sitr = sf.begin();
            sf.erase(sitr);
            sitr = sf.begin();
            CPPUNIT_ASSERT_EQUAL( 1, sf.size() );
            CPPUNIT_ASSERT_EQUAL( 1, sitr->getId() );
            CPPUNIT_ASSERT( ++sitr==sf.end() );

        }

        void
        clear()
        {
            f.clear();
            CPPUNIT_ASSERT ( f.begin()==f.end() );
            CPPUNIT_ASSERT ( f.empty() );
            CPPUNIT_ASSERT_EQUAL( 0, f.size() );

            bf.clear();
            CPPUNIT_ASSERT ( bf.begin()==bf.end() );
            CPPUNIT_ASSERT ( bf.empty() );
            CPPUNIT_ASSERT_EQUAL( 0, bf.size() );

            sf.clear();
            CPPUNIT_ASSERT ( sf.begin()==sf.end() );
            CPPUNIT_ASSERT ( sf.empty() );
            CPPUNIT_ASSERT_EQUAL( 0, sf.size() );
        }

        void
        remove()
        {
            f.remove(flt1);
            iterator itr = f.begin();
            CPPUNIT_ASSERT_EQUAL( 1, f.size() );
            CPPUNIT_ASSERT_EQUAL( 2, itr->getId() );
            CPPUNIT_ASSERT( ++itr==f.end() );
            f.remove(flt2);
            itr = f.begin();
            CPPUNIT_ASSERT_EQUAL( 0, f.size() );
            CPPUNIT_ASSERT( f.empty() );
            CPPUNIT_ASSERT( itr==f.end() );

            bf.remove(bflt1);
            iterator2 bitr = bf.begin();
            CPPUNIT_ASSERT_EQUAL( 1, bf.size() );
            CPPUNIT_ASSERT_EQUAL( 2, bitr->getId() );
            CPPUNIT_ASSERT( ++bitr==bf.end() );
            bf.remove(bflt2);
            bitr = bf.begin();
            CPPUNIT_ASSERT_EQUAL( 0, bf.size() );
            CPPUNIT_ASSERT( bf.empty() );
            CPPUNIT_ASSERT( bitr==bf.end() );

            sf.remove(sflt1);
            iterator3 sitr = sf.begin();
            CPPUNIT_ASSERT_EQUAL( 1, sf.size() );
            CPPUNIT_ASSERT_EQUAL( 2, sitr->getId() );
            CPPUNIT_ASSERT( ++sitr==sf.end() );
            sf.remove(sflt2);
            sitr = sf.begin();
            CPPUNIT_ASSERT_EQUAL( 0, sf.size() );
            CPPUNIT_ASSERT( sf.empty() );
            CPPUNIT_ASSERT( sitr==sf.end() );
        }

        void
        contains()
        {
            FLTest* flt3 = new FLTest(1);
            CPPUNIT_ASSERT( f.contains(flt1) );
            CPPUNIT_ASSERT( f.contains(flt2) );
            CPPUNIT_ASSERT( !f.contains(flt3) );
            delete flt3;
            f.remove(flt1);
            CPPUNIT_ASSERT( !f.contains(flt1) );
            CPPUNIT_ASSERT( f.contains(flt2) );

            SmartPtr<FLTest2> bflt3 = SmartPtr<FLTest2>(new FLTest2(1));
            CPPUNIT_ASSERT( bf.contains(bflt1) );
            CPPUNIT_ASSERT( bf.contains(bflt2) );
            CPPUNIT_ASSERT( !bf.contains(bflt3) );
            bf.remove(bflt1);
            CPPUNIT_ASSERT( !bf.contains(bflt1) );
            CPPUNIT_ASSERT( bf.contains(bflt2) );

            SFLTest* sflt3 = new SFLTest(1);
            CPPUNIT_ASSERT( sf.contains(sflt1) );
            CPPUNIT_ASSERT( sf.contains(sflt2) );
            CPPUNIT_ASSERT( !sf.contains(sflt3) );
            delete sflt3;
            sf.remove(sflt1);
            CPPUNIT_ASSERT( !sf.contains(sflt1) );
            CPPUNIT_ASSERT( sf.contains(sflt2) );
        }

        void
        iterators()
        {
            iterator itr1, itr2, itr3;

            itr1 = f.begin();
            CPPUNIT_ASSERT_EQUAL( 2, itr1->getId() );
            CPPUNIT_ASSERT_EQUAL( 2, (*itr1)->getId() );
            itr2 = itr1++;
            CPPUNIT_ASSERT_EQUAL( 1, itr1->getId() );
            CPPUNIT_ASSERT_EQUAL( 2, itr2->getId() );
            itr3 = ++itr1;
            CPPUNIT_ASSERT( itr3 == f.end() );
            CPPUNIT_ASSERT( itr3 == itr1 );

            itr1--;
            CPPUNIT_ASSERT( itr1->getId() == 1 );
            --itr1;
            CPPUNIT_ASSERT( itr1->getId() == 2 );
            itr1--;
            CPPUNIT_ASSERT( itr1 == f.end() );

            bflt1 = SmartPtr<FLTest2>();
            bflt2 = SmartPtr<FLTest2>();
            iterator2 bitr1, bitr2, bitr3;
            bitr1 = bf.begin();
            CPPUNIT_ASSERT_EQUAL( 2, bitr1->getId() );
            CPPUNIT_ASSERT_EQUAL( 2, (*bitr1)->getId() );
            bitr2 = bitr1++;
            CPPUNIT_ASSERT_EQUAL( 1, bitr1->getId() );
            CPPUNIT_ASSERT_EQUAL( 2, bitr2->getId() );
            bitr3 = ++bitr1;
            CPPUNIT_ASSERT( bitr3 == bf.end() );
            CPPUNIT_ASSERT( bitr3 == bitr1 );

            iterator3 sitr1, sitr2, sitr3;

            sitr1 = sf.begin();
            CPPUNIT_ASSERT_EQUAL( 2, sitr1->getId() );
            CPPUNIT_ASSERT_EQUAL( 2, (*sitr1)->getId() );
            sitr2 = sitr1++;
            CPPUNIT_ASSERT_EQUAL( 1, sitr1->getId() );
            CPPUNIT_ASSERT_EQUAL( 2, sitr2->getId() );
            sitr3 = ++sitr1;
            CPPUNIT_ASSERT( sitr3 == sf.end() );
            CPPUNIT_ASSERT( sitr3 == sitr1 );

            sitr1--;
            CPPUNIT_ASSERT( sitr1->getId() == 1 );
            --sitr1;
            CPPUNIT_ASSERT( sitr1->getId() == 2 );
            sitr1--;
            CPPUNIT_ASSERT( sitr1 == sf.end() );
        }

        void
        copy()
        {
            FastList<FLTest*> f_copy(f);
            CPPUNIT_ASSERT( f_copy.contains(flt1) );
            CPPUNIT_ASSERT( f_copy.contains(flt2) );
            CPPUNIT_ASSERT( f.contains(flt1) );
            CPPUNIT_ASSERT( f.contains(flt2) );
            CPPUNIT_ASSERT_EQUAL( 2, f.size() );
            CPPUNIT_ASSERT_EQUAL( 2, f_copy.size() );
            f.remove(flt1);
            f_copy.remove(flt2);
            CPPUNIT_ASSERT( f_copy.contains(flt1) );
            CPPUNIT_ASSERT( !f_copy.contains(flt2) );
            CPPUNIT_ASSERT( !f.contains(flt1) );
            CPPUNIT_ASSERT( f.contains(flt2) );
            CPPUNIT_ASSERT_EQUAL( 1, f.size() );
            CPPUNIT_ASSERT_EQUAL( 1, f_copy.size() );
        }

        void
        copySingleFastList()
        {
            WNS_ASSERT_ASSURE_EXCEPTION(FastList<SFLTest*> sf_copy(sf));
        }

        void
        assignment()
        {
            FastList<FLTest*> f_copy;
            f_copy = f;
            CPPUNIT_ASSERT( f_copy.contains(flt1) );
            CPPUNIT_ASSERT( f_copy.contains(flt2) );
            CPPUNIT_ASSERT( f.contains(flt1) );
            CPPUNIT_ASSERT( f.contains(flt2) );
            CPPUNIT_ASSERT_EQUAL( 2, f.size() );
            CPPUNIT_ASSERT_EQUAL( 2, f_copy.size() );
            f.remove(flt1);
            f_copy.remove(flt2);
            CPPUNIT_ASSERT( f_copy.contains(flt1) );
            CPPUNIT_ASSERT( !f_copy.contains(flt2) );
            CPPUNIT_ASSERT( !f.contains(flt1) );
            CPPUNIT_ASSERT( f.contains(flt2) );
            CPPUNIT_ASSERT_EQUAL( 1, f.size() );
            CPPUNIT_ASSERT_EQUAL( 1, f_copy.size() );
        }

        void
        assignmentSingleFastList()
        {
            FastList<SFLTest*> sf_copy;
            WNS_ASSERT_ASSURE_EXCEPTION(sf_copy = sf);
        }


        void
        copyElement()
        {
//             FLTest flt_copy(*flt1);
//             CPPUNIT_ASSERT( f.contains(&flt_copy) );
            CPPUNIT_ASSERT_MESSAGE("Needs to be implemented", false);
        }

        void
        copySingleFastListElement()
        {
            CPPUNIT_ASSERT_MESSAGE("Needs to be implemented", false);
        }

        void
        assignmentElement()
        {
            CPPUNIT_ASSERT_MESSAGE("Needs to be implemented", false);
        }

        void
        assignmentSingelFastListElement()
        {
            CPPUNIT_ASSERT_MESSAGE("Needs to be implemented", false);
        }

        void
        front()
        {
            CPPUNIT_ASSERT( f.front() == flt2 );
            CPPUNIT_ASSERT( bf.front() == bflt2 );
            CPPUNIT_ASSERT( sf.front() == sflt2 );
        }

        void
        removeFromAllLists()
        {
            FastList<FLTest*> f2;
            f2.push_back(flt1);
            CPPUNIT_ASSERT_EQUAL( 1, f2.size() );

            flt1->removeFromAllLists();
            CPPUNIT_ASSERT_EQUAL( 1, f.size() );
            CPPUNIT_ASSERT_EQUAL( 0, f2.size() );
            flt2->removeFromAllLists();
            CPPUNIT_ASSERT( f.empty() );

            sflt1->removeFromAllLists();
            CPPUNIT_ASSERT_EQUAL( 1, sf.size() );
            sflt2->removeFromAllLists();
            CPPUNIT_ASSERT( sf.empty() );
        }

        void
        destructAndRemove()
        {
            delete flt1;
            CPPUNIT_ASSERT_EQUAL( 1, f.size() );
            delete flt2;
            CPPUNIT_ASSERT( f.empty() );
            flt1 = flt2 = NULL;
            flt1 = new FLTest(1);
            flt2 = new FLTest(2);

            delete sflt1;
            CPPUNIT_ASSERT_EQUAL( 1, sf.size() );
            delete sflt2;
            CPPUNIT_ASSERT( sf.empty() );
            sflt1 = sflt2 = NULL;
            sflt1 = new SFLTest(1);
            sflt2 = new SFLTest(2);
        }

    };

    CPPUNIT_TEST_SUITE_REGISTRATION( FastListTest );
}
}
}
