#include <filesystem>
#include <fstream>
#include <sstream>
#include <vector>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <processing.h>

TEST(DirTest, BasicFileReading) {
    const std::string test_dir = "test_dir/";
    std::filesystem::create_directory(test_dir);
    std::ofstream(test_dir + "/file1.txt") << "Hello\nWorld";
    std::ofstream(test_dir + "/file2.txt") << "C++\nProcessing";

    auto result =  Dir(test_dir, false) 
    | Filter([](const std::filesystem::path& p) { return p.extension() == ".txt"; }) | OpenFiles() | Split("\n")
    | AsVector();

   ASSERT_THAT(result, testing::ElementsAre("C++", "Processing", "Hello", "World"));

    std::filesystem::remove_all(test_dir);
}

TEST(DirTest, EmptyDirectory) {
    const std::string test_dir = "test_dir/";
    std::filesystem::create_directory(test_dir);

    auto result = Dir(test_dir, false) | AsVector();
    ASSERT_TRUE(result.empty());

    std::filesystem::remove(test_dir);
}


TEST(DirTest, RecursiveSearch) {
    const std::string root_dir = "test_dir/";
    std::filesystem::create_directory(root_dir);
    std::filesystem::create_directory(root_dir + "/sub/");
    std::filesystem::create_directory(root_dir + "/sub2/");
    std::ofstream(root_dir + "/file1.txt") << "File1";
    std::ofstream(root_dir + "/sub/file2.txt") << "File2";
    std::ofstream(root_dir + "/sub/file3.tsv") << "File3";
    std::ofstream(root_dir + "/sub2/file4.txt") << "File4";

    auto files = Dir(root_dir, true) 
        | Filter([](const std::filesystem::path& path) { return path.extension() == ".txt"; })
        | AsVector();

    ASSERT_EQ(files.size(), 2);

   std::filesystem::remove_all(root_dir);
}

TEST(DirTest, RecursiveSearchReading) {
    const std::string root_dir = "test_dir/";
    std::filesystem::create_directory(root_dir);
    std::filesystem::create_directory(root_dir + "/sub/");
    std::filesystem::create_directory(root_dir + "/sub2/");
    
    std::ofstream(root_dir + "/file1.txt") << "File1, File2,,";
    std::ofstream(root_dir + "/sub/file2.txt") << "File2";
    std::ofstream(root_dir + "/sub/file3.tsv") << "File3";
    std::ofstream(root_dir + "/sub2/file4.txt") << "File4";

    bool recursive = true;
    auto files = Dir(root_dir, recursive) 
        | Filter([](const std::filesystem::path& path) { return path.extension() == ".txt";})
        | OpenFiles()
        | Split("\n ,.;") 
        | Transform([](std::string token) { 
            std::transform(token.begin(), token.end(), token.begin(), [](char c) { return std::tolower(c); }); 
            return token; 
        })
        | AggregateByKey(
            0uz, 
            [](const std::string&, size_t& count) { ++count; },
            [](const std::string& token) { return token; }) 
        | Transform([](const std::pair<std::string, size_t>& stat) { 
            return std::format("{} - {}", stat.first, stat.second);}) 
        | AsVector();

    ASSERT_EQ(files.size(), 3); 

    std::filesystem::remove_all(root_dir);
}