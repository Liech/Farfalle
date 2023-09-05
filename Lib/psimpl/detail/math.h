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

#ifndef PSIMPL_DETAIL_MATH
#define PSIMPL_DETAIL_MATH


#include <algorithm>
#include "util.h"


namespace psimpl {
    namespace math
{

    namespace detail
    {

    }

    // ---------------------------------------------------------------------------------------------

    /*!
        \brief Determines if two points have the exact same coordinates.

        Note that the value type of InputIterator1 is leading. If InputIterator2 has a different value
        type its values will be explicitly cast before comparing.

        \param[in] p1       the first coordinate of the first point
        \param[in] p2       the first coordinate of the second point
        \return             true when the points are equal; false otherwise
    */
    template
    <
        unsigned DIM,
        typename InputIterator1,
        typename InputIterator2
    >
    inline bool equal (
        InputIterator1 p1,
        InputIterator2 p2)
    {
        typedef typename std::iterator_traits <InputIterator1>::value_type value_type;

        // todo candidate for optimization based on random iterator or pointer types
        for (unsigned d = 0; d < DIM; ++d, ++p1, ++p2) {
            if (*p1 != static_cast <value_type> (*p2)) {
                return false;
            }
        }
        return true;
    }

    /*!
        \brief Adds two points to each other.

        Assumes type T is the calculation type of InputIterator.

        \param[in] p1       the first coordinate of the first point
        \param[in] p2       the first coordinate of the second point
        \param[in] result   stores the result of p1 + p2
    */
    template
    <
        unsigned DIM,
        typename InputIterator,
        typename T
    >
    inline void add (
        InputIterator p1,
        T* p2,
        T* result)
    {
        // todo candidate for optimization based on random iterator or pointer types
        for (unsigned d = 0; d < DIM; ++d, ++p1) {
            result [d] = static_cast <T> (*p1) + p2 [d];
        }
    }

    /*!
        \brief Subtracts two points from each other.

        Assumes type T is the calculation type of InputIterator1.

        \param[in] p1       the first coordinate of the first point
        \param[in] p2       the first coordinate of the second point
        \param[in] result   stores the result of p1 - p2
    */
    template
    <
        unsigned DIM,
        typename InputIterator1,
        typename InputIterator2,
        typename T
    >
    inline void subtract (
        InputIterator1 p1,
        InputIterator2 p2,
        T* result)
    {
        // todo candidate for optimization based on random iterator or pointer types or when InputIterator1==InputIterator2
        for (unsigned d = 0; d < DIM; ++d, ++p1, ++p2) {
            result [d] = static_cast <T> (*p1) - *p2;
        }
    }

    /*!
        \brief Multiplies a point with a value.

        \param[in] p    the first coordinate of the first point
        \param[in] v    the multiplication value
    */
    template
    <
        unsigned DIM,
        typename T
    >
    inline void multiply (
        T* p,
        T  v)
    {
        // todo candidate for optimization using compile time loop unrolling
        for (unsigned d = 0; d < DIM; ++d) {
            p [d] *= v;
        }
    }

    /*!
        \brief Computes the dot product of two vectors.

        \param[in] v1   the first coordinate of the first vector
        \param[in] v2   the first coordinate of the second vector
        \return         the dot product v1 * v2
    */
    template
    <
        unsigned DIM,
        typename T
    >
    inline T dot (
        T* v1,
        T* v2)
    {
        // todo candidate for optimization based on random iterator or pointer types
        T result = 0;

        for (unsigned d = 0; d < DIM; ++d) {
            result += v1 [d] * v2 [d];
        }
        return result;
    }

    /*!
        \brief Computes the squared distance of two points.

        \param[in] p1   the first coordinate of the first point
        \param[in] p2   the first coordinate of the second point
        \return         the squared distance
    */
    template
    <
        unsigned DIM,
        typename ForwardIterator1,
        typename ForwardIterator2
    >
    inline typename util::select_calculation_type <ForwardIterator1>::type point_distance2 (
        ForwardIterator1 p1,
        ForwardIterator2 p2)
    {
        // todo candidate for optimization based on random iterator or pointer types
        typedef typename util::select_calculation_type <ForwardIterator1>::type calc_type;
        calc_type result = 0;

        for (unsigned d = 0; d < DIM; ++d, ++p1, ++p2) {
            result += (static_cast <calc_type> (*p1) - *p2) *
                      (static_cast <calc_type> (*p1) - *p2);
        }
        return result;
    }

    /*!
        \brief Computes the squared distance between an infinite line (l1, l2) and a point p.

        \param[in] l1   the first coordinate of the first point on the line
        \param[in] l2   the first coordinate of the second point on the line
        \param[in] p    the first coordinate of the test point
        \return         the squared distance
    */
    template
    <
        unsigned DIM,
        typename ForwardIterator
    >
    typename util::select_calculation_type <ForwardIterator>::type line_distance2 (
        ForwardIterator l1,
        ForwardIterator l2,
        ForwardIterator p)
    {
        typedef typename util::select_calculation_type <ForwardIterator>::type calc_type;

        calc_type v [DIM];                 // vector l1 --> l2
        calc_type w [DIM];                 // vector l1 --> p

        subtract <DIM> (l2, l1, v);
        subtract <DIM> ( p, l1, w);

        calc_type cv = dot <DIM> (v, v);   // squared length of v
        calc_type cw = dot <DIM> (w, v);   // project w onto v

        // avoid problems with divisions when calc_type is an integer type (unlikely)
        calc_type fraction = cv == 0 ? 0 : cw / cv;
        multiply <DIM> (v, fraction);

        calc_type proj [DIM];    // p projected onto segement (s1, s2)
        add <DIM> (l1, v, proj);

        return point_distance2 <DIM> (proj, p);
    }

    /*!
        \brief Computes the squared distance between a ray (r1, r2) and a point p.

        \param[in] r1   the first coordinate of the start point of the ray
        \param[in] r2   the first coordinate of a point on the ray
        \param[in] p    the first coordinate of the test point
        \return         the squared distance
    */
    template
    <
        unsigned DIM,
        typename ForwardIterator
    >
    typename util::select_calculation_type <ForwardIterator>::type ray_distance2 (
        ForwardIterator r1,
        ForwardIterator r2,
        ForwardIterator p)
    {
        typedef typename util::select_calculation_type <ForwardIterator>::type calc_type;

        calc_type v [DIM];    // vector r1 --> r2
        calc_type w [DIM];    // vector r1 --> p

        subtract <DIM> (r2, r1, v);
        subtract <DIM> (p,  r1, w);

        calc_type cv = dot <DIM> (v, v);    // squared length of v
        calc_type cw = dot <DIM> (w, v);    // project w onto v

        if (cw <= 0) {
            // projection of w lies to the left of r1 (not on the ray)
            return point_distance2 <DIM> (r1, p);
        }

        // avoid problems with divisions when calc_type is an integer type (unlikely)
        calc_type fraction = cv == 0 ? 0 : cw / cv;
        multiply <DIM> (v, fraction);

        calc_type proj [DIM];    // p projected onto segement (s1, s2)
        add <DIM> (r1, v, proj);

        return point_distance2 <DIM> (proj, p);
    }

    /*!
        \brief Computes the squared distance between a line segment (s1, s2) and a point p.

        \param[in] s1   the first coordinate of the start point of the segment
        \param[in] s2   the first coordinate of the end point of the segment
        \param[in] p    the first coordinate of the test point
        \return         the squared distance
    */
    template
    <
        unsigned DIM,
        typename ForwardIterator1,
        typename ForwardIterator2
    >
    typename util::select_calculation_type <ForwardIterator1>::type segment_distance2 (
        ForwardIterator1 s1,
        ForwardIterator1 s2,
        ForwardIterator2 p)
    {
        typedef typename util::select_calculation_type <ForwardIterator1>::type calc_type;

        calc_type v [DIM];    // vector s1 --> s2
        calc_type w [DIM];    // vector s1 --> p

        subtract <DIM> (s2, s1, v);
        subtract <DIM> (p,  s1, w);

        calc_type cw = dot <DIM> (w, v);    // project w onto v
        if (cw <= 0) {
            // projection of w lies to the left of s1
            return point_distance2 <DIM> (s1, p);
        }

        calc_type cv = dot <DIM> (v, v);    // squared length of v
        if (cv <= cw) {
            // projection of w lies to the right of s2
            return point_distance2 <DIM> (s2, p);
        }

        calc_type fraction = cw / cv;        // cv is always > 0
        multiply <DIM> (v, fraction);

        calc_type proj [DIM];                // p projected onto segement (s1, s2)
        add <DIM> (s1, v, proj);

        return point_distance2 <DIM> (proj, p);
    }

}}

#endif // PSIMPL_DETAIL_MATH
