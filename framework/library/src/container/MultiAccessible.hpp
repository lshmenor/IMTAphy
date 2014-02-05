/*******************************************************************************
 * This file is part of openWNS (open Wireless Network Simulator)
 * _____________________________________________________________________________
 *
 * Copyright (C) 2004-2007
 * Chair of Communication Networks (ComNets)
 * Kopernikusstr. 5, D-52074 Aachen, Germany
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


#ifndef WNS_CONTAINER_MULTIACCESSIBLE_HPP
#define WNS_CONTAINER_MULTIACCESSIBLE_HPP

#include <WNS/Assure.hpp>

#include <cstddef>
#include <cassert>
#include <algorithm>

namespace wns { namespace container {
    /**
     *  @brief Contains definitions valid for all MultiAccessible-classes.
     */
    namespace MultiAccessibleDefs
    {
        /**
         *  @brief Type used for sizes.
         */
        typedef std::size_t SizeType;
    }

    /**
     * @brief Interface and base for classes which the [] operator can be
     * applied to multiple times, allowing (const) read access.
     *
     * @ingroup cppgoodies
     *
     * @param T Type of the values we want to access.
     * @param N Number of dimensions.
     * @param I Type of the index in the [] operator. This defaults to
     * MultiAccessibleDefs::SizeType.
     *
     * This class specifies following interface:

     * "variable of type MultiReadAccessible<@a T,@a N,@a I>"[index]
     * returns a const reference to a MultiReadAccessible<@a T,@a N-1,@a I>,
     * unless @a N == 1, where
     * "variable of type MultiReadAccessible<@a T,1,@a I>"[index] returns @a T.
     * Thus say Map is of type MultiReadAccessible<T,3,I>, then Map[2]
     * returns a const reference to a
     * MultiReadAccessible<@a T,2,@a I>, Map[2][3] returns a const reference
     * to a MultiReadAccessible<@a T,1,@a I> and Map[2][3][1] returns a @a T.
     *
     * Because only const references and element copies (in case @a N == 1)
     * are returned, it is not possible to write to MultiReadAccessibles.
     *
     * You can also access the values by using @link at(const InputIterator
     * first, const InputIterator last) const at @endlink.
     */
    template<typename T,
         MultiAccessibleDefs::SizeType N,
         typename R = const T&,
         typename I = MultiAccessibleDefs::SizeType
         >
    class MultiReadAccessible
    {
    public:
        /**
         *  @brief The type of the values that are accessed.
         */
        typedef T ValueType;

        /**
         *  @brief The type for sizes used in this class.
         */
        typedef MultiAccessibleDefs::SizeType SizeType;

        /**
         *  @brief The type of indexes in the [] operator.
         */
        typedef I IndexType;

        /**
         * @brief The return type for the operator [] in the first Dimension
         */
        typedef R ReturnType;

        /**
         *  @brief Constant reference to the next hyperplane (what is
         *  returned after [] has been applied once).
         */
        typedef const MultiReadAccessible<ValueType, N-1, ReturnType, IndexType>& ConstReference;

        /**
         *  @brief Destructor.
         */
        virtual ~MultiReadAccessible()
        {
        };

        /**
         *  @brief Select a hyperplane.
         *
         *  Returns a ConstReference to the hyperplane specified by the
         *  @a index.
         */
        virtual ConstReference operator[](const IndexType& index) const = 0;

        /**
         *  @brief Select an element.
         *
         *  @param first (in) InputIterator pointing to the first index.
         *  @param last  (in) InputIterator pointing past the last index.
         *  @return Value of the element specified by first and last.
         *
         *  http://www.sgi.com/tech/stl/InputIterator.html
         *
         *  Get element specified by the indices from *@a first up to
         *  but not including *@a last. So you can access the values
         *  with e.g. every STL container and even C arrays.
         *
         *  The distance from @a first to @a last must be N.
         *
         */
        template<class InputIterator>
        ValueType at(const InputIterator& first, const InputIterator& last) const
        {
            InputIterator temp = first;
            return (*this)[*first].at(++temp, last);
        }

        /**
         *  @brief Get the size of a dimension.
         *
         *  @param dim (in) The dimension to get the size for.
         *  @return Size of @a dim.
         */
        virtual SizeType dimSize(const SizeType& dim) const = 0;

        /**
         *  @brief Get the size of the total structure.
         *
         *  @return dimSize(0) * dimSize(1) * ... * dimSize(N-1).
         */
        virtual SizeType size() const
        {
            SizeType s = 1;
            for (SizeType i = 0; i < N; ++i) s *= dimSize(i);
            return s;
        }
    };

    template<typename T,
         typename ReturnType,
         typename I
         >
    class MultiReadAccessible<T, 1, ReturnType, I>
    {
    public:
        typedef T ValueType;
        typedef MultiAccessibleDefs::SizeType SizeType;
        typedef I IndexType;

        virtual ~MultiReadAccessible()
        {
        };

        virtual ReturnType operator[](const IndexType& index) const = 0;

        template<class InputIterator>
#ifndef WNS_NDEBUG
        ValueType at(const InputIterator& first, const InputIterator& last) const
#else
        ValueType at(const InputIterator& first, const InputIterator& /*last*/) const
#endif
        {
            assure(std::distance(first, last) == 1, "range error");
            return (*this)[*first];
        }

        virtual SizeType dimSize(const SizeType& dim) const = 0;

        virtual SizeType size() const
        {
            return dimSize(0);
        }
    };

    /**
     * @brief Interface and base for classes which the [] operator can be
     * applied to multiple times, allowing write access.
     *
     * @ingroup cppgoodies
     *
     * @param T Type of the values we want to access.
     * @param N Number of dimensions.
     * @param I Type of the index in the [] operator. This defaults to
     * MultiAccessibleDefs::SizeType.
     *
     * This class specifies following interface:
     * "variable of type MultiWriteAccessible<@a T,@a N,@a I>"[index]
     * returns a reference to a MultiWriteAccessible<@a T,@a N-1,@a I>,
     * unless @a N == 1, where "variable of type
     * MultiWriteAccessible<@a T,1,@a I>"[index] returns a reference
     * to a @a T.
     * Thus say Map is of type MultiWriteAccessible<@a T,3,@a I>,
     * then Map[2] returns a reference to a
     * MultiWriteAccessible<@a T,2,@a I>, Map[2][3] returns a reference
     * to a MultiWriteAccessible<@a T,1,@a I>
     * and Map[2][3][1] returns a reference to a @a T.
     *
     * Because only references are returned, you can write to
     * MultiWriteAccessibles, e.g.: Map[2][3][1] = 5.
     *
     * You can also access the values by using @link at(const InputIterator
     * first, const InputIterator last) const at @endlink.
     */
    template<typename T,
         MultiAccessibleDefs::SizeType N,
         typename I = MultiAccessibleDefs::SizeType>
    class MultiWriteAccessible
    {
    public:
        /**
         *  @brief The type of the values that are accessed.
         */
        typedef T ValueType;

        /**
         *  @brief The type for sizes used in this class.
         */
        typedef MultiAccessibleDefs::SizeType SizeType;

        /**
         *  @brief The type of indexes in the [] operator.
         */
        typedef I IndexType;

        /**
         *  @brief Reference to the next hyperplane (what is returned
         *  after [] has been applied once).
         */
        typedef MultiWriteAccessible<ValueType, N-1, IndexType>& Reference;

        /**
         *  @brief Desctructor.
         */
        virtual ~MultiWriteAccessible()
        {
        };

        /**
         *  @brief Select a hyperplane.
         *
         *  Returns a Reference to the hyperplane specified by the index.
         */
        virtual Reference operator[](const IndexType& index) = 0;

        /**
         *  @brief Select an element.
         *
         *  @param first (in) InputIterator pointing to the first index.
         *  @param last  (in) InputIterator pointing past the last index.
         *  @return Reference to the element specified by first and last.
         *
         *  http://www.sgi.com/tech/stl/InputIterator.html
         *
         *  Get a reference to the element specified by the indices from
         *  *@a first up to but not including *@a last. So you can
         *  access the values with e.g. every STL container and even C
         *  arrays.
         *
         *  The distance from @a first to @a last must be N.
         *
         */
        template<class InputIterator>
        ValueType& at(const InputIterator& first, const InputIterator& last)
        {
            InputIterator temp = first;
            return (*this)[*first].at(++temp, last);
        }

        /**
         *  @brief Get the size of a dimension.
         *
         *  @param dim (in) The dimension to get the size for.
         *  @return Size of @a dim.
         */
        virtual SizeType dimSize(const SizeType& dim) const = 0;

        /**
         *  @brief Get the size of the total structure.
         *
         *  @return dimSize(0) * dimSize(1) * ... * dimSize(N-1).
         */
        virtual SizeType size() const
        {
            SizeType s = 1;
            for (SizeType i = 0; i < N; ++i) s *= dimSize(i);
            return s;
        }
    };

    template<typename T,
         typename I>
    class MultiWriteAccessible<T, 1, I>
    {
    public:
        typedef T ValueType;
        typedef MultiAccessibleDefs::SizeType SizeType;
        typedef I IndexType;

        typedef ValueType& Reference;

        virtual ~MultiWriteAccessible() {};

        virtual Reference operator[](const IndexType& index) = 0;

        template<class InputIterator>
        Reference at(const InputIterator& first, const InputIterator& last)
        {
            assert(std::distance(first, last) == 1);
            return (*this)[*first];
        }

        virtual SizeType dimSize(const SizeType& dim) const = 0;

        virtual SizeType size() const
        {
            return dimSize(0);
        }
    };

    /**
     *  @brief Interface and base for classes which the [] operator can be
     *  applied to multiple times, allowing write and read access.
     *
     * @ingroup cppgoodies
     *
     *  @param T Type of the values we want to access.
     *  @param N Number of dimensions.
     *  @param I Type of the index in the [] operator. This defaults to
     *  MultiAccessibleDefs::SizeType.
     *
     *  This class is a combination of MultiReadAccessible and MultiWriteAccessible.
     */
    template<typename T,
         MultiAccessibleDefs::SizeType N,
         typename R = const T&,
         typename I = MultiAccessibleDefs::SizeType>
    class MultiReadWriteAccessible : public MultiReadAccessible<T, N, R, I>,
                     public MultiWriteAccessible<T, N, I>
    {
    public:
        /**
         *  @brief The type of the values that are accessed.
         */
        typedef T ValueType;

        /**
         *  @brief The type for sizes used in this class.
         */
        typedef MultiAccessibleDefs::SizeType SizeType;

        /**
         *  @brief The type of indexes in the [] operator.
         */
        typedef I IndexType;

        /**
         * @brief The return type for the operator [] in the first Dimension
         */
        typedef R ReturnType;

        /**
         *  @brief Destructor.
         */
        virtual ~MultiReadWriteAccessible()
        {
        };

        /**
         *  @brief Get the size of a dimension.
         *
         *  @param dim (in) The dimension to get the size for.
         *  @return Size of dimension @a dim.
         */
        virtual SizeType dimSize(const SizeType& dim) const = 0;

        /**
         *  @brief Get the size of the total structure.
         *
         *  @return dimSize(0) * dimSize(1) * ... * dimSize(N-1).
         */
        virtual SizeType size() const
        {
            SizeType s = 1;
            for (SizeType i = 0; i < N; ++i) s *= dimSize(i);
            return s;
        }
    };
}
}

#endif // _MULTIACCESSIBLE_HPP
