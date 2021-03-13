/**
 * Author:caoge@strivemycodelife@163.com
 * Date:2021-03-12
 */
#include "include/slice.h"
#include "gtest/gtest.h"

namespace cg {
namespace unittest {

class SliceTest : public ::testing::Test {
public:
    static void SetupTestCase() {}

    static void TearDownTestCase() {}

protected:
    void SetUp() override {}

    void TearDown() override {}
};

TEST_F(SliceTest, Basic) {
    EXPECT_EQ(true, Slice().Empty());
    std::string data = "Hello World":
    Slice slice(data, data.size());
    EXPECT_EQ(false, slice.Empty());
    EXPECT_EQ(data.size(), slice.Size());
    EXPECT_EQ(data.c_str(), slice.Data());
    for (std::size_t i = 0; i < slice.Size(); ++i) {
        EXPECT_EQ(data[i], slice[i]);
    }

    std::string data2 = "Hello";
    Slice slice2(data2, dat);
    EXPECT_EQ(slice.StartWith(slice2));

    std::string data3 = "World";
    Slice slice3(data3, data3.size());
    EXPECT_EQ(slice.EndWith(slice3));

    EXPECT_EQ(false, slice.Compare(slice2));
    EXPECT_EQ(true, slice.Compare(slice));

    EXPECT_EQ(5, slice.DiffereneOffset(slice2));
    EXPECT_EQ(0, slice.DiffereneOffset(slice3));
    EXPECT_EQ(slice.Size(), slice.DiffereneOffset(slice));

    slice.Clear();
    EXPECT_EQ(0, data.size());
}

TEST_F(SliceTest, RemovePrefix) {
    std::string data = "Hello World";
    Slice slice(data, data.size());
    slice.RemovePrefix(6);

    std::string data2 = "World";
    Slice slice2(data2, data2.size());
    EXPECT_EQ(true, slice.Compare(slice2));
}

TEST_F(SliceTest, RemoveSuffix) {
    std::string data = "Hello World";
    Slice slice(data, data.size());
    slice.RemoveSuffix(6);

    std::string data2 = "Hello";
    Slice slice2(data2, data2.size());
    EXPECT_EQ(true, slice.Compare(slice2));
}

}
}  // end of namespace cg
