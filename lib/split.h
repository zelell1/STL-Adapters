#pragma once

#include "veiw.h"

template <typename Range>
class SplitViewIterator {
private:
    using iterator = iterator_type<Range>;
    using stream_iterator = std::istreambuf_iterator<char>;
    
    iterator begin_;
    iterator end_;
    stream_iterator istream_begin_;
    stream_iterator istream_end_;
    std::string substr_;
    std::string pred_;
    bool is_end_;

    void GetSubstr() {
        substr_.clear();
        if (is_end_ || (begin_ == end_ && istream_begin_ == istream_end_)) {
            is_end_ = true;
            return;
        }
        if (istream_begin_ == istream_end_) {
            ++begin_;
            if (begin_ != end_) {
                begin_->clear();
                begin_->seekg(0, std::ios::beg);
                istream_begin_ = std::istreambuf_iterator<char>(*begin_);
                istream_end_ = std::istreambuf_iterator<char>();
            } else {
                is_end_ = true;
                return;
            }
        }
        while (istream_begin_ != istream_end_) {
            char c = *istream_begin_;
            if (pred_.find(c) != std::string::npos) {
                ++istream_begin_;
                break;
            }
            substr_.push_back(c);
            ++istream_begin_;
        }
    }

public:
    using difference_type = std::ptrdiff_t;
    using value_type = std::string;
    using pointer = const value_type*;
    using reference = value_type&;
    using iterator_category = std::forward_iterator_tag;

    SplitViewIterator(Range& range, std::string pred, bool is_end = false) : begin_(range.begin()), end_(range.end()), 
    istream_begin_(begin_ != end_ ? std::istreambuf_iterator<char>(*begin_) : std::istreambuf_iterator<char>()), 
    istream_end_(std::istreambuf_iterator<char>()), pred_(pred), substr_(""), is_end_(is_end) {
        if (!is_end && begin_ != end_) {
            begin_->clear();
            begin_->seekg(0, std::ios::beg);
            istream_begin_ = std::istreambuf_iterator<char>(*begin_);
            istream_end_ = std::istreambuf_iterator<char>();
            GetSubstr(); 
        }
    };

    SplitViewIterator& operator++() {
        GetSubstr();
        return *this;
    }

    SplitViewIterator operator++(int) {
        SplitViewIterator tmp = *this;
        ++(*this);
        return tmp;
    }

    value_type operator*() const { 
        return substr_; 
    }

    value_type operator->() const { 
        return substr_; 
    }

    bool operator==(const SplitViewIterator& other) const {
        return (is_end_ == other.is_end_ && (is_end_ || (begin_ == other.begin_ && istream_begin_ == other.istream_begin_)));
    }

    bool operator!=(const SplitViewIterator& other) const {
        return !(*this == other);
    }
};

template <typename Range, typename Pred>
class SplitView : public ViewInterface<SplitView<Range, Pred>> {
private:
    Range& range_;
    Pred pred_;

public:
    SplitView(Range& range, Pred pred) : range_(range), pred_(pred) {};

    auto begin() { 
        return SplitViewIterator<Range>(range_, pred_);
    }
    
    auto end() {
        return SplitViewIterator<Range>(range_, pred_, true);
    }
};

template <typename Pred>
class Split {
private:
    Pred pred_;

public:
    Split(Pred pred) : pred_(pred) {};

    template <typename Range>
    auto operator()(Range&& range) {
        return SplitView<Range, Pred>(range, pred_);
    }
};