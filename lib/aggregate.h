#pragma once

#include "veiw.h"
#include <unordered_map>
#include <vector>

template <typename InitialValue, typename Aggregator, typename GetKey>
class AggregateByKey {
private:
    InitialValue init_;
    Aggregator aggregator_;
    GetKey get_key_;

public:
    explicit AggregateByKey(InitialValue init, Aggregator aggregator, GetKey get_key)
                : init_(init), aggregator_(aggregator), get_key_(get_key) {};

    template <typename Range>
    auto operator()(Range&& range) {
        using elem_type = range_value_type<Range>;
        using key_type = std::decay_t<std::invoke_result_t<GetKey, elem_type>>;
        std::unordered_map<key_type, InitialValue> collect_values;
        std::vector<key_type> order;
        for (auto el : range) {
            key_type key = get_key_(el);
            if (std::find(order.begin(), order.end(), key) == order.end()) {
                order.push_back(key);
                collect_values[key] = init_;
            }
            aggregator_(el, collect_values[key]);
        }
        std::vector<std::pair<key_type, InitialValue>> result;
        result.reserve(order.size());
        for (const auto& key : order) {
            result.emplace_back(key, collect_values[key]);
        }
        return result; 
    }
};