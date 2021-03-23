#include <gtest/gtest.h>
#include <string>
#include <cstring>

extern "C" {
#include "../src/string_buffer.h"
}

int simple_copy_to_buffer_callback(char *to, const char *from, size_t count, void*) {
    std::memcpy(to, from, count);
    return 0;
}
 
TEST(StringBuffer, Append) {
    std::string tmp = "Test";

    string_buffer buffer;
    buffer.allocator = nullptr;
    buffer.substring_new = [](void*){ return new substring; };
    buffer.substring_free = [](void*, substring* ss){ return delete ss; };

    string_buffer_init(&buffer);

    EXPECT_EQ(0, string_buffer_capacity(&buffer));

    auto result = string_buffer_append(&buffer, tmp.c_str(), tmp.size(), simple_copy_to_buffer_callback, nullptr);
    EXPECT_EQ(STRING_ENTRY_LEN, string_buffer_capacity(&buffer));
    EXPECT_EQ(0, result);
    EXPECT_EQ(tmp.size(), string_buffer_length(&buffer));
}
 
int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
