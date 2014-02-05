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

#ifndef WNS_SMARTPTRBASE_HPP
#define WNS_SMARTPTRBASE_HPP

#include <WNS/TypeInfo.hpp>
#include <WNS/Backtrace.hpp>

#include <list>
#include <stdint.h>
#include <iostream>

namespace wns {
	/**
	 * @brief Base class to SmartPtr for debugging purpose
	 *
	 * This class extends the debugging support of SmartPtr. It is basically
	 * here in order to be able to put all SmartPtr into _one_
	 * container. Additionally this class provides a ID for each
	 * SmartPtrBase that has ever been allocated. This helps to identify
	 * each pointer and trap its time/point of creation. Furthermore the
	 * SmartPtr is forced to provide an ID for each distinct TYPE of
	 * SmartPtr.
	 *
	 * @note The bookkeeping is rather expensive and will definetly slow
	 * down your simulation.
	 */
	class SmartPtrBase
	{
		typedef std::list<SmartPtrBase*> SmartPtrBaseContainer;
	public:
		/**
		 * @brief Identifier type
		 */
		typedef long long int Id;

		/**
		 * @brief Does bookkeeping for all SmartPtr for debugging support
		 */
		SmartPtrBase() :
			globalId(++SmartPtrBase::getCounter())
		{
			SmartPtrBase::getAllPointers().push_back(this);
                        wns::Backtrace bt;
                        bt.snapshot();
                        this->backtrace = bt.toString();
		}

		/**
		 * @brief Does bookkeeping for all SmartPtr for debugging support
		 */
		virtual
		~SmartPtrBase()
		{
			SmartPtrBase::getAllPointers().remove(this);
		}

		/**
		 * @brief Prints information about all SmartPtr instances
		 * currently existing.
		 *
		 * This class keeps track of all currently existing
		 * SmartPtr. This method will print information about these
		 * SmartPtr (like the pointer type, it's id and it's global id).
		 */
		static void
		printAllExistingPointers()
		{
			std::cout << "Currently existing SmartPtrs:" << "\n";
			for(SmartPtrBaseContainer::const_iterator itr = SmartPtrBase::getAllPointers().begin();
			    itr != SmartPtrBase::getAllPointers().end();
			    ++itr) {
				std::cout << "Pointer type: " << (*itr)->getTypeInfo()
					  << " id: " << (*itr)->getId()
					  << " global id: " << (*itr)->globalId << "\n"
                                          << (*itr)->getBacktrace() << std::endl;
			}
		}

                std::string
                getBacktrace()
                {
                    return this->backtrace;
                }
	private:
		/**
		 * @brief Provide type information on the SmartPtr
		 */
		virtual wns::TypeInfo
		getTypeInfo() const = 0;

		/**
		 * @brief Provides ID for SmartPtr instances. IDs are unique
		 * within different SmartPtr types.
		 */
		virtual Id
		getId() const = 0;

		/**
		 * @brief Counter to provide the unique IDs
		 */
		static Id&
		getCounter()
		{
			static Id counter = 0;
			return counter;
		}

		/**
		 * @brief Stores all currently existing SmartPtr instances
		 */
		static SmartPtrBaseContainer&
		getAllPointers()
		{
			static SmartPtrBaseContainer allPointers;
			return allPointers;
		}

		/**
		 * @brief Global identifier
		 */
		Id globalId;

                /**
                 * @brief To record the backtrace for all created SmartPtrs
                 */ 
                std::string backtrace;
	};
} // wns

#endif // NOT defined WNS_SMARTPTRBASE_HPP


