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

#ifndef WNS_MODULE_VERSION_HPP
#define WNS_MODULE_VERSION_HPP

#include <WNS/module/DateTime.hpp>
#include <WNS/module/Release.hpp>

namespace wns { namespace module {
	/**
	 * @brief Class to manage version information.
	 * @author Swen Kühnlein <swen@comnets.rwth-aachen.de>
	 */
	class Version
	{
	public:
		/**
		 * @brief Component constructor.
		 *
		 * This constructor creates a Version instance initialized by the
		 * component values given as its parameters.
		 *
		 * @param bRelease The release.
		 * @param bTime The build time.
		 * @param bUser The build user.
		 * @param bHost The build host.
		 * @param bFlags The compiler build flags.
		 * @param mTime The project's modification time.
		 * @param mFile The file that mTime was taken from.
		 */
		Version(const Release bRelease, const DateTime bTime = DateTime(),
			const std::string bUser = "", const std::string bHost = "",
			const std::string bFlags = "", const DateTime mTime = DateTime(),
			const std::string mFile = "");

		/**
		 * @brief Single string constructor.
		 *
		 * This constructor creates a Version instance initialized by a
		 * single string that must be in the same format as returned by
		 * getString().
		 *
		 * @param s initialization string.
		 * @see std::string Version::getString()
		 */
		Version(const std::string s);

		/**
		 * @brief Equality operator.
		 *
		 * @note If buildUser or buildHost of this instance or the b
		 * instance is empty, they are considered equal. This way people
		 * can specify that they don't care about who built it and
		 * where. Just leave the fields empty.
		 */
		bool operator==(const Version b) const;
		/**
		 * @brief Inequality operator.
		 */
		bool operator!=(const Version b) const;
		/**
		 * @brief Less operator.
		 */
		bool operator<(const Version b) const;
		/**
		 * @brief Greater operator.
		 */
		bool operator>(const Version b) const;
		/**
		 * @brief Less than or equal to operator.
		 */
		bool operator<=(const Version b) const;
		/**
		 * @brief Greater than or equal to operator.
		 */
		bool operator>=(const Version b) const;

		/**
		 * @brief Return the build time.
		 */
		DateTime getBuildTime() const;
		/**
		 * @brief Return the build user.
		 */
		std::string getBuildUser() const;
		/**
		 * @brief Return the build host.
		 */
		std::string getBuildHost() const;
		/**
		 * @brief Return the build release.
		 */
		Release getBuildRelease() const;
		/**
		 * @brief Return the build flags.
		 */
		std::string getBuildFlags() const;
		/**
		 * @brief Return the project's modification time.
		 */
		DateTime getModificationTime() const;
		/**
		 * @brief Return the file name the modification time was taken from.
		 */
		std::string getModifiedFile() const;

		/**
		 * @brief Set the build time.
		 */
		void setBuildTime(const DateTime bTime);
		/**
		 * @brief Set the build user.
		 */
		void setBuildUser(const std::string bUser);
		/**
		 * @brief Set the build host.
		 */
		void setBuildHost(const std::string bHost);
		/**
		 * @brief Set the build release.
		 */
		void setBuildRelease(const Release bRelease);
		/**
		 * @brief Set the build flags.
		 */
		void setBuildFlags(const std::string bFlags);
		/**
		 * @brief Set the project's modification time.
		 */
		void setModificationTime(const DateTime mTime);
		/**
		 * @brief Set the file name the modification time was taken from.
		 */
		void setModifiedFile(const std::string mFile);

		/**
		 * @brief Get version as one string.
		 *
		 * Return Version as a string that can be passed as a parameter to
		 * the single string constructor. The format of the string is:
		 * release "," buildTime "," buildUser "," buildHost "," modificationTime "," modifiedFile
		 * Example:
		 * msg@comnets.rwth-aachen.de--central/sgoose--nibbler-dev--1.0--patch-123,2004-05-06 12:34,someone,somehost,1091013526,file.cpp
		 * @returns Version a a single string.
		 *
		 * @see Version::Version(std::string s)
		 * @see DateTime::DateTime(std::string t)
		 */
		std::string getString() const;

		/**
		 * @brief Get version as a human readable string.
		 * @note The returned string may contain newlines!
		 * @returns eye candied string
		 */
		std::string getNiceString(bool verbose, std::string offset) const;

	private:
		DateTime buildTime;
		std::string buildUser;
		std::string buildHost;
		std::string buildFlags;
		Release buildRelease;
		DateTime modificationTime;
		std::string modifiedFile;

		static bool stringEqualOrEmpty(const std::string s1, const std::string s2) { return ((s1=="") || (s2=="") || (s1==s2)); }
		static std::string cutAt(const std::string s, const std::string pattern, const int n);

		/**
		 * @brief contains methods to extract specific parts of a
		 * version string
		 *
		 * @internal A nested namespace would be optimal, but that's not possible,
		 * so now it's a "class" with static members only.
		 */
		class Syntax {
		public:
			static std::string extractRelease(const std::string s);
			static std::string extractTime(const std::string s);
			static std::string extractUser(const std::string s);
			static std::string extractHost(const std::string s);
			static std::string extractModificationTime(const std::string s);
			static std::string extractModifiedFile(const std::string s);
			static std::string extractFlags(const std::string s);
		};
	};

} // module
} // wns

#endif // NOT defined WNS_MODULE_VERSION_HPP
