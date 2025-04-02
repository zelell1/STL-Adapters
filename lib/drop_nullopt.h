#pragma once

#include "veiw.h"
#include <optional>
#include <algorithm>

template <typename Range>
class DropNulloptIterator {
private:
    using iterator = iterator_type<Range>;
    iterator begin_;
    iterator end_;
    
    struct Predicate {
        auto operator()(const auto& value) {
            return value.has_value();
        }
    } pred_;

public:
    using difference_type = std::ptrdiff_t;
    using value_type = iterator_value_type<iterator>::value_type;
    using pointer = const value_type*;
    using reference = const value_type&;
    using iterator_category = std::forward_iterator_tag;

    DropNulloptIterator(Range& range, bool end = false) : begin_(range.begin()), end_(range.end()) {
        if (!end) {
            begin_ = std::find_if(begin_, end_, pred_);
        } else {
            begin_ = end_;
        }
    };

    DropNulloptIterator& operator++() {
        ++begin_;
        begin_ = std::find_if(begin_, end_, pred_);
        return *this;
    }

    DropNulloptIterator operator++(int) {
        DropNulloptIterator temp(*this);
        ++(*this);
        return temp;
    }

    reference operator*() const {
        return **begin_; 
    }

    pointer operator->() const {
        return &(**begin_);
    }

    bool operator==(const DropNulloptIterator& other) const {
        return begin_ == other.begin_;
    }

    bool operator!=(const DropNulloptIterator& other) const {
        return !(*this == other);
    } 
};

template <typename Range>
class DropNulloptView : public ViewInterface<DropNulloptView<Range>> {
private:
    Range& range_;

public:
    DropNulloptView(Range& range) : range_(range) {};

    auto begin() const {
        return DropNulloptIterator<Range>(range_);
    }

    auto end() const {
        return DropNulloptIterator<Range>(range_, true);
    }
};

class DropNullopt {
public:
    template <typename Range>
    auto operator()(Range&& range) {
        return DropNulloptView<Range>(range);
    }
};
