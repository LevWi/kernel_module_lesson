#include <gtest/gtest.h>
#include <unordered_set>
#include <string_view>

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <iostream>

std::vector<std::string_view> split(const std::string_view s, const std::string_view delimiter) {
    std::vector<std::string_view> out;

    size_t pos = 0;
    size_t posEnd = 0;
    while ((posEnd = s.find(delimiter, pos)) != std::string::npos) {
        out.push_back(s.substr(pos, posEnd - pos));
        pos = delimiter.length() + posEnd;
    }

    if (pos != 0 ) {
        auto tmp = s.substr(pos);
        if (tmp.size() != 0)
            out.push_back(std::move(tmp));
    }
    return out;
}

TEST(FIFO_DEVICE, ReadWrite) {
    const std::string_view delimiter = "==== END ====";
    std::unordered_set<std::string_view> test_strings =
    {
    "small test string1",
    "small test string2",

    "It was in July, 1805, and the speaker was the well-known Anna Pavlovna Scherer, " \
    "maid of honor and favorite of the Empress Marya Fedorovna. With these words she greeted " \
    "Prince Vasili Kuragin, a man of high rank and importance, who was the first to " \
    "arrive at her reception. Anna Pavlovna had had a cough for some days. She was, as "\
    "she said, suffering from la grippe; grippe being then a new word in " \
    "St. Petersburg, used only by the elite.",

    "small test string3",

    "If you have nothing better to do, Count [or Prince], and if the prospect of "              \
    "spending an evening with a poor invalid is not too terrible, I shall be very "             \
    "charmed to see you tonight between 7 and 10 — Annette Scherer."                            \
    "\"Heavens! what a virulent attack!\" replied the prince, not in the least "                \
    "disconcerted by this reception. He had just entered, wearing an embroidered "              \
    "court uniform, knee breeches, and shoes, and had stars on his breast and a serene "        \
    "expression on his flat face. He spoke in that refined French in which our "                \
    "grandfathers not only spoke but thought, and with the gentle, patronizing intonation "     \
    "natural to a man of importance who had grown old in society and at court. "                \
    "He went up to Anna Pavlovna, kissed her hand, presenting to her his bald, "                \
    "scented, and shining head, and complacently seated himself on the sofa."                   \
    "\"First of all, dear friend, tell me how you are. Set your friend's mind at rest,\" said " \
    "he without altering his tone, beneath the politeness and affected sympathy of which "      \
    "indifference and even irony could be discerned.",

    "small test string4",
    };

    struct FDWrapper {
        const int fd;
        ~FDWrapper() {
            if (fd >= 0) {
                close(fd);
            }
        }
    };

    const std::string device_path = "/dev/fifo";

    FDWrapper dev { open(device_path.c_str(), O_RDONLY) };
    ASSERT_GE(0, dev.fd);

    FDWrapper dev2 { open(device_path.c_str(), O_WRONLY) };
    ASSERT_GE(0, dev2.fd);

    std::cout << "Writing started\n";

    for (const std::string_view el : test_strings) {
        auto writed_result = write(dev2.fd, el.data(), el.size());
        ASSERT_GE(0, writed_result);
        writed_result = write(dev2.fd, delimiter.data(), delimiter.size());
        ASSERT_GE(0, writed_result);
    }

    std::cout << "Reading started\n";

    std::string input_buffer; input_buffer.reserve(2048);
    char tmp_buff[256];
    ssize_t readed_result = 0;
    do {
        readed_result = read(dev.fd, tmp_buff, sizeof (tmp_buff));
        ASSERT_GE(0, readed_result);
        input_buffer.append(tmp_buff, readed_result);
    } while (readed_result == sizeof (tmp_buff) );

    std::cout << "Checking...\n";

    auto readed_strings = split(input_buffer, delimiter);

    EXPECT_EQ(readed_strings.size(), test_strings.size());

    for (const std::string_view el : readed_strings) {
        EXPECT_TRUE(test_strings.find(el) != test_strings.end());
    }
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

