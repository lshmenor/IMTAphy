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

#ifndef WNS_DISTRIBUTION_OPERATION_HPP
#define WNS_DISTRIBUTION_OPERATION_HPP

#include <WNS/distribution/Distribution.hpp>
#include <WNS/pyconfig/View.hpp>

namespace wns { namespace distribution { namespace operation {

    class Binary :
        public Distribution,
        public IHasMean
    {
    public:
        Binary(const pyconfig::View& config);
        Binary(wns::rng::RNGen* rng, const pyconfig::View& config);

    protected:
        Distribution* first_;
        Distribution* second_;
        wns::pyconfig::View config_;

    private:
        void
        init();

    }; // Binary

    class ADD :
        public Binary
    {
    public:
        ADD(const pyconfig::View& config) :
            Binary(config)
        {
        }

        ADD(wns::rng::RNGen* rng, const pyconfig::View& config) :
            Binary(rng, config)
        {
        }

        virtual double
        operator()();

        virtual double
        getMean() const;

        virtual std::string
        paramString() const;
    }; // ADD

    class MUL :
        public Binary
    {
    public:
        MUL(const pyconfig::View& config) :
            Binary(config)
        {
        }

        MUL(wns::rng::RNGen* rng,const pyconfig::View& config) :
            Binary(rng, config)
        {
        }

        virtual double
        operator()();

        virtual double
        getMean() const;

        virtual std::string
        paramString() const;
    }; // MUL

	class SUB :
		public Binary
	{
	public:
        SUB(const pyconfig::View& config) :
            Binary(config)
        {
        }

        SUB(wns::rng::RNGen* rng, const pyconfig::View& config) :
            Binary(rng, config)
        {
        }

        virtual double
        operator()();

        virtual double
        getMean() const;

        virtual std::string
        paramString() const;
    };  // SUB

    class DIV :
        public Binary
    {
    public:
        DIV(const pyconfig::View& config) :
            Binary(config)
        {
        }

        DIV(wns::rng::RNGen* rng, const pyconfig::View& config) :
            Binary(rng, config)
        {
        }

        virtual double
        operator()();

        virtual double
        getMean() const;

        virtual std::string
        paramString() const;
    }; // DIV

    class DistributionAndFloat :
        public Distribution
    {
    public:
        DistributionAndFloat(const pyconfig::View& config);
        DistributionAndFloat(wns::rng::RNGen* rng, const pyconfig::View& config);

    protected:
        Distribution* subject_;
        double arg_;
        wns::pyconfig::View config_;

    private:
        void
        init();

    }; // DistributionAndFloat

    class Above :
        public DistributionAndFloat
    {
    public:
        Above(const pyconfig::View& config) :
            DistributionAndFloat(config)
        {
        }

        Above(wns::rng::RNGen* rng, const pyconfig::View& config) :
            DistributionAndFloat(rng, config)
        {
        }

        virtual double
        operator()();

        virtual std::string
        paramString() const;
    }; // Above

    class Below :
        public DistributionAndFloat
    {
    public:
        Below(const pyconfig::View& config) :
            DistributionAndFloat(config)
        {
        }

        Below(wns::rng::RNGen* rng, const pyconfig::View& config) :
            DistributionAndFloat(rng, config)
        {
        }

        virtual double
        operator()();

        virtual std::string
        paramString() const;
    }; // Below

} // operation
} // distribution
} // wns


#endif // NOT defined WNS_DISTRIBUTION_OPERATION_HPP

/*
  Local Variables:
  mode: c++
  fill-column: 80
  c-basic-offset: 8
  c-comment-only-line-offset: 0
  c-tab-always-indent: t
  indent-tabs-mode: t
  tab-width: 8
  End:
*/

