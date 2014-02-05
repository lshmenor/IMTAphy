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

#ifndef WNS_PROBE_BUS_DETAIL_STATEVALTABLE_HPP
#define WNS_PROBE_BUS_DETAIL_STATEVALTABLE_HPP

#include <WNS/container/DynamicMatrix.hpp>
#include <WNS/evaluation/statistics/stateval.hpp>
#include <WNS/probe/bus/detail/Sorter.hpp>

#include <vector>
#include <list>
#include <iostream>


namespace wns { namespace probe { namespace bus { namespace detail {

        /** @brief Wrapper for default construction of a StatEval Object */
        class Storage
        {
                    wns::evaluation::statistics::StatEval se;
        public:
            Storage();

            ~Storage();

            void
            put(double);

            double
            get(const std::string& valueType) const;
        };

        class OutputFormatter;

        /** @brief Table functionality wrapper class around DynamicMatrix */
        template <typename T>
        class Table
        {
            friend class OutputFormatter;

        protected:
            typedef T ValueType;

            class OutOfRange :
                public wns::Exception
            {
            public:
                ~OutOfRange() throw() {}
            };

            std::vector<detail::Sorter> sorters;
            wns::container::DynamicMatrix<ValueType>* root;

        public:
            Table(std::vector<detail::Sorter> config) :
                sorters(config),
                root(NULL)
                {
                    std::list<int> dimensions;
                    for (size_t ii = 0; ii<sorters.size(); ++ii)
                    {
                        dimensions.push_back(sorters.at(ii).getResolution());
                    }
                    root = new wns::container::DynamicMatrix<ValueType>(dimensions);
                }

            ~Table()
                {
                    delete root;
                }

            T&
            get(const std::list<IDType>& ids)
                {
                    return root->getValue( this->getCoords(ids) );
                }

            const T&
            get(const std::list<IDType>& ids) const
                {
                    return root->getValue( this->getCoords(ids) );
                }

            const T&
            getByIndex(const std::list<int>& indices) const
                {
                    return root->getValue(indices);
                }

            void
            set(const std::list<IDType>& ids, ValueType value)
                {
                    return root->setValue( this->getCoords(ids),value );
                }

        private:
            std::list<int>
            getCoords(const std::list<IDType>& ids) const
                {
                    assure(ids.size() == sorters.size(), "ID/Sorter Mismatch");
                    std::list<IDType>::const_iterator iter = ids.begin();
                    std::list<IDType>::const_iterator end  = ids.end();

                    std::vector<detail::Sorter>::const_iterator siter = sorters.begin();
                    std::vector<detail::Sorter>::const_iterator send  = sorters.end();

                    std::list<int> coords;

                    while(iter != end && siter != send)
                    {
                        const Sorter& sorter = *siter;
                        if (sorter.checkIndex(*iter) == true)
                        {
                            coords.push_back(sorter.getIndex(*iter));
                        }
                        else
                        {
                            OutOfRange up;
                            throw(up);
                        }
                        ++iter;
                        ++siter;
                    }
                    return coords;
                }
        };

        typedef Table<Storage> StatEvalTable;
}}}}

#endif // not defined WNS_PROBE_BUS_DETAIL_STATEVALTABLE_HPP
