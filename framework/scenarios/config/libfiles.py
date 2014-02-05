libname = 'scenarios'

srcFiles = [
    ]

hppFiles = [
    ]

pyconfigs = [
    'scenarios/__init__.py',
    'scenarios/interfaces.py',
    'scenarios/antenna/__init__.py',
    'scenarios/antenna/isotropic.py',
    'scenarios/placer/__init__.py',
    'scenarios/placer/hexagonal.py',
    'scenarios/placer/circular.py',
    'scenarios/placer/rectangular.py',
    'scenarios/placer/linear.py',
    'scenarios/placer/positionList.py',
    'scenarios/placer/tests/__init__.py',
    'scenarios/placer/tests/placer.py',
    'scenarios/builders/__init__.py',
    'scenarios/builders/creatorplacer.py',
    'scenarios/traffic/__init__.py',
    'scenarios/simple/__init__.py',
    'scenarios/simple/singlecell.py',
    'scenarios/plotting/__init__.py',
    'scenarios/plotting/Plotting.py',
    'scenarios/ituM2135/__init__.py',
    'scenarios/ituM2135/antenna.py',
    'scenarios/ituM2135/placer.py',
    'scenarios/ituM2135/channelmodelcreator.py',
    'scenarios/ituM2135/creatorplacer.py',
    'scenarios/toolsupport/__init__.py',
    'scenarios/toolsupport/pytreevisitors/__init__.py',
    'scenarios/toolsupport/pytreevisitors/builders/__init__.py',
    'scenarios/toolsupport/pytreevisitors/builders/creatorplacer.py',
    'scenarios/binding/__init__.py',
    'scenarios/binding/binding.py',
    'scenarios/scenariosets/__init__.py',
    'scenarios/scenariosets/scenariosets.py',
    'scenarios/channelmodel/__init__.py',
    'scenarios/channelmodel/channelmodelcreator.py',
]

dependencies = []
# Put in any external lib here as you would pass it to a -l compiler flag, e.g.
# dependencies = ['boost_date_time']
Return('libname srcFiles hppFiles pyconfigs dependencies')
