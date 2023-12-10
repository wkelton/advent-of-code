#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <common/strings.h>

#include <cassert>


struct race_t {
    uint64_t time;
    uint64_t distance;
};

using races_t = std::vector<race_t>;

uint64_t calc_num_ways(const race_t& race) {
    // -d^2 + t*d -dist = 0
    auto discriminant = race.time * race.time - 4ul * race.distance;
    assert(discriminant >= 0);
    auto start = std::ceil((static_cast<double>(race.time) - std::sqrt(discriminant)) / 2.0);
    auto last = std::floor((static_cast<double>(race.time) + std::sqrt(discriminant)) / 2.0);
    return last - start + 1ul;
}

uint64_t part1(const races_t& races) {
    int result = 1;
    for (const auto& race : races) {
        result *= calc_num_ways(race);
    }
    return result;
}

uint64_t part2(const races_t& races) {
    std::string time_string;
    std::string distance_string;
    for (auto& r : races) {
        time_string += std::to_string(r.time);
        distance_string += std::to_string(r.distance);
    }
    return calc_num_ways(race_t{std::stoul(time_string), std::stoul(distance_string)});
}

void parse_line(const std::string& line, races_t& races) {
    auto cat_split = advent::strings::split(line, ':');
    assert(cat_split.size() == 2ul);
    auto nums = advent::strings::split(cat_split[1], ' ', true);
    assert(!nums.empty());
    if (cat_split[0].find("Time") != std::string::npos) {
        assert(races.empty());
        for (auto& n : nums) {
            if (!n.empty()) {
                races.emplace_back(race_t{std::stoull(n), 0});
            }
        }
    } else {
        assert(cat_split[0].find("Distance") != std::string::npos);
        assert(!races.empty());
        size_t i = 0ul;
        for (auto& n : nums) {
            if (!n.empty()) {
                assert(i < races.size());
                races[i].distance = std::stoull(n);
                i++;
            }
        }
    }
}

void run_solution() {
    std::string line;
    std::ifstream input_file("../../../../2023/solutions/day6/input.txt");
    if (input_file.is_open()) {
        races_t races;
        while (std::getline(input_file, line)) {
            parse_line(line, races);
        }
        input_file.close();

        assert(races.back().distance != 0);
        std::cout << "Part 1: " << part1(races) << std::endl;
        std::cout << "Part 2: " << part2(races) << std::endl;
    } else {
        std::cout << "Cannot open input file" << std::endl;
    }
}

int main() {
    run_solution();
    return 0;
}
