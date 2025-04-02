#pragma once

#include "veiw.h"
#include <filesystem>
#include <fstream>    
#include <memory>     

template <typename Range>
class OpenFilesIterator {
private:
    using iterator = iterator_type<Range>;
    iterator begin_;
    iterator end_;
    std::shared_ptr<std::ifstream> curr_file_;

    void GetFile() {
        if (begin_ == end_) {
            curr_file_.reset();
            return;
        }
        curr_file_ = std::make_shared<std::ifstream>(begin_->string());
        while (begin_ != end_ && ! curr_file_->is_open()) {
            ++begin_;
            if (begin_ != end_) {
                curr_file_ = std::make_shared<std::ifstream>(begin_->string());
            } else {
                curr_file_.reset();
            }
        }
    }

public:
    using difference_type = iterator::difference_type; 
    using value_type = std::ifstream;                                
    using pointer = std::shared_ptr<std::ifstream>;                              
    using reference = std::ifstream&;                                 
    using iterator_category = std::input_iterator_tag;

    OpenFilesIterator(const Range& range, bool is_end = false) : begin_(range.begin()), 
    end_(range.end()), curr_file_(nullptr) {
        if (is_end) {
            begin_ = range.end();
        }
        GetFile();
    };

    OpenFilesIterator(const OpenFilesIterator& other) : begin_(other.begin_), end_(other.end_), 
    curr_file_(other.curr_file_) {};

    OpenFilesIterator& operator++() {
        ++begin_;     
        GetFile();     
        return *this;
    }

    OpenFilesIterator operator++(int) {
        OpenFilesIterator tmp(*this);
        ++(*this);
        return tmp;
    }

    reference operator*() const {
        return *curr_file_;
    }

    pointer operator->() const {
        return curr_file_;
    }

    bool operator==(const OpenFilesIterator& other) const {
        return (begin_ == other.begin_ && end_ == other.end_ && curr_file_ == other.curr_file_);
    }

    bool operator!=(const OpenFilesIterator& other) const {
        return !(*this == other);
    }
};

template <typename Range>
class OpenFilesView {
private:
    Range& range_;

public:
    using value_type = std::ifstream;

    OpenFilesView(Range& range) : range_(range) {};

    auto begin() const {
        return OpenFilesIterator<Range>(range_);
    }

    auto end() const {
        return OpenFilesIterator<Range>(range_, true);
    }
};

class OpenFiles {
public:
    template <typename Range>
    auto operator()(Range&& range) {
        return OpenFilesView<Range>(range);
    }
};