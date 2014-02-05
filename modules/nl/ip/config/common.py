import os
import CNBuildSupport
from CNBuildSupport import CNBSEnvironment
import wnsbase.RCS as RCS

commonEnv = CNBSEnvironment(PROJNAME       = 'ip',
                            AUTODEPS       = ['wns'],
                            PROJMODULES    = ['TEST', 'BASE', 'IPTABLES', 'TRACE', 'TUNNEL'],
                            LIBRARY        = True,
                            FLATINCLUDES   = False,
                            SHORTCUTS      = True,
                            REVISIONCONTROL = RCS.Bazaar('../', 'ip', 'unstable', '1.2'),
                            )
Return('commonEnv')
