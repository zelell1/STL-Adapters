#pragma once

#include "veiw.h"
#include <expected>
#include <iterator>
#include <utility>

template <typename Range>
class ExpectedIterator {
private:
    using iterator = iterator_type<Range>;

    mutable iterator begin_;
    iterator end_;

    void GetExpected() const {
        while (begin_ != end_ && !begin_->has_value()) {
            ++begin_;
        }
    }

public:
    using value_type = typename std::iterator_traits<iterator>::value_type::value_type;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using reference = const value_type&;
    using iterator_category = std::forward_iterator_tag;

    ExpectedIterator(const Range& range, bool is_end = false) : begin_(is_end ? range.end() : range.begin()), 
    end_(range.end()) {};

    auto operator*() const {
        GetExpected(); 
        if (!begin_->has_value()) {
            return value_type();
        }
        return begin_->value(); 
    }

    auto operator->() const { 
        return &(begin_->value()); 
    }

    ExpectedIterator& operator++() {
        ++begin_;
        GetExpected();
        return *this;
    }

    ExpectedIterator operator++(int) {
        ExpectedIterator temp(*this);
        ++(*this);
        return temp;
    }

    bool operator==(const ExpectedIterator& other) const { 
        return begin_ == other.begin_; 
    }

    bool operator!=(const ExpectedIterator& other) const { 
        return begin_ != other.begin_;
    }
};

template <typename Range>
class UnexpectedIterator {
private:
    using iterator = iterator_type<Range>;

    mutable iterator begin_;
    iterator end_;

    void GetUnexpected() const {
        while (begin_ != end_ && begin_->has_value()) {
            ++begin_;
        }
    }

public:
    using value_type = typename std::iterator_traits<iterator>::value_type::error_type;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using reference = const value_type&;
    using iterator_category = std::forward_iterator_tag;

    UnexpectedIterator(const Range& range, bool is_end = false) : begin_(is_end ? range.end() : range.begin()), 
    end_(range.end()) {};

    UnexpectedIterator& operator++() {
        ++begin_;
        GetUnexpected();
        return *this;
    }

    UnexpectedIterator operator++(int) {
        UnexpectedIterator temp(*this);
        ++(*this);
        return temp;
    }

    auto operator*() const {
        GetUnexpected();
        if (begin_->has_value()) {
            return value_type();
        }
        return begin_->error(); 
    }

    auto operator->() const { 
        return &(begin_->error()); 
    }

    bool operator==(const UnexpectedIterator& other) const { 
        return begin_ == other.begin_ ; 
    }

    bool operator!=(const UnexpectedIterator& other) const { 
        return begin_ != other.begin_; 
    }

};

template <typename Range>
class ExpectedView : public ViewInterface<ExpectedView<Range>> {
private:
    Range& range_;
    
public:
    ExpectedView(Range& range) : range_(range) {};

    auto begin() const { 
        return ExpectedIterator<Range>(range_); 
    }

    auto end() const { 
        return ExpectedIterator<Range>(range_, true); 
    }
};

template <typename Range>
class UnexpectedView : public ViewInterface<UnexpectedView<Range>> {
private:
    Range& range_;

public:
    UnexpectedView(Range& range) : range_(range) {}

    auto begin() const { 
        return UnexpectedIterator<Range>(range_); 
    }

    auto end() const { 
        return UnexpectedIterator<Range>(range_, true); 
    }


};

struct SplitExpected {
    template <typename Range>
    auto operator()(Range&& range) const {
        return std::make_pair(UnexpectedView<Range>(range), ExpectedView<Range>(range));
    }
};
