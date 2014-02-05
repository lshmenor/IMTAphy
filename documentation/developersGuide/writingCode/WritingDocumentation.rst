=====================
Writing Documentation
=====================

Documentation is written either in Doxygen style, inline with the source code, for the API documentation or in reStructured Text for this handbook.

Extracting Examples
-------------------

Whenever you write documentation for a piece of code it should remain valid
throughout the lifetime of that code. If the code changes, the documentation
should be adopted. However, this is a tedious process and cannot be
automated. Human beings often forget to update the documentation whenever
their code changes.

Therefore, it is explicitely encouraged to extract examples from unit tests that
are contained within the main test suite. The commit policy enforces that
all tests in the main test suite must succeed before commiting. If you extract
your examples from these tests, then it is quite clear that all your examples
will either remain valid or will be changed by developers if the break them.

You can mark a code snippet by using the ``begin example`` and
``endexample`` environment. The examples will be automatically
extracted by ``playground.py`` and will be made available to
Doxygen and Sphinx for reference. Below is an example of this.

.. code-block:: c++

   // begin example "wns.avaragetest.header.example"
   #include <WNS/Average.hpp>
   #include <WNS/TestFixture.hpp>

   namespace wns { namespace tests {
	     class AverageTest : public CppUnit::TestFixture  {
		   CPPUNIT_TEST_SUITE( AverageTest );
		   CPPUNIT_TEST( testPutAndGet );
		   CPPUNIT_TEST( testReset );
		   CPPUNIT_TEST_SUITE_END();
		public:
		   void setUp();
		   void tearDown();
		   void testPutAndGet();
		   void testReset();
		private:
		   Average<double> average;
	     };
    }}
    // end example

Including Examples in the API Documentaion
------------------------------------------

You can include an example in some other doxygen documentation by 
using the ``@include`` statement of Doxygen. Just reference your
example by its name and it will be rendered in place.

::

   /**
    * @page wns_AboutTheAverageTest About the Average Test
    *
    * The @c AverageTest is used to test @c wns::Average. Its class
    * definition is as follows:
    *
    * @include wns.avaragetest.header.example
    */


Including Examples in Sphinx Manuals
------------------------------------

You can include an example in reStructured Text as it is used by 
Sphinx with the ``literalinclude`` directive. This directive is 
used in this section to include the example above. The reST for 
this section is showed below the included example. Here is the 
example:

.. literalinclude:: ../../../.createManualsWorkingDir/wns.avaragetest.header.example

The reST source for this section is:

::

    Including Examples in Sphinx Manuals
    ------------------------------------

    You can include an example in reStructured Text as it is used by 
    Sphinx with the ``literalinclude`` directive. This directive is 
    used in this section to include the example above. The reST for 
    this section is showed below the included example. Here is the 
    example:

    .. literalinclude:: ../../../.createManualsWorkingDir/wns.avaragetest.header.example

    The reST source for this section is:

