/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is
 * 'psimpl - generic n-dimensional polyline simplification'.
 *
 * The Initial Developer of the Original Code is
 * Elmar de Koning (edekoning@gmail.com).
 *
 * Portions created by the Initial Developer are Copyright (C) 2010-2011
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * ***** END LICENSE BLOCK ***** */

/*
    psimpl - generic n-dimensional polyline simplification
    Copyright (C) 2010-2011 Elmar de Koning, edekoning@gmail.com

    This file is part of psimpl and is hosted at SourceForge:
    http://psimpl.sf.net/, http://sf.net/projects/psimpl/
*/

#ifndef PSIMPL_DETAIL_UTIL
#define PSIMPL_DETAIL_UTIL


#include <algorithm>


namespace psimpl {
    namespace util
{

    //! \note scoped_array, and all type traits, will be removed when moving to C++0x.
    namespace detail
    {
        // ---------- type traits ------------------------------------------------------------------
        template <typename T>
        struct remove_const
        {
            typedef T type;
        };
        template <typename T>
        struct remove_const <const T>
        {
            typedef T type;
        };

        template <typename T>
        struct remove_volatile
        {
            typedef T type;
        };
        template <typename T>
        struct remove_volatile <volatile T>
        {
            typedef T type;
        };

        template <typename T>
        struct remove_cv
        {
            typedef typename remove_const <typename remove_volatile <T>::type>::type type;
        };

        template <typename T>
        struct promote_to_floating_point_helper
        {
            typedef T type;
        };
        template <typename T>
        struct promote_to_floating_point
        {
            typedef typename promote_to_floating_point_helper <typename remove_cv <T>::type>::type type;
        };
    }

    // ---------------------------------------------------------------------------------------------

    /*!
        \brief Copies and advances the key from [first, first+DIM) to [result, result+DIM).

        \param[in,out] first    the first coordinate of the key
        \param[in,out] result   destination of the copied coordinates
    */
    template
    <
        unsigned DIM,
        typename InputIterator,
        typename OutputIterator
    >
    inline void copy_key_advance (
        InputIterator& first,
        OutputIterator& result)
    {
        for (unsigned d = 0; d < DIM; ++d) {
            *result = *first;
            ++first;
            ++result;
        }
    }

    /*!
        \brief Copies the key from [first, first+DIM) to [result, result+DIM).

        \param[in]     first    the first coordinate of the key
        \param[in,out] result   destination of the copied coordinates
    */
    template
    <
        unsigned DIM,
        typename InputIterator,
        typename OutputIterator
    >
    inline void copy_key (
        InputIterator first,
        OutputIterator& result)
    {
        copy_key_advance <DIM> (first, result);
    }

    /*!
        \brief Copies all the keys from [first, last) to result.

        \param[in]     first    the first coordinate of the first polyline point
        \param[in]     last     one beyond the last coordinate of the last polyline point
        \param[in]     keys     defines which points in [first, last) are keys
        \param[in,out] result   destination of the copied coordinates
    */
    template
    <
        unsigned DIM,
        typename ForwardIterator,
        typename InputIterator,
        typename OutputIterator
    >
    inline void copy_keys (
        ForwardIterator first,
        ForwardIterator last,
        InputIterator keys,
        OutputIterator& result)
    {
        while (first != last) {
            if (*keys++) {
                util::copy_key <DIM> (first, result);
            }
            std::advance (first, DIM);
        }
    }

    /*!
        \brief Increments the iterator by n points if possible.

        If there are fewer than n points remaining the iterator will be incremented to the last
        point.

        \param[in,out] it           iterator to be advanced
        \param[in]     n            number of points to advance
        \param[in,out] remaining    number of points remaining after it
        \return                     the actual amount of points that the iterator advanced
    */
    template
    <
        unsigned DIM,
        typename InputIterator,
        typename Distance
    >
    inline Distance forward (
        InputIterator& it,
        Distance n,
        Distance& remaining)
    {
        n = std::min (n, remaining);
        std::advance (it, n * DIM);
        remaining -= n;
        return n;
    }

    /*!
        \brief Decrements the iterator by n points.

        \param[in,out] it           iterator to be advanced
        \param[in]     n            number of points to advance
        \param[in,out] remaining    number of points remaining after it
    */
    template
    <
        unsigned DIM,
        typename BidirectionalIterator,
        typename Distance
    >
    inline void backward (
        BidirectionalIterator& it,
        Distance n,
        Distance& remaining)
    {
        typedef typename std::iterator_traits <BidirectionalIterator>::difference_type diff_type;

        std::advance (it, -n * static_cast <diff_type> (DIM));
        remaining += n;
    }

    // ---------------------------------------------------------------------------------------------

    /*!
        \brief A smart pointer for holding a dynamically allocated array.
    */
    template <typename T>
    class scoped_array
    {
    public:
        scoped_array (unsigned n) {
            data = new T [n];
        }

        ~scoped_array () {
            delete [] data;
        }

        T& operator [] (int offset) {
            return data [offset];
        }

        const T& operator [] (int offset) const {
            return data [offset];
        }

        T* get () const {
            return data;
        }

        void swap (scoped_array& b) {
            T* tmp = b.data;
            b.data = data;
            data = tmp;
        }

    private:
        scoped_array (const scoped_array&);
        scoped_array& operator= (const scoped_array&);

    private:
        T* data;
    };

    template <typename T> inline void swap (scoped_array <T>& a, scoped_array <T>& b) {
        a.swap (b);
    }

    // ---------------------------------------------------------------------------------------------

    //! \brief Meta function: selects a calculation type based on an interator type.
    template <typename Iterator>
    struct select_calculation_type
    {
        typedef typename detail::promote_to_floating_point
                            <
                                typename std::iterator_traits <Iterator>::value_type
                            >::type type;
    };

}}

/*!
    \brief Defines a type promotion from an integer type to a floating point type.

    \note Only use this macro outside any namespace!
*/
#define PSIMPL_DEF_FLOATING_POINT_PROMOTION(intType,fpType)    \
    namespace psimpl { namespace util { namespace detail {     \
        template <>                                            \
        struct promote_to_floating_point_helper <intType>      \
        {                                                      \
            typedef fpType type;                               \
        };                                                     \
    }}}


#endif // PSIMPL_DETAIL_UTIL
