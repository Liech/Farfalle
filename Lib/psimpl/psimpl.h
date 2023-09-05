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

/*!
    \mainpage psimpl - generic n-dimensional polyline simplification

<pre>
    Author  - Elmar de Koning
    Support - edekoning@gmail.com
    Website - http://psimpl.sf.net/
    Project - http://sf.net/projects/psimpl/
    Article - http://www.codeproject.com/KB/recipes/PolylineSimplification.aspx
    License - MPL 1.1
</pre><br>

    \section sec_psimpl psimpl
<pre>
    'psimpl' is a c++ polyline simplification library that is generic, easy to use, and supports
    the following algorithms:

    Simplification
    + Nth point - A naive algorithm that keeps only each nth point
    + Distance between points - Removes successive points that are clustered together
    + Perpendicular distance - Removes points based on their distance to the line segment defined
      by their left and right neighbors
    + Reumann-Witkam - Shifts a strip along the polyline and removes points that fall outside
    + Opheim - A constrained version of Reumann-Witkam
    + Lang - Similar to the Perpendicular distance routine, but instead of looking only at direct
      neighbors, an entire search region is processed
    + Douglas-Peucker - A classic simplification algorithm that provides an excellent approximation
      of the original line

    Errors
    + positional error - Distance of each polyline point to its simplification

    All the algorithms have been implemented in a header-only library using an STL-style interface
    that operates on input and output iterators. Supported polylines can be of any dimension, and
    defined using floating point or integer data types.
</pre><br>

    \section sec_changelog changelog
<pre>
    28-09-2010 - Initial version
    23-10-2010 - Changed license from CPOL to MPL
    26-10-2010 - Clarified input (type) requirements, and changed the behavior of the algorithms
                 under invalid input
    01-12-2010 - Added the nth point, perpendicular distance and Reumann-Witkam routines; moved all
                 functions related to distance calculations to the math namespace
    10-12-2010 - Fixed a bug in the perpendicular distance routine
    27-02-2011 - Added Opheim simplification, and functions for computing positional errors due to
                 simplification; renamed simplify_douglas_peucker_alt to simplify_douglas_peucker_n
    18-06-2011 - Added Lang simplification; fixed divide by zero bug when using integers; fixed a
                 bug where incorrect output iterators were returned under invalid input; fixed a bug
                 in douglas_peucker_n where an incorrect number of points could be returned; fixed a
                 bug in compute_positional_errors2 that required the output and input iterator types
                 to be the same; fixed a bug in compute_positional_error_statistics where invalid
                 statistics could be returned under questionable input; documented input iterator
                 requirements for each algorithm; miscellaneous refactoring of most algorithms.
    ??-??-???? - Complete rewrite; split code across multiple headers; split dp; more accurate
                 results with intergers; improved compute pos error now supports ...
</pre>
*/

#ifndef PSIMPL_GENERIC
#define PSIMPL_GENERIC


#include "detail/algo.h"
#include "detail/error.h"
#include "detail/math.h"
#include "detail/util.h"


// Several algorithms may use divisions and thus need to operate on floating points for accurate
// results. The list below defines what integral types may be promoted to what floating point
// types. Any type not listed here will never get promoted. Feel free to extend this list with
// other types you want to use.
PSIMPL_DEF_FLOATING_POINT_PROMOTION(         char,  double)
PSIMPL_DEF_FLOATING_POINT_PROMOTION(unsigned char,  double)
PSIMPL_DEF_FLOATING_POINT_PROMOTION(  signed char,  double)
PSIMPL_DEF_FLOATING_POINT_PROMOTION(unsigned short, double)
PSIMPL_DEF_FLOATING_POINT_PROMOTION(  signed short, double)
PSIMPL_DEF_FLOATING_POINT_PROMOTION(unsigned int,   double)
PSIMPL_DEF_FLOATING_POINT_PROMOTION(  signed int,   double)
PSIMPL_DEF_FLOATING_POINT_PROMOTION(unsigned long,  double)
PSIMPL_DEF_FLOATING_POINT_PROMOTION(  signed long,  double)


/*!
    \brief Root namespace of the polyline simplification library.
*/
namespace psimpl
{
    /*!
        \brief Performs the nth point simplification routine (NP).

        NP is an O(n) algorithm for polyline simplification. It keeps only the first, last and
        each nth point. As an example, consider any random line of 8 points. Using n = 3 will
        always yield a simplification consisting of points: 1, 4, 7, 8

        \image html psimpl_np.png

        NP is applied to the range [first, last). The resulting simplified polyline is copied
        to the output range [result, result + m*DIM), where m is the number of vertices of the
        simplified polyline. The return value is the end of the output range: result + m*DIM.

        Input (Type) requirements:
        1- DIM is not 0, where DIM represents the dimension of the polyline
        2- The ForwardIterator value type is convertible to a value type of the OutputIterator
        3- The range [first, last) contains only vertex coordinates in multiples of DIM, f.e.:
           x, y, z, x, y, z, x, y, z when DIM = 3
        4- The range [first, last) contains at least 2 vertices
        5- n > 0

        In case these requirements are not met, the entire input range [first, last) is copied
        to the output range [result, result + (last - first)) OR compile errors may occur.

        \param[in] first    the first coordinate of the first polyline point
        \param[in] last     one beyond the last coordinate of the last polyline point
        \param[in] n        specifies 'each nth point'
        \param[in] result   destination of the simplified polyline
        \return             one beyond the last coordinate of the simplified polyline
    */
    template
    <
        unsigned DIM,
        typename ForwardIterator,
        typename Size,
        typename OutputIterator
    >
    OutputIterator simplify_nth_point (
        ForwardIterator first,
        ForwardIterator last,
        Size n,
        OutputIterator result)
    {
        return algo::nth_point
            <
                DIM,
                ForwardIterator,
                Size,
                OutputIterator
            >::simplify (first, last, n, result);
    }

    /*!
        \brief Performs the radial distance simplification routine (RD).

        RD is a brute-force O(n) algorithm for polyline simplification. It reduces successive
        vertices that are clustered too closely to a single vertex, called a key. The resulting
        keys form the simplified polyline.

        \image html psimpl_rd.png

        RD is applied to the range [first, last) using the specified tolerance tol. The
        resulting simplified polyline is copied to the output range [result, result + m*DIM),
        where m is the number of vertices of the simplified polyline. The return value is the
        end of the output range: result + m*DIM.

        Input (Type) requirements:
        1- DIM is not 0, where DIM represents the dimension of the polyline
        2- The ForwardIterator value type is convertible to a value type of the OutputIterator
        3- The range [first, last) contains only vertex coordinates in multiples of DIM, f.e.:
           x, y, z, x, y, z, x, y, z when DIM = 3
        4- The range [first, last) contains at least 2 vertices
        5- tol > 0

        In case these requirements are not met, the entire input range [first, last) is copied
        to the output range [result, result + (last - first)) OR compile errors may occur.

        \param[in] first    the first coordinate of the first polyline point
        \param[in] last     one beyond the last coordinate of the last polyline point
        \param[in] tol      radial (point-to-point) distance tolerance
        \param[in] result   destination of the simplified polyline
        \return             one beyond the last coordinate of the simplified polyline
    */
    template
    <
        unsigned DIM,
        typename ForwardIterator,
        typename Distance,
        typename OutputIterator
    >
    OutputIterator simplify_radial_distance (
        ForwardIterator first,
        ForwardIterator last,
        Distance tol,
        OutputIterator result)
    {
        return algo::radial_distance
            <
                DIM,
                ForwardIterator,
                Distance,
                OutputIterator
            >::simplify (first, last, tol, result);
    }

    /*!
        \brief Performs the perpendicular distance simplification routine (PD).

        PD is an O(n) algorithm for polyline simplification. It computes the perpendicular
        distance of each point pi to the line segment S(pi-1, pi+1). Only when this distance is
        larger than the given tolerance will point pi be part of the simpification. Note that the
        original polyline can only be reduced by a maximum of 50%. Multiple passes are required
        to achieve higher point reductions.

        \image html psimpl_pd.png

        PD is applied to the range [first, last) using the specified tolerance tol. The
        resulting simplified polyline is copied to the output range [result, result + m*DIM),
        where m is the number of vertices of the simplified polyline. The return value is the
        end of the output range: result + m*DIM.

        Input (Type) requirements:
        1- DIM is not 0, where DIM represents the dimension of the polyline
        2- The ForwardIterator value type is convertible to a value type of the OutputIterator
        3- The range [first, last) contains only vertex coordinates in multiples of DIM, f.e.:
           x, y, z, x, y, z, x, y, z when DIM = 3
        4- The range [first, last) contains at least 2 vertices
        5- tol > 0

        In case these requirements are not met, the entire input range [first, last) is copied
        to the output range [result, result + (last - first)) OR compile errors may occur.

        \param[in] first    the first coordinate of the first polyline point
        \param[in] last     one beyond the last coordinate of the last polyline point
        \param[in] tol      perpendicular (point-to-segment) distance tolerance
        \param[in] result   destination of the simplified polyline
        \return             one beyond the last coordinate of the simplified polyline
    */
    template
    <
        unsigned DIM,
        typename ForwardIterator,
        typename Distance,
        typename OutputIterator
    >
    OutputIterator simplify_perpendicular_distance (
        ForwardIterator first,
        ForwardIterator last,
        Distance tol,
        OutputIterator result)
    {
        return algo::perpendicular_distance
            <
                DIM,
                ForwardIterator,
                Distance,
                OutputIterator
            >::simplify (first, last, tol, result);
    }

    /*!
        \brief Repeatedly performs the perpendicular distance simplification routine (PD).

        The algorithm stops after calling the PD routine 'repeat' times OR when the
        simplification does not improve. Note that this algorithm will need to store
        up to two intermediate simplification results.

        \param[in] first    the first coordinate of the first polyline point
        \param[in] last     one beyond the last coordinate of the last polyline point
        \param[in] tol      perpendicular (point-to-segment) distance tolerance
        \param[in] repeat   the number of times to successively apply the PD routine
        \param[in] result   destination of the simplified polyline
        \return             one beyond the last coordinate of the simplified polyline
    */
    template
    <
        unsigned DIM,
        typename ForwardIterator,
        typename Distance,
        typename Size,
        typename OutputIterator
    >
    OutputIterator simplify_perpendicular_distance (
        ForwardIterator first,
        ForwardIterator last,
        Distance tol,
        Size repeat,
        OutputIterator result)
    {
        return algo::perpendicular_distance_repeat
            <
                DIM,
                ForwardIterator,
                Distance,
                Size,
                OutputIterator
            >::simplify (first, last, tol, repeat, result);
    }

    /*!
        \brief Performs Reumann-Witkam approximation (RW).

        The O(n) RW routine uses a point-to-line (perpendicular) distance tolerance. It defines
        a line through the first two vertices of the original polyline. For each successive
        vertex vi its perpendicular distance to this line is calculated. A new key is found at
        vi-1, when this distance exceeds the specified tolerance. The vertices vi and vi+1 are
        then used to define a new line, and the process repeats itself.

        \image html psimpl_rw.png

        RW routine is applied to the range [first, last) using the specified perpendicular
        distance tolerance tol. The resulting simplified polyline is copied to the output range
        [result, result + m*DIM), where m is the number of vertices of the simplified polyline.
        The return value is the end of the output range: result + m*DIM.

        Input (Type) Requirements:
        1- DIM is not 0, where DIM represents the dimension of the polyline
        2- The ForwardIterator value type is convertible to a value type of the OutputIterator
        3- The range [first, last) contains vertex coordinates in multiples of DIM,
           f.e.: x, y, z, x, y, z, x, y, z when DIM = 3
        4- The range [first, last) contains at least 2 vertices
        5- tol > 0

        In case these requirements are not met, the entire input range [first, last) is copied
        to the output range [result, result + (last - first)) OR compile errors may occur.

        \param[in] first    the first coordinate of the first polyline point
        \param[in] last     one beyond the last coordinate of the last polyline point
        \param[in] tol      perpendicular (point-to-line) distance tolerance
        \param[in] result   destination of the simplified polyline
        \return             one beyond the last coordinate of the simplified polyline
    */
    template
    <
        unsigned DIM,
        typename ForwardIterator,
        typename Distance,
        typename OutputIterator
    >
    OutputIterator simplify_reumann_witkam (
        ForwardIterator first,
        ForwardIterator last,
        Distance tol,
        OutputIterator result)
    {
        return algo::reumann_witkam
            <
                DIM,
                ForwardIterator,
                Distance,
                OutputIterator
            >::simplify (first, last, tol, result);
    }

    /*!
        \brief Performs Opheim approximation (OP).

        The O(n) OP routine is very similar to the Reumann-Witkam (RW) routine, and can be seen
        as a constrained version of that RW routine. OP uses both a minimum and a maximum
        distance tolerance to constrain the search area. For each successive vertex vi, its
        radial distance to the current key vkey (initially v0) is calculated. The last point
        within the minimum distance tolerance is used to define a ray R (vkey, vi). If no
        such vi exists, the ray is defined as R(vkey, vkey+1). For each successive vertex vj
        beyond vi its perpendicular distance to the ray R is calculated. A new key is found at
        vj-1, when this distance exceeds the minimum tolerance Or when the radial distance
        between vj and the vkey exceeds the maximum tolerance. After a new key is found, the
        process repeats itself.

        \image html psimpl_op.png

        OP routine is applied to the range [first, last) using the specified distance tolerances
        min_tol and max_tol. The resulting simplified polyline is copied to the output range
        [result, result + m*DIM), where m is the number of vertices of the simplified polyline.
        The return value is the end of the output range: result + m*DIM.

        Input (Type) Requirements:
        1- DIM is not 0, where DIM represents the dimension of the polyline
        2- The ForwardIterator value type is convertible to a value type of the OutputIterator
        3- The range [first, last) contains vertex coordinates in multiples of DIM,
           f.e.: x, y, z, x, y, z, x, y, z when DIM = 3
        4- The range [first, last) contains at least 2 vertices
        5- min_tol > 0
        6- max_tol > 0

        In case these requirements are not met, the entire input range [first, last) is copied
        to the output range [result, result + (last - first)) OR compile errors may occur.

        \param[in] first    the first coordinate of the first polyline point
        \param[in] last     one beyond the last coordinate of the last polyline point
        \param[in] min_tol  radial and perpendicular (point-to-ray) distance tolerance
        \param[in] max_tol  radial distance tolerance
        \param[in] result   destination of the simplified polyline
        \return             one beyond the last coordinate of the simplified polyline
    */
    template
    <
        unsigned DIM,
        typename ForwardIterator,
        typename Distance,
        typename OutputIterator
    >
    OutputIterator simplify_opheim (
        ForwardIterator first,
        ForwardIterator last,
        Distance min_tol,
        Distance max_tol,
        OutputIterator result)
    {
        return algo::opheim
            <
                DIM,
                ForwardIterator,
                Distance,
                OutputIterator
            >::simplify (first, last, min_tol, max_tol, result);
    }

    /*!
        \brief Performs Lang approximation (LA).

        The LA routine defines a fixed size search-region. The first and last points of that
        search region form a segment. This segment is used to calculate the perpendicular
        distance to each intermediate point. If any calculated distance is larger than the
        specified tolerance, the search region will be shrunk by excluding its last point. This
        process will continue untill all calculated distances fall below the specified tolerance
        , or there are no more intermediate points. At this point all intermediate points are
        removed and a new search region is defined starting at the last point from old search
        region.
        Note that the size of the search region (look_ahead parameter) controls the maximum
        amount of simplification, e.g.: a size of 20 will always result in a simplification that
        contains at least 5% of the original points.

        \image html psimpl_la.png

        LA routine is applied to the range [first, last) using the specified tolerance and
        look ahead values. The resulting simplified polyline is copied to the output range
        [result, result + m*DIM), where m is the number of vertices of the simplified polyline.
        The return value is the end of the output range: result + m*DIM.

        Input (Type) Requirements:
        1- DIM is not 0, where DIM represents the dimension of the polyline
        2- The BidirectionalIterator value type is convertible to a value type of the OutputIterator
        3- The range [first, last) contains vertex coordinates in multiples of DIM,
           f.e.: x, y, z, x, y, z, x, y, z when DIM = 3
        4- The range [first, last) contains at least 2 vertices
        5- tol > 0
        6- look_ahead > 0

        In case these requirements are not met, the entire input range [first, last) is copied
        to the output range [result, result + (last - first)) OR compile errors may occur.

        \param[in] first      the first coordinate of the first polyline point
        \param[in] last       one beyond the last coordinate of the last polyline point
        \param[in] tol        perpendicular (point-to-segment) distance tolerance
        \param[in] look_ahead defines the size of the search region
        \param[in] result     destination of the simplified polyline
        \return               one beyond the last coordinate of the simplified polyline
    */
    template
    <
        unsigned DIM,
        typename BidirectionalIterator,
        typename Distance,
        typename Size,
        typename OutputIterator
    >
    OutputIterator simplify_lang (
        BidirectionalIterator first,
        BidirectionalIterator last,
        Distance tol,
        Size look_ahead,
        OutputIterator result)
    {
        return algo::lang
            <
                DIM,
                BidirectionalIterator,
                Distance,
                Size,
                OutputIterator
            >::simplify (first, last, tol, look_ahead, result);
    }

    /*!
        \brief Performs Douglas-Peucker approximation (DPc).

        The DPc algorithm starts with a simplification that is the single edge joining the first
        and last vertices of the polyline. The distance of the remaining vertices to that edge
        are computed. The vertex that is furthest away from theedge (called a key), and has a
        computed distance that is larger than a specified tolerance, will be added to the
        simplification. This process will recurse for each edge in the current simplification,
        untill all vertices of the original polyline are within tolerance. The algorithm is O(n2)
        in worst case and O(n log n) on average.

        \image html psimpl_dp.png

        DPc is applied to the range [first, last) using the specified tolerance tol. The resulting
        simplified polyline is copied to the output range [result, result + m*DIM), where m is the
        number of vertices of the simplified polyline. The return value is the end of the output
        range: result + m*DIM.

        Input (Type) requirements:
        1- DIM is not 0, where DIM represents the dimension of the polyline
        2- The RandomAccessIterator value type is convertible to a value type of the OutputIterator
        3- The range [first, last) contains vertex coordinates in multiples of DIM, f.e.:
           x, y, z, x, y, z, x, y, z when DIM = 3
        4- The range [first, last) contains at least 2 vertices
        5- tol > 0

        In case these requirements are not met, the entire input range [first, last) is copied
        to the output range [result, result + (last - first)) OR compile errors may occur.

        \param[in] first    the first coordinate of the first polyline point
        \param[in] last     one beyond the last coordinate of the last polyline point
        \param[in] tol      perpendicular (point-to-segment) distance tolerance
        \param[in] result   destination of the simplified polyline
        \return             one beyond the last coordinate of the simplified polyline
    */
    template
    <
        unsigned DIM,
        typename RandomAccessIterator,
        typename Distance,
        typename OutputIterator
    >
    OutputIterator simplify_douglas_peucker_classic (
        RandomAccessIterator first,
        RandomAccessIterator last,
        Distance tol,
        OutputIterator result)
    {
        return algo::douglas_peucker_classic
            <
                DIM,
                RandomAccessIterator,
                Distance,
                OutputIterator
            >::simplify (first, last, tol, result);
    }

    /*!
        \brief Performs Douglas-Peucker approximation, but uses RD as a preprocessing step (DP).

        The DP algorithm uses the radial distance (RD) routine O(n) as a preprocessing step.
        After RD the algorithm is O (n m) in worst case and O(n log m) on average, where m < n
        (m is the number of points after RD).

        Note that this algorithm will create a copy of the input polyline during the RD
        preprocessing step.

        RD followed by DP is applied to the range [first, last) using the specified tolerance
        tol. The resulting simplified polyline is copied to the output range
        [result, result + m*DIM), where m is the number of vertices of the simplified polyline.
        The return value is the end of the output range: result + m*DIM.

        Input (Type) requirements:
        1- DIM is not 0, where DIM represents the dimension of the polyline
        2- The ForwardIterator value type is convertible to a value type of the OutputIterator
        3- The range [first, last) contains vertex coordinates in multiples of DIM, f.e.:
           x, y, z, x, y, z, x, y, z when DIM = 3
        4- The range [first, last) contains at least 2 vertices
        5- tol > 0

        In case these requirements are not met, the entire input range [first, last) is copied
        to the output range [result, result + (last - first)) OR compile errors may occur.

        \param[in] first    the first coordinate of the first polyline point
        \param[in] last     one beyond the last coordinate of the last polyline point
        \param[in] tol      perpendicular (point-to-segment) distance tolerance
        \param[in] result   destination of the simplified polyline
        \return             one beyond the last coordinate of the simplified polyline
    */
    template
    <
        unsigned DIM,
        typename ForwardIterator,
        typename Distance,
        typename OutputIterator
    >
    OutputIterator simplify_douglas_peucker (
        ForwardIterator first,
        ForwardIterator last,
        Distance tol,
        OutputIterator result)
    {
        return algo::douglas_peucker
            <
                DIM,
                ForwardIterator,
                Distance,
                OutputIterator
            >::simplify (first, last, tol, result);
    }

    /*!
        \brief Performs Douglas-Peucker approximation, but uses a point count tolerance (DPn).

        This algorithm is a variation of the original implementation. Instead of considering
        one polyline segment at a time, all segments of the current simplified polyline are
        evaluated at each step. Only the vertex with the maximum distance from its edge is
        added to the simplification. This process will recurse untill the the simplification
        contains the desired amount of vertices.

        The algorithm, which does not use the radial distance simplification routine as a
        preprocessing step, is O(n2) in worst case and O(n log n) on average.

        DPn is applied to the range [first, last). The resulting simplified polyline consists
        of count vertices and is copied to the output range [result, result + count). The
        return value is the end of the output range: result + count.

        Input (Type) requirements:
        1- DIM is not 0, where DIM represents the dimension of the polyline
        2- The RandomAccessIterator value type is convertible to a value type of the OutputIterator
        3- The range [first, last) contains vertex coordinates in multiples of DIM, f.e.:
           x, y, z, x, y, z, x, y, z when DIM = 3
        4- The range [first, last) contains a minimum of count vertices
        5- count > 2

        In case these requirements are not met, the entire input range [first, last) is copied
        to the output range [result, result + (last - first)) OR compile errors may occur.

        \param[in] first    the first coordinate of the first polyline point
        \param[in] last     one beyond the last coordinate of the last polyline point
        \param[in] count    the maximum number of points of the simplified polyline
        \param[in] result   destination of the simplified polyline
        \return             one beyond the last coordinate of the simplified polyline
    */
    template
    <
        unsigned DIM,
        typename RandomAccessIterator,
        typename Size,
        typename OutputIterator
    >
    OutputIterator simplify_douglas_peucker_n (
        RandomAccessIterator first,
        RandomAccessIterator last,
        Size count,
        OutputIterator result)
    {
        return algo::douglas_peucker_n
            <
                DIM,
                RandomAccessIterator,
                Size,
                OutputIterator
            >::simplify (first, last, count, result);
    }

    /*!
        \brief Computes the squared positional error between a polyline and its simplification.

        For each point in the range [original_first, original_last) the squared distance to the
        simplification [simplified_first, simplified_last) is calculated. Each positional error
        is copied to the output range [result, result + count), where count is the number of
        points in the original polyline. The return value is the end of the output range:
        result + count.

        \image html psimpl_pos_error.png

        Input (Type) requirements:
        1- DIM is not 0, where DIM represents the dimension of the polyline
        2- The value type of ForwardIterator1 is convertible to the value type of ForwardIterator2
        3- The value type of ForwardIterator2, possibly promoted to a floating point type, is convertible
           to the value type of OutputIterator
        4- The ranges [original_first, original_last) and [simplified_first, simplified_last)
           contain vertex coordinates in multiples of DIM, f.e.: x, y, z, x, y, z, x, y, z
           when DIM = 3
        5- The ranges [original_first, original_last) and [simplified_first, simplified_last)
           each contain a minimum of 2 vertices
        6- The range [simplified_first, simplified_last) represents a simplification of the
           range [original_first, original_last), meaning each point in the simplification
           has the exact same coordinates as some point from the original polyline

        In case these requirements are not met, the valid flag is set to false OR
        compile errors may occur.

        \param[in] original_first   the first coordinate of the first polyline point
        \param[in] original_last    one beyond the last coordinate of the last polyline point
        \param[in] simplified_first the first coordinate of the first simplified polyline point
        \param[in] simplified_last  one beyond the last coordinate of the last simplified polyline point
        \param[in] result           destination of the squared positional errors
        \param[out] valid           [optional] indicates if the computed positional errors are valid
        \return                     one beyond the last computed positional error
    */
    template
    <
        unsigned DIM,
        typename ForwardIterator1,
        typename ForwardIterator2,
        typename OutputIterator
    >
    OutputIterator compute_positional_errors2 (
        ForwardIterator1 original_first,
        ForwardIterator1 original_last,
        ForwardIterator2 simplified_first,
        ForwardIterator2 simplified_last,
        OutputIterator result,
        bool* valid=0)
    {
        return error::positional
            <
                DIM,
                ForwardIterator1,
                ForwardIterator2,
                OutputIterator
            >::compute (original_first, original_last,
                        simplified_first, simplified_last,
                        result, valid);
    }

    /*!
        \brief Computes statistics for the positional errors between a polyline and its simplification.

        Various statistics (mean, max, sum, std) are calculated for the positional errors
        between the range [original_first, original_last) and its simplification the range
        [simplified_first, simplified_last).

        Input (Type) requirements:
        1- DIM is not 0, where DIM represents the dimension of the polyline
        2- The value type of ForwardIterator1 is convertible to the value type of ForwardIterator2
        3- The value type of ForwardIterator2, possibly promoted to a floating point type, is convertible
           to double
        3- The ranges [original_first, original_last) and [simplified_first, simplified_last)
           contain vertex coordinates in multiples of DIM, f.e.: x, y, z, x, y, z, x, y, z
           when DIM = 3
        4- The ranges [original_first, original_last) and [simplified_first, simplified_last)
           contain a minimum of 2 vertices
        5- The range [simplified_first, simplified_last) represents a simplification of the
           range [original_first, original_last), meaning each point in the simplification
           has the exact same coordinates as some point from the original polyline

        In case these requirements are not met, the valid flag is set to false OR
        compile errors may occur.

        \sa ComputePositionalErrors2

        \param[in] original_first   the first coordinate of the first polyline point
        \param[in] original_last    one beyond the last coordinate of the last polyline point
        \param[in] simplified_first the first coordinate of the first simplified polyline point
        \param[in] simplified_last  one beyond the last coordinate of the last simplified polyline point
        \param[out] valid           [optional] indicates if the computed statistics are valid
        \return                     the computed statistics
    */
    template
    <
        unsigned DIM,
        typename ForwardIterator1,
        typename ForwardIterator2
    >
    error::statistics compute_positional_error_statistics (
        ForwardIterator1 original_first,
        ForwardIterator1 original_last,
        ForwardIterator2 simplified_first,
        ForwardIterator2 simplified_last,
        bool* valid=0)
    {
        return error::positional_statistics
            <
                DIM,
                ForwardIterator1,
                ForwardIterator2
            >::compute (original_first, original_last,
                        simplified_first, simplified_last,
                        valid);
    }
}

#endif // PSIMPL_GENERIC
