#pragma once

#include "veiw.h"

template <typename OutStream>
class Out {
private:
    OutStream& stream_;

public:
    Out(OutStream& stream) : stream_(stream) {};

    template <typename Range>
    auto operator()(Range&& range) {
        for (const auto& el : range) {
            stream_ << el << '\n';
        }
        return range;
    }
};
