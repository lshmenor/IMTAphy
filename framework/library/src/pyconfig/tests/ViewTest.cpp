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

#include <WNS/Python.hpp>

#include <WNS/pyconfig/tests/ViewTest.hpp>
#include <WNS/Exception.hpp>

#include <vector>
#include <list>
#include <deque>
#include <set>

using namespace std;
using namespace wns::pyconfig::tests;

CPPUNIT_TEST_SUITE_REGISTRATION( ViewTest );

void
ViewTest::prepare()
{
	pyco = new Parser();
} // setUp


void
ViewTest::cleanup()
{
	delete pyco;
} // tearDown


void
ViewTest::testTypes()
{
	pyco->loadString(
		"i = 42\n"
		"f = 42.42\n"
		"s = '42.42.42'\n"
		);

	int i;
	CPPUNIT_ASSERT(pyco->get(i, "i"));
	CPPUNIT_ASSERT_EQUAL(42, i);

	CPPUNIT_ASSERT_EQUAL(42, pyco->get<int>("i"));

	double f;
	CPPUNIT_ASSERT(pyco->get(f, "f"));
	CPPUNIT_ASSERT_DOUBLES_EQUAL(42.42, f, 0.1);

	CPPUNIT_ASSERT_DOUBLES_EQUAL(42.42, pyco->get<double>("f"), 0.1);

	string s;
	CPPUNIT_ASSERT(pyco->get(s, "s"));
	CPPUNIT_ASSERT_EQUAL(string("42.42.42"), s);

	CPPUNIT_ASSERT_EQUAL(string("42.42.42"), pyco->get<string>("s"));
} // testTypes


void
ViewTest::testBool()
{
	pyco->loadString(
		"t1 = True\n"
		"t2 = 2\n"
		"t3 = 1 == 1\n"
		"f1 = False\n"
		"f2 = 0\n"
		"f3 = 1 == 0\n"
		);

	CPPUNIT_ASSERT(pyco->get<bool>("t1"));
	CPPUNIT_ASSERT(pyco->get<bool>("t2"));
	CPPUNIT_ASSERT(pyco->get<bool>("t3"));
	CPPUNIT_ASSERT(!pyco->get<bool>("f1"));
	CPPUNIT_ASSERT(!pyco->get<bool>("f2"));
	CPPUNIT_ASSERT(!pyco->get<bool>("f3"));
} // testBool

void
ViewTest::testString()
{
	pyco->loadString(
		"foo='bar'\n"
		"multiFoo='bar\\nbaz'\n"
		);

	CPPUNIT_ASSERT(pyco->get<std::string>("foo") == std::string("bar"));
	CPPUNIT_ASSERT(pyco->get<std::string>("multiFoo") == std::string("bar\nbaz"));

} // testBool


void
ViewTest::testNested()
{
	pyco->loadString(
		"class Foo:\n"
		"  bar = 42\n"
		"foo = Foo()\n"
		"foo.bar = 43\n"
		);

	int i;
	CPPUNIT_ASSERT(pyco->get(i, "Foo.bar"));
	CPPUNIT_ASSERT_EQUAL(42, i);

	CPPUNIT_ASSERT(pyco->get(i, "foo.bar"));
	CPPUNIT_ASSERT_EQUAL(43, i);
} // testNested


void
ViewTest::testMultipleLoads()
{
	pyco->loadString("i = 23\n");
	pyco->loadString("j = 42\n");

	CPPUNIT_ASSERT_EQUAL(23, pyco->get<int>("i"));
	CPPUNIT_ASSERT_EQUAL(42, pyco->get<int>("j"));
} // testMultipleLoads


void
ViewTest::testKnows()
{
	pyco->loadString("i = 4\n");

	CPPUNIT_ASSERT( pyco->knows("i") );
	CPPUNIT_ASSERT( !pyco->knows("j") );
} // testKnows


void
ViewTest::testUnknown()
{
	int i;
	CPPUNIT_ASSERT( !pyco->get(i, "j") );
} // testUnknown


void
ViewTest::testView()
{
	pyco->loadString(
		"class view:\n"
		"  thang = 4711\n"
		);

	View* view = new View(*pyco, "view");
	CPPUNIT_ASSERT_EQUAL(4711, view->get<int>("thang"));
	delete view;
} // testView


void
ViewTest::testGetWithView()
{
	pyco->loadString(
		"class view1:\n"
		"  thang = 4711\n"
		"class view2:\n"
		"  thang = 666\n"
		);

	View view = pyco->get<View>("view1");
	CPPUNIT_ASSERT_EQUAL(4711, view.get<int>("thang"));

	pyco->get(view, "view2");
	CPPUNIT_ASSERT_EQUAL(666, view.get<int>("thang"));
} // testGetWithView


void
ViewTest::testSequenceView()
{
	pyco->loadString(
		"class view:\n"
		"  def __init__(self, i): self.i = i\n"
		"foo = [view(23), view(42)]\n"
		);

	View view = View(*pyco, "foo", 1);
	CPPUNIT_ASSERT_EQUAL( 42, view.get<int>("i") );
} // testSequenceView


void
ViewTest::testSequenceGetWithView()
{
	pyco->loadString(
		"class view:\n"
		"  def __init__(self, i): self.i = i\n"
		"foo = [view(23), view(42)]\n"
		);

	View view = pyco->get<View>("foo", 0);
	CPPUNIT_ASSERT_EQUAL( 23, view.get<int>("i") );

	pyco->get(view, "foo", 1);
	CPPUNIT_ASSERT_EQUAL( 42, view.get<int>("i") );
} // testSequenceGetWithView


void
ViewTest::testContext()
{
	pyco->loadString(
		"class A:\n"
		"  class B:\n"
		"    class C:\n"
		"      pass\n"
		);

	CPPUNIT_ASSERT(pyco->context() == "<string>");

	View v1 = View(*pyco, "A");
	CPPUNIT_ASSERT(v1.context() == "<string>::A");

	View v2 = View(v1, "B");
	CPPUNIT_ASSERT(v2.context() == "<string>::A::B");

	CPPUNIT_ASSERT(v2.knows("C"));
} // testContext


void
ViewTest::testInheritance()
{
	pyco->loadString(
		"class A:\n"
		"  foo = 42\n"
		"class B(A):\n"
		"  pass\n"
		);

	CPPUNIT_ASSERT_EQUAL(42, pyco->get<int>("B.foo"));
} // testInheritance


void
ViewTest::testInstance()
{
	pyco->loadString(
		"class A:\n"
		"  foo = 42\n"
		"a = A()\n"
		);

	CPPUNIT_ASSERT_EQUAL(42, pyco->get<int>("a.foo"));
} // testInstance


void
ViewTest::testInheritanceInstance()
{
	pyco->loadString(
		"class A:\n"
		"  foo = 42\n"
		"class B(A):\n"
		"  pass\n"
		"b = B()\n"
		);

	CPPUNIT_ASSERT_EQUAL(42, pyco->get<int>("b.foo"));
} // testInheritanceInstance


void
ViewTest::testCopy()
{
	pyco->loadString("foo = 42\n");
	CPPUNIT_ASSERT(pyco->dict.obj_->ob_refcnt == 2);

	{
		View foo = View(*pyco);

		CPPUNIT_ASSERT(pyco->dict.obj_->ob_refcnt == 3);
		CPPUNIT_ASSERT_EQUAL(42, foo.get<int>("foo"));
		CPPUNIT_ASSERT(foo.context() == "<string>");
	}

	CPPUNIT_ASSERT(pyco->dict.obj_->ob_refcnt == 2);
} // testCopy


void
ViewTest::testAssignment()
{
	pyco->loadString("foo = 42\n");
	CPPUNIT_ASSERT(pyco->dict.obj_->ob_refcnt == 2);

	{
		View foo;

		foo = *pyco;

		CPPUNIT_ASSERT(pyco->dict.obj_->ob_refcnt == 3);
		CPPUNIT_ASSERT_EQUAL(42, foo.get<int>("foo"));
		CPPUNIT_ASSERT(foo.context() == "<string>");
	}

	CPPUNIT_ASSERT(pyco->dict.obj_->ob_refcnt == 2);
} // testAssignment


void
ViewTest::testAutomatic()
{
	Parser foo;
	foo.loadString(
		"foo = 42\n"
		"class bar:\n"
		"  baz = 43\n"
		);

	CPPUNIT_ASSERT(foo.get<int>("foo") == 42);

	View bar = View(foo, "bar");
	CPPUNIT_ASSERT(bar.get<int>("baz") == 43);
} // testAutomatic


void
ViewTest::testSequence()
{
	Parser foo;
	foo.loadString(
		"foo = [4, 3, 2, 1, 0]\n"
		);

	CPPUNIT_ASSERT_EQUAL( 5, foo.len("foo") );
	CPPUNIT_ASSERT_EQUAL( 4, foo.get<int>("foo", 0) );
	CPPUNIT_ASSERT_EQUAL( 0, foo.get<int>("foo", 4) );
} // testSequence


void
ViewTest::testNotASequence1()
{
	Parser foo;
	foo.loadString(
		"foo = 42\n"
		);

	int result;
	CPPUNIT_ASSERT( !foo.len(result, "foo") );
} // testNotASequence1


void
ViewTest::testNotASequence2()
{
	Parser foo;
	foo.loadString(
		"foo = 42\n"
		);

	int result;
	CPPUNIT_ASSERT( !foo.get(result, "foo", 1) );
} // testNotASequence2


void
ViewTest::testOutOfRange()
{
	Parser foo;
	foo.loadString(
		"foo = [4, 3, 2, 1, 0]\n"
		);

	int result;
	CPPUNIT_ASSERT( !foo.get(result, "foo", 6) );
} // testOutOfRange

void
ViewTest::testGetSequence()
{
	Parser foo;
	foo.loadString(
		"foo = [4, 3, 2, 1, 0]\n"
		);

	vector<int> resultVec = foo.getSequence< vector<int> >("foo");
	CPPUNIT_ASSERT_EQUAL( 4, resultVec[0] );
	CPPUNIT_ASSERT_EQUAL( 3, resultVec[1] );
	CPPUNIT_ASSERT_EQUAL( 2, resultVec[2] );
	CPPUNIT_ASSERT_EQUAL( 1, resultVec[3] );
	CPPUNIT_ASSERT_EQUAL( 0, resultVec[4] );
	list<int> resultList = foo.getSequence< list<int> >("foo");
	list<int>::iterator itr = resultList.begin();
	for(int i=4; i>=0; --i) {
		CPPUNIT_ASSERT_EQUAL( i, *itr);
		itr++;
	}
	deque<int> resultDeq = foo.getSequence< deque<int> >("foo");
	CPPUNIT_ASSERT_EQUAL( 4, resultDeq[0] );
	CPPUNIT_ASSERT_EQUAL( 3, resultDeq[1] );
	CPPUNIT_ASSERT_EQUAL( 2, resultDeq[2] );
	CPPUNIT_ASSERT_EQUAL( 1, resultDeq[3] );
	CPPUNIT_ASSERT_EQUAL( 0, resultDeq[4] );
} // testGetSequence


void
ViewTest::testPatch()
{
	Parser config;
	config.loadString(
		"foo = 42"
		);

	config.patch("foo = 23");
	CPPUNIT_ASSERT_EQUAL(23, config.get<int>("foo"));
} // testPatch


void
ViewTest::testIsNone()
{
	Parser config;
	config.loadString("class A:\n"
			  " a = None\n"
			  "class B:\n"
			  " b = 1\n"
			  "foo = A()\n");
	CPPUNIT_ASSERT( config.isNone("A.a") );
	CPPUNIT_ASSERT( !config.isNone("B.b") );
	CPPUNIT_ASSERT( config.isNone("foo.a") );
	CPPUNIT_ASSERT( !config.isNone("foo") );
}

void
ViewTest::testMatrix()
{
	// try to read a matrix style set of lists. Problem here is that the
	// inner lists are unamed. As long as we don't have Sequences we can
	// only build the indices with strings and call them directly.
	pyco->loadString("a = [ [0,1], [2,3] ]");
	for(int ii = 0; ii < pyco->len("a"); ++ii)
	{
		std::stringstream currentIndex;
		currentIndex << ii;

		CPPUNIT_ASSERT_EQUAL( ii*2, pyco->get<int>("a["+currentIndex.str()+"]", 0) );
		CPPUNIT_ASSERT_EQUAL( ii*2+1, pyco->get<int>("a["+currentIndex.str()+"]", 1) );
	}
}

void
ViewTest::testId()
{
	pyco->loadString("class A:\n"
			 " pass\n"
			 "a = A()\n"
			 "b = A()\n"
			 "aa = a\n");

	CPPUNIT_ASSERT( pyco->get("a").getId() == pyco->get("a").getId() );
	CPPUNIT_ASSERT( pyco->get("a").getId() == pyco->get("aa").getId() );
	CPPUNIT_ASSERT( pyco->get("b").getId() != pyco->get("a").getId() );
}

void
ViewTest::testEqualOp()
{
	pyco->loadString("class A:\n"
			" pass\n"
			"a = A()\n"
			"b = A()\n"
			"aa = a\n");

	CPPUNIT_ASSERT( pyco->get("a") == pyco->get("a") );
	CPPUNIT_ASSERT( pyco->get("a") == pyco->get("aa") );
	CPPUNIT_ASSERT( pyco->get("b") != pyco->get("a") );
}

void
ViewTest::testDict()
{
	pyco->loadString("foo = {'a': 23, 'b': 42}");

	CPPUNIT_ASSERT_EQUAL(42, pyco->get<int>("foo['b']"));
}

void
ViewTest::testStoreInMap()
{
	pyco->loadString("class A:\n"
			" pass\n"
			"a = A()\n"
			"b = A()\n"
			"aa = a\n");

	std::map<View, int> m;

	m[pyco->get("a")] = 1;
	m[pyco->get("b")] = 2;

	CPPUNIT_ASSERT_EQUAL( 1, m[pyco->get("a")] );
	CPPUNIT_ASSERT_EQUAL( 1, m[pyco->get("aa")] );
	CPPUNIT_ASSERT_EQUAL( 2, m[pyco->get("b")] );
}

void
ViewTest::testBoundMethod()
{
	pyco->loadString(
		"class A:\n"
		"  def foo(self, x):\n"
		"    return self.y * x\n"
		"  y = 2\n"
		"a = A()\n"
		);
	View view(*pyco, "a");
	CPPUNIT_ASSERT_EQUAL(42, view.get<int>("foo(21)"));
}

void
ViewTest::testDontConvertNone()
{
	pyco->loadString("foo = None\n");
	CPPUNIT_ASSERT_THROW(pyco->get<int>("foo"), wns::Exception);
	CPPUNIT_ASSERT_THROW(pyco->get<double>("foo"), wns::Exception);
	CPPUNIT_ASSERT_THROW(pyco->get<std::string>("foo"), wns::Exception);
	CPPUNIT_ASSERT_THROW(pyco->get<bool>("foo"), wns::Exception);
	CPPUNIT_ASSERT_THROW(pyco->get("foo"), wns::Exception);
}

void
ViewTest::testViewFromNestedSequence()
{

	pyco->loadString("class Foo:\n"
			 "  a = 5\n"
			 "  b = 7\n"
			 "\n"
			 "seq = [ Foo(), [ Foo(), Foo() ], Foo() ]\n"
			 "seq[1][0].a=42\n"
			 );


	Sequence seq = pyco->getSequence("seq");
	Sequence seq2 = seq.getSequenceAt(1);
	wns::pyconfig::View foo(seq2, 0);

	CPPUNIT_ASSERT_EQUAL( 3, seq.size() );
	CPPUNIT_ASSERT_EQUAL( 2, seq2.size() );
	CPPUNIT_ASSERT_EQUAL( 42, foo.get<int>("a"));
	CPPUNIT_ASSERT_EQUAL(std::string("<string>::seq[1][0]"), foo.context());
}


