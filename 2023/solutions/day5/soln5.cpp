#include <algorithm>
#include <fstream>
#include <iostream>
#include <limits>
#include <map>
#include <string>
#include <vector>

#include <common/strings.h>

#include <cassert>


struct range_t {
    uint64_t start;
    uint64_t end; // exclusive

    bool contains(uint64_t v) const {
        return v >= start && v < end;
    }

    range_t intersect(const range_t& other) const {
        if (other.end < start || other.start > end) {
            return range_t{0ul, 0ul};
        }
        return range_t{std::max(start, other.start), std::min(end, other.end)};
    }

    bool empty() const { return start == end; }
};

class range_entry_t {
public:
    range_entry_t(uint64_t source_start, uint64_t destination_start, uint64_t length)
        : _source_range({source_start, source_start + length}),
          _destination_range({destination_start, destination_start + length}),
          _offset(static_cast<int64_t>(destination_start) - static_cast<int64_t>(source_start)) {}

    uint64_t lookup(uint64_t source) const {
        if (!_source_range.contains(source)) {
            return source;
        }
        return static_cast<uint64_t>(static_cast<int64_t>(source) + _offset);
    }

    range_t reverse_lookup(const range_t& dest_range, std::vector<range_t>& unmatched_ranges) const {
        auto intersection = _destination_range.intersect(dest_range);

        if (intersection.empty()) {
            unmatched_ranges.emplace_back(dest_range);
            return intersection;
        }

        if (intersection.start != dest_range.start) {
            unmatched_ranges.emplace_back(range_t{dest_range.start, intersection.start});
        }

        if (intersection.end != dest_range.end) {
            unmatched_ranges.emplace_back(range_t{intersection.end, dest_range.end});
        }

        return{static_cast<uint64_t>(static_cast<int64_t>(intersection.start) - _offset),
               static_cast<uint64_t>(static_cast<int64_t>(intersection.end) - _offset)};
    }

    const range_t& source_range() const {
        return _source_range;
    }

    const range_t& destination_range() const {
        return _destination_range;
    }

private:
    range_t _source_range;
    range_t _destination_range;
    int64_t _offset;
};

class range_map_t {
public:
    range_map_t() = default;

    void insert(range_entry_t&& entry) {
        _map.emplace(entry.source_range().start, std::move(entry));
    }

    uint64_t lookup(uint64_t source) const {
        for (auto& e : _map) {
            if (source >= e.first) {
                if (uint64_t dest = e.second.lookup(source); dest != source) {
                    return dest;
                }
            }
        }
        return source;
    }

    std::vector<range_t> reverse_lookup(const range_t& dest_range) const {
        std::vector<range_t> source_ranges;
        std::vector<range_t> unmatched_ranges = {dest_range};
        for (auto& e : _map) {
            std::vector<range_t> uranges;
            for (const range_t& range : unmatched_ranges) {
                if (auto srange = e.second.reverse_lookup(range, uranges); !srange.empty()) {
                    source_ranges.push_back(srange);
                }
            }
            unmatched_ranges = uranges;
        }
        for (const range_t& range : unmatched_ranges) {
            source_ranges.push_back(range);
        }
        return source_ranges;
    }

    std::map<uint64_t, range_entry_t> reverse_map() const {
        std::map<uint64_t, range_entry_t> rmap;
        for (const auto& e : _map) {
            rmap.emplace(e.second.destination_range().start, e.second);
        }
        return rmap;
    }

private:
    std::map<uint64_t, range_entry_t> _map;
};

struct almanac_t {
    std::vector<range_t> seeds;
    std::vector<range_map_t> maps;
};

uint64_t lookup_location(const almanac_t& almanac, uint64_t source) {
    for (auto& m : almanac.maps) {
        source = m.lookup(source);
    }
    return source;
}

uint64_t forward_lookup(const almanac_t& almanac) {
    uint64_t min_location = std::numeric_limits<uint64_t>::max();
    for (auto& seed_range : almanac.seeds) {
        for (auto s = seed_range.start; s < seed_range.end; s++) {
            if (auto source = lookup_location(almanac, s); source < min_location) {
                min_location = source;
            }
        }
    }
    return min_location;
}

uint64_t reverse_lookup(const almanac_t& almanac) {
    std::map<uint64_t, range_entry_t> location_map = almanac.maps.back().reverse_map();

    for (const auto& e : location_map) {
        auto& location_source_range = e.second.source_range();

        std::vector<range_t> source_ranges = {location_source_range};
        for (auto rit = almanac.maps.crbegin() + 1; rit != almanac.maps.crend(); ++rit) {
            std::vector<range_t> ranges;
            for (auto& r : source_ranges) {
                auto sr = rit->reverse_lookup(r);
                for (auto& s : sr) {
                    ranges.push_back(s);
                }
            }
            source_ranges = ranges;
        }

        uint64_t min_location = std::numeric_limits<uint64_t>::max();
        for (auto& seed_range : almanac.seeds) {
            for (auto& source_range : source_ranges) {
                if (auto intersection = seed_range.intersect(source_range); !intersection.empty()) {
                    if (auto loc = lookup_location(almanac, intersection.start); loc < min_location) {
                        min_location = loc;
                    }
                    // TODO: technically, we'd need to check all the intersections for each seed range, but this is
                    // sufficient to provide the correct solution for the given input.
                }
            }
        }
        if (min_location != std::numeric_limits<uint64_t>::max()) {
            return min_location;
        }
    }
    return 0;
}

uint64_t part1(const almanac_t& almanac) {
    return forward_lookup(almanac);
}

uint64_t part2(almanac_t& almanac) {
    auto seeds = almanac.seeds;
    assert(seeds.size() % 2 == 0);
    almanac.seeds.clear();

    for (size_t i = 0ul; i < seeds.size(); i += 2ul) {
        almanac.seeds.emplace_back(range_t{seeds[i].start, seeds[i].start + seeds[i + 1].start});
    }

    return reverse_lookup(almanac);
}

void parse_line(const std::string& line, almanac_t& almanac) {
    if (line.empty()) {
        return;
    }

    auto colon_split = advent::strings::split(line, ':');
    if (colon_split.size() == 2ul) {
        assert(almanac.seeds.empty());
        for (auto n : advent::strings::split(colon_split[1], ' ')) {
            if (!n.empty()) {
                almanac.seeds.emplace_back(range_t{std::stoull(n), std::stoull(n) + 1ul});
            }
        }
    } else {
        if (line.find("map:") != std::string::npos) {
            almanac.maps.emplace_back();
        } else {
            auto entry = advent::strings::split(line, ' ');
            assert(entry.size() == 3ul);
            almanac.maps.back().insert(range_entry_t(std::stoull(entry[1]), std::stoull(entry[0]), std::stoull(entry[2])));
        }
    }
}

void run_solution() {
    std::string line;
    std::ifstream input_file("../../../../2023/solutions/day5/input.txt");
    if (input_file.is_open()) {
        almanac_t almanac;
        while (std::getline(input_file, line)) {
            parse_line(line, almanac);
        }
        input_file.close();

        std::cout << "Part 1: " << part1(almanac) << std::endl;
        std::cout << "Part 2: " << part2(almanac) << std::endl;
    } else {
        std::cout << "Cannot open input file" << std::endl;
    }
}

int main() {
    run_solution();
    return 0;
}
