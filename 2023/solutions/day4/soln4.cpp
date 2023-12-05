#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <common/strings.h>

#include <cassert>


struct card_t {
    int id;
    std::vector<int> winning_numbers;
    std::vector<int> test_numbers;
};

int num_matches(const card_t& card) {
    int matches = 0;
    for (auto num : card.test_numbers) {
        matches += std::count(card.winning_numbers.cbegin(), card.winning_numbers.cend(), num);
    }
    return matches;
}

int part1(const card_t& card) {
    int matches = num_matches(card);
    if (!matches) return 0;
    return std::pow(2, matches - 1);
}

int part2(const card_t& card, std::vector<int>& won_cards) {
    int matches = num_matches(card);

    auto cur_index = card.id - 1;
    assert(won_cards.size() >= cur_index);
    if (won_cards.size() == cur_index) {
        won_cards.push_back(1);
    }

    for (int i = 1; i <= matches; i++) {
        if (won_cards.size() == cur_index + i) {
            won_cards.push_back(1 + won_cards[cur_index]);
        } else {
            won_cards[cur_index + i] += won_cards[cur_index];
        }
    }

    return won_cards[cur_index];
}

card_t parse_line(const std::string& line) {
    auto card_split = advent::strings::split(line, ':');
    assert(card_split.size() == 2ul);
    card_t card;
    card.id = std::stoi(advent::strings::split(card_split.front(), ' ').back());

    auto winning_split = advent::strings::split(card_split[1], '|');
    assert(winning_split.size() == 2ul);

    for (auto num : advent::strings::split(winning_split.front(), ' ')) {
        num.erase(num.begin(), std::find_if(num.begin(), num.end(), [](unsigned char ch) {
            return !std::isspace(ch);
        }));
        if (!num.empty()) {
            card.winning_numbers.push_back(std::stoi(num));
        }
    }
    for (auto num : advent::strings::split(winning_split[1], ' ')) {
        num.erase(num.begin(), std::find_if(num.begin(), num.end(), [](unsigned char ch) {
            return !std::isspace(ch);
        }));
        if (!num.empty()) {
            card.test_numbers.push_back(std::stoi(num));
        }
    }

    assert(!card.winning_numbers.empty());
    assert(!card.test_numbers.empty());
    return card;
}

void run_solution() {
    std::string line;
    std::ifstream input_file("../../../../2023/solutions/day4/input.txt");
    if (input_file.is_open()) {
        int part1_sum = 0;
        int part2_sum = 0;
        std::vector<int> won_cards;
        while (std::getline(input_file, line)) {
            auto card = parse_line(line);
            part1_sum += part1(card);
            part2_sum += part2(card, won_cards);
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
