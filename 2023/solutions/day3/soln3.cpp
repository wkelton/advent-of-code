#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <cassert>

using grid_t = std::vector<std::string>;

class grid_data_t {
public:
    grid_data_t(grid_t&& grid) : _grid(std::move(grid)), _visited() {
        assert(!_grid.empty());
        assert(!_grid.front().empty());

        for (size_t i = 0ul; i < num_rows(); i++) {
            _visited.push_back(std::vector<bool>(num_columns(), false));
        }
    }

    const grid_t& grid() const { return _grid; }
    size_t num_rows() const { return _grid.size(); }
    size_t last_row() const { return num_rows() - 1ul; }
    size_t num_columns() const { return _grid.front().size(); }
    size_t last_column() const { return num_columns() - 1ul; }

    bool has_visited(size_t i, size_t j) const { return _visited[i][j]; }
    void visit(size_t i, size_t j) { _visited[i][j] = true; }

private:
    grid_t _grid;
    std::vector<std::vector<bool>> _visited;
};

bool is_digit_left(const grid_data_t& data, size_t i, size_t j) {
    return j > 0ul && std::isdigit(data.grid()[i][j - 1]);
}

bool is_digit_right(const grid_data_t& data, size_t i, size_t j) {
    return j < data.last_column() && std::isdigit(data.grid()[i][j + 1]);
};

int parse_number(grid_data_t& data, size_t i, size_t j, bool mark_visited) {
    // For Part 1, it's quite clear that we need to be sure not to double count numbers we find
    // as we are searching based on adjacent digits to symbols (we could find the same digit from different symbols).
    // For Part 2, it's not clear or specified what to do if there is a shared number around 2 '*'s.
    // The input data does not appear to have this situation; so, not worrying about it *shrug*
    if (mark_visited && data.has_visited(i, j)) return 0;
    auto left_j = j;
    while(is_digit_left(data, i, left_j)) {
        left_j--;
    }

    auto right_j = j;
    while(is_digit_right(data, i, right_j)) {
        right_j++;
    }

    if (mark_visited) {
        for (auto k = left_j; k <= right_j; k++) {
            data.visit(i, k);
        }
    }

    return std::stoi(data.grid()[i].substr(left_j, right_j)); 
}

int part1(grid_data_t& data) {
    auto sum_neighbors = [&data](auto i, auto j) {
        int s = 0;
        if (is_digit_left(data, i, j)) {
            s += parse_number(data, i, j - 1, true);
        }
        if (is_digit_right(data, i, j)) {
            s += parse_number(data, i, j + 1, true);
        }
        return s;
    };

    int sum = 0;

    for (size_t i = 0ul; i < data.num_rows(); i++) {
        for (size_t j = 0ul; j < data.num_columns(); j++) {
            if (data.grid()[i][j] != '.' && !std::isdigit(data.grid()[i][j])) {
                // above
                if (i > 0ul) {
                    if (std::isdigit(data.grid()[i - 1][j])) {
                        sum += parse_number(data, i - 1, j, true);
                    } else {
                        sum += sum_neighbors(i - 1, j);
                    }
                }

                sum += sum_neighbors(i, j);

                // below
                if (i < data.last_row()) {
                    if (std::isdigit(data.grid()[i + 1][j])) {
                        sum += parse_number(data, i + 1, j, true);
                    } else {
                        sum += sum_neighbors(i + 1, j);
                    }
                }

            }
        }
    }

    return sum;
}

int part2(grid_data_t& data) {
    auto check_neighbors = [&data](auto i, auto j, int& count) {
        int p = 1;
        if (is_digit_left(data, i, j)) {
            p *= parse_number(data, i, j - 1, false);
            count++;
        }
        if (is_digit_right(data, i, j)) {
            p *= parse_number(data, i, j + 1, false);
            count++;
        }
        return p;
    };

    int sum = 0;

    for (size_t i = 0ul; i < data.num_rows(); i++) {
        for (size_t j = 0ul; j < data.num_columns(); j++) {
            if (data.grid()[i][j] == '*') {
                int gear_ratio = 1;
                int adjacent_count = 0;

                // above
                if (i > 0ul) {
                    if (std::isdigit(data.grid()[i - 1][j])) {
                        gear_ratio *= parse_number(data, i - 1, j, false);
                        adjacent_count++;
                    } else {
                        gear_ratio *= check_neighbors(i - 1, j, adjacent_count);
                    }
                }

                gear_ratio *= check_neighbors(i, j, adjacent_count);

                // below
                if (i < data.last_row()) {
                    if (std::isdigit(data.grid()[i + 1][j])) {
                        gear_ratio *= parse_number(data, i + 1, j, false);
                        adjacent_count++;
                    } else {
                        gear_ratio *= check_neighbors(i + 1, j, adjacent_count);
                    }
                }

                if (adjacent_count == 2) {
                    sum += gear_ratio;
                }
            }
        }
    }

    return sum;
}

void run_solution() {
    std::string line;
    std::ifstream input_file("../../../../2023/solutions/day3/input.txt");
    if (input_file.is_open()) {
        grid_t grid;
        while (std::getline(input_file, line)) {
            grid.push_back(line);
        }
        input_file.close();

        grid_data_t data(std::move(grid));

        std::cout << "Part 1: " << part1(data) << std::endl;
        std::cout << "Part 2: " << part2(data) << std::endl;
    } else {
        std::cout << "Cannot open input file" << std::endl;
    }
}

int main() {
    run_solution();
    return 0;
}
