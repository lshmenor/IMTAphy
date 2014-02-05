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

#ifndef WNS_CONTAINER_DYNAMICMATRIX
#define WNS_CONTAINER_DYNAMICMATRIX

#include <WNS/Exception.hpp>

#include <vector>
#include <list>

namespace wns { namespace container {

    /** @brief Element of a Tree-based Matrix implementation
     *
     *  The special feature of this implementation is that the number of dimensions
     *  can be configured at run-time and not at compile-time, as in solutions based
     *  on recursive templates.
     *
     *  @note that the Node may only contain elements that have a default
     *  constructor.
     *
     *  @author Ralf Pabst <pab@comnets.rwth-aachen.de>
     */

    template <typename T>
    class DynamicMatrix
    {
        typedef T ValueType;

        /** @brief The last recursion contains an actual value, wrapped in a Cell
         * Object */
        struct Cell
        {
            /** @brief Constructor */
            Cell() : value_() {}
            /** @brief The value object */
            ValueType value_;
        };

        /** @brief array of subtrees connected to this node */
        std::vector<DynamicMatrix*> subMatrices_;

        /** @brief number of dimensions this (sub-)matrix has */
        int dim_;
        /** @brief pointer to a cell with the value. Only set when dim_ == 0 */
        Cell* cell_;

    public:
        /** @brief Constructor
         *
         * The constructor recursively constructs a number of sub-matrices with the
         * dimensions and sizes given in the list
         */
        DynamicMatrix(std::list<int> dimensionSizes) :
            subMatrices_(),
            dim_(dimensionSizes.size()),
            cell_(NULL)
            {
                if (dim_ != 0)
                {
                    int thisSize = dimensionSizes.front();
                    dimensionSizes.pop_front();

                    for (int ii = 0; ii < thisSize; ++ii)
                    {
                        subMatrices_.push_back( new DynamicMatrix(dimensionSizes) );
                    }
                }
                else if (dim_ == 0)
                {
                    cell_ = new Cell;
                }
            }

        /** @brief Recursively delete all submatrices and the cells */
        ~DynamicMatrix()
            {
                if (dim_ == 0)
                {
                    delete cell_;
                }
                else
                {
                    for (size_t ii = 0; ii < subMatrices_.size(); ++ii)
                    {
                        delete subMatrices_.at(ii);
                    }
                    subMatrices_.clear();
                }
            }

        /** @brief Get const reference to Value at position given through list of
         * indices */
        const ValueType&
        getValue(std::list<int> indices) const
            {
                if (this->dim_ == 0)
                {
                    if (! indices.empty())
                        throw(wns::Exception("Inconsistent Matrix!"));
                    if (! subMatrices_.empty())
                        throw(wns::Exception("Inconsistent Matrix!"));

                    return this->getValue();
                }

                int nextindex = indices.front();
                indices.pop_front();
                return subMatrices_.at(nextindex)->getValue(indices);
            }


        /** @brief Get reference to Value at position given through list of
         * indices */
        ValueType&
        getValue(std::list<int> indices)
            {
                if (this->dim_ == 0)
                {
                    if (! indices.empty())
                        throw(wns::Exception("Inconsistent Matrix!"));
                    if (! subMatrices_.empty())
                        throw(wns::Exception("Inconsistent Matrix!"));

                    return this->getValue();
                }

                int nextindex = indices.front();
                indices.pop_front();
                return subMatrices_.at(nextindex)->getValue(indices);
            }

        /** @brief Set value at position given through list of
         * indices */
        void
        setValue(std::list<int> indices, ValueType value)
            {
                if (this->dim_ == 0)
                {
                    if (! indices.empty())
                        throw(wns::Exception("Inconsistent Matrix!"));
                    if (! subMatrices_.empty())
                        throw(wns::Exception("Inconsistent Matrix!"));

                    this->setValue(value);
                    return;
                }

                int nextindex = indices.front();
                indices.pop_front();
                subMatrices_.at(nextindex)->setValue(indices, value);
            }

    private:
        /** @brief return cell value reference */
        ValueType&
        getValue()
            {
                if (dim_ == 0)
                    return cell_->value_;

                throw(wns::Exception("Inconsistent Matrix!"));
            }

        /** @brief return cell value const reference */
        const ValueType&
        getValue() const
            {
                if (dim_ == 0)
                    return cell_->value_;

                throw(wns::Exception("Inconsistent Matrix!"));
            }

        /** @brief set cell value */
        void
        setValue(ValueType v)
            {
                if (dim_ == 0)
                {
                    cell_->value_ = v;
                    return;
                }

                throw(wns::Exception("Inconsistent Matrix!"));
            }

    };
} // container
} // wns

#endif // not defined WNS_CONTAINER_DYNAMICMATRIX
