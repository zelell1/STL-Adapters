#pragma once

#include "veiw.h"
#include <string>
#include <iterator>
#include <sstream>
#include <vector>

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
        while (begin_ != end_) {
            while (istream_begin_ != istream_end_ && pred_.find(*istream_begin_) != std::string::npos) {
                ++istream_begin_;
            }
            if (istream_begin_ == istream_end_) {
                ++begin_;
                if (begin_ != end_) {
                    istream_begin_ = std::istreambuf_iterator<char>(*begin_);
                    istream_end_ = std::istreambuf_iterator<char>();
                }
                continue;
            }
            while (istream_begin_ != istream_end_ && pred_.find(*istream_begin_) == std::string::npos) {
                substr_.push_back(*istream_begin_);
                ++istream_begin_;
            }
            if (!substr_.empty())
                break;
        }
    }

public:
    using difference_type = std::ptrdiff_t;
    using value_type = std::string;
    using pointer = const value_type*;
    using reference = const value_type&;
    using iterator_category = std::forward_iterator_tag;

    SplitViewIterator(Range& range, std::string pred, bool is_end = false) : pred_(pred), substr_(""), is_end_(is_end) {
        begin_ = range.begin();
        end_ = range.end();
        istream_begin_ = std::istreambuf_iterator<char>(*begin_);
        istream_end_ = std::istreambuf_iterator<char>();
        if (is_end) {
            begin_ = range.end();
            end_ = range.end();
            istream_begin_ = std::istreambuf_iterator<char>();
            istream_end_ = std::istreambuf_iterator<char>();
        }
    }

    SplitViewIterator& operator++() {
        GetSubstr();
        return *this;
    }

    SplitViewIterator operator++(int) {
        SplitViewIterator tmp = *this;
        ++(*this);
        return tmp;
    }

    reference operator*() { 
        if (substr_.empty()) {
            GetSubstr();
        }
        return substr_; 
    }

    bool operator==(const SplitViewIterator& other) const {
        return (begin_ == end_ && istream_begin_ == istream_end_);
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
    explicit SplitView(Range range, Pred pred) : range_(range), pred_(pred) {}

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
    explicit Split(Pred pred) : pred_(pred) {}

    template <typename Range>
    auto operator()(Range&& range) {
        return SplitView<Range, Pred>(std::forward<Range>(range), pred_);
    }
};