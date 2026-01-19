#pragma once

#include <iostream>
#include <vector>

class AsVector {
public:
    template <typename Range>
    auto operator()(Range&& range) {
        std::vector<range_value_type<Range>> res;
        for (const auto& el : range) {
            res.push_back(el);
        }
        return res;
    }
};