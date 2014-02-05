libname = 'dllbase'
srcFiles = [
    'src/Module.cpp',
    'src/StationManager.cpp',
    'src/Layer2.cpp',
    'src/UpperConvergence.cpp',
    'src/RANG.cpp',

    # Services
    'src/services/control/Association.cpp',
    'src/services/management/InterferenceCache.cpp',
    'src/services/management/ESMFunc.cpp',

    # compoundFilter
    'src/compoundSwitch/CompoundSwitch.cpp',
    'src/compoundSwitch/CompoundSwitchConnector.cpp',
    'src/compoundSwitch/CompoundSwitchDeliverer.cpp',
    'src/compoundSwitch/Filter.cpp',
    'src/compoundSwitch/filter/FilterAll.cpp',
    'src/compoundSwitch/filter/FilterNone.cpp',
    'src/compoundSwitch/filter/FilterCommand.cpp',
    'src/compoundSwitch/filter/FilterName.cpp',
    'src/compoundSwitch/filter/FilterMACAddress.cpp',

    # Tests
    'src/tests/LayerStub.cpp',
    'src/tests/FUTestBase.cpp',
    'src/services/management/tests/InterferenceCacheTest.cpp',
]

hppFiles = [
'src/compoundSwitch/CompoundSwitchConfigCreator.hpp',
'src/compoundSwitch/CompoundSwitchConnector.hpp',
'src/compoundSwitch/CompoundSwitchDeliverer.hpp',
'src/compoundSwitch/CompoundSwitch.hpp',
'src/compoundSwitch/filter/FilterAll.hpp',
'src/compoundSwitch/filter/FilterCommand.hpp',
'src/compoundSwitch/filter/FilterMACAddress.hpp',
'src/compoundSwitch/filter/FilterName.hpp',
'src/compoundSwitch/filter/FilterNone.hpp',
'src/compoundSwitch/Filter.hpp',
'src/Layer2.hpp',
'src/Module.hpp',
'src/RANG.hpp',
'src/services/control/Association.hpp',
'src/services/management/InterferenceCache.hpp',
'src/services/management/ESMFunc.hpp',
'src/services/management/tests/InterferenceCacheTest.hpp',
'src/StationManager.hpp',
'src/tests/FUTestBase.hpp',
'src/tests/LayerStub.hpp',
'src/UpperConvergence.hpp',
]

pyconfigs = [
'dll/Services.py',
'dll/UpperConvergence.py',
'dll/CompoundSwitch.py',
'dll/Layer2.py',
'dll/DLL.py',
'dll/Association.py',
'dll/__init__.py',
]
dependencies = []
Return('libname srcFiles hppFiles pyconfigs dependencies')
