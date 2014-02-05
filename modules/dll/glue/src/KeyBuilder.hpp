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

#ifndef GLUE_KEYBUILDER_HPP
#define GLUE_KEYBUILDER_HPP

#include <WNS/ldk/Key.hpp>
#include <WNS/service/dll/Address.hpp>

#include <WNS/logger/Logger.hpp>
#include <WNS/pyconfig/View.hpp>

namespace glue { namespace convergence {
        class UnicastUpper;
    } // convergence
} // glue

namespace glue {

    class Key
        : public wns::ldk::Key
    {
    public:
        typedef wns::service::dll::UnicastAddress Address;

        Key(const Address& address1,
            const Address& address2);

        virtual
        ~Key()
        {}

        virtual bool
        operator<(const wns::ldk::Key& other) const;

        virtual std::string
        str() const;

    private:
        Address address1_;
        Address address2_;
    };

    class KeyBuilder
        : public wns::ldk::KeyBuilder
    {
    public:

        KeyBuilder(const wns::ldk::fun::FUN* fun,
                   const wns::pyconfig::View& config);

        virtual
        ~KeyBuilder()
        {}

        virtual void
        onFUNCreated();

        virtual wns::ldk::ConstKeyPtr
        operator () (const wns::ldk::CompoundPtr& compound, int direction) const;

    private:
        const wns::ldk::fun::FUN* fun_;
        wns::pyconfig::View config_;
        struct Friends {
            glue::convergence::UnicastUpper *unicastUpper;
        } friends_;
    };

} // glue

#endif // NOT defined GLUE_KEYBUILDER_HPP


