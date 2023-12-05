#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include <cassert>

#include <common/strings.h>


struct dice_t {
    int red = 0;
    int green = 0;
    int blue = 0;
};

struct game_t {
    int id = -1;
    std::vector<dice_t> dice_rounds;
};

game_t parse_line(const std::string& line) {
    auto game_split = advent::strings::split(line, ':');
    assert(game_split.size() == 2ul);
    assert(game_split.front().size() > 5);

    game_t game;
    game.id = std::stoi(game_split.front().substr(5));

    auto rounds_split = advent::strings::split(game_split[1], ';');
    assert(!rounds_split.empty());
    for (const auto& round : rounds_split) {
        auto dice_split = advent::strings::split(round, ',');
        assert(!dice_split.empty());
        game.dice_rounds.emplace_back();

        for (auto& d : dice_split) {
            d.erase(d.begin(), std::find_if(d.begin(), d.end(), [](unsigned char ch) {
                return !std::isspace(ch);
            }));
            auto num_split = advent::strings::split(d, ' ');
            assert(num_split.size() == 2ul);

            if (num_split[1].find("red") != std::string::npos) {
                game.dice_rounds.back().red = std::stoi(num_split.front());
            } else if (num_split[1].find("green") != std::string::npos) {
                game.dice_rounds.back().green = std::stoi(num_split.front());
            } else if (num_split[1].find("blue") != std::string::npos) {
                game.dice_rounds.back().blue = std::stoi(num_split.front());
            } else {
                assert(false);
            }
        }
    }

    return game;
}

int part1_parse(const game_t& game) {
    constexpr dice_t target_dice = {12, 13, 14};

    for (const auto& round : game.dice_rounds) {
        if (round.red > target_dice.red || round.green > target_dice.green || round.blue > target_dice.blue) {
            return 0;
        }
    }

    return game.id;
}

int part2_parse(const game_t& game) {
    assert(!game.dice_rounds.empty());
    dice_t min_dice_set = {
        std::max_element(game.dice_rounds.cbegin(), game.dice_rounds.cend(), [](const dice_t& l, const dice_t& r) {
            return l.red < r.red;
        })->red,
        std::max_element(game.dice_rounds.cbegin(), game.dice_rounds.cend(), [](const dice_t& l, const dice_t& r) {
            return l.green < r.green;
        })->green,
        std::max_element(game.dice_rounds.cbegin(), game.dice_rounds.cend(), [](const dice_t& l, const dice_t& r) {
            return l.blue < r.blue;
        })->blue,
    };

    return min_dice_set.red * min_dice_set.green * min_dice_set.blue;
}

void run_solution() {
    int part1_sum = 0;
    int part2_sum = 0;
    std::string line;
    std::ifstream input_file("../../../../2023/solutions/day2/input.txt");
    if (input_file.is_open()) {
        while (std::getline(input_file, line)) {
            auto game = parse_line(line);
            part1_sum += part1_parse(game);
            part2_sum += part2_parse(game);
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
