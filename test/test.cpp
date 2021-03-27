#include <gtest/gtest.h>
#include <string>
#include <cstring>
#include <string_view>

extern "C" {
#include "../src/string_buffer.h"
}

int simple_copy_to_buffer_callback(char *to, const char *from, size_t count, void*) {
    std::memcpy(to, from, count);
    return 0;
}

int g_allocations_counter = 0;

void setAllocator(string_buffer& buffer ) {
    buffer.allocator = nullptr;
    buffer.substring_new = [](void*){
        g_allocations_counter++;
        return new substring; };
    buffer.substring_free = [](void*, substring* ss){
        g_allocations_counter--;
        delete ss;
        return; };
}
 
TEST(StringBuffer, Append) {
    std::string tmp = "Test";

    string_buffer buffer;
    setAllocator( buffer );

    string_buffer_init(&buffer);

    EXPECT_EQ(0, string_buffer_capacity_available(&buffer));

    auto result = string_buffer_append(&buffer, tmp.c_str(), tmp.size(), simple_copy_to_buffer_callback, nullptr);

    EXPECT_EQ(0, result);
    EXPECT_EQ(STRING_ENTRY_LEN - tmp.size(), string_buffer_capacity_available(&buffer));
    EXPECT_EQ(STRING_ENTRY_LEN - tmp.size(), buffer.r_cursor);
    EXPECT_EQ(STRING_ENTRY_LEN, buffer.capacity);
    EXPECT_EQ(0, buffer.f_cursor);
    EXPECT_EQ(0, std::strncmp(tmp.c_str(), buffer.head->payload, tmp.size()) );
    EXPECT_EQ(buffer.head, buffer.tail);
    EXPECT_EQ(tmp.size(), string_buffer_length(&buffer));

    string_buffer_clear(&buffer);
    EXPECT_EQ(0, buffer.capacity);
    EXPECT_EQ(0, string_buffer_capacity_available(&buffer));
    EXPECT_EQ(STRING_ENTRY_LEN, buffer.r_cursor);
    EXPECT_EQ(buffer.head, buffer.tail);
    EXPECT_EQ(buffer.head, nullptr);
}

TEST(StringBuffer, AppendBigData) {
    std::string_view tmp = "It was in July, 1805, and the speaker was the well-known Anna Pavlovna Scherer, " \
"maid of honor and favorite of the Empress Marya Fedorovna. With these words she " \
"greeted Prince Vasili Kuragin, a man of high rank and importance, who was the " \
"first to arrive at her reception. Anna Pavlovna had had a cough for some days. " \
"She was, as she said, suffering from la grippe; grippe being then a new word in " \
"St. Petersburg, used only by the elite.";


    string_buffer buffer;
    setAllocator( buffer );

    string_buffer_init(&buffer);

    EXPECT_EQ(0, string_buffer_capacity_available(&buffer));

    auto result = string_buffer_append(&buffer, tmp.data(), tmp.size(), simple_copy_to_buffer_callback, nullptr);

    EXPECT_EQ(0, result);
    EXPECT_EQ(4, string_buffer_capacity_available(&buffer));
    EXPECT_EQ(4, buffer.r_cursor);
    EXPECT_EQ(STRING_ENTRY_LEN * 22, buffer.capacity);
    EXPECT_EQ(0, buffer.f_cursor);
    EXPECT_EQ(tmp.size(), string_buffer_length(&buffer));
    EXPECT_EQ(22, g_allocations_counter);

    string_buffer_clear(&buffer);
    EXPECT_EQ(0, buffer.capacity);
    EXPECT_EQ(0, string_buffer_capacity_available(&buffer));
    EXPECT_EQ(STRING_ENTRY_LEN, buffer.r_cursor);
    EXPECT_EQ(buffer.head, buffer.tail);
    EXPECT_EQ(buffer.head, nullptr);
    EXPECT_EQ(0, g_allocations_counter);
}

TEST(StringBuffer, ReadData) {
    std::string input = "qwertyuiopasdfghjkl9876543210qwertyuiop0123456789";

    char char_buffer[32] = { '\0' };


    string_buffer buffer;
    setAllocator( buffer );

    string_buffer_init(&buffer);

    EXPECT_EQ(0, string_buffer_capacity_available(&buffer));


    auto result = string_buffer_extract(&buffer, char_buffer, 10, simple_copy_to_buffer_callback, nullptr);

    EXPECT_EQ(10, result);

    result = string_buffer_append(&buffer, input.data(), input.size(), simple_copy_to_buffer_callback, nullptr);

    EXPECT_EQ(0, result);

    std::string output; output.reserve(50);
    while (result == 0) {
        const int part = 11;
        result = string_buffer_extract(&buffer, char_buffer, part, simple_copy_to_buffer_callback, nullptr);
        EXPECT_GE(result, 0);
        if (result < 0) {
            break;
        }
        output.append(char_buffer, part - result);
    }

    EXPECT_EQ(input, output);
    EXPECT_EQ(0, buffer.f_cursor);
    EXPECT_EQ(STRING_ENTRY_LEN, buffer.r_cursor);
    EXPECT_EQ(STRING_ENTRY_LEN, string_buffer_capacity_available(&buffer));
    EXPECT_EQ(0, string_buffer_length(&buffer));
    EXPECT_EQ(1, g_allocations_counter);


    string_buffer_clear(&buffer);
    EXPECT_EQ(0, buffer.capacity);
    EXPECT_EQ(0, string_buffer_capacity_available(&buffer));
    EXPECT_EQ(STRING_ENTRY_LEN, buffer.r_cursor);
    EXPECT_EQ(buffer.head, buffer.tail);
    EXPECT_EQ(buffer.head, nullptr);
    EXPECT_EQ(0, g_allocations_counter);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
