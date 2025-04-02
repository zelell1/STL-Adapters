#pragma once

#include "veiw.h"
#include <type_traits>

template <typename Range, typename Pred>
class TransformViewIterator {
public:
    using difference_type = std::ptrdiff_t;
    using value_type = std::decay_t<decltype(std::declval<Pred>()(std::declval<range_value_type<Range>>()))>;
    using pointer = const value_type*;
    using reference = const value_type&;
    using iterator_category = std::forward_iterator_tag;

private:
    using iterator = iterator_type<Range>;
    iterator begin_;
    iterator end_;
    Pred pred_;
    value_type current_value_;

public:
    TransformViewIterator(Range& range, const Pred& pred, bool is_end = false) : begin_(range.begin()), 
    end_(range.end()), pred_(pred) {
        if (is_end) {
            begin_ = end_;
        }
        if (begin_ != end_) {
            current_value_ = pred_(*begin_);
        }
    };

    TransformViewIterator(const TransformViewIterator& other) : begin_(other.begin_),  end_(other.end_), 
    pred_(other.pred_) {};

    TransformViewIterator& operator++() {
        ++begin_;
        if (begin_ != end_) {
            current_value_ = pred_(*begin_);
        }
        return *this;
    }

    TransformViewIterator operator++(int) {
        TransformViewIterator temp(*this);
        ++(*this);
        return temp;
    }

    value_type operator*() const {
        return current_value_;
    }

    pointer operator->() const {
        return &current_value_;
    }

    bool operator==(const TransformViewIterator& other) const {
        return (begin_ == other.begin_ && end_ == other.end_);
    }

    bool operator!=(const TransformViewIterator& other) const {
        return !(*this == other);
    }
};

template <typename Range, typename Pred>
class TransformView : public ViewInterface<TransformView<Range, Pred>> {
private:
    Range& range_;
    Pred pred_;

public:
    using value_type = range_value_type<Range>;

    TransformView(Range& range, Pred pred) : range_(range), pred_(pred) {}

    auto begin() const {
        return TransformViewIterator<Range, Pred>(range_, pred_);
    }

    auto end() const {
        return TransformViewIterator<Range, Pred>(range_, pred_, true);
    }
};

template <typename Pred>
class Transform {
private:
    using DecayedPred = std::decay_t<Pred>;
    DecayedPred pred_;

public:
    Transform(const Pred& pred) : pred_(pred) {}

    template <typename Range>
    auto operator()(Range&& range) {
        return TransformView<Range, DecayedPred>(range, pred_);
    }
};

