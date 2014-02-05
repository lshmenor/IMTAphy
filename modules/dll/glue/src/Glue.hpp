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

#ifndef GLUE_GLUE_HPP
#define GLUE_GLUE_HPP

#include <WNS/module/Module.hpp>

namespace glue {
	/**
	 * @brief Module - loaded by WNS
	 * @author Fabian Debus <fds@comnets.rwth-aachen.de>
	 * @author Marc Schinnenburg <msg@comnets.rwth-aachen.de>
	 */
	class Glue :
		public wns::module::Module<Glue>
	{
	public:
		/**
		 * @brief Constructor
		 */
		explicit
		Glue(const wns::pyconfig::View& pyco);

		/**
		 * @brief Destructor
		 */
		virtual
		~Glue();

		/**
		 * @name Module Interface
		 *
		 * @brief Implements wns::module::Base
		 */
		//@{
		/**
		 * @brief Implements wns::module::Base::configure()
		 */
		virtual void
		configure();

		/**
		 * @brief Implements wns::module::Base::startUp()
		 */
		virtual void
		startUp();

		/**
		 * @brief Implements wns::module::Base::shutDown()
		 */
		virtual void
		shutDown();
		//@}
	}; // Glue

} // glue

#endif // NOT defined GLUE_GLUE_HPP



