import os
import CNBuildSupport
from CNBuildSupport import CNBSEnvironment
import wnsbase.RCS as RCS

commonEnv = CNBSEnvironment(PROJNAME       = 'dll',
                            AUTODEPS       = ['wns'],
                            PROJMODULES    = ['TEST', 'BASE'],
                            LIBRARY        = True,
                            SHORTCUTS      = True,
                            DEFAULTVERSION = True,
                            FLATINCLUDES   = False,
                            REVISIONCONTROL = RCS.Bazaar('../', 'DLLBase', 'main', '0.1'),
                            )
Return('commonEnv')
