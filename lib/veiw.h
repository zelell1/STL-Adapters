#pragma once

#include <algorithm>

template <typename Range>
using range_value_type = typename std::decay_t<decltype(std::declval<Range>().begin())>::value_type;

template <typename Range>
using iterator_type = decltype(std::declval<Range>().begin());

template <typename Iterator>
using iterator_value_type = typename std::iterator_traits<Iterator>::value_type;

template <typename Derrived>
class ViewInterface {
public:
   auto begin() const {
        return static_cast<const Derrived*>(this)->begin();
   }
   
   auto end() const {
        return static_cast<const Derrived*>(this)->end();
   }
};


