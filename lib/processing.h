#pragma once

#include <optional>
#include "dir.h"
#include "filter.h"
#include "data_flow.h"
#include "out.h"
#include "as_vector.h"
#include "write.h"
#include "transform.h"
#include "split.h"
#include "drop_nullopt.h"
#include "aggregate.h"
#include "open_files.h"

template <typename Key, typename Value>
struct KV {
	Key key;
	Value value;
};

template <typename Base, typename Joined>
struct JoinResult {
	Base base;
	std::optional<Joined> joined;
};

template <typename Range, typename Adapter>
decltype(auto) operator|(Range&& range, Adapter&& adapter) {
    return adapter(range);
}