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

#include <CONSTANZE/GeneratorMMPP.hpp>
#include <CONSTANZE/Generator.hpp>
#include <CONSTANZE/Binding.hpp>
#include <CONSTANZE/ConstanzeComponent.hpp>

#include <WNS/module/Base.hpp>
#include <WNS/distribution/Distribution.hpp>
#include <WNS/node/Node.hpp>

#include <fstream>

using namespace constanze;
using namespace wns::markovchain;

// objects of these classes are created by name:
// the names are specified in Constanze.py as __plugin__
STATIC_FACTORY_REGISTER_WITH_CREATOR(constanze::GeneratorMMPP, constanze::GeneratorBase, "MMPP", wns::PyConfigViewCreator);
STATIC_FACTORY_REGISTER_WITH_CREATOR(constanze::GeneratorMMPP, constanze::GeneratorBase, "CTMMPP", wns::PyConfigViewCreator);

/*************************** Generator *******************************/

GeneratorMMPP::GeneratorMMPP( const wns::pyconfig::View& config) :
	GeneratorBase(config), // config goes into pyco (member)
	MarkovContinuousTimeTraffic(config.get<int>("numberOfChains")), // other base class
	//numberOfChains(config.get<int>("numberOfChains")), // already in MarkovBase class
	targetRate(0.0),
	loggerName(config.get<std::string>("logger.name"))
{
	assure(config.knows("rateScale"),"missing rateScale parameter");
	assure(config.knows("targetRate"),"missing targetRate parameter");
	assure((config.isNone("rateScale") && !config.isNone("targetRate"))
	       || (!config.isNone("rateScale") && config.isNone("targetRate")),
	       "rateScale and targetRate are mutually exclusive. Set one to None!");
	if (!config.isNone("rateScale")) {
		rateScale=config.get<double>("rateScale"); // in MarkovContinuosTimeTraffic class
	} else {
		targetRate=config.get<double>("targetRate");
	}
	assure(rateScale>0.0,"bad rateScale");
	transitionScale=config.get<double>("transitionScale"); // in MarkovContinuousTime class
	assure(transitionScale>0.0,"bad transitionScale");
	// we have two loggers, one from each base class
	//logger = log; // from MarkovBase::logger GeneratorBase::log
	logger = wns::logger::Logger(log.getModuleName(), loggerName + ".CTMC");
	logger.setLevel(log.getLevel()); // copy logging level

	// sub structure "mmppParams":
	wns::pyconfig::View mmppParamsConfig(config, "mmppParams");
	int startState = mmppParamsConfig.get<int>("startState"); // valid for all chains
	bool fromFile  = mmppParamsConfig.get<bool>("fromFile");
	std::string transitionDistributionSpec;
	if (mmppParamsConfig.knows("transitionDistribution")
	    && !mmppParamsConfig.isNone("transitionDistribution")) {
		transitionDistributionSpec = mmppParamsConfig.get<std::string>("transitionDistribution");
	} else {
		transitionDistributionSpec = "NegExp(X)";
	}

	MESSAGE_BEGIN(NORMAL, log, m, "New GeneratorMMPP: ");
	m << "C=" << numberOfChains << ", rs=" << rateScale << ", ts=" << transitionScale
	  << ", TD=" << transitionDistributionSpec;
	MESSAGE_END();

	if (fromFile) { // read parameters from GDF file (format used in Opnet)
		// examples are in ./framework/libwns--main--3.0/PyConfig/wns/markov/*.gdf
		std::string filename = mmppParamsConfig.get<std::string>("filename");
		MESSAGE_BEGIN(NORMAL, log, m, "reading from ");
		m << "filename=" << filename;
		MESSAGE_END();
		//std::fstream in(filename.c_str(),std::ios::in); // opens automatically
		std::ifstream in(filename.c_str()); // opens automatically
		assure(in.good(),"error opening file "+filename);
		MarkovContinuousTimeTraffic::readNumberOfStates(in);
		MarkovContinuousTimeTraffic::readStatesFromFile(in);
		MarkovContinuousTimeTraffic::readTransitionsFromFile(in);
		in.close();
		for(int chain=0; chain<numberOfChains; chain++)
			startStates[chain]=startState; // here: one value for all
	} else { // params from class MMPPparams
		int numberOfStates = mmppParamsConfig.get<int>("numberOfStates");
		assure(mmppParamsConfig.len("transitionMatrix") == numberOfStates, "transitionMatrix: wrong dimension");
		assure(mmppParamsConfig.len("stateList") == numberOfStates, "stateList: wrong dimension");
		setNumberOfStates(numberOfStates);
		if (!mmppParamsConfig.knows("startStates")
		    || mmppParamsConfig.isNone("startStates")) { // no startStates[] specified
			for(int chain=0; chain<numberOfChains; chain++)
				startStates[chain] = startState;
		} else { // startStates[] specified as an array
			assure(mmppParamsConfig.len("startStates") == numberOfChains, "startStates: wrong dimension");
			for(int chain=0; chain<numberOfChains; chain++) {
				int aStartState = mmppParamsConfig.get<int>("startStates", chain);
				startStates[chain] = aStartState;
			}
		} // else: startStates[] specified as an array
		for(int state=0; state < numberOfStates; state++) {
			// get the state specifications:
			wns::pyconfig::View stateConfig = mmppParamsConfig.get("stateList", state);
			vectorOfStates[state] = TrafficSpec(stateConfig); // inherited member

			// the following lines in MESSAGE_BEGIN/MESSAGE_END are
			// only for showing the parameters of the state:
			MESSAGE_BEGIN(NORMAL, log, m, "State["<<state<<"]: ");
			m << "iat=" << *(vectorOfStates[state].getInterArrivalTimeDistribution())
			  << ", packetSize=" << *(vectorOfStates[state].getPacketSizeDistribution())
			  << ", mean=" << vectorOfStates[state].getMeanRate() * rateScale << " bits/s";
			MESSAGE_END();

			// get the matrix specification:
			std::stringstream index;
			index << state;
			std::string subviewName = "transitionMatrix" +
				std::string("[") + index.str() + std::string("]");
			for(int col = 0; col < numberOfStates; ++col) {
				double rate = mmppParamsConfig.get<double>(subviewName,col);
				// rate *= transitionScale; // done here or in Markov* classes?
				//MESSAGE_SINGLE(NORMAL, log,"r["<<state<<","<<col<<"]="<<rate);
				setTransitionMatrixElement(state,col,rate);
			} // column
		} // forall rows (=states)
	} // else: taken from MMPPparams
	setTransitionDistributionSpec(transitionDistributionSpec); // can only be done after numberOfStates is known
	// now we have all parameters inside
	calculateStateProbabilities(); // also determines meanRate
	// use calculated meanRate to scale with rateScale to achieve targetRate
	if (!config.isNone("targetRate")) { // targetRate given
		assure(meanRate>0.0,"meanRate must be >0");
		rateScale = targetRate / meanRate;
		MESSAGE_SINGLE(NORMAL, log, "scaling for targetRate="<< targetRate << " => rateScale=" << rateScale);
		// recalculate mean, min, max:
		calculateStateProbabilities();
	} else {
		targetRate = meanRate; // from MarkovContinuousTimeTraffic
	}
} // constructor


GeneratorMMPP::~GeneratorMMPP()
{
	MESSAGE_SINGLE(VERBOSE, log, "~GeneratorMMPP()");
	cancelAllTimeouts(); // remove state transition events (from MultipleTimeout.hpp)
}

// begin of the traffic generation
void GeneratorMMPP::start()
{
	MESSAGE_SINGLE(NORMAL, log, "GeneratorMMPP::start()");
	// now we know the binding:
	//SubGenerator aSubGenerator(binding,log,NULL,NULL,rateScale); // empty
	SubGenerator aSubGenerator(this,log,NULL,NULL,rateScale); // empty
	subGenerators = std::vector<SubGenerator>(numberOfChains,aSubGenerator); // empty constructor needed here
	for(int chain=0; chain<numberOfChains; chain++) {
		wns::logger::Logger logSub("CONSTANZE", loggerName + ".Subgen[" + stringify(chain) + "]");
		subGenerators[chain].reconfigLogger(logSub);
	}
	startEvents(); // prepares matrix and runs events
}

// end of the traffic generation
void GeneratorMMPP::stop()
{
	MESSAGE_SINGLE(NORMAL, log, "GeneratorMMPP::stop()");
	//cancelAllTimeouts(); // remove state transition events (from MultipleTimeout.hpp)
	// use a method in MarkovContinuousTime:
	stopMarkovProcess();
	for(int chain=0; chain<numberOfChains; chain++)
		subGenerators[chain].stop();
}

void GeneratorMMPP::stateChangeNotification(const int &chainNumber) {
	//int newState = actualStates[chainNumber];
	int newState = getActualStateIndex(chainNumber);
	MESSAGE_SINGLE(NORMAL, log, "GeneratorMMPP::stateChangeNotification(chain="<<chainNumber<<") to state "<<newState);
	//TrafficSpec newTrafficSpec = vectorOfStates[newState];
        const TrafficSpec *newTrafficSpec = getStateContent(newState);
	// reconfig automatically starts events:
	subGenerators[chainNumber].reconfig(
		newTrafficSpec->getInterArrivalTimeDistribution(),
		newTrafficSpec->getPacketSizeDistribution());
}

/* onTimeout() is handled by the parent class. A hook would be like that:
void GeneratorMMPP::onTimeout(const int &chainNumber) {
   MarkovContinuousTimeTraffic::onTimeout(chainNumber); // parent class
} // onTimeout()
*/


