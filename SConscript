import os
Import('env')
libname,srcFiles,headers,pyconfigs,dependencies = SConscript(os.path.join('config','libfiles.py'))

pyconfigs = ['PyConfig/' + config for config in pyconfigs]

phyEnv = env.Clone()

try:
    addOnLibname,addOnSrcFiles,addOnHeaders,addOnPyconfigs,addOnDependencies = SConscript(os.path.join('addOn', 'config', 'libfiles.py'))
    srcFiles += ['addOn/' + sourceFile for sourceFile in addOnSrcFiles]
    pyconfigs += ['addOn/PyConfig/' + pyconfig for pyconfig in addOnPyconfigs]
    dependencies += addOnDependencies

except TypeError:
    pass

multiThreaded = True

# default is to compile 64 bit
# for 32 bit, set to True and make sure that the MKL variables are set appropriately, 
# e.g.:  . /opt/intel/mkl/bin/mklvars.sh ia32
# NOTE: 32 bit support is experimental and not thoroughly tested
compile32Bit = False


if(env.get('arch32')):
    print "--arch32 set, compiling 32bit"
    compile32Bit = True

# if the Intel MKL is installed on the system (the MKLROOT phyEnvironment variable has to 
# be exported as done by sourcing the mklvars64.sh shell script)
# link against MKL (this is a science of its own, see the MKL User's Guide, Chapter 5):

mklroot = ""
includePath = ""
libraryPath = ""

try:
    mklroot = os.environ['MKLROOT']
except:
    pass
# MKL / the Intel compiler define these environment variables:
try:
    includePath = os.environ['INCLUDE']
except:
    pass
try:
    libraryPath = os.environ['LIBRARY_PATH']
except:
    pass

if not os.path.exists(mklroot):
    print "MKL path not found, exiting. Please make sure the Intel Math Kernel library is installed and the MKLROOT environment variable is set "
    Exit(1)

externalLIBS = phyEnv['externalLIBS']
externalLIBS.append('itpp')

if compile32Bit:
    if 'mkl_intel' not in externalLIBS:
        externalLIBS.append('mkl_intel') # Interface Layer
else:
    if 'mkl_intel_lp64' not in externalLIBS:
        externalLIBS.append('mkl_intel_lp64') # Interface Layer

phyEnv.Append(CPPDEFINES = {'MKL' : '1'})
phyEnv.Append(CPPPATH = includePath.split(':'))
phyEnv.Append(LIBPATH = libraryPath.split(':'))

# without the -fno-ipa-cp-clone option, the optimized (-O3) release build will not work with GCC 4.6
# because the M2135<float> and M2135<double> symbols are undefined in imtaphy
phyEnv.Append(CXXFLAGS = ['-fno-ipa-cp-clone'] )

if multiThreaded:
    conf = Configure(phyEnv.Clone())
    if conf.CheckLib(['iomp5', 'pthread']): #iomp5 depends on pthreads, cannot be found without
        print "Intel openMP lib iomp5 found"
        phyEnv.Append(CXXFLAGS = ['-fopenmp'])

        if 'mkl_intel_thread' not in externalLIBS:
            externalLIBS.append('mkl_intel_thread') # Threading layer: use mkl_gnu_thread or mkl_intel_thread for multi-threaded, and mkl_sequential for single-threaded
        if 'mkl_core' not in externalLIBS:
            externalLIBS.append('mkl_core') # the computational library, this is enoguh for VML
        if 'iomp5' not in externalLIBS:
            externalLIBS.append('iomp5') # runtime must come before -lpthread
            
    else:
        if conf.CheckLib('gomp'):
            print "GNU openMP lib found"
            phyEnv.Append(CXXFLAGS = ['-fopenmp'])

            if 'mkl_gnu_thread' not in externalLIBS:
                externalLIBS.append('mkl_gnu_thread') # Threading layer: use mkl_gnu_thread or mkl_intel_thread for multi-threaded, and mkl_sequential for single-threaded
            if 'mkl_core' not in externalLIBS:
                externalLIBS.append('mkl_core') # the computational library, this is enoguh for VML
            if 'gomp' not in externalLIBS:
                externalLIBS.append('gomp')
        else:
            print "Neither Intel libiomp5 nor GNU openMP lib found; disabling multithreading"
            if 'mkl_sequential' not in externalLIBS:
                externalLIBS.append('mkl_sequential') # Threading layer
            if 'mkl_core' not in externalLIBS:
                externalLIBS.append('mkl_core') # the computational library, this is enoguh for VML
            multiThreaded = False 
    conf.Finish()
else:
    if 'mkl_sequential' not in externalLIBS:
        externalLIBS.append('mkl_sequential') # Threading layer
    if 'mkl_core' not in externalLIBS:
        externalLIBS.append('mkl_core') # the computational library, this is enoguh for VML


def appendToEnd(lib, libsSet, appendList):
    if lib in libsSet:
        appendList.append(lib)
        libsSet.discard(lib)
    return (libsSet, appendList)


# remove duplicates and move certain libs to the end in the specified order
# MKL is really picky with the order libs are linked on some systems
libsSet = set(externalLIBS)
appendLibs = []

for lib in ['mkl_intel_lp64', 'mkl_intel',  'mkl_intel_thread', 'mkl_gnu_thread', 'mkl_sequential', 'mkl_core', 'iomp5', 'gomp', 'pthread', 'm']:
    (libsSet, appendLibs) = appendToEnd(lib, libsSet, appendLibs)
#    print "appending "+lib+" to the end, now"+str(appendLibs)

externalLIBS = list(libsSet)
externalLIBS.extend(appendLibs)


# this does not seem to work, adding to LIBS = dependencies below does
phyEnv['externalLIBS'] = externalLIBS



if len(srcFiles) != 0:
    if phyEnv['static']:
        lib = phyEnv.StaticLibrary(libname, srcFiles)
    else:
        lib = phyEnv.SharedLibrary(libname, srcFiles, LIBS = dependencies + externalLIBS)
    phyEnv.Install(os.path.join(phyEnv.installDir, 'lib'), lib )

#preparingCampaign = ARGUMENTS.get('preparingcampaign', 0) 
#if preparingCampaign: # in case of preparecampaign, install pyconfigs as usual
for config in pyconfigs:
    phyEnv.InstallAs(os.path.join(phyEnv.installDir, 'lib', config.replace('addOn/', '')), config)



