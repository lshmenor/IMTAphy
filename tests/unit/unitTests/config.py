# import the WNS module. Contains all sub-classes needed for
# configuration of WNS
import openwns
import os

moduleList = os.listdir(os.path.join("..","..","..","sandbox","dbg","lib","PyConfig"))

for module in moduleList:
	__import__(module)
		    
import openwns.node
# create an instance of the WNS configuration
# The variable must be called WNS!!!!
WNS = openwns.Simulator(simulationModel = openwns.node.NodeSimulationModel())
WNS.outputStrategy = openwns.simulator.OutputStrategy.DELETE
WNS.masterLogger.enabled = False

openwns.setSimulator(WNS)
