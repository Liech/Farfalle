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

#ifndef PSIMPL_DETAIL_ALGO
#define PSIMPL_DETAIL_ALGO


#include <queue>
#include <stack>
#include "math.h"
#include "util.h"


namespace psimpl {
    namespace algo
{

    namespace detail
    {
        /*!
            \brief Key finder.
        */
        template
        <
            unsigned DIM,
            typename RandomAccessIterator
        >
        struct find_key
        {
            typedef typename std::iterator_traits <RandomAccessIterator>::difference_type diff_type;
            typedef typename util::select_calculation_type <RandomAccessIterator>::type calc_type;

            /*!
                \brief Basic information about a single found key.

                Note that the key is valid (found) if 0 < index.
            */
            struct key {
                key (diff_type index=0, calc_type dist2=0) :
                    index (index), dist2 (dist2) {}

                diff_type index;    //!< coord index of the key
                calc_type dist2;    //!< squared distance of the key
            };

            /*!
                \brief Finds the key in a sub polyline.

                The key is the point which has the highest perpendicular distance to the line-segment
                defined by the first and last point of the sub polyline.

                \param[in] poly     the first coordinate of the first polyline point
                \param[in] first    the first coordinate index of the first point of the sub polyline
                \param[in] last     the first coordinate index of the last point of the sub polyline
                \return             the found key between poly [first] and poly [last]
            */
            static key apply (
                RandomAccessIterator poly,
                diff_type first,
                diff_type last)
            {
                key result;
                // define segment S (s1, s2)
                RandomAccessIterator s1 = poly;
                RandomAccessIterator s2 = poly;
                std::advance (s1, first);
                std::advance (s2, last);
                // (coord)index of the current test point
                diff_type index = first + DIM;
                std::advance (poly, first + DIM);

                // test all internal points against segment S (s1, s2)
                while (index < last) {
                    calc_type d2 = math::segment_distance2 <DIM> (s1, s2, poly);

                    if (result.dist2 <= d2) {
                        // update maximum squared distance and the point it belongs to
                        result.index = index;
                        result.dist2 = d2;
                    }
                    index += DIM;
                    std::advance (poly, DIM);
                }
                return result;
            }
        };
    }

    /*!
        \brief Nth point routine (NP).
    */
    template
    <
        unsigned DIM,
        typename ForwardIterator,
        typename Size,
        typename OutputIterator
    >
    struct nth_point
    {
        typedef typename std::iterator_traits <ForwardIterator>::difference_type diff_type;

        /*!
            \brief Performs the nth point simplification routine.
        */
        static OutputIterator simplify (
            ForwardIterator first,
            ForwardIterator last,
            Size n,
            OutputIterator result)
        {
            diff_type coordCount = std::distance (first, last);
            diff_type pointCount = DIM               // protect against zero DIM
                                   ? coordCount / DIM
                                   : 0;

            // validate input and check if simplification required
            if (coordCount % DIM || pointCount < 3 || n < 2) {
                return std::copy (first, last, result);
            }

            diff_type remaining = pointCount - 1;    // the number of points remaining after key
            ForwardIterator key = first;             // indicates the current key

            // the first point is always part of the simplification
            util::copy_key <DIM> (key, result);

            // copy each nth point
            while (util::forward <DIM> (key, static_cast <diff_type> (n), remaining)) {
                util::copy_key <DIM> (key, result);
            }

            return result;
        }
    };

    /*!
        \brief Radial distance routine (RD).
    */
    template
    <
        unsigned DIM,
        typename ForwardIterator,
        typename Distance,
        typename OutputIterator
    >
    struct radial_distance
    {
        typedef typename std::iterator_traits <ForwardIterator>::difference_type diff_type;

        /*!
            \brief Performs the radial distance simplification routine.
        */
        static OutputIterator simplify (
            ForwardIterator first,
            ForwardIterator last,
            Distance tol,
            OutputIterator result)
        {
            diff_type coordCount = std::distance (first, last);
            diff_type pointCount = DIM          // protect against zero DIM
                                   ? coordCount / DIM
                                   : 0;
            Distance tol2 = tol * tol;          // squared distance tolerance

            // validate input and check if simplification required
            if (coordCount % DIM || pointCount < 3 || tol2 <= 0) {
                return std::copy (first, last, result);
            }

            ForwardIterator current = first;    // indicates the current key
            ForwardIterator next = first;       // used to find the next key

            // the first point is always part of the simplification
            util::copy_key_advance <DIM> (next, result);

            // Skip first and last point, because they are always part of the simplification
            for (diff_type index = 1; index < pointCount - 1; ++index) {
                if (tol2 <= math::point_distance2 <DIM> (current, next)) {
                    current = next;
                    util::copy_key <DIM> (next, result);
                }
                std::advance (next, DIM);
            }
            // the last point is always part of the simplification
            util::copy_key <DIM> (next, result);
            return result;
        }
    };

    /*!
        \brief Perpendicular distance routine (PD).
    */
    template
    <
        unsigned DIM,
        typename ForwardIterator,
        typename Distance,
        typename OutputIterator
    >
    struct perpendicular_distance
    {
        typedef typename std::iterator_traits <ForwardIterator>::difference_type diff_type;
        typedef typename std::iterator_traits <ForwardIterator>::value_type value_type;
        typedef typename util::select_calculation_type <ForwardIterator>::type calc_type;

        /*!
            \brief Performs the perpendicular distance simplification routine.
        */
        static OutputIterator simplify (
            ForwardIterator first,
            ForwardIterator last,
            Distance tol,
            OutputIterator result)
        {
            diff_type coordCount = std::distance (first, last);
            diff_type pointCount = DIM      // protect against zero DIM
                                   ? coordCount / DIM
                                   : 0;
            Distance tol2 = tol * tol;      // squared distance tolerance

            // validate input and check if simplification required
            if (coordCount % DIM || pointCount < 3 || tol2 <= 0) {
                return std::copy (first, last, result);
            }

            ForwardIterator p0 = first;
            ForwardIterator p1 = p0;
            std::advance (p1, DIM);
            ForwardIterator p2 = p1;
            std::advance (p2, DIM);

            // the first point is always part of the simplification
            util::copy_key <DIM> (p0, result);

            while (p2 != last) {
                // test p1 against line segment S(p0, p2)
                if (math::segment_distance2 <DIM> (p0, p2, p1) < tol2) {
                    util::copy_key <DIM> (p2, result);
                    // move up by two points
                    p0 = p2;
                    std::advance (p1, 2 * DIM);
                    if (p1 == last) {
                        // protect against advancing p2 beyond last
                        break;
                    }
                    std::advance (p2, 2 * DIM);
                }
                else {
                    util::copy_key <DIM> (p1, result);
                    // move up by one point
                    p0 = p1;
                    p1 = p2;
                    std::advance (p2, DIM);
                }
            }
            // make sure the last point is part of the simplification
            if (p1 != last) {
                util::copy_key <DIM> (p1, result);
            }
            return result;
        }
    };

    /*!
        \brief Repeated perpendicular distance routine (PD).
    */
    template
    <
        unsigned DIM,
        typename ForwardIterator,
        typename Distance,
        typename Size,
        typename OutputIterator
    >
    struct perpendicular_distance_repeat
    {
        typedef typename std::iterator_traits <ForwardIterator>::value_type value_type;
        typedef typename std::iterator_traits <ForwardIterator>::difference_type diff_type;

        /*!
            \brief Repeatedly performs the perpendicular distance simplification routine.
        */
        static OutputIterator simplify (
            ForwardIterator first,
            ForwardIterator last,
            Distance tol,
            Size repeat,
            OutputIterator result)
        {
            if (repeat == 1) {
                // single pass
                return perpendicular_distance
                    <
                        DIM,
                        ForwardIterator,
                        Distance,
                        OutputIterator
                    >::simplify (first, last, tol, result);
            }
            // only validate repeat; other input is validated by simplify_perpendicular_distance
            if (repeat < 1) {
                return std::copy (first, last, result);
            }
            diff_type coordCount = std::distance (first, last);

            // first pass: [first, last) --> temporary array 'tempPoly'
            util::scoped_array <value_type> tempPoly (static_cast<unsigned>(coordCount));

            diff_type tempCoordCount = std::distance (
                tempPoly.get (),
                perpendicular_distance
                    <
                        DIM,
                        ForwardIterator,
                        Distance,
                        value_type*
                    >::simplify (first, last, tol, tempPoly.get ()));

            // check if simplification did not improve
            if (coordCount == tempCoordCount) {
                return std::copy (tempPoly.get (), tempPoly.get () + coordCount, result);
            }
            std::swap (coordCount, tempCoordCount);
            --repeat;

            // intermediate passes: temporary array 'tempPoly' --> temporary array 'tempResult'
            if (1 < repeat) {
                util::scoped_array <value_type> tempResult (static_cast<unsigned>(coordCount));

                while (--repeat) {
                    tempCoordCount = std::distance (
                        tempResult.get (),
                        perpendicular_distance
                            <
                                DIM,
                                value_type*,
                                Distance,
                                value_type*
                            >::simplify (tempPoly.get (), tempPoly.get () + coordCount, tol, tempResult.get ()));

                    // check if simplification did not improve
                    if (coordCount == tempCoordCount) {
                        return std::copy (tempPoly.get (), tempPoly.get () + coordCount, result);
                    }
                    util::swap (tempPoly, tempResult);
                    std::swap (coordCount, tempCoordCount);
                }
            }

            // final pass: temporary array 'tempPoly' --> result
            return perpendicular_distance
                <
                    DIM,
                    value_type*,
                    Distance,
                    OutputIterator
                >::simplify (tempPoly.get (), tempPoly.get () + coordCount, tol, result);
        }
    };

    /*!
        \brief Reumann-Witkam approximation (RW).
    */
    template
    <
        unsigned DIM,
        typename ForwardIterator,
        typename Distance,
        typename OutputIterator
    >
    struct reumann_witkam
    {
        typedef typename std::iterator_traits <ForwardIterator>::difference_type diff_type;
        typedef typename std::iterator_traits <ForwardIterator>::value_type value_type;
        typedef typename util::select_calculation_type <ForwardIterator>::type calc_type;

        /*!
            \brief Performs Reumann-Witkam approximation.
        */
        static OutputIterator simplify (
            ForwardIterator first,
            ForwardIterator last,
            Distance tol,
            OutputIterator result)
        {
            diff_type coordCount = std::distance (first, last);
            diff_type pointCount = DIM      // protect against zero DIM
                                   ? coordCount / DIM
                                   : 0;
            Distance tol2 = tol * tol;      // squared distance tolerance

            // validate input and check if simplification required
            if (coordCount % DIM || pointCount < 3 || tol2 <= 0) {
                return std::copy (first, last, result);
            }

            // define the line L(p0, p1)
            ForwardIterator p0 = first; // indicates the current key
            ForwardIterator p1 = p0;    // indicates the next point after p0
            std::advance (p1, DIM);

            // keep track of two test points
            ForwardIterator pi = p1;    // the previous test point
            ForwardIterator pj = p1;    // the current test point (pi+1)

            // the first point is always part of the simplification
            util::copy_key <DIM> (p0, result);

            // check each point pj against L(p0, p1)
            for (diff_type j = 2; j < pointCount; ++j) {
                pi = pj;
                std::advance (pj, DIM);

                if (math::line_distance2 <DIM> (p0, p1, pj) < tol2) {
                    continue;
                }
                // found the next key at pi
                util::copy_key <DIM> (pi, result);
                // define new line L(pi, pj)
                p0 = pi;
                p1 = pj;
            }
            // the last point is always part of the simplification
            util::copy_key <DIM> (pj, result);

            return result;
        }
    };

    /*!
        \brief Opheim approximation (OP).
    */
    template
    <
        unsigned DIM,
        typename ForwardIterator,
        typename Distance,
        typename OutputIterator
    >
    struct opheim
    {
        typedef typename std::iterator_traits <ForwardIterator>::difference_type diff_type;
        typedef typename std::iterator_traits <ForwardIterator>::value_type value_type;
        typedef typename util::select_calculation_type <ForwardIterator>::type calc_type;

        /*!
            \brief Performs Opheim approximation.
        */
        static OutputIterator simplify (
            ForwardIterator first,
            ForwardIterator last,
            Distance min_tol,
            Distance max_tol,
            OutputIterator result)
        {
            diff_type coordCount = std::distance (first, last);
            diff_type pointCount = DIM              // protect against zero DIM
                                   ? coordCount / DIM
                                   : 0;
            Distance min_tol2 = min_tol * min_tol;  // squared minimum distance tolerance
            Distance max_tol2 = max_tol * max_tol;  // squared maximum distance tolerance

            // validate input and check if simplification required
            if (coordCount % DIM || pointCount < 3 || min_tol2 <= 0 || max_tol2 <= 0) {
                return std::copy (first, last, result);
            }

            // define the ray R(r0, r1)
            ForwardIterator r0 = first;  // indicates the current key and start of the ray
            ForwardIterator r1 = first;  // indicates a point on the ray
            bool rayDefined = false;

            // keep track of two test points
            ForwardIterator pi = r0;     // the previous test point
            ForwardIterator pj = pi;     // the current test point (pi+1)
            std::advance (pj, DIM);

            // the first point is always part of the simplification
            util::copy_key <DIM> (r0, result);

            for (diff_type j = 2; j < pointCount; ++j) {
                pi = pj;
                std::advance (pj, DIM);

                if (!rayDefined) {
                    // discard each point within minimum tolerance
                    if (math::point_distance2 <DIM> (r0, pj) < min_tol2) {
                        continue;
                    }
                    // the last point within minimum tolerance pi defines the ray R(r0, r1)
                    r1 = pi;
                    rayDefined = true;
                }

                // check each point pj against R(r0, r1)
                if (math::point_distance2 <DIM> (r0, pj) < max_tol2 &&
                    math::ray_distance2 <DIM> (r0, r1, pj) < min_tol2)
                {
                    continue;
                }
                // found the next key at pi
                util::copy_key <DIM> (pi, result);
                // define new ray R(pi, pj)
                r0 = pi;
                rayDefined = false;
            }
            // the last point is always part of the simplification
            util::copy_key <DIM> (pj, result);

            return result;
        }
    };

    /*!
        \brief Lang approximation (LA).
    */
    template
    <
        unsigned DIM,
        typename BidirectionalIterator,
        typename Distance,
        typename Size,
        typename OutputIterator
    >
    struct lang
    {
        typedef typename std::iterator_traits <BidirectionalIterator>::difference_type diff_type;
        typedef typename std::iterator_traits <BidirectionalIterator>::value_type value_type;
        typedef typename util::select_calculation_type <BidirectionalIterator>::type calc_type;

        /*!
            \brief Performs Lang approximation.
        */
        static OutputIterator simplify (
            BidirectionalIterator first,
            BidirectionalIterator last,
            Distance tol,
            Size look_ahead,
            OutputIterator result)
        {
            diff_type coordCount = std::distance (first, last);
            diff_type pointCount = DIM              // protect against zero DIM
                                   ? coordCount / DIM
                                   : 0;
            Distance tol2 = tol * tol;              // squared minimum distance tolerance

            // validate input and check if simplification required
            if (coordCount % DIM || pointCount < 3 || look_ahead < 2 || tol2 <= 0) {
                return std::copy (first, last, result);
            }

            BidirectionalIterator current = first;  // indicates the current key
            BidirectionalIterator next = first;     // used to find the next key

            diff_type remaining = pointCount - 1;   // the number of points remaining after current
            diff_type moved = util::forward <DIM> (next, static_cast <diff_type> (look_ahead), remaining);

            // the first point is always part of the simplification
            util::copy_key <DIM> (current, result);

            while (moved) {
                calc_type d2 = 0;
                BidirectionalIterator p = current;
                std::advance (p, DIM);

                while (p != next) {
                    d2 = std::max (d2, math::segment_distance2 <DIM> (current, next, p));
                    if (tol2 < d2) {
                        break;
                    }
                    std::advance (p, DIM);
                }
                if (d2 < tol2) {
                    current = next;
                    util::copy_key <DIM> (current, result);
                    moved = util::forward <DIM> (next, static_cast <diff_type> (look_ahead), remaining);
                }
                else {
                    int iremaining = static_cast<int>(remaining) ; // pass by ref
                    util::backward <DIM> (next, 1, iremaining);
                }
            }
            return result;
        }
    };

    /*!
        \brief Douglas-Peucker approximation (DPc).
    */
    template
    <
        unsigned DIM,
        typename RandomAccessIterator,
        typename Distance,
        typename OutputIterator
    >
    struct douglas_peucker_classic
    {
        typedef typename std::iterator_traits <RandomAccessIterator>::difference_type diff_type;
        typedef typename std::iterator_traits <RandomAccessIterator>::value_type value_type;
        typedef typename util::select_calculation_type <RandomAccessIterator>::type calc_type;
        typedef typename detail::find_key <DIM, RandomAccessIterator> key_finder;
        typedef typename detail::find_key <DIM, RandomAccessIterator>::key key_type;

        /*!
            \brief Performs Douglas-Peucker approximation.
        */
        static OutputIterator simplify (
            RandomAccessIterator first,
            RandomAccessIterator last,
            Distance tol,
            OutputIterator result)
        {
            diff_type coordCount = std::distance (first, last);
            diff_type pointCount = DIM      // protect against zero DIM
                                   ? coordCount / DIM
                                   : 0;
            Distance tol2 = tol * tol;      // squared distance tolerance

            // validate input and check if simplification required
            if (coordCount % DIM || pointCount < 3 || tol2 <= 0) {
                return std::copy (first, last, result);
            }

            // keep track of what points are part of the simplification (key)
            util::scoped_array <unsigned char> keys (static_cast<unsigned>(pointCount));
            std::fill_n (keys.get (), pointCount, 0);
            keys [0] = 1;                   // the first point is always a key
            keys [static_cast<int>(pointCount - 1)] = 1;      // the last point is always a key

            // keep track of all sub polylines that still need to be processed
            std::stack <sub_poly> stack;    // LIFO job-queue
            sub_poly poly (0, coordCount-DIM);
            stack.push (poly);              // add complete poly

            while (!stack.empty ()) {
                poly = stack.top ();    // take a sub poly
                stack.pop ();           // and find its key

                key_type key = key_finder::apply (first, poly.first, poly.last);
                if (key.index && tol2 < key.dist2) {
                    // store the key if valid
                    keys [static_cast<int>(key.index / DIM)] = 1;
                    // split the polyline at the key and recurse
                    stack.push (sub_poly (key.index, poly.last));
                    stack.push (sub_poly (poly.first, key.index));
                }
            }
            // copy keys
            util::copy_keys <DIM> (first, last, keys.get (), result);
            return result;
        }

    private:
        /*!
            \brief Defines a sub polyline.
        */
        struct sub_poly {
            sub_poly (diff_type first=0, diff_type last=0) :
                first (first), last (last) {}

            diff_type first;    //!< coord index of the first point
            diff_type last;     //!< coord index of the last point
        };
    };

    /*!
        \brief Douglas-Peucker approximation, but with RD as a preprocessing step (DP).
    */
    template
    <
        unsigned DIM,
        typename ForwardIterator,
        typename Distance,
        typename OutputIterator
    >
    struct douglas_peucker
    {
        typedef typename std::iterator_traits <ForwardIterator>::difference_type diff_type;
        typedef typename std::iterator_traits <ForwardIterator>::value_type value_type;
        typedef typename util::select_calculation_type <ForwardIterator>::type calc_type;

        /*!
            \brief Performs Douglas-Peucker approximation, but uses RD as a preprocessing step.
        */
        static OutputIterator simplify (
            ForwardIterator first,
            ForwardIterator last,
            Distance tol,
            OutputIterator result)
        {
            diff_type coordCount = std::distance (first, last);
            diff_type pointCount = DIM      // protect against zero DIM
                                   ? coordCount / DIM
                                   : 0;
            // validate input and check if simplification required
            if (coordCount % DIM || pointCount < 3 || tol <= 0) {
                return std::copy (first, last, result);
            }
            // radial distance simplification routine
            util::scoped_array <value_type> reduced (static_cast<unsigned>(coordCount));    // radial distance results
            diff_type reducedCoordCount = std::distance (
                reduced.get (),
                radial_distance
                    <
                        DIM,
                        ForwardIterator,
                        Distance,
                        value_type*
                    >::simplify (first, last, tol, reduced.get ()));

            // douglas-peucker approximation
            result =
                douglas_peucker_classic
                <
                    DIM,
                    value_type*,
                    Distance,
                    OutputIterator
                >::simplify (reduced.get (), reduced.get () + reducedCoordCount, tol, result);

            return result;
        }
    };

    /*!
        \brief Douglas-Peucker approximation, but with a point count tolerance (DPn).
    */
    template
    <
        unsigned DIM,
        typename RandomAccessIterator,
        typename Size,
        typename OutputIterator
    >
    struct douglas_peucker_n
    {
        typedef typename std::iterator_traits <RandomAccessIterator>::difference_type diff_type;
        typedef typename std::iterator_traits <RandomAccessIterator>::value_type value_type;
        typedef typename util::select_calculation_type <RandomAccessIterator>::type calc_type;
        typedef typename detail::find_key <DIM, RandomAccessIterator> key_finder;
        typedef typename detail::find_key <DIM, RandomAccessIterator>::key key_type;

        /*!
            \brief Performs Douglas-Peucker approximation, but uses a point count tolerance.
        */
        static OutputIterator simplify (
            RandomAccessIterator first,
            RandomAccessIterator last,
            Size tol,
            OutputIterator result)
        {
            diff_type coordCount = std::distance (first, last);
            diff_type pointCount = DIM      // protect against zero DIM
                                   ? coordCount / DIM
                                   : 0;
            // validate input and check if simplification required
            if (coordCount % DIM || pointCount <= static_cast <diff_type> (tol) || tol < 2) {
                return std::copy (first, last, result);
            }

            // keep track of what points are part of the simplification (keys)
            util::scoped_array <unsigned char> keys (static_cast<unsigned>(pointCount));
            std::fill_n (keys.get (), pointCount, 0);
            keys [0] = 1;                   // the first point is always a key
            keys [static_cast<int>(pointCount - 1)] = 1;      // the last point is always a key
            Size keyCount = 2;

            if (tol == 2) {
                util::copy_keys <DIM> (first, last, keys.get (), result);
                return result;
            }

            // keep track of all sub polylines that still need to be processed
            std::priority_queue <sub_poly> queue;    // sorted (max key dist2) job queue
            sub_poly poly (0, coordCount-DIM);
            poly.key = key_finder::apply (first, poly.first, poly.last);
            queue.push (poly);                       // add complete poly

            while (!queue.empty ()) {
                poly = queue.top ();                 // take a sub poly
                queue.pop ();
                // store the key
                keys [static_cast<int>(poly.key.index / DIM)] = 1;
                // check point count tolerance
                if (++keyCount == tol) {
                    break;
                }
                // split the polyline at the key and recurse
                sub_poly left (poly.first, poly.key.index);
                left.key = key_finder::apply (first, left.first, left.last);
                if (left.key.index) {
                    queue.push (left);
                }
                sub_poly right (poly.key.index, poly.last);
                right.key = key_finder::apply (first, right.first, right.last);
                if (right.key.index) {
                    queue.push (right);
                }
            }
            // copy keys
            util::copy_keys <DIM> (first, last, keys.get (), result);
            return result;
        }

    private:
        /*!
            \brief Defines a sub polyline.
        */
        struct sub_poly {
            sub_poly (diff_type first=0, diff_type last=0) :
                first (first), last (last) {}

            diff_type first;    //!< coord index of the first point
            diff_type last;     //!< coord index of the last point
            key_type key;       //!< key of this sub poly

            bool operator< (const sub_poly& other) const {
                return key.dist2 < other.key.dist2;
            }
        };
    };

}}

#endif // PSIMPL_DETAIL_ALGO
