#pragma once

#include "veiw.h"

template <typename OutStream>
class Write {
private:
    OutStream& stream_;
    char delim_;

public:
    Write(OutStream& stream, const char& delim) : stream_(stream), delim_(delim) {};

    template <typename Range>
    auto operator()(Range&& range) {
        for (const auto& el : range) {
            stream_ << el << delim_;
        }
        return range;
    }
};