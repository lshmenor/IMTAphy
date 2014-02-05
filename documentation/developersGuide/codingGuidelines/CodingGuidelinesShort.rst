Coding Guidelines Reference Card
================================


Layout Rules
------------

===================  =================================================
Topic                Rule
===================  =================================================
Indenting            spaces, not tabs
Maximum line length  not fixed (although above 100 is considered long)
===================  =================================================

Naming Conventions
------------------

Names are always written in CamelCase style. Not in underscore_style. The only exceptions are defines (include guards / macros).

=============== ======================================  ===================
Topic           Rule                                    Example
=============== ======================================  ===================
Local variables Variables start lower case, even if     ``double someVariable``
                they start with an acronym.             
                                                        ``Station umtsStation``
--------------- --------------------------------------  -------------------
Methods         Methods start lower case. They          ``void sendData()``
                describe an action. Therefore, they     
                should contain a verb. NVI is prefixed  ``void doSendData()``
                with ``do``, event-based interface 
                with ``on``                             ``void onData()``
--------------- --------------------------------------  -------------------
Classes         Classes start with an upper case        ``class PositionProvider``
                letter and are written in CamelCase     
--------------- --------------------------------------  -------------------
Interfaces      Interfaces start with an I              ``class IComponent``
--------------- --------------------------------------  -------------------
Class members   End with an underscore                  ``int bar_``
--------------- --------------------------------------  -------------------
Namespaces      Namespace are written all in lowercase  ``namespace wns``
                letters                                 
                                                        ``using namespace wns::simulator``
--------------- --------------------------------------  -------------------
Template        Template parameters are written in      ``template <typename KEY, typename VALUE>``
Parameters      upper case letters
--------------- --------------------------------------  -------------------
Macros          Marcos are written in upper case        ``#define WNS_ADD(x, y) (x)+(y)``
                letters. Underscores should be used 
                for better readability.
--------------- --------------------------------------  -------------------
Include Guards  Include guards, like Macros, are        ``MODULE_DIR_SUBDIR_SUBSUBDIR_FILE``
                written in upper case letters with      
                underscores to enhance readability      ``#ifndef CP_CONGESTION_TAHOE_HPP``
=============== ======================================  ===================






