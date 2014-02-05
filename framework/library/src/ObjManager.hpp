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
/**
 * @file
 */

#ifndef WNS_OBJMANAGER_HPP
#define WNS_OBJMANAGER_HPP

#include <algorithm>
#include <list>

#include <WNS/Exception.hpp>
#include <WNS/SmartPtr.hpp>

namespace wns {
	/**
	 * @brief Generic object manager.
	 */
	template <class OBJECT>
	class ObjectManager
	{
	public:
		typedef SmartPtr<OBJECT> ObjPtr;
		typedef OBJECT Object;


		template <class>
		void
		clear()
		{
			objects.clear();
		}

		void append(const OBJECT& obj)
		{
			objects.push_back( obj );
		}

		/// Execute a function for all objects.
		template<class FUNCTION>
		FUNCTION for_each( FUNCTION function)
		{
			return std::for_each( objects.begin(), objects.end(), function );
		}

		size_t size() const
		{
			return objects.size();
		}

	protected:

		std::list< Object > objects;
	};
}
#endif

