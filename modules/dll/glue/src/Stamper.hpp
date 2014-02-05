/******************************************************************************
 * Glue                                                                       *
 * __________________________________________________________________________ *
 *                                                                            *
 * Copyright (C) 2005-2006                                                    *
 * Lehrstuhl fuer Kommunikationsnetze (ComNets)                               *
 * Kopernikusstr. 16, D-52074 Aachen, Germany                                 *
 * phone: ++49-241-80-27910 (phone), fax: ++49-241-80-22242                   *
 * email: wns@comnets.rwth-aachen.de                                          *
 * www: http://wns.comnets.rwth-aachen.de                                     *
 ******************************************************************************/

#ifndef GLUE_STAMPER_HPP
#define GLUE_STAMPER_HPP

#include <WNS/ldk/fun/FUN.hpp>
#include <WNS/ldk/HasReceptor.hpp>
#include <WNS/ldk/HasConnector.hpp>
#include <WNS/ldk/HasDeliverer.hpp>
#include <WNS/ldk/Processor.hpp>
#include <WNS/ldk/CommandTypeSpecifier.hpp>
#include <WNS/pyconfig/View.hpp>
#include <WNS/logger/Logger.hpp>
#include <WNS/Cloneable.hpp>
#include <WNS/Assure.hpp>

#include <string>

namespace glue {

    /**
     * @brief This FU stamps all outgoing compounds with a pre-set command.
     */
    template <class COMMAND>
    class Stamper :
	public wns::ldk::Processor<Stamper<COMMAND> >,
	public wns::ldk::CommandTypeSpecifier<COMMAND>,
	public wns::ldk::HasReceptor<>,
	public wns::ldk::HasConnector<>,
	public wns::ldk::HasDeliverer<>,
	public wns::Cloneable<Stamper<COMMAND> >
    {
    public:
	typedef COMMAND Command;

	Stamper(wns::ldk::fun::FUN* fun, wns::pyconfig::View& config) :
	    wns::ldk::Processor<Stamper<Command> >(),
	    wns::ldk::CommandTypeSpecifier<COMMAND>(fun),
	    wns::ldk::HasReceptor<>(),
	    wns::ldk::HasConnector<>(),
	    wns::ldk::HasDeliverer<>(),
	    wns::Cloneable<Stamper<COMMAND> >(),

	    stamp(NULL),
	    logger(config.get<wns::pyconfig::View>("logger")),
	    logMessage(config.get<std::string>("logMessage"))
	{
	}	    

	virtual
	~Stamper()
	{
	    if (stamp != NULL) delete stamp;
	}

	// ProcessorInterface
	virtual void
	processIncoming(const wns::ldk::CompoundPtr& /* compound */)
	{
	}
	
	virtual void
	processOutgoing(const wns::ldk::CompoundPtr& compound)
	{
	    assure(stamp != NULL, "I'm a stamper without a stamp. I'm damn useless.");
	    if (logMessage != "") {
		MESSAGE_BEGIN(NORMAL, logger, m, this->getFUN()->getName());
		m << logMessage;
		MESSAGE_END();
	    }
	    Command* command = this->activateCommand(compound->getCommandPool());
	    (*command) = (*stamp);
	}

	// Selectors
	/**
	 * @brief Set the stamp.
	 *
	 * After calling this method, all outgoing compounds will get
	 * stamped with the given command as it was by the time this
	 * method was called.
	 */
	void
	setStamp(const Command& stamp)
	{
	    if (this->stamp != NULL) delete this->stamp;
	    this->stamp = new Command(stamp);
	}

	/**
	 * @brief Return the stamp.
	 */
	const Command&
	getStamp() const
	{
	    return *stamp;
	}

    private:
	Command* stamp;
	wns::logger::Logger logger;
	std::string logMessage;
    };

}

#endif // GLUE_STAMPER_HPP
