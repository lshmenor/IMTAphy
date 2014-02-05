Coding Guidelines
=================

.. note::

   For a short version please see \ref wns_documentation_codingguidelines_short

Why do we need a style guide for openWNS?
-----------------------------------------

This describes the coding conventions (guidelines, styles, rules, ... whatever you want) applying to the openWNS standard library and the main distribution. This style guide is insipred from "Style Guide for Python Code" (Python Enhancement Proposal: http://www.python.org/dev/peps/pep-0008/) .

To give you an idea why these guidelines exists, we can again refer to "Style Guide for Python Code":

::

    One of Guido's key insights is that code is read much more often than it
    is written.  The guidelines provided here are intended to improve the
    readability of code and make it consistent across the wide spectrum of
    Python code.  As PEP 20 says, "Readability counts".

    A style guide is about consistency.  Consistency with this style guide is
    important.  Consistency within a project is more important. Consistency
    within one module or function is most important.

    But most importantly: know when to be inconsistent -- sometimes the style
    guide just doesn't apply.  When in doubt, use your best judgment.  Look
    at other examples and decide what looks best.  And don't hesitate to ask!

    Two good reasons to break a particular rule:

    (1) When applying the rule would make the code less readable, even for
        someone who is used to reading code that follows the rules.

    (2) To be consistent with surrounding code that also breaks it (maybe for
        historic reasons) -- although this is also an opportunity to clean up
        someone else's mess (in true XP style).

Code layout
-----------

Indenting
'''''''''

**Use 4 spaces per indentation level.**

If tabs are used for indentation many editors mix tabs and spaces to get a nice indentation. This will result in an ugly indentation if the code is viewed with other settings for the tab-width. Example:

.. code-block:: c++

  // edited with tab-width: 4

  void
  Foo::bar(int foo,
	   string bar)
  {
      // some code here
  }


Since we have 9 characters in front of the type ``string``, the editor
set to tab-width 4 will use 2 tabs and one space to align ``string`` with ``int``.

Opening this file in an editor with tab-width set to 8 looks like this:

.. code-block:: c++

  // viewed with tab-width: 8

  void
  Foo::bar(int foo,
		   string bar)
  {
	  // some code here
  }


This is because two tabs is 8 spaces and one space add up to 17 spaces. This means tab-width is not adjustable. Thus the indentation style for openWNS is 4 spaces per indentation level.

Indentation in Emacs
''''''''''''''''''''

The following shows a c-style for emacs that provides the required indentation rules for the openWNS.

.. code-block:: none

  ;; The wns-c-style.
  (defconst wns-c-style
    '((c-tab-always-indent . t)
      (c-basic-offset . 4)
      (indent-tabs-mode . nil)
      (c-comment-only-line-offset . 4)
      (c-offsets-alist . ((comment-intro . 0)
			  (statement-block-intro . +)
			  (knr-argdecl-intro . +)
			  (substatement-open . 0)
			  (label . 0)
			  (statement-cont . +)
			  (inline-open . 0)
			  (inexpr-class . 0)
			  (inher-intro . ++)
			  ))
      ;;(c-echo-syntactic-information-p . t)
      )
    "WNS C/C++ Programming Style")

  (c-add-style "WNS" wns-c-style)

Feel free to include the WNS c-style into you ~/.emacs file.

Maximum line length
'''''''''''''''''''

**Keep the line length readable**

Suggestion is: around 80 characters or up to 10 words. Studies have shown that 10 words text-width are optimal for eye-tracking (from C++ Coding-Standards, Alexandrescu and Sutter, 2004).

Whitespaces
'''''''''''

Conventional operators should be surrounded by a space character. C++ reserved words should be followed by a white space.  Commas should be followed by a white space. Semicolons in for statments should be followed by a space character. Always surround these binary operators with a single space on either side: assignment (=), augmented assignment (+=, -= etc.),
comparisons (==, <, >, !=, <=, >=).

.. code-block:: c++

  a = (b + c) * d;                // NOT: a=(b+c)*d
  while (true)                    // NOT: while(true) { ...
  doSomething(a, b, c, d);        // NOT: doSomething(a,b,c,d);
  for (int ii = 0; ii < 10; ++ii) // NOT: for(i=0;i<10;i++)


Avoid extraneous whitespace in the following situations:

Immediately inside parentheses, brackets or braces. Immediately before the open parenthesis that starts the argument list of a function call. Immediately before the open parenthesis that starts an indexing. To align operators with others.


.. code-block:: c++

  spam(ham, eggs(2))         // NOT: spam( ham, eggs (2) )
  spam(1)                    // NOT: spam (1)
  map['key'] = vector[index] // NOT: map ['key'] = vector [index]
  int a = 0;                 // NOT: int a     = 0
  int b = 0;                 // NOT: int b     = 0
  int index = 0;             // NOT: int index = 0



Namespaces
''''''''''

.. code-block:: c++

  namespace a { namespace b { namespace c {

  } // namespace c
  } // namespace b
  } // namespace a


Classes and Templates
'''''''''''''''''''''

.. code-block:: c++

  template <typename FOO, typename BAR>
  class FooBar :
      public virtual SuperClassA
      private SuperClassB
  {
  public:
      // Public stuff first (most interesting)
  protected:
      // Then protected stuff (only interesting if deriving)
  private:
      // Last private stuff (internal realization)
  };


Methods
'''''''

.. code-block:: c++

  // of course a virtual template method is impossible
  // but the keywords should be placed like this
  template <typename FOO, typename BAR>
  virtual const BAR&
  scheduleFooBar(const FOO& foo) const
  {
      // some code
  }


Constructors
''''''''''''

.. code-block:: c++

  // in case you have only one argument don't forget to add
  // explicit keyword to avoid automatic type cast
  explicit
  FooBar::FooBar(Arg arg) :
     SuperClass(arg),
     argument(arg)
  {
       // no member initialization here!
  }


Control structures (if, while, for)
'''''''''''''''''''''''''''''''''''

.. code-block:: c++

  if (this->empty() == true)
  {
      // some code here
  }
  else
  {
      // some code here
  }

  while (this->empty() == false)
  {
      // some code here
  }

  for (int ii = 0; ii < 4; ++ii)
  {
      // some code here
  }


Type modifiers
''''''''''''''

``*`` and ``&`` belong to the type.

.. code-block:: c++

  int* a = new a;   // NOT: int *a = new a; or int * a = new a;
  int& b = c;       // NOT: int &b = c; or int & b = c;


Comments
''''''''

Comments are placed above the code block to be commented. Usage of trailing comments is explictly discouraged:

.. code-block:: c++

  // Yes:
  // Make sure the station got registered
  if(std::find(stationContainer.begin(), stationContainer.end(), station))
  {
      station->move();
  }

  // No:
  if(std::find(stationContainer.begin(), stationContainer.end(), station)) // Make sure the station got registered
  {
      station->move();
  }


Don't write comments that just repeat the code. They will likely get out of sync.

.. code-block:: c++

  // No:
  if(!container.empty()) // true if container not empty
  {
     container.clear();
  }


Naming Conventions
------------------

Names are always written in ``CamelCase`` style. **Not** in ``underscore_style``. The only exceptions are defines (include guards / macros).

Local variables
'''''''''''''''

Variables start lower case, even if they start with an acronym. The acronym is all written in lower case then:

.. code-block:: c++

  void
  someMethod()
  {
      double someVariable;
      Station umtsStation;
  }


Methods
'''''''

Methods start lower case. They describe an action. Therefore, they should contain a verb:

.. code-block:: c++

  void
  sendData(const PDU& pdu)
  {
  }


For non-virtual interfaces (NVI)s the method to actually dispatch the request is prefixed with ``do``:

.. code-block:: c++

  void
  sendData(const PDU& pdu)
  {
      this->doSendData(pdu);
  }

  virtual void
  doSendData(const PDU& pdu) = 0;


Methods, that represent (asynchronous) event-based interfaces are prefixed ``on``:

.. code-block:: c++

  void
  onConnectionEstablished()
  {
  }


And for an event-based NVI:

.. code-block:: c++

  void
  onConnectionEstablished()
  {
      this->doOnConnectionEstablished();
  }
  virtual void
  doOnConnectionEstablished() = 0;



Classes
'''''''

Classes start with an upper case letter and are written in CamelCase:

.. code-block:: c++

  class PositionProvider
  {
  };


If a part of the name is an acronym, the acronym is written in upper case letters (or in mixed case if this is the normal way of spelling it):

.. code-block:: c++

  class UMTSTransmitter
  {
  };

  class WiMAXReceiver
  {
  };


Interfaces
''''''''''

Interfaces start with an ``I``. This avoids name collisions as it is quite common to have ``IFoo`` and a class named ``Foo`` that implements ``IFoo``.

.. code-block:: c++

  class IComponent
  {
  public:
      virtual void
      connect() = 0;
  };


Class Members
'''''''''''''

Scope identification of a variable is important. Is it a local scratch variable or a class member. For easy identification of class members a underscore at the end of the member is used, regardless if the member is private, protected or public:

.. code-block:: c++

  class Foo
  {
  private:
      int bar_;
  };


Namespaces
''''''''''

Namespace are written all in lowercase letters:

.. code-block:: c++

  namespace wns { namespace simulator {
  }
  }

  using namespace wns::simulator;


Template Parameters
'''''''''''''''''''

Template parameters are written in upper case letters:

.. code-block:: c++

  template <typename KEY, typename VALUE>
  class Registry
  {
  }


Macros
''''''

Marcos are written in upper case letters. Underscores should be used for better readability. The name should always begin with ``WNS_`` to avoid collisions with other macros (e.g. REF and DEREF of Qt).

.. code-block:: c++

  #define WNS_ADD(x, y) (x)+(y)


Include Guards
''''''''''''''

Include guards, like Macros, are written in upper case letters with underscores to enhance readability. To avoid name clashes here the following rule must be followed when choosing a name for an include guard:

``MODULE_DIR_SUBDIR_SUBSUBDIR_FILE``

Hence, if the file is placed in the TCP module in ``src/congestion/Tahoe.hpp`` the include guard is:

.. code-block:: c++

  #ifndef TCP_CONGESTION_TAHOE_HPP
  #define TCP_CONGESTION_TAHOE_HPP
  // some code
  #endif // NOT defined TCP_CONGESTION_TAHOE_HPP


At the closing ``#endif`` you should state what is (not) defined here.

Programming recommendations
---------------------------

Namespaces
''''''''''

Some rules:
#. Never use ``using namespace xyz`` in header files
#. For an implementation: Only use ``using namespace xyz`` for the corresponding class definition
#. You should omit the additional namespace qualifiers if you are already in that namespace (e.g. in a class definition)
#. You should always use the full namespace qualifier for any out-of-current-namespace-scope types

.. code-block:: c++

  namespace foo {
      class Bar
      {
	  void
	  clone(Bar*);
      };
  }

  // NOT
  namespace foo {
      class Bar
      {
	  void
	  clone(foo::Bar*);
      };
  }


Comparison
''''''''''

When testing for something (e.g. in an if-statement), always be explicit about what you expect:

.. code-block:: c++

  // No

  if (foo)

  // Yes (because it can be any of these)

  // in case of bool
  if (foo == true)

  // in case of pointer
  if (foo != NULL)

  // in case of integer
  if (foo != 0)


Call-by-value, call-by-reference
''''''''''''''''''''''''''''''''

Use call by reference (const) where possible for complex data types, but always call-by-value for Plain Old Data Types (PODs):

.. code-block:: c++

  // YES
  void
  foo(const Bar& bar)

  // NO
  void
  foo(Bar bar)

  // NO, only use if you need to modify bar inside foo! and even then with care!
  void
  foo(Bar& bar)

  // NO, only use if you need polymorphism
  void
  foo(Bar* bar)

  // YES
  void
  foo(double x)

  // NO
  void
  foo(const double& x)



License Statement
-----------------
openWNS is released under the Lesser GNU Public License Version 2. We follow the
guidelines of the Free Software Foundation for releasing software under the LGPLv2
(see http://www.gnu.org/licenses/gpl-howto.html for details).

This recommendation requires to put a Header at the beginning of every released
file that states that this source code is part of openWNS, that it is released under
the GPLv2 and that it comes with no warranty. Below you find a prepared header both
for Python and C++. Every module should also contain a copy of the GNU Public License
and the Lesser GNU Public License. Please place the files http://www.openwns.org/Wiki/CodingStyles?action=AttachFile&do=view&target=COPYING and http://www.openwns.org/Wiki/CodingStyles?action=AttachFile&do=view&target=COPYING.LESSER in the root of your module.

The LGPLv2 Header for C++ to be used in openWNS is as follows:

::

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


The LGPLv2 Header for Python to be used in openWNS is as follows:

::

   ###############################################################################
   # This file is part of openWNS (open Wireless Network Simulator)
   # _____________________________________________________________________________
   #
   # Copyright (C) 2004-2007
   # Chair of Communication Networks (ComNets)
   # Kopernikusstr. 16, D-52074 Aachen, Germany
   # phone: ++49-241-80-27910,
   # fax: ++49-241-80-22242
   # email: info@openwns.org
   # www: http://www.openwns.org
   # _____________________________________________________________________________
   #
   # openWNS is free software; you can redistribute it and/or modify it under the
   # terms of the GNU Lesser General Public License version 2 as published by the
   # Free Software Foundation;
   #
   # openWNS is distributed in the hope that it will be useful, but WITHOUT ANY
   # WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
   # A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
   # details.
   #
   # You should have received a copy of the GNU Lesser General Public License
   # along with this program.  If not, see <http://www.gnu.org/licenses/>.
   #
   ###############################################################################



