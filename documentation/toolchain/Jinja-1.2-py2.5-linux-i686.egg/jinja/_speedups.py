def __bootstrap__():
   global __bootstrap__, __loader__, __file__
   import sys, pkg_resources, imp
   __file__ = pkg_resources.resource_filename(__name__,'_speedups.so')
   del __bootstrap__

   # Somehow __loader__ is undefined when butler builds the documentation
   # So do not delete it in that case
   try:
	del  __loader__
   except:
	pass

   imp.load_dynamic(__name__,__file__)
__bootstrap__()
