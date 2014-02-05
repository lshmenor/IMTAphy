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

#include <GLUE/KeyBuilder.hpp>
#include <GLUE/convergence/Upper.hpp>

#include <WNS/ldk/fun/FUN.hpp>

using namespace glue;

STATIC_FACTORY_REGISTER_WITH_CREATOR(KeyBuilder, wns::ldk::KeyBuilder,
                                     "glue.KeyBuilder",
                                     wns::ldk::FUNConfigCreator);

Key::Key(const Address& address1,
         const Address& address2)
    : address1_(address1),
      address2_(address2)
{
    if (address2_.getInteger() < address1_.getInteger())
    {
        address2_ = address1;
        address1_ = address2;
    }
}

bool
Key::operator<(const wns::ldk::Key& other) const
{
    assure( typeid( other ) == typeid( Key ),
            "comparing different key types");
    const Key* otherID = static_cast<const Key*>(&other);

    if ((address1_.getInteger() + 10000 * address2_.getInteger()) <
        (otherID->address1_.getInteger() + 10000 * otherID->address2_.getInteger()))
    {
        return true;
    }
    return false;
}

std::string
Key::str() const
{
    std::stringstream ss;
    ss << "(" << address1_.getInteger() << "|" << address2_.getInteger() << ")";
    return ss.str();
}


KeyBuilder::KeyBuilder(const wns::ldk::fun::FUN* fun,
                       const wns::pyconfig::View& config)
    : fun_(fun),
      config_(config)
{

}

void
KeyBuilder::onFUNCreated()
{
    friends_.unicastUpper = fun_->findFriend<convergence::UnicastUpper*>(config_.get<std::string>("unicastUpper"));
}

wns::ldk::ConstKeyPtr
KeyBuilder::operator()(const wns::ldk::CompoundPtr& compound, int) const
{
    convergence::UnicastUpperCommand* command = friends_.unicastUpper->getCommand(compound->getCommandPool());

    return wns::ldk::ConstKeyPtr(new Key(command->peer.sourceMACAddress,
                                         command->peer.targetMACAddress));
}
