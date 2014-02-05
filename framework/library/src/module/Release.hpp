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

#ifndef WNS_MODULE_RELEASE_HPP
#define WNS_MODULE_RELEASE_HPP

#include <string>

namespace wns { namespace module {
	/**
	 * @brief Exception superclass.
	 * @author Swen Kühnlein <swen@comnets.rwth-aachen.de>
	 */
	class ReleaseError
	{
	public:
		virtual
		~ReleaseError()
		{}
	};

	/**
	 * @brief Category Match Error exception.
	 * @author Swen Kühnlein <swen@comnets.rwth-aachen.de>
	 *
	 * Used to advertise an unmatching category when using
	 * one of the operators <, <=, >, >=.
	 */
	class CategoryMatchError : public ReleaseError { };

	/**
	 * @brief Branch Match Error exception.
	 * @author Swen Kühnlein <swen@comnets.rwth-aachen.de>
	 *
	 * Used to advertise an unmatchning branch when using
	 * one of the operators <, <=, >, >=.
	 */
	class BranchMatchError : public ReleaseError { };

	/**
	 * @brief Class to represent a TLA FQRN.
	 *
	 * @author Swen Kühnlein <swen@comnets.rwth-aachen.de>
	 *
	 * @note Although Release contains the archive the project is from,
	 * the archive attribute is currently just for informational purposes, i.e.
	 * it is never compared but it's returned by getString().
	 *
	 * @todo: add support for #.#.# version numbers be more generic (not for
	 * tla only)
	 */
	class Release
	{
	public:
		/**
		 * @brief Default constructor.
		 *
		 * Creates an empty Release instance.
		 * @see isEmpty()
		 */
		Release() :
			empty(true),
			archive(""),
			category(""),
			branch(""),
			version(""),
			revision("")
		{}

		/**
		 * @brief One string constructor.
		 *
		 * This constructor creates a Release instance initialized by a string
		 * containing a TLA FQRN with archive in front:
		 * archive/category--branch--version--revision
		 * where version should be "#.#" and revision "patch-#".
		 * Example:
		 * msg@comnets.rwth-aachen.de--central/sgoose--nibbler-dev--1.0--patch-111
		 * Please note that tla tree-version gives you this string with the revision
		 * omitted.
		 * @param s TLA archive and FQRN to initialize Release with.
		 */
		Release(const std::string s);
		/**
		 * @brief Component constructor.
		 *
		 * This contructor creates a Release instance initialized by five strings
		 * containing archive, category, branch, version and revision.
		 * The parameter version is expected to look like "#.#" and revision like
		 * "patch-#", where # is a number (that may contain multiple digits).
		 * @param archive The TLA archive.
		 * @param category The TLA FQRN category (project name)
		 * @param branch The branch.
		 * @param version The version.
		 * @param revision The revision.
		 */
		Release(const std::string archive, const std::string category,
			const std::string branch, const std::string version,
			const std::string revision);

		/**
		 * @brief Returns the archive.
		 */
		std::string getArchive() const { return archive; }

		/**
		 * @brief Returns the category.
		 */
		std::string getCategory() const { return category; }

		/**
		 * @brief Returns the branch.
		 */
		std::string getBranch() const { return branch; }

		/**
		 * @brief Returns the version.
		 */
		std::string getVersion() const { return version; }

		/**
		 * @brief Returns the revision.
		 */
		std::string getRevision() const { return revision; }

		/**
		 * @brief Equality operator.
		 */
		bool operator==(const Release b) const;
		/**
		 * @brief Inequality operator.
		 */
		bool operator!=(const Release b) const;
		/**
		 * @brief Less operator.
		 * @note Throws an exception if category or branch don't match.
		 */
		bool operator<(const Release b) const
			throw (CategoryMatchError, BranchMatchError);
		/**
		 * @brief Greater operator.
		 * @note Throws an exception if category or branch don't match.
		 */
		bool operator>(const Release b) const;
		/**
		 * @brief Less than or equal to operator.
		 * @note Throws an exception if category or branch don't match.
		 */
		bool operator<=(const Release b) const;
		/**
		 * @brief Greater than or equal to operator.
		 * @note Throws an exception if category or branch don't match.
		 */
		bool operator>=(const Release b) const;

		/**
		 * @brief Get Release as string.
		 * @returns A string formatted like archive/category--branch--version--revision.
		 */
		std::string getString() const;
		/**
		 * @brief Get Release as human readable string.
		 * @note The returned string may contain newlines!
		 * @returns A string that is easier readable for humans than getString().
		 */
		std::string getNiceString() const;
	private:
		// Private attributes
		bool empty;
		std::string archive;
		std::string category;
		std::string branch;
		std::string version;
		std::string revision;

		/**
		 * @brief Compare Versions.
		 */
		bool versionLowerThan(std::string s) const;
		/**
		 * @brief Compare Revisions
		 */
		bool revisionLowerThan(std::string s) const;
	};
} // module
} // wns
#endif // NOT defined WNS_MODULE_RELEASE_HPP

