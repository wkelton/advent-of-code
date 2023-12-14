#include <fstream>
#include <iostream>
#include <set>
#include <string>

#include <common/strings.h>

#include <cassert>


enum class hand_type_t {
    unknown = 0,
    high,
    one_pair,
    two_pair,
    three_kind,
    full_house,
    four_kind,
    five_kind,
};

hand_type_t determine_hand_type(const std::string& cards, bool enable_jokers) {
    std::vector<bool> visited(cards.size(), false);
    std::vector<int> all_matches;
    int num_jokers = 0;
    for (size_t i = 0ul; i < cards.size(); i++) {
        if (enable_jokers && cards[i] == 'J') {
            num_jokers++;
        }
        int matches = 1;
        if (!visited[i]) {
            for (size_t j = i + 1ul; j < cards.size(); j++) {
                if (!visited[j] && cards[i] == cards[j]) {
                    if (!enable_jokers || cards[i] != 'J') {
                        matches++;
                    }
                    visited[i] = true;
                    visited[j] = true;
                }
            }
        }
        if (matches > 1) {
            all_matches.push_back(matches);
        }
    }
    assert(all_matches.size() <= 2ul);

    auto matches_to_type = [](int num_cards) {
        switch(num_cards) {
        case 1:
            return hand_type_t::high;
        case 2:
            return hand_type_t::one_pair;
        case 3:
            return hand_type_t::three_kind;
        case 4:
            return hand_type_t::four_kind;
        case 5:
            return hand_type_t::five_kind;
        default:
            assert(false);
            return hand_type_t::unknown;
        }
    };

    auto hand_type = hand_type_t::unknown;
    int num_cards_that_match = 1;
    if (all_matches.empty()) {
        if (enable_jokers && num_jokers == 5){
            num_cards_that_match = 5;
        }
        hand_type = matches_to_type(num_cards_that_match);
    } else if (all_matches.size() == 1ul) {
        num_cards_that_match = all_matches.front();
        hand_type = matches_to_type(num_cards_that_match);
    } else {
        if (auto sum = all_matches[0] + all_matches[1]; sum == 5) {
            hand_type = hand_type_t::full_house;
        } else if (sum == 4) {
            hand_type = hand_type_t::two_pair;
        } else {
            assert(false);
        }
    }

    if (enable_jokers && num_jokers) {
        switch (hand_type) {
        case hand_type_t::two_pair:
            assert(num_jokers == 1);
            hand_type = hand_type_t::full_house;
            break;
        case hand_type_t::four_kind:
            assert(num_jokers <= 1);
        case hand_type_t::three_kind:
            assert(num_jokers <= 2);
        case hand_type_t::one_pair:
            assert(num_jokers <= 3);
        case hand_type_t::high:
            assert(num_jokers <= 5);
            hand_type = matches_to_type(num_cards_that_match + num_jokers);
            break;
        case hand_type_t::five_kind:
            break;
        case hand_type_t::full_house:
        default:
            assert(false);
        }
    }

    return hand_type;
}

bool compare_cards(char left, char right, bool enable_jokers) {
    auto is_num = [](char c) { return c >= '0' && c <= '9'; };
    auto left_is_num = is_num(left);
    auto right_is_num = is_num(right);

    if (enable_jokers) {
        if (right == 'J') {
            return false;
        } else if (left == 'J') {
            return true;
        }
    }

    if (left_is_num && right_is_num) {
        return left < right;
    } else if (left_is_num) {
        return true;
    } else if (right_is_num) {
        return false;
    } else {
        switch (left) {
        case 'A':
            return false;
        case 'K':
            return right == 'A' || right == 'K';
        case 'Q':
            return right == 'A' || right == 'K' || right == 'Q';
        case 'J':
            return right != 'T' && right != 'J';
        case 'T':
            return right != 'T';
        default:
            assert(false);
        }
        return false;
    }
}

class hand_t {
public:
    hand_t(const std::string& cards, int bid, bool enable_jokers)
        : _cards(cards), _bid(bid), _enable_jokers(enable_jokers) {
            assert(_cards.size() == 5ul);
        }

    bool operator==(const hand_t& other) const {
        return other._cards == _cards && other._bid == _bid;
    }

    bool operator<(const hand_t& other) const {
        if (static_cast<std::underlying_type_t<hand_type_t>>(hand_type())
            == static_cast<std::underlying_type_t<hand_type_t>>(other.hand_type())) {
            for (size_t i = 0ul; i < _cards.size(); i++) {
                if (_cards[i] != other._cards[i]) {
                    return compare_cards(_cards[i], other._cards[i], _enable_jokers);
                }
            }
            assert(false);
        } else {
            return static_cast<std::underlying_type_t<hand_type_t>>(hand_type())
                < static_cast<std::underlying_type_t<hand_type_t>>(other.hand_type());
        }
    }

    hand_type_t hand_type() const {
        if (_hand_type == hand_type_t::unknown) {
            const_cast<hand_t*>(this)->_hand_type = determine_hand_type(_cards, _enable_jokers);
        }
        return _hand_type;
    }

    int winnings(int rank) const { return _bid * rank; }
    const std::string& cards() const { return _cards; }
    int bid() const { return _bid; }

private:
    std::string _cards;
    int _bid;
    hand_type_t _hand_type = hand_type_t::unknown;
    bool _enable_jokers;
};

using hands_t = std::set<hand_t>;

uint64_t part1(const hands_t& hands) {
    uint64_t total_winnings = 0;
    int rank = 1;
    for (auto& hand : hands) {
        total_winnings += hand.winnings(rank++);
    }
    return total_winnings;
}

uint64_t part2(const hands_t& hands_) {
    hands_t hands;
    for (const auto& hand : hands_) {
        hands.emplace(hand.cards(), hand.bid(), true);
    }
    uint64_t total_winnings = 0;
    int rank = 1;
    for (auto& hand : hands) {
        total_winnings += hand.winnings(rank++);
    }
    return total_winnings;
}

hand_t parse_line(const std::string& line) {
    auto split = advent::strings::split(line, ' ');
    assert(split.size() == 2ul);
    return hand_t(split[0], std::stoi(split[1]), false);
}

void run_solution() {
    std::string line;
    std::ifstream input_file("../../../../2023/solutions/day7/input.txt");
    if (input_file.is_open()) {
        hands_t hands;
        while (std::getline(input_file, line)) {
            hands.insert(parse_line(line));
        }
        input_file.close();

        std::cout << "Part 1: " << part1(hands) << std::endl;
        std::cout << "Part 2: " << part2(hands) << std::endl;
    } else {
        std::cout << "Cannot open input file" << std::endl;
    }
}

int main() {
    run_solution();
    return 0;
}
