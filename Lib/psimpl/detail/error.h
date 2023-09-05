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

#ifndef PSIMPL_DETAIL_ERROR
#define PSIMPL_DETAIL_ERROR


#include <numeric>
#include <cmath>


namespace psimpl {
    namespace error
{
    /*!
        \brief POD structure for storing statistical values.
    */
    struct statistics
    {
        statistics () :
            max (0),
            sum (0),
            mean (0),
            std (0)
        {}

        double max;
        double sum;
        double mean;
        double std;     //! standard deviation
    };

    // ---------------------------------------------------------------------------------------------

    namespace detail
    {
        /*!
            \brief Computes various statistics for the range [first, last)

            \param[in] first   the first value
            \param[in] last    one beyond the last value
            \return            the calculated statistics
        */
        template
        <
            class InputIterator
        >
        inline statistics compute_statistics (
            InputIterator first,
            InputIterator last)
        {
            typedef typename std::iterator_traits <InputIterator>::value_type value_type;
            typedef typename std::iterator_traits <InputIterator>::difference_type diff_type;

            statistics stats;

            diff_type count = std::distance (first, last);
            if (count == 0) {
                return stats;
            }

            value_type init = 0;
            stats.max = static_cast <double> (*std::max_element (first, last));
            stats.sum = static_cast <double> (std::accumulate (first, last, init));
            stats.mean = stats.sum / static_cast<double>(count);
            std::for_each(first, last, [&stats](auto& x) { x -= stats.mean; });
            stats.std = std::sqrt (static_cast <double> (std::inner_product (first, last, first, init)) / static_cast<double>(count));
            return stats;
        }
    }

    // ---------------------------------------------------------------------------------------------


    /*!
        \brief Squared positional error between a polyline and its simplification.

        \note all calculated error values are of the type util::select_calculation_type <ForwardIterator2>::type
    */
    template
    <
        unsigned DIM,
        typename ForwardIterator1,
        typename ForwardIterator2,
        typename OutputIterator
    >
    struct positional
    {
        typedef typename std::iterator_traits <ForwardIterator1>::difference_type ori_diff_type;
        typedef typename std::iterator_traits <ForwardIterator2>::difference_type sim_diff_type;

        /*!
            \brief Computes the squared positional error between a polyline and its simplification.
        */
        static OutputIterator compute (
            ForwardIterator1 original_first,
            ForwardIterator1 original_last,
            ForwardIterator2 simplified_first,
            ForwardIterator2 simplified_last,
            OutputIterator result,
            bool* valid=0)
        {
            ori_diff_type original_coordCount = std::distance (original_first, original_last);
            ori_diff_type original_pointCount = DIM     // protect against zero DIM
                                                ? original_coordCount / DIM
                                                : 0;

            sim_diff_type simplified_coordCount = std::distance (simplified_first, simplified_last);
            sim_diff_type simplified_pointCount = DIM   // protect against zero DIM
                                                ? simplified_coordCount / DIM
                                                : 0;

            // validate input
            if (original_coordCount % DIM || original_pointCount < 2 ||
                simplified_coordCount % DIM || simplified_pointCount < 2 ||
                original_pointCount < simplified_pointCount ||
                !math::equal <DIM> (simplified_first, original_first))
            {
                if (valid) {
                    *valid = false;
                }
                return result;
            }

            // define (simplified) line segment S(simplified_prev, simplified_first)
            ForwardIterator2 simplified_prev = simplified_first;
            std::advance (simplified_first, DIM);

            // process each simplified line segment
            while (simplified_first != simplified_last) {
                // process each original point until it equals the end of the line segment
                while (original_first != original_last &&
                       !math::equal <DIM> (simplified_first, original_first))
                {
                    *result = math::segment_distance2 <DIM> (simplified_prev, simplified_first,
                                                             original_first);
                    ++result;
                    std::advance (original_first, DIM);
                }
                // update line segment S
                simplified_prev = simplified_first;
                std::advance (simplified_first, DIM);
            }
            // check if last original point matched
            if (original_first != original_last) {
                *result = 0;
                ++result;
            }

            if (valid) {
                *valid = original_first != original_last;
            }
            return result;
        }
    };

    /*!
        \brief Squared positional error statistics between a polyline and its simplification.

        todo move statistics class and compute_statistics function from math to here, set statistics value type to calc type of simplification???
    */
    template
    <
        unsigned DIM,
        typename ForwardIterator1,
        typename ForwardIterator2
    >
    struct positional_statistics
    {
        /*!
            \brief Computes the squared positional error statistics between a polyline and its simplification.
        */
        static statistics compute (
            ForwardIterator1 original_first,
            ForwardIterator1 original_last,
            ForwardIterator2 simplified_first,
            ForwardIterator2 simplified_last,
            bool* valid=0)
        {
            typedef typename std::iterator_traits <ForwardIterator1>::difference_type diff_type;

            diff_type pointCount = std::distance (original_first, original_last) / DIM;
            util::scoped_array <double> errors (static_cast<unsigned>(pointCount));

            diff_type errorCount = std::distance (
                errors.get (),
                positional
                <
                    DIM,
                    ForwardIterator1,
                    ForwardIterator2,
                    double*
                >::compute (original_first, original_last,
                            simplified_first, simplified_last,
                            errors.get (), valid));

            std::transform(
                errors.get(),
                errors.get() + errorCount,
                errors.get(),
                [](double f) -> auto { return sqrt(f); } // call the double version
            );

            return detail::compute_statistics (errors.get (), errors.get () + errorCount);
        }
    };
}}


#endif // PSIMPL_DETAIL_ERROR
