from openwns.evaluation import *

probeNamePrefix = 'ltea.'

maxValues = {
    'compound': 50000, # max 50000 packets per second
    'bit': 3E8,        # max. 300 MBit/s throughput
    }



def installForDownlinkAndUplink(sim, node, child):
    ues = node.appendChildren(Accept(by = 'NodeType', ifIn = [1], suffix="DL"))
    ues.appendChildren(child)
        
    eNBs = node.appendChildren(Accept(by = 'NodeType', ifIn = [2], suffix="UL"))
    eNBs.appendChildren(child)
    
def installSystemTestProbes(sim, 
                            settlingTime = 0.0, 
                            users = 1, 
                            bandwidthDL = 1, 
                            bandwidthUL = 1, 
                            restrictToBSIds=None):

    # To distinguish between node types, check for 
    # NodeType  0==EPC, 1==UE, 2 == BS

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
    eNBs = time.appendChildren(Accept(by = 'NodeType', ifIn = [2], suffix="UL"))

    ues.appendChildren(blerPDF)
    eNBs.appendChildren(blerPDF)

    ues.appendChildren(Plot2D(xDataKey = 'mcsIndex',
                              minX = 0,
                              maxX = 29,
                              resolution = 29,
                              statEvals = ['trials', 'mean']))
    eNBs.appendChildren(Plot2D(xDataKey = 'mcsIndex',
                               minX = 0,
                               maxX = 29,
                               resolution = 29,
                               statEvals = ['trials', 'mean']))


    

    node = openwns.evaluation.createSourceNode(sim, "avgSINR")

    avgSINRpdf = PDF(name = "Post receiver linear avg. SINR in dB",
                     description = "Post receiver linear avg. SINR in dB",
                     minXValue = -15,
                     maxXValue = 65,
                     resolution = 800)
    installForDownlinkAndUplink(sim, node = node, child = avgSINRpdf)
    
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
    installForDownlinkAndUplink(sim, node = openwns.evaluation.createSourceNode(sim, "instantaneousSINR"), child = instSINRpdf)
                     
                     
    stddevSINRpdf = PDF(name = "Per user instantaneous SINR linear relative std.dev.",
                     description = 'Per user instantaneous SINR linear relative std.dev.',
                     minXValue = -30,
                     maxXValue = 30,
                     resolution = 600)
    node = openwns.evaluation.createSourceNode(sim, "stdDevSINR")
    installForDownlinkAndUplink(sim, node=node , child = stddevSINRpdf)
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
    installForDownlinkAndUplink(sim, node = node, child = avgIOTpdf)

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
    installForDownlinkAndUplink(sim, node = node , child = instIoTpdf)

   
                     
    stddevIoTpdf = PDF(name = "Per user instantaneous IoT linear relative std.dev.",
                     description = 'Per user instantaneous IoT linear relative std.dev.',
                     minXValue = -20,
                     maxXValue = 20,
                     resolution = 400)
    installForDownlinkAndUplink(sim, node = openwns.evaluation.createSourceNode(sim, "stdDevIoT"), child = stddevIoTpdf)
    


    # Link Adaptation
    node = openwns.evaluation.createSourceNode(sim, "laMismatch")
    laMismatch = PDF(name = "Actual effective SINR minus SINR estimated by link adaptation in dB",
                     description = "Actual effective SINR minus SINR estimated by link adaptation in dB",
                     minXValue = -20,
                     maxXValue = 20,
                     resolution = 400)
    settling = node.appendChildren(SettlingTimeGuard(settlingTime=settlingTime))
    sep = settling.appendChildren(Separate(by = "attempt", forAll = [1], format = "Attempt%d"))
    ues = sep.appendChildren(Accept(by = 'direction', ifIn = [0], suffix="DL"))
    ues.appendChildren(laMismatch)
    
    eNBs = sep.appendChildren(Accept(by = 'direction', ifIn = [1], suffix="UL"))
    eNBs.appendChildren(laMismatch)



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
                           minXValue = 0.0, maxXValue = 1.0, resolution=1000, scalingFactor=1/bandwidthUL))
    eNBs = node.appendChildren(Accept(by = 'NodeType', ifIn = [2], suffix="TotalCellNormalized_DL")) 
    eNBs.appendChildren(PDF(name = 'Total cell spectral efficiency downlink bit/s/Hz per cell', 
                            description = 'Total cell spectral efficiency downlink bit/s/Hz per cell',
                            minXValue = 0.0, maxXValue = 3.5, resolution=3500, scalingFactor=1/bandwidthDL))


    # Delay probe
    sourceName = 'ltea.packet.incoming.delay'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    time = node.appendChildren(SettlingTimeGuard(settlingTime=settlingTime))
    ues = time.appendChildren(Accept(by = 'NodeType', ifIn = [1], suffix="DL")) 
    ues.appendChildren(PDF(name = 'Packet delay downlink in seconds', description = 'Packet delay downlink in seconds', minXValue = 0.0, maxXValue = 0.25, resolution=1000, scalingFactor=1.0))
    bs = time.appendChildren(Accept(by = 'NodeType', ifIn = [2], suffix="UL")) 
    bs.appendChildren(PDF(name = 'Packet delay uplink in seconds', description = 'Packet delay uplink in seconds', minXValue = 0.0, maxXValue = 0.25, resolution=1000, scalingFactor=1.0))
