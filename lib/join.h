#pragma once

#include "veiw.h"
#include <map>

template <typename Key, typename Value>
struct KV {
	Key key;
	Value value;

    using key_type = Key;
    using value_type = Value;
};

template <typename Base, typename Joined>
struct JoinResult {
	Base base;
	std::optional<Joined> joined;

    bool operator==(const JoinResult& other) const {
        return base == other.base && joined == other.joined;
    }
};

template <typename RhsRange, typename LhsPattern, typename RhsPattern>
class JoinRanges {
private:
    RhsRange rhs_range_;
    LhsPattern lhs_pattern_;
    RhsPattern rhs_pattern_;

public:
    JoinRanges(RhsRange range, LhsPattern lhs_pattern, RhsPattern rhs_pattern) : rhs_range_(range), 
    lhs_pattern_(lhs_pattern), rhs_pattern_(rhs_pattern) {};

    template <typename Range>
    auto operator()(Range&& range) const {
        using lhs_type = range_value_type<Range>;
        using rhs_type = range_value_type<RhsRange>;
        using key_type = std::decay_t<decltype(lhs_pattern_(std::declval<lhs_type>()))>;

        std::multimap<key_type, rhs_type> map;
        for (const auto& el : rhs_range_) {
            map.emplace(rhs_pattern_(el), el);
        }

        std::vector<JoinResult<lhs_type, rhs_type>> result;
        for (const auto& left : range) {
            auto key = lhs_pattern_(left);
            auto [begin, end] = map.equal_range(key);
            if (begin == end) {
                result.push_back({left, std::nullopt});
            } else {
                for (auto it = begin; it != end; ++it) {
                    result.push_back({left, it->second});
                }
            }
        }
        return result;
    }
};

template <typename RhsRange>
class JoinKV {
private:
    RhsRange rhs_range_;

public:
    JoinKV(const RhsRange& range) : rhs_range_(range) {};

    template <typename Range>
    auto operator()(Range&& range) const {
        using lhs_type = range_value_type<Range>;        
        using rhs_type = range_value_type<RhsRange>;    
        using lhs_key_type = typename lhs_type::key_type;   
        using lhs_value_type = typename lhs_type::value_type; 
        using rhs_key_type = typename rhs_type::key_type;
        using rhs_value_type = typename rhs_type::value_type;

        std::multimap<rhs_key_type, rhs_value_type> map;
        for (const auto& el : rhs_range_) {
            map.emplace(el.key, el.value);
        }

        std::vector<JoinResult<lhs_value_type, rhs_value_type>> result;
        for (const auto& left : range) {
            auto [begin, end] = map.equal_range(left.key);
            if (begin == end) {
                result.push_back({left.value, std::nullopt});
            } else {
                for (auto it = begin; it != end; ++it) {
                    result.push_back({left.value, it->second});
                }
            }
        }
        return result;
    }
};

template <typename RhsRange>
auto Join(RhsRange&& rhs_range) {
    return JoinKV<RhsRange>(rhs_range);
}

template <typename RhsRange, typename LhsPattern, typename RhsPattern>
auto Join(RhsRange&& rhs_range, const LhsPattern& lhs_pattern, const RhsPattern& rhs_pattern) {
    return JoinRanges<RhsRange, LhsPattern, RhsPattern>(rhs_range, lhs_pattern, rhs_pattern);
}


