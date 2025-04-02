#pragma once

#include "veiw.h"
#include <filesystem>
#include <variant>

template <typename Path = std::filesystem::path>
class DirIterator {
private:
    using iterator = std::variant<std::filesystem::directory_iterator, 
                        std::filesystem::recursive_directory_iterator>;
    iterator it_;

public:
    using difference_type = std::ptrdiff_t;
    using value_type = std::filesystem::path;
    using pointer = const value_type*;
    using reference = const value_type&;
    using iterator_category = std::input_iterator_tag;

    DirIterator(const Path& path, bool recursive = false) {
        if (recursive)
            it_ = std::filesystem::recursive_directory_iterator(path);
        else
            it_ = std::filesystem::directory_iterator(path);
    };

    DirIterator(const DirIterator& other) : it_(other.it_) {};
    DirIterator(bool recursive) {
        if (recursive) {
            it_ = std::filesystem::recursive_directory_iterator();
        } else {
            it_ = std::filesystem::directory_iterator();
        }
    };


    DirIterator& operator++() {
        std::visit([](auto& it) { ++it; }, it_);
        return *this;
    }

    DirIterator operator++(int) {
        DirIterator temp(*this);
        ++(*this);
        return temp;
    }

    reference operator*() const {
        return std::visit([](const auto& it) { return *it; }, it_);
    }

    pointer operator->() const {
        return std::visit([](const auto& it) { return &(*it); }, it_);
    }

    bool operator==(const DirIterator& other) const {
        return it_ == other.it_;
    }

    bool operator!=(const DirIterator& other) const {
        return !(*this == other);
    }
};

template <typename Path = std::filesystem::path>
class Dir {
private:    
    Path path_;
    bool recursive_;
    
public: 
    using value_type = Path;

    Dir(const Path& path, bool recursive = false) : path_(path), recursive_(recursive) {};

    auto begin() const { 
        return DirIterator<Path>(path_, recursive_); 
    }

    auto end() const { 
        return DirIterator<Path>(recursive_); 
    }
};