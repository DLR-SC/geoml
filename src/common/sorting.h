/*
* SPDX-License-Identifier: Apache-2.0
* SPDX-FileCopyrightText: 2018 German Aerospace Center (DLR)
*
* Created: 2018-11-23 Martin Siggel <martin.siggel@dlr.de>
*/

#ifndef SORTING_H
#define SORTING_H

#include <algorithm>

namespace geoml
{

template<class ForwardIterator>
void rotate_right(ForwardIterator begin, ForwardIterator last)
{
    for (; begin != last; ++begin) {
        std::swap(*begin, *last);
    }
}

/**
 * Sorts an array that contains a continuous number of entries, possible unordered
 */
template<class ForwardIterator, class UnaryPredicate>
void follow_sort(ForwardIterator begin, ForwardIterator end, UnaryPredicate follows)
{

    ForwardIterator sorted = begin;

    bool swapped = true;
    while (swapped) {
        swapped = false;
        for (ForwardIterator it = sorted + 1; it != end; ++it) {
            if (follows(*it, *sorted)) {
                sorted++;
                std::swap(*it, *sorted);
                swapped = true;
            }
            else if (follows(*begin, *it)) {
                sorted++;
                std::swap(*it, *sorted);
                rotate_right(begin, sorted);
                swapped = true;
            }
        }
    }

    if (sorted+1 != end) {
        throw std::invalid_argument("Vector not continuous");
    }
}

}

#endif // SORTING_H
