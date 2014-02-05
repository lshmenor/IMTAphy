/******************************************************************************
 * WNS (Wireless Network Simulator)                                           *
 * __________________________________________________________________________ *
 *                                                                            *
 * Copyright (C) 2004-2006                                                    *
 * Chair of Communication Networks (ComNets)                                  *
 * Kopernikusstr. 16, D-52074 Aachen, Germany                                 *
 * phone: ++49-241-80-27910 (phone), fax: ++49-241-80-22242                   *
 * email: wns@comnets.rwth-aachen.de                                          *
 * www: http://wns.comnets.rwth-aachen.de                                     *
 ******************************************************************************/

#ifndef WNS_DISTRIBUTION_RICE_HPP
#define WNS_DISTRIBUTION_RICE_HPP

#include <WNS/distribution/Distribution.hpp>

#include <WNS/distribution/Norm.hpp>

namespace wns { namespace distribution {
    /**
     * @brief Rice distributed random numbers.
     *
     */
    class Rice :
        public Distribution
    {
    public:
        explicit
        Rice(double mean, double variance, wns::rng::RNGen* rng);

        explicit
        Rice(const pyconfig::View& config);

        explicit
        Rice(wns::rng::RNGen* rng, const pyconfig::View& config);

        virtual
        ~Rice();

        virtual double
        operator()();

        virtual std::string
        paramString() const;

    private:
        double losFactor_;
        double variance_;

        Norm disA_;
        Norm disB_;
    }; // Geometric
} // distribution
} // wns

#endif // NOT defined WNS_DISTRIBUTION_RICE_HPP

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

