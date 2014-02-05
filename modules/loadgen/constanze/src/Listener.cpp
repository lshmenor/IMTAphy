/*******************************************************************************
 * This file is part of openWNS (open Wireless Network Simulator)
 * _____________________________________________________________________________
 *
 * Copyright (C) 2004-2007
 * Chair of Communication Networks (ComNets)
 * Kopernikusstr. 16, D-52074 Aachen, Germany
 * phone: ++49-241-80-27910,
 * fax: ++49-241-80-22242
 * email: info@openwns.org
 * www: http://www.openwns.org
 * _____________________________________________________________________________
 *
 * openWNS is free software; you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License version 2 as published by the
 * Free Software Foundation;
 *
 * openWNS is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/

#include <CONSTANZE/Listener.hpp>
#include <WNS/module/Base.hpp>
#include <iomanip> // for setprecision

using namespace constanze;

Listener::Listener( const wns::pyconfig::View& config, wns::node::component::Interface* component ):
  pyco(config),
  log(pyco.get("logger")),
  domainName(pyco.get<std::string>("domainName")),
  windowSize(config.get<double>("probeWindow")), // time in seconds
  doMMPPestimation(config.get<bool>("doMMPPestimation")),
  sizeProbe(new wns::probe::bus::ContextCollector(wns::probe::bus::ContextProviderCollection(&component->getContextProviderCollection()),
						  "traffic.endToEnd.packet.incoming.size")),
  bitsRateProbe(new wns::probe::bus::ContextCollector(wns::probe::bus::ContextProviderCollection(&component->getContextProviderCollection()),
						      "traffic.endToEnd.window.incoming.bitThroughput")),
  packetRateProbe(new wns::probe::bus::ContextCollector(wns::probe::bus::ContextProviderCollection(&component->getContextProviderCollection()),
							"traffic.endToEnd.window.incoming.packetThroughput")),
  bitsWindowedRateProbe(),
  packetWindowedRateProbe(),
  delayProbe(new wns::probe::bus::ContextCollector(wns::probe::bus::ContextProviderCollection(&component->getContextProviderCollection()),
						   "traffic.endToEnd.packet.incoming.delay")),
  bitsWindowedResult("traffic.windowedEndToEndIncomingBitThroughputTrace"),
  packetWindowedResult("traffic.windowedEndToEndIncomingPacketThroughputTrace"),
  packetCounter(0),
  bitCounter(0),
  windowedPacketCounter(0),
  windowedBitCounter(0),
  currentWindowEnd(windowSize)
{
  // The names of the probes must match those in Probes.py (no automatism)
  wns::probe::bus::ContextProviderCollection localContext(
							  &component->getContextProviderCollection());
  localContext.addProvider(wns::probe::bus::contextprovider::Container(&this->bitsWindowedResult));
  localContext.addProvider(wns::probe::bus::contextprovider::Container(&this->packetWindowedResult));
  bitsWindowedRateProbe = wns::probe::bus::ContextCollectorPtr(
							       new wns::probe::bus::ContextCollector(
												     localContext,
												     "traffic.windowedEndToEndIncomingBitThroughputTrace"));
  packetWindowedRateProbe = wns::probe::bus::ContextCollectorPtr(
								 new wns::probe::bus::ContextCollector
								 (localContext,
								  "traffic.windowedEndToEndIncomingPacketThroughputTrace"));
  // if doMMPPestimation is true, measure data flow
  if (doMMPPestimation) {
    wns::pyconfig::View measurementConfig = config.get("measurement");
    measurement = new Measurement(measurementConfig);
    /** @todo put into Measurement.hpp [rs] */
    observationVector = new std::vector<int>();
  } else {
    measurement = NULL;
  }
  MESSAGE_SINGLE(NORMAL, log, "Listener created (domainName="<<domainName<<").");
}

Listener::~Listener()
{
  MESSAGE_BEGIN(NORMAL, log, m, "Received a total of ");
  m << packetCounter << " packets with " << bitCounter << " bits";
  m << " at my socket=" << domainName;
  MESSAGE_END();

  if (doMMPPestimation) {
    // apply baum-welch algorithm
    /** @todo put into Measurement.hpp [rs] */
    HMM *hmm = measurement->calculate(
				      measurement->getEstimatorNumberOfStates(),
				      observationVector);
    measurement->output_python(
			       hmm->getTransitionsMatrix(),
			       hmm->getNumberOfStates(),
			       measurement->getMMPPestimationResultFileName());
    // set memory for free
    delete hmm;
    /** @todo put into Measurement.hpp [rs] */
    delete observationVector;
    delete measurement;
  }
}

void Listener::onData(const wns::osi::PDUPtr& _data)
{
  assure(_data , "data is empty.");
  assureType(_data.getPtr(), ConstanzePDU*);
  int bits = _data->getLengthInBits();

  sizeProbe->put(bits);

  const simTimeType now = wns::simulator::getEventScheduler()->getTime();
  const simTimeType packetCreationTime = ConstanzePDU::castPDU2ConstanzePDU(
									    _data.getPtr())->getTimeStamp();
  bitCounter += bits;
  packetCounter ++;

  // window mechanism:
  // now may be lagging several windowSizes after currentWindowEnd,
  // so roll-up each missing window one by one:
  while (now >= currentWindowEnd) { // need to shift window
    // write probes for old (outdated) window
    double bitRate = static_cast<double>(windowedBitCounter) / windowSize;

    if (doMMPPestimation) {
      /** @todo put into Measurement.hpp [rs] */
      // measurement->update(bitRate);

      bool notProperStateLevel = true;

      stateLevel = 0; // state(Level)

      for(unsigned int i = 0; notProperStateLevel == true && i < measurement->boundaryVector->size(); i++){

	if(bitRate <= measurement->boundaryVector->at(i)){
	  stateLevel = i;
	  notProperStateLevel = false;
	}
      }
      if(notProperStateLevel)
	stateLevel = measurement->boundaryVector->size();


      observationVector->push_back(stateLevel);
    }

    double packetRate = static_cast<double>(windowedPacketCounter) / windowSize;

    bitsRateProbe->put(bitRate);
    packetRateProbe->put(packetRate);
    double dummy = 0.0;
    {
      // Prepare string with result
      std::ostringstream o;
      o << std::fixed << std::setprecision( 6 ) << currentWindowEnd
	<< "\t" << std::setprecision( 2 ) << bitRate;

      // Instruct the context provider to provide the string next time it
      // is being asked
      bitsWindowedResult.set(o.str());
      // Put a dummy value
      bitsWindowedRateProbe->put(dummy);
    }
    {
      // Prepare string with result
      std::ostringstream o;
      o << std::fixed << std::setprecision( 6 ) << currentWindowEnd
	<< "\t" << std::setprecision( 2 ) << bitRate;

      // Instruct the context provider to provide the string next time it
      // is being asked
      packetWindowedResult.set(o.str());
      // Put a dummy value
      packetWindowedRateProbe->put(dummy);
    }
    // initialize next window:
    currentWindowEnd += windowSize; // may still be < now
    windowedBitCounter = 0;
    windowedPacketCounter = 0;
  }
  windowedBitCounter += bits;
  windowedPacketCounter ++;
  // delay/size probes
  double travelTime = now - packetCreationTime;
  assure(travelTime > 0.0, "packet with travel time <=0.");
  delayProbe->put(travelTime);
}

void Listener::registerBinding(constanze::Binding* _binding)
{
  binding = _binding;
  binding->registerListener(this);
}

std::string Listener::printAddress() const
{
  std::ostringstream tmp;
  tmp << domainName;
  return tmp.str();
}
