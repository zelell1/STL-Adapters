#pragma once

#include "veiw.h"
#include <expected>
#include <tuple>
#include <utility>
#include <type_traits>

template <typename Range>
class UnexpectedIterator {
private:
    using iterator = iterator_type<Range>;
    iterator current_;
    iterator end_;
    void skip() {
        while (current_ != end_ && static_cast<bool>(*current_))
            ++current_;
    }
public:
    using value_type = std::remove_reference_t<decltype((*current_).error())>;  

    UnexpectedIterator(iterator begin, iterator end)
        : current_(begin), end_(end) {
        skip();
    }

    UnexpectedIterator& operator++() {
        ++current_;
        skip();
        return *this;
    }

    auto operator*() const -> value_type {
        return (*current_).error(); 
    }

    struct arrow_proxy {
        value_type val;
        explicit arrow_proxy(value_type v) : val(v) {}
        const value_type* operator->() const { return &val; }
    };

    auto operator->() const -> arrow_proxy {
        return arrow_proxy(operator*());
    }

    bool operator==(const UnexpectedIterator& other) const {
        return current_ == other.current_;
    }

    bool operator!=(const UnexpectedIterator& other) const {
        return !(*this == other);
    }
};

template <typename Range>
class ExpectedIterator {
private:
    using iterator = iterator_type<Range>;
    iterator current_;
    iterator end_;
    void skip() {
        while (current_ != end_ && !static_cast<bool>(*current_))
            ++current_;
    }
public:
    using value_type = std::remove_reference_t<decltype((*current_).value())>;  

    ExpectedIterator(iterator begin, iterator end)
        : current_(begin), end_(end) {
        skip();
    }

    ExpectedIterator& operator++() {
        ++current_;
        skip();
        return *this;
    }

    auto operator*() const -> value_type {
        return (*current_).value();  
    }

    struct arrow_proxy {
        value_type val;
        explicit arrow_proxy(value_type v) : val(v) {}
        const value_type* operator->() const { return &val; }
    };

    auto operator->() const -> arrow_proxy {
        return arrow_proxy(operator*());
    }

    bool operator==(const ExpectedIterator& other) const {
        return current_ == other.current_;
    }

    bool operator!=(const ExpectedIterator& other) const {
        return !(*this == other);
    }
};


template <typename Range>
class UnexpectedView : public ViewInterface<UnexpectedView<Range>> {
private:
    Range& range_;
public:
    explicit UnexpectedView(Range& range) : range_(range) {}
    auto begin() const {
        return UnexpectedIterator<Range>(range_.begin(), range_.end());
    }
    auto end() const {
        return UnexpectedIterator<Range>(range_.end(), range_.end());
    }
};

template <typename Range>
class ExpectedView : public ViewInterface<ExpectedView<Range>> {
private:
    Range& range_;
public:
    explicit ExpectedView(Range& range) : range_(range) {}
    auto begin() const {
        return ExpectedIterator<Range>(range_.begin(), range_.end());
    }
    auto end() const {
        return ExpectedIterator<Range>(range_.end(), range_.end());
    }
};

class SplitExpected {
public:
    template <typename Range>
    static auto operator()(Range& range) {
        return std::make_pair(UnexpectedView<Range>(range),
                              ExpectedView<Range>(range));
    }
};
