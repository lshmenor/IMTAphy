libname = 'projname'

srcFiles = [
    'src/ProjNameModule.cpp',
    'src/SimulationModel.cpp',
    ]

hppFiles = [
    'src/ProjNameModule.hpp',
    'src/SimulationModel.hpp',
    ]

pyconfigs = [
    'projname/__init__.py',
    'projname/simulationmodel.py',
]

dependencies = []
# Put in any external lib here as you would pass it to a -l compiler flag, e.g.
# dependencies = ['boost_date_time']
Return('libname srcFiles hppFiles pyconfigs dependencies')
