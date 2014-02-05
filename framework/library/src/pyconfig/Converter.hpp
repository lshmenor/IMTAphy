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

#ifndef WNS_PYCONFIG_CONVERTER_HPP
#define WNS_PYCONFIG_CONVERTER_HPP

#include <WNS/pyconfig/Object.hpp>

#include <string>

namespace wns { namespace pyconfig {
	template <typename T>
	class Converter
	{
	public:
		typedef T TargetType;

		// boolean converter
		bool
		convert(bool& value, Object o) const
		{
			switch(o.isTrue())
			{
					case 1:
						value = true;
						break;
					case 0:
						value = false;
						break;
			}
			return false;
		} // convert bool

		// std::string converter
		bool
		convert(std::string& value, Object o) const
		{
			if(!o.isConvertibleToString())
			{
				return false;
			}

			value = o.toString();
			return true;
		} // convert string


		// default converter
		template <typename U>
		bool convert(U &value, Object o) const
		{
			if(!o.isConvertibleToString())
			{
				return false;
			}

			std::istringstream os(o.toString());
			os >> value;

			return true;
		} // convert
	};

}}

#endif // NOT defined WNS_PYCONFIG_CONVERTER_HPP


