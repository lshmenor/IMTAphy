# -*- coding: utf-8 -*-
################################################################################
# This file is part of IMTAphy
# _____________________________________________________________________________
#
# Copyright (C) 2011
# Institute of Communication Networks (LKN)
# Department of Electrical Engineering and Information Technology (EE & IT)
# Technische Universitaet Muenchen
# Arcisstr. 21
# 80333 Muenchen - Germany
# http://www.lkn.ei.tum.de/~jan/imtaphy/index.html
# 
# _____________________________________________________________________________
#
#   IMTAphy is free software: you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   IMTAphy is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with IMTAphy.  If not, see <http://www.gnu.org/licenses/>.
#
#################################################################################

libname = 'imtaphy'

srcFiles = [
    'src/IMTAphyModule.cpp',
    'src/StationPhy.cpp',
    'src/Channel.cpp',
    'src/Transmission.cpp',
    'src/Spectrum.cpp',
    'src/pathloss/M2135Pathloss.cpp',
    'src/pathloss/No.cpp',
    'src/pathloss/SingleSlope.cpp',
    'src/Link.cpp',
    
    'src/linkManagement/LinkManager.cpp',
    'src/linkManagement/classifier/StaticClassifier.cpp',
    'src/linkManagement/classifier/ITUClassifier.cpp',
    
    'src/receivers/LinearReceiver.cpp',
    'src/receivers/ProbingReceiver.cpp',
    
    'src/receivers/filters/MMSE.cpp',
    'src/receivers/filters/MRC.cpp',
    'src/receivers/filters/NoFilter.cpp',
    'src/receivers/channelEstimation/covariance/PerfectIandNCovariance.cpp',
    'src/receivers/channelEstimation/covariance/DiagonalIandNCovariance.cpp',
    'src/receivers/channelEstimation/covariance/EqualDiagonalIandNCovariance.cpp',
    'src/receivers/channelEstimation/covariance/GaussianErrorIandNCovariance.cpp',
    'src/receivers/channelEstimation/covariance/IntraAndInterCellDistinguisher.cpp',
    'src/receivers/channelEstimation/covariance/WishartEstimationModel36_829.cpp',

    
    'src/receivers/channelEstimation/channel/ThermalNoiseBasedGaussianError.cpp',
    'src/receivers/channelEstimation/channel/IandNCovarianceBased.cpp',

   
    'src/receivers/tests/LteRel8CodebookTest.cpp',
#    'src/receivers/tests/MMSEReceiverTest.cpp',
    'src/receivers/tests/LinearReceiverTest.cpp',
    'src/receivers/tests/MRCReceiverTest.cpp',

    'src/receivers/feedback/LteRel8DLFeedbackManager.cpp',
    'src/receivers/feedback/LteRel10UplinkChannelStatusManager.cpp',
    'src/receivers/feedback/PU2RCFeedbackManager.cpp',
    'src/receivers/feedback/FixedPMIPRBFeedbackManager.cpp',

   
    'src/scanner/LteSender.cpp',
    'src/scanner/LteScanner.cpp',
    'src/scanner/ChannelDumper.cpp',
    
    'src/antenna/LinearAntennaArray.cpp',
    'src/antenna/AntennaITU.cpp',
    'src/antenna/Omnidirectional.cpp',
    
    'src/spatialChannel/No.cpp',
    'src/spatialChannel/m2135/M2135.cpp',
    'src/spatialChannel/m2135/ClusterPowers.cpp',
    
    'src/lsParams/LSCorrelation.cpp',
    
    'src/spatialChannel/m2135/RayAngles.cpp',
    'src/spatialChannel/m2135/tests/M2135Test.cpp',
    'src/spatialChannel/m2135/tests/ClusterPowersTest.cpp',
    'src/spatialChannel/m2135/tests/LSCorrelationTest.cpp',
    'src/spatialChannel/m2135/tests/RayAnglesTest.cpp',
    'src/spatialChannel/m2135/tests/AllScenariosTest.cpp',
    'src/spatialChannel/m2135/Delays.cpp',
    'src/spatialChannel/m2135/tests/DelaysTest.cpp',
    
    'src/tests/ChannelStub.cpp',
    'src/tests/StationPhyStub.cpp',
    'src/tests/AnglesTest.cpp',
    
    'src/pathloss/tests/PathlossTest.cpp',
    
    'src/detail/HashRNG.cpp',
    'src/detail/LinearAlgebra.cpp',
    
    'src/detail/tests/LookupTableTest.cpp',
    'src/detail/tests/InterpolationTest.cpp',
    'src/detail/tests/LinearAlgebraTest.cpp',

    
    'src/link2System/Modulations.cpp',
    'src/link2System/MMIBeffectiveSINR.cpp',
    'src/link2System/BlockErrorModel.cpp',

    'src/link2System/tests/MMIBeffectiveSINRTest.cpp',
    'src/link2System/tests/BlockErrorModelTest.cpp',


##### LTEA files #####

    'src/ltea/rlc/SegmentingQueue.cpp',
    'src/ltea/rlc/FullQueue.cpp',
    'src/ltea/rlc/UnacknowledgedMode.cpp',
    'src/ltea/rlc/RadioBearerID.cpp',
    
    'src/ltea/pdcp/eNB.cpp',
    'src/ltea/pdcp/UE.cpp',
    
    'src/ltea/helper/Window.cpp',
    
    'src/ltea/EPCgw.cpp',
    'src/ltea/Layer2.cpp',

    'src/ltea/l2s/harq/ChaseCombiningDecoder.cpp',
    'src/ltea/l2s/PhyInterfaceRx.cpp',
    
    'src/ltea/mac/scheduler/ResourceManagerBase.cpp',
    'src/ltea/mac/scheduler/downlink/ProportionalFair.cpp',
    'src/ltea/mac/scheduler/downlink/RoundRobin.cpp',
    'src/ltea/mac/scheduler/downlink/SchedulerBase.cpp',
    'src/ltea/mac/scheduler/downlink/ZFScheduler.cpp',
    'src/ltea/mac/scheduler/downlink/PU2RCScheduler.cpp',
    

    'src/ltea/mac/scheduler/uplink/eNB/RoundRobin.cpp',
    'src/ltea/mac/scheduler/uplink/eNB/SchedulerBase.cpp',
    'src/ltea/mac/scheduler/uplink/UEScheduler.cpp',

    'src/ltea/mac/tests/PerformanceModelTest.cpp',

    'src/ltea/mac/harq/HARQentity.cpp',
    'src/ltea/mac/harq/HARQ.cpp',
    'src/ltea/mac/harq/HARQReceiverProcess.cpp',
    'src/ltea/mac/harq/HARQSenderProcess.cpp',

    'src/ltea/mac/ModulationAndCodingSchemes.cpp',

    'src/ltea/mac/linkAdaptation/downlink/SINRThresholdLA.cpp',
    'src/ltea/mac/linkAdaptation/downlink/BlerAdaptiveLA.cpp',
    'src/ltea/mac/linkAdaptation/uplink/SINRThresholdLA.cpp',
    'src/ltea/mac/linkAdaptation/uplink/LinkAdaptationBase.cpp',
    'src/ltea/mac/linkAdaptation/uplink/AdaptiveLA.cpp',
    ]

hppFiles = [
    'src/linkManagement/classifier/StaticClassifier.hpp',
    'src/linkManagement/classifier/LinkClassifierInterface.hpp',
    'src/linkManagement/classifier/ITUClassifier.hpp',
    'src/linkManagement/LinkManager.hpp',
    'src/pathloss/No.hpp',
    'src/pathloss/PathlossModelInterface.hpp',
    'src/pathloss/SingleSlope.hpp',
    'src/pathloss/M2135Pathloss.hpp',
    'src/Link.hpp',
    'src/Channel.hpp',
    'src/StationPhy.hpp',
    'src/Transmission.hpp',
    'src/scanner/ChannelDumper.hpp',
    'src/scanner/LteSender.hpp',
    'src/scanner/LteScanner.hpp',
    'src/spatialChannel/m2135/Delays.hpp',
    'src/spatialChannel/m2135/LinkPar.hpp',
    'src/spatialChannel/m2135/ClusterPowers.hpp',
    'src/spatialChannel/m2135/Phases.hpp',
    'src/spatialChannel/m2135/M2135.hpp',
    'src/spatialChannel/m2135/RayAngles.hpp',
    'src/spatialChannel/m2135/FixPar.hpp',
    'src/spatialChannel/No.hpp',
    'src/spatialChannel/SpatialChannelModelInterface.hpp',
    'src/antenna/AntennaITU.hpp',
    'src/antenna/AntennaInterface.hpp',
    'src/antenna/Omnidirectional.hpp',
    'src/antenna/LinearAntennaArray.hpp',
    'src/tests/ChannelStub.hpp',
    'src/tests/StationPhyStub.hpp',
    'src/IMTAphyModule.hpp',
    'src/Spectrum.hpp',
    'src/ltea/rlc/IQueue.hpp',
    'src/ltea/rlc/FullQueue.hpp',
    'src/ltea/rlc/SegmentingQueue.hpp',
    'src/ltea/rlc/UnacknowledgedMode.hpp',
    'src/ltea/rlc/RadioBearerID.hpp',
    'src/ltea/pdcp/PDCPCommand.hpp',
    'src/ltea/pdcp/UE.hpp',
    'src/ltea/pdcp/eNB.hpp',
    'src/ltea/lteaModule.hpp',
    'src/ltea/Layer2.hpp',
    'src/ltea/helper/Types.hpp',
    'src/ltea/helper/Window.hpp',
    'src/ltea/l2s/PhyInterfaceRx.hpp',
    'src/ltea/l2s/harq/ChaseCombiningDecoder.hpp',
    'src/ltea/l2s/harq/DecoderInterface.hpp',
    'src/ltea/mac/scheduler/downlink/ProportionalFair.hpp',
    'src/ltea/mac/scheduler/downlink/SchedulerBase.hpp',
    'src/ltea/mac/scheduler/downlink/RoundRobin.hpp',
    'src/ltea/mac/scheduler/UsersPRBManager.hpp',
    'src/ltea/mac/scheduler/uplink/eNB/SchedulerBase.hpp',
    'src/ltea/mac/scheduler/uplink/eNB/RoundRobin.hpp',
    'src/ltea/mac/scheduler/uplink/UEScheduler.hpp',
    'src/ltea/mac/scheduler/ResourceManagerInterface.hpp',
    'src/ltea/mac/DCI.hpp',
    'src/ltea/mac/tests/PerformanceModelTest.hpp',
    'src/ltea/mac/harq/HARQReceiverProcess.hpp',
    'src/ltea/mac/harq/HARQ.hpp',
    'src/ltea/mac/harq/HARQSenderProcess.hpp',
    'src/ltea/mac/harq/HARQentity.hpp',
    'src/ltea/mac/ModulationAndCodingSchemes.hpp',
    'src/ltea/mac/linkAdaptation/downlink/LinkAdaptationInterface.hpp',
    'src/ltea/mac/linkAdaptation/downlink/SINRThresholdLA.hpp',
    'src/ltea/mac/linkAdaptation/downlink/BlerAdaptiveLA.hpp',
    'src/ltea/mac/linkAdaptation/uplink/LinkAdaptationInterface.hpp',
    'src/ltea/mac/linkAdaptation/uplink/SINRThresholdLA.hpp',
    'src/ltea/mac/linkAdaptation/uplink/LinkAdaptationBase.hpp',
    'src/ltea/mac/linkAdaptation/uplink/AdaptiveLA.hpp',
    'src/ltea/EPCgw.hpp',
    'src/lsParams/LargeScaleParameters.hpp',
    'src/lsParams/LSCorrelation.hpp',
    'src/lsParams/RngMock.hpp',
    'src/link2System/EffectiveSINRModelInterface.hpp',
    'src/link2System/MMIBeffectiveSINR.hpp',
    'src/link2System/BlockErrorModel.hpp',
    'src/link2System/Modulations.hpp',
    'src/link2System/MMSE-FDE.hpp',
    'src/link2System/LteCQIs.hpp',
    'src/detail/LinearAlgebra.hpp',
    'src/detail/NodePtrCompare.hpp',
    'src/detail/LookupTable.hpp',
    'src/detail/HashRNG.hpp',
    'src/detail/Interpolation.hpp',
    'src/interface/DataReception.hpp',
    'src/interface/IMTAphyObserver.hpp',
    'src/interface/TransmissionStatus.hpp',
    'src/interface/DataTransmission.hpp',
    'src/ChannelModuleCreator.hpp',
    'src/receivers/filters/MMSE.hpp',
    'src/receivers/filters/MRC.hpp',
    'src/receivers/filters/NoFilter.hpp',
    'src/receivers/filters/FilterInterface.hpp',
    'src/receivers/feedback/LteRel10UplinkChannelStatusManager.hpp',
    'src/receivers/feedback/LteFeedback.hpp',
    'src/receivers/feedback/LteRel8DLFeedbackManager.hpp',
    'src/receivers/feedback/FixedPMIPRBFeedbackManager.hpp',
    'src/receivers/LteRel8Codebook.hpp',
    'src/receivers/Interferer.hpp',
    'src/receivers/channelEstimation/covariance/NoiseAndInterferenceCovarianceBase.hpp',
    'src/receivers/LinearReceiver.hpp',
    'src/receivers/ProbingReceiver.hpp',
    'src/receivers/ReceiverInterface.hpp',
    'src/receivers/channelEstimation/covariance/PerfectIandNCovariance.hpp',
    'src/receivers/channelEstimation/covariance/DiagonalIandNCovariance.hpp',
    'src/receivers/channelEstimation/covariance/EqualDiagonalIandNCovariance.hpp',
    'src/receivers/channelEstimation/covariance/GaussianErrorIandNCovariance.hpp',
    'src/receivers/channelEstimation/covariance/IntraAndInterCellDistinguisher.hpp',
    'src/receivers/channelEstimation/covariance/WishartEstimationModel36_829.hpp',

    'src/receivers/channelEstimation/channel/ChannelEstimationInterface.hpp',
    'src/receivers/channelEstimation/channel/ThermalNoiseBasedGaussianError.hpp',
    'src/receivers/channelEstimation/channel/IandNCovarianceBased.hpp',

    'src/ltea/mac/scheduler/downlink/PU2RCScheduler.hpp',
    'src/receivers/feedback/PU2RCFeedbackManager.hpp',
 
    'src/ltea/mac/scheduler/downlink/ZFScheduler.hpp',

    ]

pyconfig = [
    'imtaphy/__init__.py',
    'imtaphy/Station.py',
    'imtaphy/Logger.py',
    'imtaphy/Channel.py',
    'imtaphy/Spectrum.py',
    'imtaphy/Pathloss.py',
    'imtaphy/Scanner.py',
    'imtaphy/Antenna.py',
    'imtaphy/SCM.py',
    'imtaphy/LinkManagement.py',
    'imtaphy/Receiver.py',
    'imtaphy/Probes.py',
    'imtaphy/ScenarioSupport.py',
    'imtaphy/Feedback.py',
    'imtaphy/covarianceEstimation.py',
    'imtaphy/channelEstimation.py',
    
     ##### LTEA files #####
    'ltea/nodes/EPCgw.py',
    'ltea/nodes/IPv4Component.py',
    'ltea/nodes/__init__.py',
    'ltea/nodes/eNB.py',
    'ltea/nodes/UE.py',
    'ltea/__init__.py',
    'ltea/dll/pdcp.py',
    'ltea/dll/schedulers/__init__.py',
    'ltea/dll/schedulers/uplink.py',
    'ltea/dll/schedulers/downlink.py',
    'ltea/dll/schedulers/resourceManager.py',
    'ltea/dll/__init__.py',
    'ltea/dll/l2s.py',
    'ltea/dll/harq.py',
    'ltea/dll/rlc.py',
    'ltea/dll/component.py',
    'ltea/dll/linkAdaptation/__init__.py',
    'ltea/dll/linkAdaptation/uplink.py',
    'ltea/dll/linkAdaptation/downlink.py',
    'ltea/helper.py',
    'ltea/evaluation/__init__.py',
    'ltea/evaluation/default.py',
    'ltea/evaluation/probe.py',
    'ltea/evaluation/systemTestProbes.py',
]

dependencies = []
# Put in any external lib here as you would pass it to a -l compiler flag, e.g.
dependencies = []
Return('libname srcFiles hppFiles pyconfig dependencies')
