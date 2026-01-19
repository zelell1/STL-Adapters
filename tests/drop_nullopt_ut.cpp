#include <processing.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

TEST(DropNulloptTest, DropNullopt) {
    std::vector<std::optional<int>> input = {1, std::nullopt, 3, std::nullopt, 5};
    auto result = AsDataFlow(input) | DropNullopt() | AsVector();
    ASSERT_THAT(result, testing::ElementsAre(1, 3, 5));
}

TEST(DropNulloptTest, EmptyInput) {
    std::vector<std::optional<int>> input = {};
    auto result = AsDataFlow(input) | DropNullopt() | AsVector();
    ASSERT_THAT(result, testing::ElementsAre());
}

TEST(DropNulloptTest, AllNullopt) {
    std::vector<std::optional<int>> input = {std::nullopt, std::nullopt, std::nullopt};
    auto result = AsDataFlow(input) | DropNullopt() | AsVector();
    ASSERT_THAT(result, testing::ElementsAre());
}


TEST(DropNulloptTest, AllValues) {
    std::vector<std::optional<int>> input = {1, 2, 3, 4, 5};
    auto result = AsDataFlow(input) | DropNullopt() | AsVector();
    ASSERT_THAT(result, testing::ElementsAre(1, 2, 3, 4, 5));
}

TEST(DropNulloptTest, SingleValue) {
    std::vector<std::optional<int>> input = {1};
    auto result = AsDataFlow(input) | DropNullopt() | AsVector();
    ASSERT_THAT(result, testing::ElementsAre(1));
}

TEST(DropNulloptTest, SingleNullopt) {
    std::vector<std::optional<int>> input = {std::nullopt};
    auto result = AsDataFlow(input) | DropNullopt() | AsVector();
    ASSERT_THAT(result, testing::ElementsAre());
}

TEST(DropNulloptTest, DropNulloptWithTransform) {
    std::vector<std::optional<int>> input = {1, std::nullopt, 3, std::nullopt, 5, 6};
    auto result = AsDataFlow(input) | DropNullopt() | Transform([](int x) { return x * 2; }) | AsVector();
    ASSERT_THAT(result, testing::ElementsAre(2, 6, 10, 12));
}

TEST(DropNulloptTest, DropNulloptWithFilterAndAggregate) {
    std::vector<std::optional<int>> input = {1, std::nullopt, 3, std::nullopt, 5, 6};
    auto result = AsDataFlow(input) | DropNullopt() | Filter([](int x) { return x % 2 == 0; }) | AsVector();
    ASSERT_THAT(result, testing::ElementsAre(6));
}

