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

#ifndef WNS_CONTAINER_MATRIX_HPP
#define WNS_CONTAINER_MATRIX_HPP

#include <WNS/container/MultiAccessible.hpp>
#include <WNS/Assure.hpp>

#include <vector>
#include <sstream>

namespace wns { namespace container {

    /**
     * @brief This template class provides a simple N-dimensional "matrix".
     */
    template<typename T, MultiAccessibleDefs::SizeType N, typename R = const T&>
    class Matrix :
        public MultiReadWriteAccessible<T, N, R>
    {
        typedef MultiReadWriteAccessible<T, N, R> SuperType;
    public:
        typedef typename SuperType::ValueType ValueType;
        typedef typename SuperType::IndexType IndexType;
        typedef MultiAccessibleDefs::SizeType SizeType;

        typedef Matrix<T, N - 1, R> HyperplaneType;
        typedef HyperplaneType& Reference;
        typedef const HyperplaneType& ConstReference;

        Matrix() :
            SuperType(),
            v()
        {
        }

        Matrix(const SizeType sizes[N],
               const ValueType defaultValue = ValueType()) :
              SuperType(),
              v(sizes[0], HyperplaneType(&sizes[1], defaultValue))
        {
        }

        virtual
        ~Matrix()
        {
        }

        virtual Reference
        operator[](const IndexType& index)
        {
            assure(!isEmpty(), "Matrix is empty");
            assure(index < v.size(), "out of range: index " << index << " >= " << v.size());
            return v[index];
        }

        virtual ConstReference
        operator[](const IndexType& index) const
        {
            assure(!isEmpty(), "Matrix is empty");
            assure(index < v.size(), "out of range: index " << index << " >= " << v.size());
            return v[index];
        }

        virtual bool
        operator==(const Matrix& other) const
        {
            for (size_t xx = 0; xx < v.size(); ++xx)
            {
                if (other[xx] != v[xx])
                {
                    return false;
                }
            }
            return true;
        }

        virtual bool
        operator!=(const Matrix& other) const
        {
            return !(*this == other);
        }

        // this is for backward compatibility
        virtual SizeType
        getSize(const SizeType& dim) const
        {
            return dimSize(dim);
        }

        virtual SizeType
        dimSize(const SizeType& dim) const
        {
            if (dim == 0)
            {
                return v.size();
            }
            else
            {
                if (isEmpty())
                {
                    return 0;
                }
                else
                {
                    return v[0].dimSize(dim - 1);
                }
            }
        }

        //! Check if the Matrix is empty
        virtual bool
        isEmpty() const
        {
            return v.empty();
        }

    private:
        std::vector<HyperplaneType> v;
    };

    /**
     * @brief One dimensional specialization of Matrix
     */
    template<typename T,
         typename R>
    class Matrix<T, 1, R> :
        public MultiReadWriteAccessible<T, 1, R>
    {
        typedef MultiReadWriteAccessible<T, 1, R> SuperType;
    public:
        typedef typename SuperType::ValueType ValueType;
        typedef typename SuperType::IndexType IndexType;
        typedef MultiAccessibleDefs::SizeType SizeType;

        typedef ValueType& Reference;
        typedef const ValueType& ConstReference;

        Matrix() :
            SuperType(),
            v()
        {
        }

        Matrix(const SizeType sizes[1],
               const ValueType defaultValue = ValueType()) :
            SuperType(),
            v(sizes[0], defaultValue)
        {
        }

        virtual
        ~Matrix()
        {
        }

            virtual bool
        operator==(const Matrix& other) const
        {
            for(size_t xx = 0; xx < v.size(); ++xx)
            {
                if (other[xx] != v[xx])
                {
                    return false;
                }
            }
            return true;
        }

        virtual bool
        operator!=(const Matrix& other) const
        {
            return !(*this == other);
        }

        virtual Reference
        operator[](const IndexType& index)
        {
            assure(!isEmpty(), "Matrix is empty");
            assure(index < v.size(), "out of range: index " << index << " >= " << v.size());
            return v[index];
        }

        virtual typename SuperType::ReturnType
        operator[](const IndexType& index) const
        {
            assure(!isEmpty(), "Matrix is empty");
            assure(index < v.size(), "out of range: index " << index << " >= " << v.size());
            return v[index];
        }

        virtual SizeType
        getSize(const SizeType& dim) const
        {
            return dimSize(dim);
        }

#ifndef WNS_NDEBUG
        virtual SizeType
        dimSize(const SizeType& dim) const
#else
        virtual SizeType
        dimSize(const SizeType& /*dim*/) const
#endif
        {
            assure(dim == 0, "Dimension must be zero");
            return v.size();
        }

        //! Check if the matrix is empty.
        virtual bool
        isEmpty() const
        {
            return v.empty();
        }

    private:
        std::vector<ValueType> v;
    };

    /**
     * @brief some often used types
     */
    typedef wns::container::Matrix<double, 2u> MatrixDouble;

    /**
     * @brief output methods for matrices
     */
    inline
    std::ostream&
    operator <<(std::ostream& os, const wns::container::MatrixDouble& matrix)
    {
        unsigned int rows=matrix.getSize(0);
        unsigned int cols=matrix.getSize(1);
        os << "(";
        for(unsigned int row=0; row<rows; row++)
        {
            os << "(";
            for(unsigned int col=0; col<cols; col++)
            {
                os << matrix[row][col];
                if (col<cols-1) os << ", ";
            }
            os << ")";
            if (row<rows-1) os << "," << std::endl;
        }
        os << ")";
        return os;
    }
}
}


#endif // WNS_CONTAINER_MATRIX_HPP

/**
 * @file
 */
