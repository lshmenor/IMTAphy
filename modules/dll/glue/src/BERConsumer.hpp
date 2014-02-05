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

#ifndef GLUE_BERCONSUMER_HPP
#define GLUE_BERCONSUMER_HPP

namespace glue {

	class BERProvider;
	/**
	 * @brief Allows to observe a BERProvider
	 *
	 * @author Klaus Sambale <ksw@comnets.rwth-aachen.de>
	 * @author Marc Schinnenburg <msg@comnets.rwth-aachen.de>
	 *
	 * @todo (msg) Rename to BERObserver (fits better)
	 * @todo (msg) Use new generic Oberser when available
	 */
	class BERConsumer
	{
	public:
		/**
		 * @brief Constructor
		 */
		explicit
		BERConsumer();

		/**
		 * @brief Destructor
		 */
		virtual
		~BERConsumer();

		/**
		 * @name Interface
		 */
		//@{
		/**
		 * @brief Called by BERProvider if it will be deleted
		 *
		 * The BERProvider will call this, if it's going to be
		 * deleted. Afterwards it will call this->setBERProvider(NULL).
		 */
		virtual void
		onBERProviderDeleted() = 0;

		/**
		 * @brief Called by BERProvider, when new measurement is
		 * available
		 */
		virtual void
		onNewMeasurement(double BER, int packetSize) = 0;
		//@}

		/**
		 * @brief Set a BERProvider
		 */
		void
		setBERProvider(BERProvider* _berProvider);

	private:
		/**
		 * @brief The one I'm observing
		 */
		BERProvider* berProvider;
	}; // BERConsumer

} // glue

#endif // GLUE_BERCONSUMER_HPP


