Import('env')
import os
pythonFiles = SConscript(os.path.join('config', 'libfiles.py'))
sandboxSrcSubDir = os.path.join(env['sandboxDir'], "default", "lib", "python2.4", "site-packages", "pywns")

for pyFile in pythonFiles:
    env.InstallAs(os.path.join(sandboxSrcSubDir, pyFile), os.path.join('pywns', pyFile))
