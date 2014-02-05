from openwns.evaluation import *

def installEvaluation(sim):

    sourceName = sim.memConsumptionProbeBusName
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node.appendChildren(TimeSeries())
    node.appendChildren(Moments())

    sourceName = sim.simTimeProbeBusName
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node.appendChildren(TimeSeries())
    node.appendChildren(Moments())

    sourceName = sim.cpuCyclesProbeBusName
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node.appendChildren(Moments())

def installPersVoIPSchedulerEvaluation(sim, settlingTime, loggingStations, stationCount):

    maxId = stationCount + len(loggingStations) + 2

    sourceName = 'scheduler.persistentvoip.FrameOccupationFairness'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node.getLeafs().appendChildren(SettlingTimeGuard(settlingTime = settlingTime))
    node = node.getLeafs().appendChildren(Accept(by='nodeID', ifIn = loggingStations, suffix='CenterCell'))
    node.appendChildren(Accept(by = 'Spot', ifIn = [1], suffix='DL'))
    node.appendChildren(Accept(by = 'Spot', ifIn = [2], suffix='UL'))
    node.getLeafs().appendChildren(PDF(name = sourceName,
                                     description = 'Frame Occupation Fairness',
                                     minXValue = 0,
                                     maxXValue = 1,
                                     resolution = 100))


    sourceName = 'scheduler.persistentvoip.failedReactivations'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node.getLeafs().appendChildren(SettlingTimeGuard(settlingTime = settlingTime))
    node = node.getLeafs().appendChildren(Accept(by='nodeID', ifIn = loggingStations, suffix='CenterCell'))
    node.appendChildren(Accept(by = 'Spot', ifIn = [1], suffix='DL'))
    node.appendChildren(Accept(by = 'Spot', ifIn = [2], suffix='UL'))
    node.getLeafs().appendChildren(PDF(name = sourceName,
                                     description = 'Failed Reactivations',
                                     minXValue = 0,
                                     maxXValue = 1,
                                     resolution = 100))
    node.getLeafs().appendChildren(Plot2D(xDataKey = 'schedUserID',
                                            minX = 0,
                                            maxX = maxId,
                                            resolution = maxId,
                                            statEvals = ['mean','trials']))

    sourceName = 'scheduler.persistentvoip.failedSetup'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node.getLeafs().appendChildren(SettlingTimeGuard(settlingTime = settlingTime))
    node = node.getLeafs().appendChildren(Accept(by='nodeID', ifIn = loggingStations, suffix='CenterCell'))
    node.appendChildren(Accept(by = 'Spot', ifIn = [1], suffix='DL'))
    node.appendChildren(Accept(by = 'Spot', ifIn = [2], suffix='UL'))
    node.getLeafs().appendChildren(PDF(name = sourceName,
                                     description = 'Failed Setups',
                                     minXValue = 0,
                                     maxXValue = 1,
                                     resolution = 100))
    node.getLeafs().appendChildren(Plot2D(xDataKey = 'schedUserID',
                                            minX = 0,
                                            maxX = maxId,
                                            resolution = maxId,
                                            statEvals = ['mean','trials']))

    sourceName = 'scheduler.persistentvoip.failedFreqRelocation'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node.getLeafs().appendChildren(SettlingTimeGuard(settlingTime = settlingTime))
    node = node.getLeafs().appendChildren(Accept(by='nodeID', ifIn = loggingStations, suffix='CenterCell'))
    node.appendChildren(Accept(by = 'Spot', ifIn = [1], suffix='DL'))
    node.appendChildren(Accept(by = 'Spot', ifIn = [2], suffix='UL'))
    node.getLeafs().appendChildren(PDF(name = sourceName,
                                     description = 'Failed Freq Relocations',
                                     minXValue = 0,
                                     maxXValue = 1,
                                     resolution = 100))
    node.getLeafs().appendChildren(Plot2D(xDataKey = 'schedUserID',
                                            minX = 0,
                                            maxX = maxId,
                                            resolution = maxId,
                                            statEvals = ['mean','trials']))

    sourceName = 'scheduler.persistentvoip.failedTimeRelocation'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node.getLeafs().appendChildren(SettlingTimeGuard(settlingTime = settlingTime))
    node = node.getLeafs().appendChildren(Accept(by='nodeID', ifIn = loggingStations, suffix='CenterCell'))
    node.appendChildren(Accept(by = 'Spot', ifIn = [1], suffix='DL'))
    node.appendChildren(Accept(by = 'Spot', ifIn = [2], suffix='UL'))
    node.getLeafs().appendChildren(PDF(name = sourceName,
                                     description = 'Failed Time Relocations',
                                     minXValue = 0,
                                     maxXValue = 1,
                                     resolution = 100))
    node.getLeafs().appendChildren(Plot2D(xDataKey = 'schedUserID',
                                            minX = 0,
                                            maxX = maxId,
                                            resolution = maxId,
                                            statEvals = ['mean','trials']))

    sourceName = 'scheduler.persistentvoip.failedTimeFreqRelocation'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node.getLeafs().appendChildren(SettlingTimeGuard(settlingTime = settlingTime))
    node = node.getLeafs().appendChildren(Accept(by='nodeID', ifIn = loggingStations, suffix='CenterCell'))
    node.appendChildren(Accept(by = 'Spot', ifIn = [1], suffix='DL'))
    node.appendChildren(Accept(by = 'Spot', ifIn = [2], suffix='UL'))
    node.getLeafs().appendChildren(PDF(name = sourceName,
                                     description = 'Need more from other frames',
                                     minXValue = 0,
                                     maxXValue = 1,
                                     resolution = 100))
    node.getLeafs().appendChildren(Plot2D(xDataKey = 'schedUserID',
                                            minX = 0,
                                            maxX = maxId,
                                            resolution = maxId,
                                            statEvals = ['mean','trials']))

    sourceName = 'scheduler.persistentvoip.failedDynamic'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node.getLeafs().appendChildren(SettlingTimeGuard(settlingTime = settlingTime))
    node = node.getLeafs().appendChildren(Accept(by='nodeID', ifIn = loggingStations, suffix='CenterCell'))
    node.appendChildren(Accept(by = 'Spot', ifIn = [1], suffix='DL'))
    node.appendChildren(Accept(by = 'Spot', ifIn = [2], suffix='UL'))
    node.getLeafs().appendChildren(PDF(name = sourceName,
                                     description = 'Failed Dynamic',
                                     minXValue = 0,
                                     maxXValue = 1,
                                     resolution = 100))
    node.getLeafs().appendChildren(Plot2D(xDataKey = 'schedUserID',
                                            minX = 0,
                                            maxX = maxId,
                                            resolution = maxId,
                                            statEvals = ['mean','trials']))

    sourceName = 'scheduler.persistentvoip.failedSID'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node.getLeafs().appendChildren(SettlingTimeGuard(settlingTime = settlingTime))
    node = node.getLeafs().appendChildren(Accept(by='nodeID', ifIn = loggingStations, suffix='CenterCell'))
    node.appendChildren(Accept(by = 'Spot', ifIn = [1], suffix='DL'))
    node.appendChildren(Accept(by = 'Spot', ifIn = [2], suffix='UL'))
    node.getLeafs().appendChildren(PDF(name = sourceName,
                                     description = 'Failed SID',
                                     minXValue = 0,
                                     maxXValue = 1,
                                     resolution = 100))
    node.getLeafs().appendChildren(Plot2D(xDataKey = 'schedUserID',
                                            minX = 0,
                                            maxX = maxId,
                                            resolution = maxId,
                                            statEvals = ['mean','trials']))

    sourceName = 'scheduler.persistentvoip.failedHARQ'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node.getLeafs().appendChildren(SettlingTimeGuard(settlingTime = settlingTime))
    node = node.getLeafs().appendChildren(Accept(by='nodeID', ifIn = loggingStations, suffix='CenterCell'))
    node.appendChildren(Accept(by = 'Spot', ifIn = [1], suffix='DL'))
    node.appendChildren(Accept(by = 'Spot', ifIn = [2], suffix='UL'))
    node.getLeafs().appendChildren(PDF(name = sourceName,
                                     description = 'Failed SID',
                                     minXValue = 0,
                                     maxXValue = 1,
                                     resolution = 100))
    node.getLeafs().appendChildren(Plot2D(xDataKey = 'schedUserID',
                                            minX = 0,
                                            maxX = maxId,
                                            resolution = maxId,
                                            statEvals = ['mean','trials']))

    sourceName = 'scheduler.persistentvoip.ActiveConnections'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node.getLeafs().appendChildren(SettlingTimeGuard(settlingTime = settlingTime))
    node = node.getLeafs().appendChildren(Accept(by='nodeID', ifIn = loggingStations, suffix='CenterCell'))
    node.appendChildren(Accept(by = 'Spot', ifIn = [1], suffix='DL'))
    node.appendChildren(Accept(by = 'Spot', ifIn = [2], suffix='UL'))
    node.getLeafs().appendChildren(PDF(name = sourceName,
                                     description = 'Active Connections',
                                     minXValue = 0,
                                     maxXValue = stationCount,
                                     resolution = stationCount))

    sourceName = 'scheduler.persistentvoip.QueuedConnections'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node.getLeafs().appendChildren(SettlingTimeGuard(settlingTime = settlingTime))
    node = node.getLeafs().appendChildren(Accept(by='nodeID', ifIn = loggingStations, suffix='CenterCell'))
    node.appendChildren(Accept(by = 'Spot', ifIn = [1], suffix='DL'))
    node.appendChildren(Accept(by = 'Spot', ifIn = [2], suffix='UL'))
    node.getLeafs().appendChildren(PDF(name = sourceName,
                                     description = 'Queued Connections',
                                     minXValue = 0,
                                     maxXValue = stationCount,
                                     resolution = stationCount))

    sourceName = 'scheduler.persistentvoip.AllActiveConnections'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node.getLeafs().appendChildren(SettlingTimeGuard(settlingTime = settlingTime))
    node = node.getLeafs().appendChildren(Accept(by='nodeID', ifIn = loggingStations, suffix='CenterCell'))
    node.appendChildren(Accept(by = 'Spot', ifIn = [1], suffix='DL'))
    node.appendChildren(Accept(by = 'Spot', ifIn = [2], suffix='UL'))
    node.getLeafs().appendChildren(PDF(name = sourceName,
                                     description = 'All Active Connections',
                                     minXValue = 0,
                                     maxXValue = stationCount,
                                     resolution = stationCount))

    sourceName = 'scheduler.persistentvoip.TimeRelocatedConnections'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node.getLeafs().appendChildren(SettlingTimeGuard(settlingTime = settlingTime))
    node = node.getLeafs().appendChildren(Accept(by='nodeID', ifIn = loggingStations, suffix='CenterCell'))
    node.appendChildren(Accept(by = 'Spot', ifIn = [1], suffix='DL'))
    node.appendChildren(Accept(by = 'Spot', ifIn = [2], suffix='UL'))
    node.getLeafs().appendChildren(PDF(name = sourceName,
                                     description = 'Time Relocated Connections',
                                     minXValue = 0,
                                     maxXValue = stationCount,
                                     resolution = stationCount))

    sourceName = 'scheduler.persistentvoip.FreqRelocatedConnections'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node.getLeafs().appendChildren(SettlingTimeGuard(settlingTime = settlingTime))
    node = node.getLeafs().appendChildren(Accept(by='nodeID', ifIn = loggingStations, suffix='CenterCell'))
    node.appendChildren(Accept(by = 'Spot', ifIn = [1], suffix='DL'))
    node.appendChildren(Accept(by = 'Spot', ifIn = [2], suffix='UL'))
    node.getLeafs().appendChildren(PDF(name = sourceName,
                                     description = 'Frequency Relocated Connections',
                                     minXValue = 0,
                                     maxXValue = stationCount,
                                     resolution = stationCount))

    sourceName = 'scheduler.persistentvoip.TimeFreqRelocatedConnections'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node.getLeafs().appendChildren(SettlingTimeGuard(settlingTime = settlingTime))
    node = node.getLeafs().appendChildren(Accept(by='nodeID', ifIn = loggingStations, suffix='CenterCell'))
    node.appendChildren(Accept(by = 'Spot', ifIn = [1], suffix='DL'))
    node.appendChildren(Accept(by = 'Spot', ifIn = [2], suffix='UL'))
    node.getLeafs().appendChildren(PDF(name = sourceName,
                                     description = 'Time & Frequency Relocated Connections',
                                     minXValue = 0,
                                     maxXValue = stationCount,
                                     resolution = stationCount))

    sourceName = 'scheduler.persistentvoip.ReservedRBs'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node.getLeafs().appendChildren(SettlingTimeGuard(settlingTime = settlingTime))
    node = node.getLeafs().appendChildren(Accept(by='nodeID', ifIn = loggingStations, suffix='CenterCell'))
    node.appendChildren(Accept(by = 'Spot', ifIn = [1], suffix='DL'))
    node.appendChildren(Accept(by = 'Spot', ifIn = [2], suffix='UL'))
    node.getLeafs().appendChildren(PDF(name = sourceName,
                                     description = 'Reserved RBs',
                                     minXValue = 0,
                                     maxXValue = 100,
                                     resolution = 100))

    sourceName = 'scheduler.persistentvoip.PersistentConnections'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node.getLeafs().appendChildren(SettlingTimeGuard(settlingTime = settlingTime))
    node = node.getLeafs().appendChildren(Accept(by='nodeID', ifIn = loggingStations, suffix='CenterCell'))
    node.appendChildren(Accept(by = 'Spot', ifIn = [1], suffix='DL'))
    node.appendChildren(Accept(by = 'Spot', ifIn = [2], suffix='UL'))
    node.getLeafs().appendChildren(PDF(name = sourceName,
                                     description = 'Reserved RBs',
                                     minXValue = 0,
                                     maxXValue = 100,
                                     resolution = 100))

    sourceName = 'scheduler.persistentvoip.NumberOfPDCCHs'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node.getLeafs().appendChildren(SettlingTimeGuard(settlingTime = settlingTime))
    node = node.getLeafs().appendChildren(Accept(by='nodeID', ifIn = loggingStations, suffix='CenterCell'))
    node.appendChildren(Accept(by = 'Spot', ifIn = [1], suffix='DL'))
    node.appendChildren(Accept(by = 'Spot', ifIn = [2], suffix='UL'))
    node.getLeafs().appendChildren(PDF(name = sourceName,
                                     description = 'Number of PDCCHs',
                                     minXValue = 0,
                                     maxXValue = 100,
                                     resolution = 100))

    sourceName = 'scheduler.persistentvoip.NumberOfPersRelocationPDCCHs'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node.getLeafs().appendChildren(SettlingTimeGuard(settlingTime = settlingTime))
    node = node.getLeafs().appendChildren(Accept(by='nodeID', ifIn = loggingStations, suffix='CenterCell'))
    node.appendChildren(Accept(by = 'Spot', ifIn = [1], suffix='DL'))
    node.appendChildren(Accept(by = 'Spot', ifIn = [2], suffix='UL'))
    node.getLeafs().appendChildren(PDF(name = sourceName,
                                     description = 'Number of Persistent Relocation PDCCHs',
                                     minXValue = 0,
                                     maxXValue = 100,
                                     resolution = 100))


    sourceName = 'scheduler.persistentvoip.NumberOfPersSetupPDCCHs'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node.getLeafs().appendChildren(SettlingTimeGuard(settlingTime = settlingTime))
    node = node.getLeafs().appendChildren(Accept(by='nodeID', ifIn = loggingStations, suffix='CenterCell'))
    node.appendChildren(Accept(by = 'Spot', ifIn = [1], suffix='DL'))
    node.appendChildren(Accept(by = 'Spot', ifIn = [2], suffix='UL'))
    node.getLeafs().appendChildren(PDF(name = sourceName,
                                     description = 'Number of Persistent Setup PDCCHs',
                                     minXValue = 0,
                                     maxXValue = 100,
                                     resolution = 100))
    

    sourceName = 'scheduler.persistentvoip.NumberOfPersSetupTimeRelocatedPDCCHs'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node.getLeafs().appendChildren(SettlingTimeGuard(settlingTime = settlingTime))
    node = node.getLeafs().appendChildren(Accept(by='nodeID', ifIn = loggingStations, suffix='CenterCell'))
    node.appendChildren(Accept(by = 'Spot', ifIn = [1], suffix='DL'))
    node.appendChildren(Accept(by = 'Spot', ifIn = [2], suffix='UL'))
    node.getLeafs().appendChildren(PDF(name = sourceName,
                                     description = 'Number of Persistent Setup PDCCHs from earlier frames',
                                     minXValue = 0,
                                     maxXValue = 100,
                                     resolution = 100))
   

    sourceName = 'scheduler.persistentvoip.NumberOfDynamicPDCCHs'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node.getLeafs().appendChildren(SettlingTimeGuard(settlingTime = settlingTime))
    node = node.getLeafs().appendChildren(Accept(by='nodeID', ifIn = loggingStations, suffix='CenterCell'))
    node.appendChildren(Accept(by = 'Spot', ifIn = [1], suffix='DL'))
    node.appendChildren(Accept(by = 'Spot', ifIn = [2], suffix='UL'))
    node.getLeafs().appendChildren(PDF(name = sourceName,
                                     description = 'Number of Dynamic PDCCHs',
                                     minXValue = 0,
                                     maxXValue = 100,
                                     resolution = 100))


    sourceName = 'scheduler.persistentvoip.NumberOfSID_PDCCHs'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node.getLeafs().appendChildren(SettlingTimeGuard(settlingTime = settlingTime))
    node = node.getLeafs().appendChildren(Accept(by='nodeID', ifIn = loggingStations, suffix='CenterCell'))
    node.appendChildren(Accept(by = 'Spot', ifIn = [1], suffix='DL'))
    node.appendChildren(Accept(by = 'Spot', ifIn = [2], suffix='UL'))
    node.getLeafs().appendChildren(PDF(name = sourceName,
                                     description = 'Number of SID PDCCHs',
                                     minXValue = 0,
                                     maxXValue = 100,
                                     resolution = 100))


    sourceName = 'scheduler.persistentvoip.NumberOfOtherFramePDCCHs'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node.getLeafs().appendChildren(SettlingTimeGuard(settlingTime = settlingTime))
    node = node.getLeafs().appendChildren(Accept(by='nodeID', ifIn = loggingStations, suffix='CenterCell'))
    node.appendChildren(Accept(by = 'Spot', ifIn = [1], suffix='DL'))
    node.appendChildren(Accept(by = 'Spot', ifIn = [2], suffix='UL'))
    node.getLeafs().appendChildren(PDF(name = sourceName,
                                     description = 'Number of other Frame PDCCHs',
                                     minXValue = 0,
                                     maxXValue = 100,
                                     resolution = 100))


    sourceName = 'scheduler.persistentvoip.NumberOfHARQ_PDCCHs'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node.getLeafs().appendChildren(SettlingTimeGuard(settlingTime = settlingTime))
    node = node.getLeafs().appendChildren(Accept(by='nodeID', ifIn = loggingStations, suffix='CenterCell'))
    node.appendChildren(Accept(by = 'Spot', ifIn = [1], suffix='DL'))
    node.appendChildren(Accept(by = 'Spot', ifIn = [2], suffix='UL'))
    node.getLeafs().appendChildren(PDF(name = sourceName,
                                     description = 'Number of HARQ PDCCHs',
                                     minXValue = 0,
                                     maxXValue = 100,
                                     resolution = 100))

    sourceName = 'scheduler.persistentvoip.dynPDUSize'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node.getLeafs().appendChildren(SettlingTimeGuard(settlingTime = settlingTime))
    node = node.getLeafs().appendChildren(Accept(by='nodeID', ifIn = loggingStations, suffix='CenterCell'))
    node.appendChildren(Accept(by = 'Spot', ifIn = [1], suffix='DL'))
    node.appendChildren(Accept(by = 'Spot', ifIn = [2], suffix='UL'))
    node.getLeafs().appendChildren(PDF(name = sourceName,
                                     description = 'Dynamic PDU Size',
                                     minXValue = 0,
                                     maxXValue = 2000,
                                     resolution = 1000))
    node.getLeafs().appendChildren(Plot2D(xDataKey = 'schedUserID',
                                            minX = 0,
                                            maxX = maxId,
                                            resolution = maxId,
                                            statEvals = ['mean','deviation','max']))

    sourceName = 'scheduler.persistentvoip.TBSize'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node.getLeafs().appendChildren(SettlingTimeGuard(settlingTime = settlingTime))
    node = node.getLeafs().appendChildren(Accept(by='nodeID', ifIn = loggingStations, suffix='CenterCell'))
    node.appendChildren(Accept(by = 'Spot', ifIn = [1], suffix='DL'))
    node.appendChildren(Accept(by = 'Spot', ifIn = [2], suffix='UL'))
    node.getLeafs().appendChildren(PDF(name = sourceName,
                                     description = 'TB Size',
                                     minXValue = 0,
                                     maxXValue = 100,
                                     resolution = 100))
    node = node.getLeafs().appendChildren(Plot2D(xDataKey = 'schedUserID',
                                            minX = 0,
                                            maxX = maxId,
                                            resolution = maxId,
                                            statEvals = ['mean','deviation','max']))
    node.appendChildren(Accept(by = 'Kind', ifIn = [0], suffix='Pers'))
    node.appendChildren(Accept(by = 'Kind', ifIn = [1], suffix='Dyn'))
    node.getLeafs().appendChildren(Plot2D(xDataKey = 'schedUserID',
                                            minX = 0,
                                            maxX = maxId,
                                            resolution = maxId,
                                            statEvals = ['mean','deviation','max']))
    node.getLeafs().appendChildren(PDF(name = sourceName,
                                     description = 'TB Size',
                                     minXValue = 0,
                                     maxXValue = 100,
                                     resolution = 100))
    node = node.getLeafs().appendChildren(Plot2D(xDataKey = 'schedUserID',
                                            minX = 0,
                                            maxX = maxId,
                                            resolution = maxId,
                                            statEvals = ['mean','deviation','max']))

