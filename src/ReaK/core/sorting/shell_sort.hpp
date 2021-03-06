/**
 * \file shell_sort.hpp
 * 
 * This library provides a generic shell-sort function.
 * 
 * \author Sven Mikael Persson <mikael.s.persson@gmail.com>
 * \date July 2012
 */

/*
 *    Copyright 2012 Sven Mikael Persson
 *
 *    THIS SOFTWARE IS DISTRIBUTED UNDER THE TERMS OF THE GNU GENERAL PUBLIC LICENSE v3 (GPLv3).
 *
 *    This file is part of ReaK.
 *
 *    ReaK is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    ReaK is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with ReaK (as LICENSE in the root folder).  
 *    If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef REAK_SHELL_SORT_HPP
#define REAK_SHELL_SORT_HPP

#include "base/defs.hpp"

#include <algorithm>
#include <iterator>
#include <functional>

namespace ReaK {
  
/** This is the namespace for all ReaK sorting algorithms implementations. */
namespace sorting {

/**
 * This function performs a shell sort on a given range of elements, and with the 
 * given comparison functor.
 * \tparam RandomAccessIter A random-access iterator type (input and output iterator).
 * \tparam Compare A comparison functor type that can produce a bool value to order two elements.
 * \param first The start of the range to be sorted.
 * \param last One element past the end of the range to be sorted.
 * \param comp The comparison functor to use to determine the order of elements.
 */
template <typename RandomAccessIter, typename Compare>
void shell_sort(RandomAccessIter first, RandomAccessIter last, Compare comp) {
  typedef typename std::iterator_traits<RandomAccessIter>::value_type ValueType;
  std::size_t max_gap = std::distance(first,last) / 9;
  std::size_t gap = 1;
  for(; gap <= max_gap; gap = 3 * gap + 1);
  for(; gap > 0; gap = (gap + 1) / 3) {
    for(RandomAccessIter current = first + gap; current < last; ++current) {
#ifdef RK_ENABLE_CXX0X_FEATURES
      ValueType tmp = std::move(*current);
#else
      ValueType tmp = *current;
#endif
      RandomAccessIter it = current;
      for(RandomAccessIter it_prev; ((it > first + gap) && (comp(tmp, *(it_prev = it - gap)))); it -= gap)
#ifdef RK_ENABLE_CXX0X_FEATURES
        *it = std::move(*it_prev);
#else
        *it = *it_prev;
#endif
#ifdef RK_ENABLE_CXX0X_FEATURES
      *it = std::move(tmp);
#else
      *it = tmp;
#endif
    };
  };
};

/**
 * This function performs a shell sort on a given range of elements, and by using the
 * less-than operator on the elements of the range.
 * \tparam RandomAccessIter A random-access iterator type (input and output iterator).
 * \param first The start of the range to be sorted.
 * \param last One element past the end of the range to be sorted.
 */
template <typename RandomAccessIter>
void shell_sort(RandomAccessIter first, RandomAccessIter last) {
  shell_sort(first, last, std::less< typename std::iterator_traits<RandomAccessIter>::value_type >());
};




};

};

#endif



