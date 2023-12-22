#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <numeric>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <common/strings.h>

#include <cassert>


using seq_t = std::vector<size_t>;

struct node_t {
    std::array<node_t*, 2> neighbors = {nullptr, nullptr};
    bool is_ending_location = false;
};

struct nodes_t {
    std::unordered_map<std::string, node_t> full_map;
    std::vector<node_t*> starting_locations;
};

struct loc_t {
    loc_t(node_t* node_, size_t seq_) : node(node_), seq(seq_) {}
    bool operator==(const loc_t& other) const { return other.node == node && other.seq == seq; }
    node_t* node;
    size_t seq;
};

namespace std {
template <>
struct hash<loc_t> {
    std::size_t operator()(const loc_t& k) const {
        return hash<uintptr_t>()((uintptr_t)k.node) ^ hash<size_t>()(k.seq);
    }
};
}

class loop_detector_t {
public:
    loop_detector_t() = default;

    size_t check(node_t* node, size_t seq_index, size_t cur_step) {
        if (auto insert_res = _visited.emplace(loc_t(node, seq_index), cur_step); insert_res.second) {
            return cur_step;
        } else {
            return insert_res.first->second;
        }
    }

private:
    std::unordered_map<loc_t, size_t> _visited;
};

uint64_t part1(const seq_t& seq, const nodes_t& nodes) {
    size_t num_steps = 0;
    size_t seq_index = 0;
    const node_t* cur = &nodes.full_map.find("AAA")->second;
    const node_t* dest = &nodes.full_map.find("ZZZ")->second;
    do {
        cur = cur->neighbors[seq[seq_index]];
        num_steps++;
        seq_index = num_steps % seq.size();
    } while (cur != dest);
    return num_steps;
}

uint64_t part2(const seq_t& seq, const nodes_t& nodes) {
    size_t total_num_steps = 1ul;
    size_t seq_index = 0ul;

    for (auto node : nodes.starting_locations) {
        loop_detector_t loop_detector;
        size_t num_steps = 0ul;
        size_t seq_index = 0ul;
        auto cur = node;
        auto steps_last_visited = loop_detector.check(cur, seq_index, num_steps);
        while (steps_last_visited == num_steps) {
            cur = cur->neighbors[seq[seq_index]];
            seq_index = ++num_steps % seq.size();
            steps_last_visited = loop_detector.check(cur, seq_index, num_steps);
        }

        total_num_steps = std::lcm(total_num_steps, num_steps - steps_last_visited);
    }

    return total_num_steps;
}

void parse_line(const std::string& line, seq_t& seq, nodes_t& nodes) {
    auto split = advent::strings::split(line, '=', true);
    if (split.size() == 1ul && !split.front().empty()) {
        assert(seq.empty());
        for (auto c : split.front()) {
            assert(c == 'L' || c == 'R');
            seq.push_back(c == 'R');
        }
    } else if (!split.empty()) {
        assert(split.size() == 2ul);
        auto node_name = split.front();
        auto split_neighbors = advent::strings::split(split[1], ',', true);
        assert(split_neighbors.size() == 2ul);
        auto left_node_name = split_neighbors[0].substr(1, 4);
        auto right_node_name = split_neighbors[1].substr(0, 3);
        auto node_insert = nodes.full_map.emplace(node_name, node_t{});
        node_insert.first->second.neighbors[0] = &nodes.full_map.emplace(left_node_name, node_t{}).first->second;
        node_insert.first->second.neighbors[1] = &nodes.full_map.emplace(right_node_name, node_t{}).first->second;
        if (node_name[2] == 'A') {
            nodes.starting_locations.push_back(&node_insert.first->second);
        } else if (node_name[2] == 'Z') {
            node_insert.first->second.is_ending_location = true;
        }
        if (left_node_name[2] == 'Z') {
            node_insert.first->second.neighbors[0]->is_ending_location = true;
        }
        if (right_node_name[2] == 'Z') {
            node_insert.first->second.neighbors[1]->is_ending_location = true;
        }
    }
}

void run_solution() {
    std::string line;
    std::ifstream input_file("../../../../2023/solutions/day8/input.txt");
    if (input_file.is_open()) {
        seq_t seq;
        nodes_t nodes;
        while (std::getline(input_file, line)) {
            parse_line(line, seq, nodes);
        }
        input_file.close();

        assert(nodes.full_map.find("AAA") != nodes.full_map.end());
        assert(nodes.full_map.find("ZZZ") != nodes.full_map.end());
        std::cout << "Part 1: " << part1(seq, nodes) << std::endl;
        std::cout << "Part 2: " << part2(seq, nodes) << std::endl;
    } else {
        std::cout << "Cannot open input file" << std::endl;
    }
}

int main() {
    run_solution();
    return 0;
}
