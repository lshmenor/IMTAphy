from openwns.evaluation import *

probeNamePrefix = 'ltea.'

maxValues = {
    'compound': 50000, # max 50000 packets per second
    'bit': 3E8,        # max. 300 MBit/s throughput
    }

def installChannelPlottingProbes(sim, numAntennaPairs, numPRBs, numTTIs, ueNodeIDs):
    
    
    table = Table(axis1 = 'TTI', minValue1 = 1, maxValue1 = numTTIs, resolution1 = numTTIs-1,
    axis2 = 'PRB', minValue2 = 0, maxValue2 = numPRBs, resolution2 = numPRBs,
    values = ['max', 'trials'], # should be only one value per bin, trials for emtpy ones
    formats = ['MatlabReadable'])

    node = openwns.evaluation.createSourceNode(sim, "channelGain")
    #ueSep = node.getLeafs().appendChildren(Separate(by = 'wns.node.Node.id', forAll = ueNodeIDs, format = "UE%d"))
    linkSep = node.getLeafs().appendChildren(Separate(by = 'scmLinkId',
                                            forAll = range(0,10),
                                            format = "scmLinkId%d"))
    antennaSep = linkSep.getLeafs().appendChildren(Separate(by = 'antennaPair',
                                               forAll = range(0,numAntennaPairs),
                                               format = "antennaPair%02d"))
    
    antennaSep.appendChildren(table)

def installForDownlinkAndUplink(node, child):
    ues = node.appendChildren(Accept(by = 'NodeType', ifIn = [1], suffix="DL"))
    ues.appendChildren(child)
        
    eNBs = node.appendChildren(Accept(by = 'NodeType', ifIn = [2], suffix="UL"))
    eNBs.appendChildren(child)
    
def installProbes(sim, 
                  settlingTime = 0.0,
                  numBSs = 1,
                  users = 1, 
                  bandwidthDL = 1, 
                  bandwidthUL = 1, 
                  restrictToBSIds=None, 
                  scenarioConfig = None,
                  probeConfig = None):

    # To distinguish between node types, check for 
    # NodeType  0==EPC, 1==UE, 2 == BS

    if scenarioConfig is not None: 
        scenarioPlottingTable = Table(axis1 = 'scenario.x', minValue1 = scenarioConfig.xMin,
                                      maxValue1 = scenarioConfig.xMax, resolution1 = probeConfig.xBins,
                                      axis2 = 'scenario.y', minValue2 = scenarioConfig.yMin,
                                      maxValue2 = scenarioConfig.yMax, resolution2 = probeConfig.yBins,
                                      values = ['max', 'mean', 'trials'], # trials is needed for fillValue
                                      formats = ['MatlabReadable']
                                     )
        scenarioPlottingTableMS = Table(axis1 = 'MSpos.x', minValue1 = scenarioConfig.xMin,
                                      maxValue1 = scenarioConfig.xMax, resolution1 = probeConfig.xBins,
                                      axis2 = 'MSpos.y', minValue2 = scenarioConfig.yMin,
                                      maxValue2 = scenarioConfig.yMax, resolution2 = probeConfig.yBins,
                                      values = ['max', 'mean', 'trials'], # trials is needed for fillValue
                                      formats = ['MatlabReadable']
                                     )
                                     
        serving =  openwns.evaluation.createSourceNode(sim, "servingBS")
#        serving.appendChildren(Logger())
        ues = serving.appendChildren(Accept(by = 'NodeType', ifIn = [1], suffix="DL"))
        ues.appendChildren(scenarioPlottingTable)

    
    node = openwns.evaluation.createSourceNode(sim, "outdoorPropagation")
    propagationPDF = PDF(name = "Outdoor propagation",
                         description = "Outdoor propagation NLoS=0, LoS=1, UMiO2I=2",
                         minXValue = 0,
                         maxXValue = 2,
                         resolution = 2)
    node.appendChildren(propagationPDF)
    servingSep = node.appendChildren(Separate(by = "Serving", forAll = [0,1], format = "Serving%d"))
    servingSep.appendChildren(propagationPDF)
#    BSSep = node.appendChildren(Separate(by = "BSID", forAll = range(1,57), format = "eNB%d"))
#    BSSep.appendChildren(propagationPDF)
    
    
    
    node = openwns.evaluation.createSourceNode(sim, "shadowing")
    if scenarioConfig is not None: 
        ues = node.appendChildren(Accept(by = 'NodeType', ifIn = [1], suffix="MS"))
        ues.appendChildren(scenarioPlottingTable)

    shadowingPDF = PDF(name = "Shadowing dB",
                         description = "Shadowing in dB",
                         minXValue = -30,
                         maxXValue = 30,
                         resolution = 60)
                         
    # the necessary context is provided by the base stations only
    eNBs = node.appendChildren(Accept(by = 'NodeType', ifIn = [2], suffix="BS"))                        
                         
    eNBs.appendChildren(shadowingPDF)
    servingSep = eNBs.appendChildren(Separate(by = "Serving", forAll = [0,1], format = "Serving%d"))
    servingSep.appendChildren(shadowingPDF)
    
    node = openwns.evaluation.createSourceNode(sim, "blockError")

    blerPDF = PDF(name = "Block error probability",
                           description = 'Block error probability',
                           minXValue = 0,
                           maxXValue = 1,
                           resolution = 10)
#
#    # this syntax (combination of appendChildren/getLeafs) lets only measurements after the settling time
#    # through and separates into 4 files
#    sep = node.appendChildren(Separate(by = "attempt", forAll = [1, 2, 3, 4], format = "Attempt%d")) # 1, 2, 3, 4
#    sep.appendChildren(SettlingTimeGuard(settlingTime=settlingTime))
#    ues = sep.appendChildren(Accept(by = 'NodeType', ifIn = [1], suffix="DL"))
#    eNBs = sep.appendChildren(Accept(by = 'NodeType', ifIn = [2], suffix="UL"))
#    ues.appendChildren(blerPDF)
#    eNBs.appendChildren(blerPDF)
    
#    sep2 = sep.appendChildren(Separate(by = "mcsIndex", forAll = range(29), format = "MCS%02d"))
#    ues = sep2.appendChildren(Accept(by = 'NodeType', ifIn = [1], suffix="DL"))
#    eNBs = sep2.appendChildren(Accept(by = 'NodeType', ifIn = [2], suffix="UL"))
#    ues.appendChildren(blerPDF)
#    eNBs.appendChildren(blerPDF)
    
    sep = node.appendChildren(Separate(by = "attempt", forAll = [1], format = "Attempt%d")) # 1, 2, 3, 4
    time = sep.appendChildren(SettlingTimeGuard(settlingTime=settlingTime))
    ues = time.appendChildren(Accept(by = 'NodeType', ifIn = [1], suffix="DL"))
    uesPerStream = ues.appendChildren(Separate(by= "stream", forAll = [0,1], format = "Stream%d")) # 0 and 1
    eNBs = time.appendChildren(Accept(by = 'NodeType', ifIn = [2], suffix="UL"))

    uesPerStream.appendChildren(blerPDF)
    eNBs.appendChildren(blerPDF)

    uesPerStream.appendChildren(Plot2D(xDataKey = 'mcsIndex',
                              minX = 0,
                              maxX = 29,
                              resolution = 29,
                              statEvals = ['trials', 'mean']))
    eNBs.appendChildren(Plot2D(xDataKey = 'mcsIndex',
                               minX = 0,
                               maxX = 29,
                               resolution = 29,
                               statEvals = ['trials', 'mean']))

    node = openwns.evaluation.createSourceNode(sim, "tbSize")
    tbSizesPdf = PDF(name = "Size of transport block in bit",
                     description = "Size of transport block in bit",
                     minXValue = 0,
                     maxXValue = 80000,
                     resolution = 5000)
    ues = node.appendChildren(Accept(by = 'NodeType', ifIn = [1], suffix="UL"))
    ues.appendChildren(tbSizesPdf)
    eNBs = node.appendChildren(Accept(by = 'NodeType', ifIn = [2], suffix="DL"))
    eNBs.appendChildren(tbSizesPdf)
                     
    installForDownlinkAndUplink(node = node, child = tbSizesPdf)


    node = openwns.evaluation.createSourceNode(sim, "avgSINR")
    
    if scenarioConfig is not None:
        installForDownlinkAndUplink(node = node, child = scenarioPlottingTableMS)
    
    avgSINRpdf = PDF(name = "Post receiver linear avg. SINR in dB",
                     description = "Post receiver linear avg. SINR in dB",
                     minXValue = -15,
                     maxXValue = 65,
                     resolution = 800)
    installForDownlinkAndUplink(node = node, child = avgSINRpdf)
    
    avgPostFdeSINRpdf = PDF(name = "Uplink post MMSE-FDE linear avg. SINR in dB",
                     description = "Uplink post MMSE-FDE linear avg. SINR in dB",
                     minXValue = -15,
                     maxXValue = 65,
                     resolution = 800)
    node = openwns.evaluation.createSourceNode(sim,"avgUplinkPostFDESINR")
    node.appendChildren(avgPostFdeSINRpdf)

    instSINRpdf = PDF(name = "Post receiver instantaneous SINR in dB",
                     description = 'Post receiver instantaneous SINR in dB',
                     minXValue = -20,
                     maxXValue = 60,
                     resolution = 400)
    node = openwns.evaluation.createSourceNode(sim, "instantaneousSINR")
#    installForDownlinkAndUplink(node, child = instSINRpdf)
    ues = node.appendChildren(Accept(by = 'NodeType', ifIn = [1], suffix="DL"))
    ues.appendChildren(instSINRpdf)
    layer = ues.appendChildren(Separate(by = "Layer", forAll = [1,2,3,4], format = "Layer%d"))
    layer.appendChildren(instSINRpdf)
    eNBs = node.appendChildren(Accept(by = 'NodeType', ifIn = [2], suffix="UL"))
    eNBs.appendChildren(instSINRpdf)
 
                     
    stddevSINRpdf = PDF(name = "Per user instantaneous SINR linear relative std.dev.",
                     description = 'Per user instantaneous SINR linear relative std.dev.',
                     minXValue = 0,
                     maxXValue = 30,
                     resolution = 600)
    node = openwns.evaluation.createSourceNode(sim, "stdDevSINR")
    installForDownlinkAndUplink(node=node , child = stddevSINRpdf)
    if scenarioConfig is not None:
        installForDownlinkAndUplink(node = node, child = scenarioPlottingTableMS)

    #linkSep = node.getLeafs().appendChildren(Separate(by = 'AvgSINR',
                                            #forAll = range(5,18),
                                            #format = "avgSINR%d"))
    #ues = linkSep.getLeafs().appendChildren(Accept(by = 'NodeType', ifIn = [2], suffix="UL"))
    #ues.appendChildren(stddevSINRpdf)
    
    avgIOTpdf = PDF(name = "Post receiver linear avg. IoT in dB",
                     description = "Post receiver linear avg. IoT in dB",
                     minXValue = 0,
                     maxXValue = 60,
                     resolution = 600)
    node = openwns.evaluation.createSourceNode(sim, "avgIoT")
    installForDownlinkAndUplink(node = node, child = avgIOTpdf)
    
    if scenarioConfig is not None:
        installForDownlinkAndUplink(node = node, child = scenarioPlottingTableMS)
    

    #node.appendChildren(Logger())
    #linkSep = node.getLeafs().appendChildren(Separate(by = 'AvgSINR',
                                            #forAll = range(5,18),
                                            #format = "avgSINR%d"))
    #ues = linkSep.getLeafs().appendChildren(Accept(by = 'NodeType', ifIn = [2], suffix="UL"))
    #ues.appendChildren(avgIOTpdf)
    
    instIoTpdf = PDF(name = "Post receiver instantaneous IoT in dB",
                     description = 'Post receiver instantaneous IoT in dB',
                     minXValue = -20,
                     maxXValue = 60,
                     resolution = 400)
    node = openwns.evaluation.createSourceNode(sim, "instantaneousIoT")
    installForDownlinkAndUplink(node = node , child = instIoTpdf)

   
                     
    stddevIoTpdf = PDF(name = "Per user instantaneous IoT linear relative std.dev.",
                     description = 'Per user instantaneous IoT linear relative std.dev.',
                     minXValue = 0,
                     maxXValue = 20,
                     resolution = 400)
    node = openwns.evaluation.createSourceNode(sim, "stdDevIoT")
    installForDownlinkAndUplink(node = node, child = stddevIoTpdf)
    if scenarioConfig is not None:
        installForDownlinkAndUplink(node = node, child = scenarioPlottingTableMS)



    # Link Adaptation
    node = openwns.evaluation.createSourceNode(sim, "laMismatch")
    laMismatch = PDF(name = "Actual effective SINR minus SINR estimated by link adaptation in dB",
                     description = "Actual effective SINR minus SINR estimated by link adaptation in dB",
                     minXValue = -20,
                     maxXValue = 20,
                     resolution = 400)
    settling = node.appendChildren(SettlingTimeGuard(settlingTime=settlingTime))
    sep = settling.appendChildren(Separate(by = "attempt", forAll = [1], format = "Attempt%d"))
    stream = sep.appendChildren(Separate(by = "stream", forAll = [0,1], format = "Stream%d"))
    ues = stream.appendChildren(Accept(by = 'direction', ifIn = [0], suffix="DL"))
    ues.appendChildren(laMismatch)
    eNBs = stream.appendChildren(Accept(by = 'direction', ifIn = [1], suffix="UL"))
    eNBs.appendChildren(laMismatch)
    
    # SINR Estimation vs. actual transmission
    node = openwns.evaluation.createSourceNode(sim, "SINRestimationMismatch")
    sinrMismatch = PDF(name = "Actual lin. avg. SINR minus lin. avg. SINR estimated before transmission in dB",
                     description = "Actual lin. avg. SINR minus lin. avg. SINR estimated before transmission in dB",
                     minXValue = -20,
                     maxXValue = 20,
                     resolution = 400)
    time = node.appendChildren(SettlingTimeGuard(settlingTime=settlingTime))
    installForDownlinkAndUplink(node = time, child = sinrMismatch)
    

    node = openwns.evaluation.createSourceNode(sim, "wbl")
    wblPdf = PDF(name = "wbl",
                 description = 'Coupling loss in dB',
                 minXValue = -160,
                 maxXValue = -40,
                 resolution = 240)
    node.appendChildren(wblPdf)
    


    # Spectral efficiency: scale throughput by bandwidth
    # incoming should only be recorded at the UEs
    sourceName = probeNamePrefix +  'total.window.incoming.bitThroughput'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    ues = node.appendChildren(Accept(by = 'NodeType', ifIn = [1], suffix="PerUserNormalized_DL"))  #UEs have id 6 and eNBs have id 5
    #ues.appendChildren(Logger())
    ues.appendChildren(PDF(name = 'Per user spectral efficiency downlink in bit/s/Hz per user', 
                           description = 'Per user spectral efficiency downlink in bit/s/Hz per user',
                           minXValue = 0.0, maxXValue = 1.0, resolution=1000, scalingFactor=1/bandwidthDL))
    eNBs = node.appendChildren(Accept(by = 'NodeType', ifIn = [2], suffix="TotalCellNormalized_UL"))  #UEs have id 6 and eNBs have id 5
    #ues.appendChildren(Logger())
    eNBs.appendChildren(PDF(name = 'Total cell spectral efficiency uplink bit/s/Hz per cell', 
                            description = 'Total cell spectral efficiency uplink bit/s/Hz per cell',
                            minXValue = 0.0, maxXValue = 2.5, resolution=2500, scalingFactor=1/bandwidthUL))

    

    # in the UL, we want aggregated throughput to differentiate between transmitting UEs
    sourceName = probeNamePrefix +  'total.window.aggregated.bitThroughput'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    ues = node.appendChildren(Accept(by = 'NodeType', ifIn = [1], suffix="PerUserNormalized_UL")) 
    ues.appendChildren(PDF(name = 'Per user spectral efficiency uplink in bit/s/Hz per user', 
                           description = 'Per user spectral efficiency uplink in bit/s/Hz per user', 
                           minXValue = 0.0, maxXValue = 1.0, resolution=500, scalingFactor=1/bandwidthUL))
    eNBs = node.appendChildren(Accept(by = 'NodeType', ifIn = [2], suffix="TotalCellNormalized_DL")) 
    eNBs.appendChildren(PDF(name = 'Total cell spectral efficiency downlink bit/s/Hz per cell', 
                            description = 'Total cell spectral efficiency downlink bit/s/Hz per cell',
                            minXValue = 0.0, maxXValue = 3.5, resolution=1750, scalingFactor=1/bandwidthDL))

    # provide outgoing/offered traffic probing
    sourceName = probeNamePrefix +  'total.window.outgoing.bitThroughput'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    eNBs = node.appendChildren(Accept(by = 'NodeType', ifIn = [2], suffix="PerBSNormalized_DL")) 
    eNBs.appendChildren(PDF(name = 'Normalized outgoing downlink (offered) traffic in bit/s/Hz per base station', 
                           description = 'Normalized outgoing downlink (offered) traffic in bit/s/Hz per base station', 
                           minXValue = 0.0, maxXValue = 2.0*users, resolution=100, scalingFactor=1/bandwidthDL))                                


    ues = node.appendChildren(Accept(by = 'NodeType', ifIn = [1], suffix="PerMSNormalized_UL")) 
    ues.appendChildren(PDF(name = 'Normalized outgoing uplink (offered) traffic in bit/s/Hz per mobile station', 
                           description = 'Normalized outgoing uplink (offered) traffic in bit/s/Hz per mobile station', 
                           minXValue = 0.0, maxXValue = 2.0, resolution=100, scalingFactor=1/bandwidthUL))                                
                           
                           
    # Delay probe
    sourceName = 'ltea.packet.incoming.delay'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    time = node.appendChildren(SettlingTimeGuard(settlingTime=settlingTime))
    ues = time.appendChildren(Accept(by = 'NodeType', ifIn = [1], suffix="DL")) 
    ues.appendChildren(PDF(name = 'Packet delay downlink in seconds', description = 'Packet delay downlink in seconds', minXValue = 0.0, maxXValue = 0.25, resolution=1000, scalingFactor=1.0))
    bs = time.appendChildren(Accept(by = 'NodeType', ifIn = [2], suffix="UL")) 
    bs.appendChildren(PDF(name = 'Packet delay uplink in seconds', description = 'Packet delay uplink in seconds', minXValue = 0.0, maxXValue = 0.25, resolution=1000, scalingFactor=1.0))

    node = openwns.evaluation.createSourceNode(sim, "rank")
    rankPDF = PDF(name = "Rank",
                 description = 'Rank used for downlink transmission',
                 minXValue = 1,
                 maxXValue = 4,
                 resolution = 3)
    ues = node.appendChildren(Accept(by = 'NodeType', ifIn = [2], suffix="DL")) # DL is probed from eNBs
    ues.appendChildren(rankPDF)
                 
                 
    node = openwns.evaluation.createSourceNode(sim, "feedback")    
    table = Table(axis1 = 'PMI', minValue1 = 0, maxValue1 = 32+1, resolution1 = 33,
                  axis2 = 'RI',  minValue2 = 1, maxValue2 = 4+1, resolution2 = 4,
                  values = ['mean', 'trials'], 
                  formats = ['MatlabReadable'])
    node.appendChildren(table)

#    ues.appendChildren(table)
    
#     node = openwns.evaluation.createSourceNode(sim, "laProbingActualSINR")
#     node.appendChildren(Accept(by = 'NodeType', ifIn = [2], suffix="UL"))
#     node.appendChildren(Accept(by = 'NodeType', ifIn = [1], suffix="DL"))
#     bsSep = node.getLeafs().appendChildren(Separate(by = "MSID", forAll = range(numBSs + 1, numBSs + 5), format = "UE%d"))
#     prbSep = bsSep.appendChildren(Separate(by = "PRB", forAll = range(0,10), format = "PRB%d"))
#     prbSep.appendChildren(TimeSeries())
    
#     node = openwns.evaluation.createSourceNode(sim, "laProbingEstimatedSINR")
#     node.appendChildren(Accept(by = 'NodeType', ifIn = [2], suffix="UL"))
#     node.appendChildren(Accept(by = 'NodeType', ifIn = [1], suffix="DL"))
#     bsSep = node.getLeafs().appendChildren(Separate(by = "MSID", forAll = range(numBSs + 1, numBSs + 5), format = "UE%d"))
#     prbSep = bsSep.appendChildren(Separate(by = "PRB", forAll = range(0,10), format = "PRB%d"))
#     prbSep.appendChildren(TimeSeries())
    
    #node = openwns.evaluation.createSourceNode(sim, "uplinkChannelVariations")
    #node.appendChildren(Accept(by = 'NodeType', ifIn = [2], suffix="UL"))
    #bsSep = node.getLeafs().appendChildren(Separate(by = "MSID", forAll = range(numBSs + 1, numBSs + 5), format = "UE%d"))
    #prbSep = bsSep.appendChildren(Separate(by = "PRB", forAll = range(0,10), format = "PRB%d"))
    #prbSep.appendChildren(TimeSeries())
    
