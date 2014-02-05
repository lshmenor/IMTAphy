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

#ifndef WNS_MODULE_DEPENDENCY_LIST_HPP
#define WNS_MODULE_DEPENDENCY_LIST_HPP

#include <WNS/module/Version.hpp>
#include <vector>

namespace wns { namespace module {
	/**
	 * @brief Exception superclass.
	 */
	class DepListError
	{
	public:
		virtual
		~DepListError()
		{}
	};

	/**
	 * @brief Invalid initialization string error exception.
	 *
	 * This exception indicates, that the initialization string for the
	 * DepListElem(const std::string s) was invalid, i.e. it didn't contain
	 * a "<", "=" or ">" as first character.
	 */
	class DepListElemInvalidInitString :
		public DepListError
	{};

	/**
	 * @brief Class for dependency list elements.
	 * @author Swen Kühnlein <swen@comnets.rwth-aachen.de>
	 */
	class DepListElem
	{
	public:
		/**
		 * @brief Dependency relation type.
		 */
		enum depModeType { LessThan, EqualTo, GreaterThan };

		/**
		 * @brief Component constructor.
		 *
		 * This initializes a DepListElem instance with a given
		 * dependency mode and a Version object.
		 *
		 * @param mode LessThan, EqualTo or GreaterThan.
		 * @param dep depend on this version.
		 */
		DepListElem(const depModeType mode, const Version dep);

		/**
		 * @brief String constructor.
		 *
		 * This initializes a DepListElem instance by a string,
		 * consisting of "<", "=" or ">" as first character and a
		 * (sub)string accepted by Version::Version(std::string s)
		 * starting at the second character.
		 *
		 *  @param s initialization string.
		 */
		DepListElem(const std::string s) throw (DepListElemInvalidInitString);

		/**
		 * @brief Get the dependency mode.
		 *
		 * @returns the dependecy mode (LessThan, EqualTo or
		 * GreaterThan).
		 */
		depModeType getDepMode() const;

		/**
		 * @brief Get the dependency.
		 * @returns the version.
		 */
		Version getDependency() const;

		/**
		 * @brief Set the dependency mode.
		 *
		 * @param depMode the dependency mode (LessThan, EqualTo or
		 * GreaterThan).
		 */
		void setDepMode(const depModeType depMode);

		/**
		 * @brief Set the dependency's version.
		 * @param dependency the version.
		 */
		void setDependency(const Version dependency);

		/**
		 * @brief Check, if a given version meets the dependency.
		 * @param ver the version to check
		 * @returns true, if ver meets the dependecy.
		 */
		bool dependencyMetBy(const Version ver) const;

		/**
		 * @brief Return dependency as a string.
		 * @returns the dependency as a string.
		 */

		std::string getString() const;
		/**
		 * @brief Return dependency as a human readable string.
		 * @note The returned string may contain newlines!
		 * @returns dependency as human readable string.
		 */
		std::string getNiceString() const;

	private:
		/**
		 * @brief See depModeType
		 */
		depModeType depMode;

		/**
		 * @brief Version
		 */
		Version dependency;
	};

	/**
	 * @brief Class for Dependency lists.
	 *
	 * @author Swen Kühnlein <swen@comnets.rwth-aachen.de>
	 *
	 * @note Actually it's implemented as a vector, but semantically \- in
	 * colloquial language \- it's a list, thus it's named as it is.
	 */
	class DepList
	{
		typedef std::vector<DepListElem> DepListPredecessor;
	public:
		/**
		 * @brief Empty list constructor.
		 */
		DepList() :
			depListElements()
		{}

		/**
		 * @brief String constructor.
		 *
		 * Instantiate a DepList initialized by a string. The string is
		 * expected to contain initialization strings for DepListElems
		 * enclosed in parens: (DepListElem)(DepListElem) ...
		 *
		 * @param s initialization string.
		 * @see DepListElem::DepListElem(const std::string s)
		 */
		explicit
		DepList(const std::string& s);

		// public member functions

		/**
		 * @brief Check if dependencies are met.
		 *
		 * This checks, if all dependencies are met by the "list" of
		 * versions given by the parameters. The "list" of versions may
		 * be any standard C++ container.
		 *
		 * @param vers iterator pointing at the beginning of the
		 * version "list".
		 * @param end iterator pointing at the end of the version
		 * "list".
		 * @returns true, if all dependencies are met.
		 */
		template<class VerList>
		bool
		dependenciesMetBy(const VerList vers, const VerList end) const;

		/**
		 * @brief Dependency list as a string.
		 */
		std::string getString() const;
		/**
		 * @brief Dependency list as a human readable string.
		 * @note The returned string may contain newlines!
		 */
		std::string getNiceString() const;
	private:
		// The following function exists only to make dependeciesMetBy() look
		// a bit nicer and is not needed anywhere else, thus should be a nested
		// function, if that was possible. But as it isn't, it's where it is now...
		/**
		 * @brief Compares category and branch of two given version and
		 * returns true if equal.
		 */
		bool projectDoesMatch(const Version v1, const Version v2) const
		{
			return (v1.getBuildRelease().getCategory() == v2.getBuildRelease().getCategory())
				&& (v1.getBuildRelease().getBranch() == v2.getBuildRelease().getBranch());
		}

		DepListPredecessor depListElements;
	};

	/**
	 * @brief Returns true if all dependencies are met
	 * @author Swen Kühnlein <swen@comnets.rwth-aachen.de>
	 */
	template<class VerList>
	bool
	DepList::dependenciesMetBy(const VerList vers, const VerList end) const
	{
		// go through all dependencies
		for (size_t i = 0; i < depListElements.size(); ++i) {
			// ... and for each dependency try to find a matching version in the list
			VerList vi = vers;

			while ((vi != end) && !projectDoesMatch(depListElements[i].getDependency(), vi->getVersion())) ++vi;
			// abort, if no matching version was found...
			if ((vi == end) || !depListElements[i].dependencyMetBy(vi->getVersion())) return false;
			// ... otherwise continue
		}
		// all dependencies are met
		return true;
	}
} // module
} // wns

#endif // NOT defined WNS_MODULE_DEPENDENCY_LIST_HPP
