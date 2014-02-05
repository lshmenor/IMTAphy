####################
IMT-A WiMAX Scenario
####################

In this chapter you will create a WiMAX IMT-Advanced scenario

=============================
Changing an existing scenario
=============================

In the first experiment of the Scenario tutorial you got familiar with a WiMAX configuration in a simple one BS and two UTs deployment. The second experiment showed you how more complex multicellular scenarios are created. Now we bring both together.

make a copy of ``~/myOpenWNS/tests/system/wimac-tests/configOFDMA/config.py`` in the same directory and call it ``configUMi.py``. Change the ``CreatorPlacerBuilder`` to setup an IMT-A Urban Micro (UMi) scenario with 1 ring of interferers and 20 nodes. Change the channel bandwidth to 10MHz to comply to IMT-A evaluation criteria.

Run the configuration by executing ``../openwns -f configUMi.py`` and assure there are no errors. Finally use Pytree and the Wrowser Scenario Viewer to verify the setup. The UMi scenario has an inter site distance of 200m. Finally use the Scenario Viewer to get the max. SINR distribution. Set the resolution to 25m. This will take a long time. Is there something strange about the result?
