#include <processing.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <numeric>

struct Student {
    uint64_t group_id;
    std::string name;

    bool operator==(const Student& other) const = default;
};

struct Group {
    uint64_t id;
    std::string name;

    bool operator==(const Group& other) const = default;
};

TEST(SimpleTest, JoinKV) {
    std::vector<KV<int, std::string>> left = {{0, "a"}, {1, "b"}, {2, "c"}, {3, "d"}, {1, "e"}};
    std::vector<KV<int, std::string>> right = {{0, "f"}, {1, "g"}, {3, "i"}};

    auto left_flow = AsDataFlow(left);
    auto right_flow = AsDataFlow(right);
    auto result = left_flow | Join(right_flow) | AsVector();

    ASSERT_THAT(
        result,
        testing::ElementsAre(
            JoinResult<std::string, std::string>{"a", "f"},
            JoinResult<std::string, std::string>{"b", "g"},
            JoinResult<std::string, std::string>{"c", std::nullopt},
            JoinResult<std::string, std::string>{"d", "i"},
            JoinResult<std::string, std::string>{"e", "g"}
        )
    );
}

TEST(SimpleTest, JoinComparators) {
    std::vector<Student> students = {{0, "a"}, {1, "b"}, {2, "c"}, {3, "d"}, {1, "e"}};
    std::vector<Group> groups = {{0, "f"}, {1, "g"}, {3, "i"}};

    auto students_flow = AsDataFlow(students);
    auto groups_flow = AsDataFlow(groups);

    auto result =
        students_flow
            | Join(
                groups_flow,
                [](const Student& student) { return student.group_id; },
                [](const Group& group) { return group.id; })
            | AsVector()
    ;

    ASSERT_THAT(
        result,
        testing::ElementsAre(
            JoinResult<Student, Group>{Student{0, "a"}, Group{0, "f"}},
            JoinResult<Student, Group>{Student{1, "b"}, Group{1, "g"}},
            JoinResult<Student, Group>{Student{2, "c"}, std::nullopt},
            JoinResult<Student, Group>{Student{3, "d"}, Group{3, "i"}},
            JoinResult<Student, Group>{Student{1, "e"}, Group{1, "g"}}
        )
    );
}

TEST(JoinTest, NoKey) {
    std::vector<KV<int, std::string>> left = {{1, "A"}};
    std::vector<KV<int, std::string>> right = {{2, "B"}};
    
    auto left_flow = AsDataFlow(left);
    auto right_flow = AsDataFlow(right);

    auto result = left_flow | Join(right_flow) | AsVector();
    
    ASSERT_THAT(result,
        testing::ElementsAre(JoinResult<std::string, std::string>{"A", std::nullopt}));
}

TEST(JoinTest, EmptyFlows) {
    std::vector<KV<int, std::string>> left;
    std::vector<KV<int, std::string>> right;
    auto left_flow = AsDataFlow(left);
    auto right_flow = AsDataFlow(right);
    auto result = left_flow | Join(right_flow) | AsVector();
    ASSERT_THAT(result, testing::IsEmpty()); 
}

TEST(JoinTest, JoinWithFilterAndTransform) {
    std::vector<Student> students = {{0, "Alice"}, {1, "Bob"}, {2, "Charlie"}, {3, "Dave"}};
    std::vector<Group> groups = {{0, "Math"}, {1, "Physics"}, {3, "Chemistry"}};
    auto students_flow = AsDataFlow(students) | Filter([](const Student& s) { return s.group_id != 2; }); 
    auto groups_flow = AsDataFlow(groups) | Transform([](const Group& g) { return Group{g.id, g.name + " Dept"}; }); 
    auto result = students_flow | Join(groups_flow,
                                [](const Student& s) { return s.group_id; },
                                [](const Group& g) { return g.id; }) | AsVector();
    ASSERT_THAT(
        result,
        testing::ElementsAre(
            JoinResult<Student, Group>{Student{0, "Alice"}, Group{0, "Math Dept"}},
            JoinResult<Student, Group>{Student{1, "Bob"}, Group{1, "Physics Dept"}},
            JoinResult<Student, Group>{Student{3, "Dave"}, Group{3, "Chemistry Dept"}}
        )
    );
}

TEST(JoinTest, JoinWithStreams) {
    std::vector<std::stringstream> left_streams(2);
    left_streams[0] << "1:Alice\n2:Bob";
    left_streams[1] << "3:Charlie";
    std::vector<std::stringstream> right_streams(1);
    right_streams[0] << "1:Math\n3:CS";

    auto left_flow = AsDataFlow(left_streams) | Split("\n") | Transform([](const std::string& line) {
            auto pos = line.find(':');
            return KV{std::stoi(line.substr(0, pos)), line.substr(pos + 1)};});
    auto right_flow = AsDataFlow(right_streams) | Split("\n") | Transform([](const std::string& line) {
        auto pos = line.find(':');
        return KV{std::stoi(line.substr(0, pos)), line.substr(pos + 1)};});
    auto result = left_flow | Join(right_flow) | AsVector();
    ASSERT_THAT(
        result,
        testing::ElementsAre(
            JoinResult<std::string, std::string>{"Alice", "Math"},
            JoinResult<std::string, std::string>{"Bob", std::nullopt},
            JoinResult<std::string, std::string>{"Charlie", "CS"}
        )
    );
}

TEST(JoinTest, JoinWithExpected) {
    std::vector<std::string> left_input = {"1:Alice", "2:Bob", "invalid", "3:Charlie"};
    std::vector<KV<int, std::string>> right = {{1, "Math"}, {3, "CS"}};

    auto parse_student = [](const std::string& s) -> std::expected<KV<int, std::string>, std::string> {
        auto pos = s.find(':');
        if (pos == std::string::npos) return std::unexpected("Invalid format");
        return KV{std::stoi(s.substr(0, pos)), s.substr(pos + 1)};
    };

    auto left_flow = AsDataFlow(left_input) | Transform(parse_student);
    auto [errors, valid_students] = left_flow | SplitExpected();
    auto right_flow = AsDataFlow(right);
    auto result = valid_students | Join(right_flow) | AsVector();
    auto error_result = errors | AsVector();

    ASSERT_THAT(
        result,
        testing::ElementsAre(
            JoinResult<std::string, std::string>{"Alice", "Math"},
            JoinResult<std::string, std::string>{"Bob", std::nullopt},
            JoinResult<std::string, std::string>{"Charlie", "CS"}
        )
    );
    ASSERT_THAT(error_result, testing::ElementsAre("Invalid format"));
}
