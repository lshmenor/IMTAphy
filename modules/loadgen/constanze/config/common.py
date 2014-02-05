import os
import CNBuildSupport
from CNBuildSupport import CNBSEnvironment
import wnsbase.RCS as RCS

commonEnv = CNBSEnvironment(PROJNAME       = 'constanze',
                            PROJMODULES    = ['TEST', 'BASE'],
                            LIBRARY        = True,
                            SHORTCUTS      = True,
                            FLATINCLUDES   = False,
                            REVISIONCONTROL = RCS.Bazaar('../', 'Constanze', 'unstable', '1.0'),
                            )
Return('commonEnv')
