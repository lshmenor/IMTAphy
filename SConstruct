import os
import sys
import wnsbase.playground.Project
import subprocess

class EnvInfo:
    def __init__(self):
        self.lsb = {}
        self.python = {}

        try:
            f = open('/etc/lsb-release')
            c = f.read()
            f.close()
            c = c.split("\n")
            for l in c:
                if len(l.lstrip()) == 0:
                    continue
                k,v = l.split("=")
                self.lsb[k] = v

        except IOError:
            print
            print "WARNING Cannot acces /etc/lsb-release. Unknown distribution. Trying fallback."
            print
            self.lsb["DISTRIB_ID"]="Unknown"

        try:
            output = subprocess.Popen(["python", "-V"], stderr=subprocess.PIPE).communicate()[1]
            output = output.split()
            version = output[1].split(".")
            self.python["version_major"] = "%s.%s" % (version[0], version[1])            
        except OSError:
            print
            print "Error!!! Cannot find python command"
            print
            exit(1)

        try:
            self.python["version_minor"] = "0"
	    self.python["version_minor"] = "%s" % (version[2])
	except IndexError:
	    pass

        guess_python_config = "python-config"

        if self.lsb["DISTRIB_ID"] == "Ubuntu" and self.lsb["DISTRIB_CODENAME"] == 'lucid':
            guess_python_config = "python2.6-config"

        try:
            output = subprocess.Popen([guess_python_config, "--includes"], stderr=subprocess.PIPE).communicate()[1]
            output = subprocess.Popen([guess_python_config, "--libs"], stderr=subprocess.PIPE).communicate()[1]
        except OSError: 
            print
            print "ERROR!!! Cannot find python-config or python2.6-config which is needed to find correct Python paths"
            print
            sys.exit(1)

        self.pythonConfigCmd = guess_python_config

    def dump(self):
        print self.lsb
        print self.python


envInfo = EnvInfo()

sys.path.append('config')
import projects
sys.path.remove('config')

SetOption('num_jobs', os.sysconf('SC_NPROCESSORS_ONLN'))
SetOption('implicit_cache', True)
SourceSignatures('timestamp')
TargetSignatures('timestamp')

# Load options from file and from command line
optionFile = os.path.join('config','options.py')
if not os.path.exists(optionFile):
    print "No options file found. Creating empty options file..."
    defaultoptions = \
    """# This is the build option file. Specify your build options here.

# The 'buildDir' option specifies a directory where the object files are built.
# It is a good idea to choose a directory on a fast harddisk to speed up the
# build process.
#
# buildDir = '/path/to/fast/harddisk/'

# The 'profile' option specifies if the openWNS is built with profiling support.
#
# profile = False

# The 'sandboxDir' specifies the directory where the modules and the openwns 
# executable will be installed.
#
# sandboxDir = '/path/to/the/sandbox'

# The 'cacheDir' option specifies a shared directory for object files.
#
# cacheDir = '/path/to/cacheDir'

# With the 'static' option set to True, all modules will be linked statically.
# 
# static = False

# 'Set to enable smart pointer debugging'
#
# smartPtrDBG = False

# Set to enable callgrind profiler instrumentation
#
# callgrind = False

# Set to enable 32bit compiling
#
# arch32 = False

# Path to the object cache, if not False
#
# cacheDir = False
"""
    of = open(optionFile, 'w')
    of.write(defaultoptions)
    of.close

opts = Options(os.path.join('config','options.py'))
opts.Add(BoolOption('static', 'Set to build the static version', False))
opts.Add(PathOption('buildDir', 'Path to the build directory',  os.path.join(os.getcwd(), '.build'), PathOption.PathIsDirCreate))
opts.Add(BoolOption('profile', 'Set to enable profiling support', False))
opts.Add(BoolOption('smartPtrDBG', 'Set to enable smart pointer debugging', False))
opts.Add(BoolOption('callgrind', 'Set to enable callgrind profiler', False))
opts.Add(BoolOption('arch32', 'Set to enable 32bit compiling', False))
opts.Add(PathOption('sandboxDir', 'Path to the sandbox', os.path.join(os.getcwd(), 'sandbox'), PathOption.PathIsDirCreate))
opts.Add(PackageOption('cacheDir', 'Path to the object cache', False))
environments = []
installDirs = {}


# Create the debug environment
dbgenv = Environment(options = opts)
dbgenv.Append(CXXFLAGS = ['-g', '-O0', '-fno-inline'])

dbgenv.flavour = 'dbg'
environments.append(dbgenv)

# Create the environment for optassuremsg
optassuremsgenv = Environment(options = opts)
optassuremsgenv.Append(CXXFLAGS = ['-O3',
                          '-fno-strict-aliasing',
                          '-Wno-unused-variable',
                          '-Wno-unused-parameter'])

optassuremsgenv.flavour = 'optassuremsg'
environments.append(optassuremsgenv)

# Create the opt Environment
optenv = Environment(options = opts, CPPDEFINES= {'NDEBUG': '1',
                                                  'WNS_NDEBUG' : '1',
                                                  'WNS_NO_LOGGING' : '1'})

optenv.Append(CXXFLAGS = ['-O3',
                          '-fno-strict-aliasing',
                          '-Wno-unused-variable',
                          '-Wno-unused-parameter'])
optenv.flavour = 'opt'
environments.append(optenv)



# We use the debug environment to generate the help text for the options.
Help( """
The openWNS SDK provides rich tools to build the framework and modules.

Type: 'scons' to build all libraries and modules for the debug flavour.
      'scons opt' to build all libraries and module for the opt flavour.
      'scons sandbox/<flavour>/lib/<lib of your choice>' to build a single library.
      'scons static=yes' to build openwns executable with statically linked libraries.
      'scons pyconfig' to install the pyconfig files for all flavours.

You can customize your build with the following arguments:
""" +  opts.GenerateHelpText(dbgenv) 
)


# Check if we have to modify the compiler
def CheckICECCBuilder(context):
    context.Message('Checking for icecc compiler...')
    result = context.TryCompile('int main (int, char**){return 0;}', '.cpp')
    context.Result(result)
    return result
CXX = dbgenv['CXX']
confenv = dbgenv.Clone()
confenv.Replace(CXX ='icecc')
conf = Configure(confenv, custom_tests ={'CheckICECCBuilder': CheckICECCBuilder})

if conf.CheckICECCBuilder():
    CXX = 'icecc'

# Check for external libraries
externalLIBS = []
for lib in ['cppunit']:
    if conf.CheckLib(lib):
        externalLIBS.append(lib)
    else:
        print lib+ ' library missing'
        Exit(1)


for lib,alt in [('boost_program_options-mt', 'boost_program_options'),
                ('boost_signals-mt', 'boost_signals'),
                ('boost_date_time-mt', 'boost_date_time'),
                ('boost_filesystem-mt', 'boost_filesystem')]:
    if conf.CheckLib(lib):
        externalLIBS.append(lib)
    else:
        if conf.CheckLib(alt):
            externalLIBS.append(alt)
        else:
            print lib, ' and ', alt,' missing. At least one of them must be available.'
            Exit(1)

confenv = conf.Finish()

# Overwrite compiler from command line, if available
if ARGUMENTS.get('CXX'):
    CXX = ARGUMENTS.get('CXX')

allHeaders = []
includeDir = os.path.join(os.getcwd(),'.include')
libraries = []
for project in projects.all:
    if isinstance(project, (wnsbase.playground.Project.Root, wnsbase.playground.Project.SystemTest, wnsbase.playground.Project.Generic)):
        continue
    if isinstance(project, wnsbase.playground.Project.Library) and not isinstance(project, wnsbase.playground.Project.AddOn):
        libname,srcFiles,headers,pyconfigs,dependencies = SConscript(os.path.join(project.getDir(), 'config', 'libfiles.py'))
        if len(srcFiles) != 0:
            libraries.append(libname)
            # if we static link we also need to include the dependencies of the libraries
            # if we compile for a different flavour than debug, the following is also true
            if dbgenv['static']:
                libraries += dependencies

    if isinstance(project, wnsbase.playground.Project.Python):
        # here we install python files from pure Python projects
        # w. l. o. g. we can use the dbg environment here
        env = dbgenv
        env.SConscript(os.path.join(project.getDir(), 'SConscript'), exports='env')

    if project.includeBaseName is not None:
        libname,srcFiles,headers,pyconfigs,dependencies = SConscript(os.path.join(project.getDir(), 'config', 'libfiles.py'))
        headertargets = [header.replace('src/', '') for header in headers]
        InstallAs([os.path.join(includeDir, project.includeBaseName ,target) for target in headertargets],\
                  [os.path.join(project.getDir(), header) for header in headers])
        allHeaders += [os.path.join(project.getDir(), header) for header in headers]

# remove duplicates
libraries = list(set(libraries))
pyConfigDirs = []

for env in environments:
    env.Append(CPPPATH = ['#.include'])
    env.Append(LIBPATH = os.path.join(env['sandboxDir'], env.flavour, 'lib'))

    env.ParseConfig(envInfo.pythonConfigCmd + " --includes")

    env.Replace(CXX = CXX)
    env.installDir = os.path.join(env['sandboxDir'], env.flavour)
    env.includeDir = includeDir
    env['libraries'] = libraries
    env['externalLIBS'] = externalLIBS
    pylibs = env.ParseFlags(env.backtick(envInfo.pythonConfigCmd + ' --libs'))
    env['externalLIBS'] += pylibs['LIBS']
    if env['cacheDir']:
        env.CacheDir(env['cacheDir'])
    installDirs[env.flavour] = Dir(env.installDir)
    Alias(env.flavour, installDirs[env.flavour])
    pyConfigDirs.append(Alias(env.flavour + 'PyConfig', Dir(os.path.join(env.installDir, 'lib', 'PyConfig'))))

    if env['profile']:
        env.Append(CXXFLAGS = ['-pg', '-g'])
        env.Append(LINKFLAGS = '-pg')
    if env['smartPtrDBG']:
        env.Append(CPPDEFINES = {'WNS_SMARTPTR_DEBUGGING' : '1'})
    if env['callgrind']:
        env.Append(CPPDEFINES = {'CALLGRIND': '1'})
        env.Append(CXXFLAGS = ['-g'])
    if env['arch32']:
        env.Append(CXXFLAGS = '-m32')
        env.Append(LINKFLAGS = '-m32')
        env.Append(CPPDEFINES = {'__x86_32__': '1'})

    for project in projects.all:
        if isinstance(project, (wnsbase.playground.Project.Root, wnsbase.playground.Project.SystemTest, wnsbase.playground.Project.Generic, wnsbase.playground.Project.AddOn, wnsbase.playground.Project.Python)):
            continue
        buildDir = os.path.join(env['buildDir'], env.flavour, project.getRCSSubDir())
        env.BuildDir(buildDir, project.getDir(), duplicate = False)
        env.SConscript(os.path.join(buildDir, 'SConscript'), exports='env')
    
Alias('default', os.path.join(env['sandboxDir'], 'default'))
Default(installDirs['dbg'])
Alias('pyconfig', pyConfigDirs)

Command('tags', allHeaders , 'etags -o $TARGET $SOURCES')
