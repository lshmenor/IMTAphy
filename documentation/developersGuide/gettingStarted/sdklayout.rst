---------
SDKLayout
---------

The SDK (Software Development Kit) keeps all other sub projects of
openWNS. The structure of the SDK as well as the location of the sub
projects is as follows (note, a directory followed by a name in square
means the directory is a sub project):

 - ``openWNS-sdk/``: master project
 - ``openWNS-sdk/bin/`` : helper scripts
 - ``openWNS-sdk/config/`` : configuration for the SDK
 - ``openWNS-sdk/config/projects.py`` : defines the projects being part of this working copy
 - ``openWNS-sdk/config/private.py`` : user defined compilation settings
 - ``openWNS-sdk/config/pushMailRecipients.py`` : list of recipients for mail on 'bzr push'
 - ``openWNS-sdk/config/valgrind.supp`` : openWNS-specific valgrind suppressions
 - ``openWNS-sdk/documentation/`` : the documentation project
 - ``openWNS-sdk/framework/`` : core part (lib, simulator application) of openWNS
 - ``openWNS-sdk/framework/application/`` : core application project
 - ``openWNS-sdk/framework/library/`` : core library project
 - ``openWNS-sdk/framework/rise/`` : Layer 1 and channel library
 - ``openWNS-sdk/framework/dllbase/`` : Layer 2 library
 - ``openWNS-sdk/framework/pywns/`` : post processing, system tests in python

 - ``openWNS-sdk/modules/`` : Modules for different entities in the ISO/OSI protocol stack 

 - ``openWNS-sdk/sandbox/`` : the build system will install libs and apps here

 - ``openWNS-sdk/tests/``
 - ``openWNS-sdk/tests/unit/`` : Python annd C++ unit tests
 - ``openWNS-sdk/tests/system/`` : System tests

 - ``openWNS-sdk/wnsbase/`` : SDK builtins

Most important are probably the ``framework`` and ``tests/unit``
directory ;-).

.. note:
   When openwns-sdk is initial download the sub projects are not
   contained in the SDK. See @ref download for further instructions on how to
   fetch the missing parts.
