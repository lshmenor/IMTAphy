/*******************************************************************************
 * This file is part of openWNS (open Wireless Network Simulator)
 * _____________________________________________________________________________
 *
 * Copyright (C) 2004-2007
 * Chair of Communication Networks (ComNets)
 * Kopernikusstr. 5, D-52074 Aachen, Germany
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

#ifndef WNS_LDK_CLASSIFIER_HPP
#define WNS_LDK_CLASSIFIER_HPP

#include <WNS/ldk/CommandTypeSpecifier.hpp>
#include <WNS/ldk/HasReceptor.hpp>
#include <WNS/ldk/HasConnector.hpp>
#include <WNS/ldk/HasDeliverer.hpp>
#include <WNS/ldk/Processor.hpp>

#include <WNS/ldk/ldk.hpp>

NAMESPACE_BEGIN(wns)
NAMESPACE_BEGIN(ldk)

typedef long ClassificationID;

class ClassifierCommand
    : public wns::ldk::Command
{
public:
	enum { Invalid = -1 };

	struct Local {
	};
	Local local;

    struct Peer {
		ClassificationID id;
	};
	Peer peer;
	struct Magic {
		int bsId;
	};
	Magic magic;
};

/** Classifier implements a FunctionalUnit that classifies compounds by the
 *	ClassificationPolicy.
 */
template <class ClassificationPolicy>
class Classifier
    : public wns::ldk::CommandTypeSpecifier<ClassifierCommand>,
      public wns::ldk::HasReceptor<>,
      public wns::ldk::HasConnector<>,
	  public wns::ldk::HasDeliverer<>,
      public wns::ldk::Processor< Classifier<ClassificationPolicy> >,
	  public ClassificationPolicy,
	  public Cloneable< Classifier<ClassificationPolicy> >

{
public:
    Classifier( wns::ldk::fun::FUN* fuNet, const wns::pyconfig::View& )
			: wns::ldk::CommandTypeSpecifier<ClassifierCommand>(fuNet),
			  ClassificationPolicy(fuNet)
	{}

    /// Process compounds on uplink.
    virtual void processIncoming(const CompoundPtr& compound)
	{
		ClassifierCommand* command = 0;

		if (getFUN()->getProxy()->commandIsActivated( compound->getCommandPool(), this))
			command = getCommand( compound->getCommandPool() );
		else
			command = activateCommand( compound->getCommandPool() );

		command->peer.id = ClassificationPolicy::classify( compound );
	}

	/// Process compounds on downlink.
    virtual void processOutgoing(const CompoundPtr& compound)
	{
		ClassifierCommand* command = 0;

		if(getFUN()->getProxy()->commandIsActivated( compound->getCommandPool(), this))
			command = getCommand( compound->getCommandPool() );
		else
			command = activateCommand( compound->getCommandPool() );

		command->peer.id = ClassificationPolicy::classify( compound );
	}

	typedef wns::ldk::FUNConfigCreator< Classifier<ClassificationPolicy> > ClassifierCreator;
	typedef wns::StaticFactory<ClassifierCreator> ClassifierFactory;
};

NAMESPACE_END
NAMESPACE_END

#endif


