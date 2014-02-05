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


#ifndef WNS_POSITIONABLE_HPP
#define WNS_POSITIONABLE_HPP

#include <WNS/pyconfig/View.hpp>
#include <WNS/Position.hpp>
#include <WNS/PositionObserver.hpp>
#include <WNS/Subject.hpp>

namespace wns
{
	/**
	 * @brief Provides an interface for positionable objects
	 *
	 * Has a list of PositionObservers that will be informed if the Position
	 * of the object changes and provides and interface for distance
	 * calculation to other Positionable object.
	 */

	class PositionableInterface :
		virtual public SubjectInterface< ObserverInterface<PositionObserver> >
	{
	public:
		virtual
		~PositionableInterface()
		{};

		virtual const wns::Position&
		getPosition() const = 0;

		virtual double
		getDistance(PositionableInterface* p) = 0;

		virtual double
		getDistance(const PositionableInterface& p) = 0;

        virtual double
        getAngle(const PositionableInterface& p) = 0;
	};

	class Positionable :
		virtual public PositionableInterface,
		public Subject<PositionObserver>
	{
		typedef Subject<PositionObserver> SubjectType;
	public:
		Positionable();

		Positionable(const Positionable& other);

		explicit
		Positionable(const Position& p);

		explicit
		Positionable(const wns::pyconfig::View& positionView);

		virtual
		~Positionable();

		virtual const wns::Position&
		getPosition() const;

		virtual double
		getDistance(PositionableInterface* p);

		virtual double
		getDistance(const PositionableInterface& p);

        virtual double
        getAngle(const PositionableInterface& p);

	protected:
		/**
		 * @brief After setting the Position all observers are informed
		 */
		void
		setPosition(const wns::Position& p);

	private:
		Position position;
	};
}

#endif // WNS_POSITIONABLE_HPP


