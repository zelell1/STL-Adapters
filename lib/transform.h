#pragma once

#include "veiw.h"

template <typename Range, typename Pred>
class TransformViewIterator {
private:
    using iterator = iterator_type<Range>;
    iterator begin_;
    iterator end_;
    Pred pred_;

public:
    using difference_type = std::ptrdiff_t;
    using value_type = decltype(pred_(*begin_));
    using pointer = value_type*;
    using iterator_category = std::forward_iterator_tag;

    TransformViewIterator(Range& range, const Pred& pred, bool end = false) : begin_(range.begin()), end_(range.end()), pred_(pred) {
        if (end) {
            begin_ = end_;
        }
    };


    TransformViewIterator(const TransformViewIterator& other) : begin_(other.begin_), end_(other.end_), pred_(other.pred_) {};

    TransformViewIterator& operator++() {
        ++begin_;
        return *this;
    }

    TransformViewIterator operator++(int) {
        TransformViewIterator temp(*this);
        ++(*this);
        return temp;
    }
 
    value_type operator*() {
        return pred_(*begin_);
    }

    pointer operator->() {
        return &(operator*());
    }

    bool operator==(const TransformViewIterator& other) const {
        return (begin_ == other.begin_ && end_ == other.end_ && pred_ == other.pred_);
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

    explicit TransformView(Range range, Pred pred) : range_(range), pred_(pred) {};

    auto begin() const {
        return TransformViewIterator(range_, pred_);
    }

    auto end() const {
        return TransformViewIterator(range_, pred_, true);
    }
};

template <typename Pred>
class Transform {
private:
    Pred pred_;

public:
    explicit Transform(const Pred& pred) : pred_(pred) {};

    template <typename Range> 
    auto operator()(Range&& range) {
        return TransformView<Range, Pred>(range, pred_);
    }
};