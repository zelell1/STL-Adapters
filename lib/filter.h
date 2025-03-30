#pragma once

#include "veiw.h"

template <typename Range, typename Pred>
class FilterIterator {
private:
    using iterator = iterator_type<Range>;
    iterator begin_;
    iterator end_;
    Pred pred_;

public:
    using difference_type = std::ptrdiff_t;
    using value_type =  iterator_value_type<iterator>;
    using pointer = const value_type*;
    using reference = const value_type&;
    using iterator_category = std::forward_iterator_tag;

    FilterIterator(Range& range, const Pred& pred, bool end = false) : begin_(range.begin()), end_(range.end()), pred_(pred) {
        if (end) {
            begin_ = end_;
        }
    };

    FilterIterator& operator++() {
        begin_ = std::find_if(begin_, end_, pred_);
        ++begin_;
        begin_ = std::find_if(begin_, end_, pred_);
        return *this;
    }

    FilterIterator operator++(int) {
        FilterIterator temp(*this);
        ++(*this);
        return temp;
    }

    reference operator*() {
        begin_ = std::find_if(begin_, end_, pred_);
        return *begin_;
    }

    pointer operator->() {
        begin_ = std::find_if(begin_, end_, pred_);
        return &(*begin_);
    }

    bool operator==(const FilterIterator& other) const {
        return (begin_ == other.begin_ && end_ == other.end_ && pred_ == other.pred_);
    }

    bool operator!=(const FilterIterator& other) const {
        return !(*this == other);
    }
};  


template <typename Range, typename Pred>
class FilterView : public ViewInterface<FilterView<Range, Pred>> {
private:
    Range& range_;
    Pred pred_;

public:
    using value_type = range_value_type<Range>;

    FilterView(const Range& range, const Pred& pred) : range_(range), pred_(pred) {};

    auto begin() const {
        return FilterIterator<Range, Pred>(range_, pred_);
    }

    auto end() const {
        return FilterIterator<Range, Pred>(range_, pred_, true);
    }
};

template <typename Pred>
class Filter {
private:
    using DecayedPred = std::decay_t<Pred>;
    DecayedPred pred_;

public:
    explicit Filter(const Pred& pred) : pred_(pred) {};

    template <typename Range>
    auto operator()(Range&& range) {
        return FilterView<Range, DecayedPred>(range, pred_);
    }
};