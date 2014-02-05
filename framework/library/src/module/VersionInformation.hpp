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

#ifndef WNS_MODULE_VERSIONINFORMATION_HPP
#define WNS_MODULE_VERSIONINFORMATION_HPP

#include <WNS/module/DependencyList.hpp>

namespace wns { namespace module {
	/**
	 * @brief Class to manage version and dependency information.
	 * @author Swen Kühnlein <swen@comnets.rwth-aachen.de>
	 */
	class VersionInformation
	{
	public:
		/**
		 * @brief Version and DepList constructor.
		 *
		 * Instantiate a VersionInformation object initialized by a
		 * Version object and a dependency list.
		 *
		 * @param ver the version.
		 * @param deps the dependencies.
		 */
		explicit
		VersionInformation(const Version ver, const DepList deps = DepList())
			: version(ver), dependencies(deps)
		{}

		/**
		 * @brief  Component constructor.
		 *
		 * Create a VersionInformation object and initialize with
		 * release, dependency list, time, user and host. All parameters
		 * except bRelease default to be empty if omitted.
		 *
		 * @param bRelease the release.
		 * @param deps the dependencies, defaults to no dependencies (empty list).
		 * @param bTime the build time, default to empty DateTime.
		 * @param bUser the build user, defaults to empty string.
		 * @param bHost the build host, defaults to empty host.
		 * @param bFlags the compiler build flags.
		 * @param mTime the project's modification time.
		 * @param mFile the file that mTime was taken from.
		 */
		explicit
		VersionInformation(const Release bRelease,
				   const DepList deps = DepList(),
				   const DateTime bTime = DateTime(),
				   const std::string bUser = "",
				   const std::string bHost = "",
				   const std::string bFlags = "",
				   const DateTime mTime = DateTime(),
				   const std::string mFile = "")
			: version(bRelease, bTime, bUser, bHost, bFlags, mTime, mFile),
			  dependencies(deps)
		{}
		/**
		 * @brief String constructor.
		 *
		 * Create a VersionInformation object initialized by the content of
		 * the given string. The string must be Version "::" DepList.
		 *
		 * @param s initialization string.
		 * @see Version::Version(const std::string s)
		 * @see DepList::DepList(const std::string s)
		 */
		explicit
		VersionInformation(const std::string& s)
			: version(Syntax::extractVersion(s)),
			  dependencies(Syntax::extractDependencies(s))
		{}

		/**
		 * @brief Return the version.
		 */
		Version getVersion() const
		{
			return version;
		}

		/**
		 * @brief Return the dependencies.
		 */
		DepList getDependencies() const
		{
			return dependencies;
		}

		/**
		 * @brief Set the version.
		 */
		void setVersion(const Version ver)
		{
			version = ver;
		}

		/**
		 * @brief Set the dependencies.
		 */
		void setDependencies(const DepList deps)
		{
			dependencies = deps;
		}

		/**
		 * @brief Return version information as string.
		 */
		std::string getString() const;
		/**
		 * @brief Return version information as human readable string.
		 * @note May contain newlines!
		 */
		std::string getNiceString() const;

	private:
		// nested namespaces workaround
		/**
		 * @brief Syntax to extract stuff from VersionInformation
		 */
		class Syntax
		{
		public:
			static std::string extractVersion(const std::string s);
			static std::string extractDependencies(const std::string s);
		};

		Version version;
		DepList dependencies;
	};
} // module
} // wns
#endif // NOT defined WNS_MODULE_VERSIONINFORMATION_HPP
