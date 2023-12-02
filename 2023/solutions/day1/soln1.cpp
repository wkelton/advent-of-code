#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>
#include <cassert>

int part1_parse(const std::string& s) {
    auto first_itr = std::find_if(s.cbegin(), s.cend(), [](const char& c) {
        return c >= '0' && c <= '9';
    });
    auto last_itr = std::find_if(s.crbegin(), s.crend(), [](const char& c) {
        return c >= '0' && c <= '9';
    });
    if (first_itr != s.cend()) {
        assert(last_itr != s.crend());
        return (*first_itr - '0') * 10 + (*last_itr - '0');
    }
    return 0;
}

int part2_parse(const std::string& s) {
    static std::vector<std::string> string_digits{
        "zero",
        "one",
        "two",
        "three",
        "four",
        "five",
        "six",
        "seven",
        "eight",
        "nine",
    };

    auto first_numeric_itr = std::find_if(s.cbegin(), s.cend(), [](const char& c) {
        return c >= '0' && c <= '9';
    });
    auto last_numeric_itr = std::find_if(s.crbegin(), s.crend(), [](const char& c) {
        return c >= '0' && c <= '9';
    });

    int first_num = first_numeric_itr != s.cend() ? *first_numeric_itr - '0' : -1;
    int last_num = last_numeric_itr != s.crend() ? *last_numeric_itr - '0' : -1;

    auto first_pos = std::distance(s.cbegin(), first_numeric_itr);
    auto last_pos = s.size() - std::distance(s.crbegin(), last_numeric_itr) - 1;

    for (int i = 0; i < string_digits.size(); i++) {
        auto first_string_pos = s.find(string_digits[i]);
        if (first_string_pos < first_pos) {
            first_pos = first_string_pos;
            first_num = i;
        }

        auto last_string_pos = s.rfind(string_digits[i]);
        if (last_string_pos < s.size() && last_string_pos > last_pos) {
            last_pos = last_string_pos;
            last_num = i;
        }
    }

    if (first_num >= 0) {
        assert(last_num >= 0);
        return first_num * 10 + last_num;
    }

    return 0;
}

void run_solution() {
    int part1_sum = 0;
    int part2_sum = 0;
    std::string line;
    std::ifstream input_file("../../../../2023/solutions/day1/input.txt");
    if (input_file.is_open()) {
        while (std::getline(input_file, line)) {
            part1_sum += part1_parse(line);
            part2_sum += part2_parse(line);
        }
        input_file.close();

        std::cout << "Part 1: " << part1_sum << std::endl;
        std::cout << "Part 2: " << part2_sum << std::endl;
    } else {
        std::cout << "Cannot open input file" << std::endl;
    }
}

int main() {
    run_solution();
    return 0;
}
