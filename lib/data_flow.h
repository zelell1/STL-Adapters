#pragma once

#include "veiw.h"
#include <iterator>
#include <type_traits>
#include <utility>

template <typename Range>
class AsDataFlowIterator {
private:
    using iterator = iterator_type<Range>;
    iterator begin_;
    iterator end_;

public:
    using difference_type = typename std::iterator_traits<iterator>::difference_type;
    using value_type = iterator_value_type<iterator>;
    using pointer = typename std::iterator_traits<iterator>::pointer;
    using reference = typename std::iterator_traits<iterator>::reference;
    using iterator_category = std::input_iterator_tag;

    AsDataFlowIterator(iterator begin, iterator end) : begin_(begin), end_(end) {};

    AsDataFlowIterator() = default;

    AsDataFlowIterator& operator++() {
        ++begin_;
        return *this;
    }

    AsDataFlowIterator operator++(int) {
        AsDataFlowIterator tmp(*this);
        ++(*this);
        return tmp;
    }

    reference operator*() const {
        return *begin_;
    }

    pointer operator->() const {
        return &(*begin_);
    }

    bool operator==(const AsDataFlowIterator& other) const {
        return begin_ == other.begin_;
    }

    bool operator!=(const AsDataFlowIterator& other) const {
        return !(*this == other);
    }
};

template <typename Range>
class AsDataFlow : public ViewInterface<AsDataFlow<Range>> {
private:
    Range& range_;

public: 
    using value_type = range_value_type<Range>;

    AsDataFlow(Range& range) : range_(range) {};

    auto begin() const {
        return AsDataFlowIterator<Range>(range_.begin(), range_.end());
    }

    auto end() const {
        return AsDataFlowIterator<Range>(range_.end(), range_.end());
    }
};