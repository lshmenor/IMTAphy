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

#ifndef GLUE_BERPROVIDER_HPP
#define GLUE_BERPROVIDER_HPP

#include <list>

namespace glue {

	class BERConsumer;

	/**
	 * @brief Reports BER measurments to attached BER observers
	 * @author Klaus Sambale <ksw@comnets.rwth-aachen.de>
	 */
	class BERProvider
	{
	public:
		typedef std::list<BERConsumer*> BERConsumerContainer;

		/**
		 * @brief Constructor
		 */
		explicit
		BERProvider();

		/**
		 * @brief Destructor
		 */
		virtual
		~BERProvider();

		/**
		 * @brief Add an observer
		 */
		virtual void
		attachBERConsumer(BERConsumer* berConsumer);

		/**
		 * @brief Remove an observer
		 */
		virtual void
		detachBERConsumer(BERConsumer* berConsumer);

		/**
		 * @brief Call reportBER of all attached BER observers
		 *
		 * @todo (msg) Is packet size in Bit? -> use Bit as type, if
		 * not: why not?
		 */
		void
		notifyBERConsumers(double BER, int packetSize) const;

		/**
		 * @brief Provides access to all attached BER observers
		 */
		const BERConsumerContainer&
		getBERConsumersAttached() const;

	private:
		/**
		 * @brief Stores all attached BER observers
		 */
		BERConsumerContainer berConsumers;
	};

} // glue

#endif // GLUE_BERPROVIDER_HPP


