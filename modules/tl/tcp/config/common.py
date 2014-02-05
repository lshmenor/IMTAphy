import os
import CNBuildSupport
from CNBuildSupport import CNBSEnvironment
import wnsbase.RCS as RCS

commonEnv = CNBSEnvironment(PROJNAME       = 'tcp',
                            AUTODEPS       = ['wns'],
                            PROJMODULES    = ['TEST', 'BASE'],
                            LIBRARY        = True,
                            SHORTCUTS      = True,
                            FLATINCLUDES   = False,
			    REVISIONCONTROL = RCS.Bazaar('../', 'tcp', 'unstable', '1.0'),
                            )
Return('commonEnv')
