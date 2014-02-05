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

#ifndef WNS_PROBE_BUS_DETAIL_OUTPUTFORMATTER_HPP
#define WNS_PROBE_BUS_DETAIL_OUTPUTFORMATTER_HPP

#include <WNS/StaticFactory.hpp>
#include <WNS/probe/bus/detail/StatEvalTable.hpp>

namespace wns { namespace probe { namespace bus { namespace detail {

    /**
	 * @brief Creator implementation to be used with StaticFactory.
	 */
    template <typename T, typename KIND = T>
    struct StatEvalTableCreator :
        public StatEvalTableCreator<KIND, KIND>
    {
        virtual KIND*
        create(const StatEvalTable& data)
            {
                return new T(data);
            }

        virtual ~StatEvalTableCreator()
            {};
    };

    template <typename KIND>
    struct StatEvalTableCreator<KIND, KIND>
    {
    public:
        virtual KIND*
        create(const StatEvalTable& data) = 0;

        virtual ~StatEvalTableCreator()
            {};
    };

    /**
	 * @brief abstract Base class for all output formatters
	 *
	 * through an appropriate implementation of the abstract method "print" you
	 * can define how the output of a StatEvalTable shall look like.
	 */
    class OutputFormatter
    {
        /**
		 * @brief Implement this to control the formatting.
		 *
		 * This method is meant to recursively walk over the tree structure of the DynamicMatrix
		 */
        virtual
        void doPrint(std::ostream& strm,
                     std::list<int> fixedIndices,
                     int dim,
                     std::string valueType) const = 0;
    protected:
        /** @brief Reference to the table containing the data */
        const StatEvalTable& data;

        /**
		 * @brief provides deriving classes access to the sorter objects of the
		 * StatEvalTable, which is needed for walking the tree and for formatting
		 */
        const std::vector<Sorter>&
        sorters() const
            {
                return data.sorters;
            }
    public:
        /**
		 * @brief Constructor, has to be initialized with the data the
		 * Formatter should work upon
		 */
        OutputFormatter(const StatEvalTable& table) :
            data(table)
            {}

        virtual
        ~OutputFormatter(){}

        /**
		 * @brief invokes printing the formatted output into the given stream object
		 */
        virtual void
        print(std::ostream& strm, std::string valueType) const;

        /**
		 * @brief returns the comment prefix, empty by default
		 */
        virtual std::string
        getPrefix() const { return ""; }

        /**
		 * @brief returns the filename suffix, empty by default
		 */
        virtual std::string
        getFilenameSuffix() const { return ""; }
    };

    /** @brief Formatter Implementation that produces human-readable tabular output */
    class HumanReadable :
        public OutputFormatter
    {
        virtual
        void doPrint(std::ostream& strm,
                     std::list<int> fixedIndices,
                     int dim,
                     std::string valueType) const;

    public:
        HumanReadable(const StatEvalTable& table) :
            OutputFormatter(table)
            {}

        virtual
        ~HumanReadable(){}
    };

    /** @brief Formatter Implementation that produces Python-parseable output */
    class PythonReadable :
        public OutputFormatter
    {
        virtual
        void doPrint(std::ostream& strm,
                     std::list<int> fixedIndices,
                     int dim,
                     std::string valueType) const;

    public:
        PythonReadable(const StatEvalTable& table) :
            OutputFormatter(table)
            {}

        virtual
        ~PythonReadable(){}

        virtual void
        print(std::ostream& strm, std::string valueType) const;

        virtual std::string
        getPrefix() const { return "# "; };

        virtual std::string
        getFilenameSuffix() const { return ".py"; }
    };

    /** @brief Formatter Implementation that produces Matlab-parseable output */
    class MatlabReadable :
        public OutputFormatter
    {
        virtual
        void doPrint(std::ostream& strm,
                     std::list<int> fixedIndices,
                     int dim,
                     std::string valueType) const;

    public:
        MatlabReadable(const StatEvalTable& table) :
            OutputFormatter(table)
            {}

        virtual
        ~MatlabReadable(){}

        virtual void
        print(std::ostream& strm, std::string valueType) const;

        virtual std::string
        getPrefix() const { return "% "; };

        virtual std::string
        getFilenameSuffix() const { return ".m"; }
    };

    /** @brief Formatter Implementation that produces Matlab-parseable output */
    class MatlabReadableSparse :
        public MatlabReadable
    {
        virtual
        void doPrint(std::ostream& strm,
                     std::list<int> fixedIndices,
                     int dim,
                     std::string valueType) const;

    public:
        MatlabReadableSparse(const StatEvalTable& table) :
            MatlabReadable(table)
            {}

        virtual
        ~MatlabReadableSparse(){}
    };

    /** @brief Formatter Implementation that produces Wrowser LogEval readable output. */
    class LogEvalReadable :
        public OutputFormatter
    {
        virtual
        void doPrint(std::ostream& strm,
                     std::list<int> fixedIndices,
                     int dim,
                     std::string valueType) const;

        public:
            LogEvalReadable(const StatEvalTable& table) :
                OutputFormatter(table)
                {}
    
            virtual
            ~LogEvalReadable(){}

            virtual std::string
            getPrefix() const { return "# "; };
    
            virtual std::string
            getFilenameSuffix() const { return "_Log.log.dat"; }
    };

    /** @brief Formatter Implementation that produces Wrowser readable output read as LogEval. */
    class Plain :
        public MatlabReadable
    {
        public:
            Plain(const StatEvalTable& table) :
                MatlabReadable(table)
                {}

            virtual
            ~Plain(){}

            virtual std::string
            getFilenameSuffix() const { return ".dat"; }
    };

    typedef StatEvalTableCreator<OutputFormatter, OutputFormatter> FormatterCreator;
    typedef StaticFactory<FormatterCreator> FormatterFactory;

}}}}

#endif // not defined WNS_PROBE_BUS_DETAIL_OUTPUTFORMATTER_HPP

