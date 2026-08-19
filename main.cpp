#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <random>
#include <stack>
#include <utility>
#include <vector>

#include "input.hpp"

#define BOARD_LENGTH 10

#define ANSI_RESET "\033[0m"
#define ANSI_RED_BACKGROUND "\033[41m"
#define ANSI_BRIGHTER_RED_BACKGROUND "\033[101m"
#define ANSI_CLEAR_TERMINAL "\033[2J\033[1;1H"

enum Cell_State {
    Unmarked,
    Miss,
    Hit,
    Aircraft_Carrier,
    Battleship,
    Cruiser,
    Submarine,
    Destroyer,
};

enum Menu {
    Start,
    Play_Against_Bot,
    Play_Against_Human,
};

std::stack<Menu> menu;
Cell_State player_1_defending_board[BOARD_LENGTH][BOARD_LENGTH] = {};
Cell_State player_2_defending_board[BOARD_LENGTH][BOARD_LENGTH] = {};
Cell_State player_1_attacking_board[BOARD_LENGTH][BOARD_LENGTH] = {};
Cell_State player_2_attacking_board[BOARD_LENGTH][BOARD_LENGTH] = {};

void init_board(Cell_State (&board)[BOARD_LENGTH][BOARD_LENGTH]) {
    for (unsigned int i = 0; i < BOARD_LENGTH; i++) {
        for (unsigned int j = 0; j < BOARD_LENGTH; j++) {
            board[i][j] = Cell_State::Unmarked;
        }
    }
}

void clear_terminal() {
  std::cout << ANSI_CLEAR_TERMINAL;
// #ifdef _WIN32
//     std::system("cls");
// #else
//     std::system("clear");
// #endif
}

int get_random_number_inclusive(const int& bound_1, const int& bound_2) {
    std::random_device rd;
    const int lower_bound = std::min(bound_1, bound_2);
    const int upper_bound = std::max(bound_1, bound_2);
    std::uniform_int_distribution<int> dist(lower_bound, upper_bound);
    const int random_number = dist(rd);
    return random_number;
}

bool is_option_selection_input_valid(int key_pressed, std::uint8_t num_options) {
    const char key_pressed_char = static_cast<char>(key_pressed);
    if ('1' <= key_pressed_char && key_pressed_char <= '1' + num_options - 1) {
        return true;
    }
    return false;
}

void print_options(const std::vector<std::string>& options) {
    for (std::uint8_t i = 0; i < options.size(); i++) {
        std::cout << i + 1 << " | " + options[i] << '\n';
    }
}

std::uint8_t get_option_selected(const std::vector<std::string>& options) {
    int key_pressed;
    do {
        key_pressed = get_keystroke();
    } while (!is_option_selection_input_valid(key_pressed, options.size()));

    const char key_pressed_char = static_cast<char>(key_pressed);

    return (key_pressed_char - '1' + 1);
}

void handle_start() {
    std::cout << "PRESS NUMBER KEY TO MAKE SELECTION\n";

    const std::vector<std::string> options = {
        "Play Against Bot",
        "Play Against Another Person"
    };
    print_options(options);
    switch(get_option_selected(options)) {
    case 1:
        menu.push(Menu::Play_Against_Bot);
        break;
    case 2:
        menu.push(Menu::Play_Against_Human);
        break;
    }
}

void print_board(const Cell_State (&board)[BOARD_LENGTH][BOARD_LENGTH]) {
    for (const auto& row : board) {
        for (const auto& cell : row) {
            switch(cell) {
            case Cell_State::Unmarked:
                std::cout << '-';
                break;
            case Cell_State::Miss:
                std::cout << 'O';
                break;
            case Cell_State::Hit:
                std::cout << 'X';
                break;
            case Cell_State::Aircraft_Carrier: // TODO: make ships display in diff colors 
                std::cout << 'a'; //red
                break;
            case Cell_State::Battleship:
                std::cout << 'b'; //blue
                break;
            case Cell_State::Cruiser:
                std::cout << 'c'; //green
                break;
            case Cell_State::Submarine:
                std::cout << 's'; //brown
                break;
            case Cell_State::Destroyer:
                std::cout << 'd'; //purple
                break;
            }
            std::cout << ' ';
        }
        std::cout << '\n';
    }
}

bool are_cell_and_neighbors_unmarked(const int& row, const int& col, const Cell_State (&board)[BOARD_LENGTH][BOARD_LENGTH]) {
    for (int i = row - 1; i <= row + 1; i++) {
        for (int j = col - 1; j <= col + 1; j++) {
            if (i < 0 || i >= BOARD_LENGTH || j < 0 || j >= BOARD_LENGTH) {
                continue;
            }
            if (board[i][j] != Cell_State::Unmarked) {
                return false;
            }
        }
    }
    return true;
}

void place_ship_randomly(const Cell_State& ship_type, Cell_State (&board)[BOARD_LENGTH][BOARD_LENGTH]) {
    unsigned int length;
    switch (ship_type) {
    case Cell_State::Aircraft_Carrier: length = 5; break;
    case Cell_State::Battleship:       length = 4; break;
    case Cell_State::Cruiser:          length = 3; break;
    case Cell_State::Submarine:        length = 3; break;
    case Cell_State::Destroyer:        length = 2; break;
    }

    while (true) { // there should be no case where there is no valid placement for a ship
        const unsigned int direction = get_random_number_inclusive(1, 2);
        if (direction == 1) { // left -> right
            const unsigned int row = get_random_number_inclusive(0, BOARD_LENGTH - 1);
            const unsigned int col = get_random_number_inclusive(0, BOARD_LENGTH - 1 - length + 1);

            bool is_placement_valid = true;
            for (unsigned int i = 0; i < length; i++) {
                if (!are_cell_and_neighbors_unmarked(row, col + i, board)) {
                    is_placement_valid = false;
                }
            }
            
            if (!is_placement_valid) {
                continue;
            }

            for (unsigned int i = 0; i < length; i++) {
                board[row][col + i] = ship_type;
            }
            
            break;
        } else if (direction == 2) { // up -> down
            const unsigned int row = get_random_number_inclusive(0, BOARD_LENGTH - 1 - length + 1);
            const unsigned int col = get_random_number_inclusive(0, BOARD_LENGTH - 1);

            bool is_placement_valid = true;
            for (unsigned int i = 0; i < length; i++) {
                if (!are_cell_and_neighbors_unmarked(row + i, col, board)) {
                    is_placement_valid = false;
                }
            }

            if (!is_placement_valid) {
                continue;
            }

            for (unsigned int i = 0; i < length; i++) {
                board[row + i][col] = ship_type;
            }

            break;
        }
    }
}

std::string get_ship_name(const Cell_State& ship_type) {
    switch(ship_type) {
        case Cell_State::Aircraft_Carrier:
            return "Aircraft Carrier";
        case Cell_State::Battleship:
            return "Battleship";
        case Cell_State::Cruiser:
            return "Cruiser";
        case Cell_State::Submarine:
            return "Submarine";
        case Cell_State::Destroyer:
            return "Destroyer";
        default:
            return "\033[34;43mERROR: NOT A SHIP NAME\033[0m"; // This text should never show
    }
}

void move_ship(const int (&direction)[2], const Cell_State& ship_type, Cell_State (&board)[BOARD_LENGTH][BOARD_LENGTH]) {
    const int delta_row = direction[0];
    const int delta_col = direction[1];

    bool can_move = true;
    for (unsigned int row = 0; row < BOARD_LENGTH; row++) {
        for (unsigned int col = 0; col < BOARD_LENGTH; col++) {
            if (board[row][col] == ship_type) {
                const int new_row = row + delta_row;
                const int new_col = col + delta_col;

                if (new_row < 0 || new_row >= BOARD_LENGTH || new_col < 0 || new_col >= BOARD_LENGTH) {
                    can_move = false;
                    continue;
                }

                if (board[new_row][new_col] != Cell_State::Unmarked && board[new_row][new_col] != ship_type) {
                    can_move = false;
                    continue;
                }
            }
        }
    }

    if (!can_move) {
        std::cout << ANSI_RED_BACKGROUND << "\nCANNOT MOVE THERE" << ANSI_RESET << '\n';
        std::cout << "Press any key to continue\n";
        get_keystroke();
        return;
    }

    std::vector<std::pair<int, int>> ship_cells;
    std::vector<std::pair<int, int>> ship_new_cells;
    for (unsigned int row = 0; row < BOARD_LENGTH; row++) {
        for (unsigned int col = 0; col < BOARD_LENGTH; col++) {
            if (board[row][col] == ship_type) {
                const int new_row = row + delta_row;
                const int new_col = col + delta_col;

                ship_cells.push_back(std::make_pair(row, col));
                ship_new_cells.push_back(std::make_pair(new_row, new_col));
            }
        }
    }
    for (const auto& location : ship_cells) {
        board[location.first][location.second] = Cell_State::Unmarked;
    }
    for (const auto& location : ship_new_cells) {
        board[location.first][location.second] = ship_type;
    }
}

void rotate_ship(const Cell_State& ship_type, Cell_State (&board)[BOARD_LENGTH][BOARD_LENGTH]) {
    unsigned int cells_until_center_of_rotation;
    switch(ship_type) {
        case Aircraft_Carrier: cells_until_center_of_rotation = 3; break;
        case Battleship:       cells_until_center_of_rotation = 2; break;
        case Cruiser:          cells_until_center_of_rotation = 2; break;
        case Submarine:        cells_until_center_of_rotation = 2; break;
        case Destroyer:        cells_until_center_of_rotation = 1; break;
    }

    unsigned int center_row;
    unsigned int center_col;
    bool orientation; // 0 is left-right, 1 is top-down
    unsigned int ship_cells_seen = 0;
    for (size_t row = 0; row < BOARD_LENGTH; row++) {
        for (size_t col = 0; col < BOARD_LENGTH; col++) {
            if (board[row][col] == ship_type) {
                ship_cells_seen++;
                if (ship_cells_seen == cells_until_center_of_rotation) {
                    center_row = row;
                    center_col = col;
                    if (col == 0) {
                        orientation = 1;
                    } else if (board[row][col - 1] == board[row][col]) {
                        orientation = 0;
                    } else {
                        orientation = 1;
                    }
                }
            }   
        }
    }
    
    bool can_rotate = true;
    switch(ship_type) {
    case Aircraft_Carrier:
        if (orientation == 0) {
            if (center_row <= 1 || center_row >= BOARD_LENGTH - 2) {
                can_rotate = false;
                break;
            }
            if (board[center_row - 1][center_col] != Unmarked || 
                board[center_row - 2][center_col] != Unmarked ||
                board[center_row + 1][center_col] != Unmarked ||
                board[center_row + 2][center_col] != Unmarked) {
                can_rotate = false;
                break;
            }
        } else if (orientation == 1) {
            if (center_col <= 1 || center_col >= BOARD_LENGTH - 2) {
                can_rotate = false;
                break;
            }
            if (board[center_row][center_col - 1] != Unmarked || 
                board[center_row][center_col - 2] != Unmarked ||
                board[center_row][center_col + 1] != Unmarked ||
                board[center_row][center_col + 2] != Unmarked) {
                can_rotate = false;
                break;
            }
        }
    break;
    case Battleship:
        if (orientation == 0) {
            if (center_row <= 0 || center_row >= BOARD_LENGTH - 2) {
                can_rotate = false;
                break;
            }
            if (board[center_row - 1][center_col] != Unmarked || 
                board[center_row + 1][center_col] != Unmarked ||
                board[center_row + 2][center_col] != Unmarked) {
                can_rotate = false;
                break;
            }
        } else if (orientation == 1) {
            if (center_col <= 0 || center_col >= BOARD_LENGTH - 2) {
                can_rotate = false;
                break;
            }
            if (board[center_row][center_col - 1] != Unmarked || 
                board[center_row][center_col + 1] != Unmarked ||
                board[center_row][center_col + 2] != Unmarked) {
                can_rotate = false;
                break;
            }
        }
    break;
    case Cruiser: // same as submarine
    case Submarine:
        if (orientation == 0) {
            if (center_row <= 0 || center_row >= BOARD_LENGTH - 1) {
                can_rotate = false;
                break;
            }
            if (board[center_row - 1][center_col] != Unmarked || 
                board[center_row + 1][center_col] != Unmarked) {
                can_rotate = false;
                break;
            }
        } else if (orientation == 1) {
            if (center_col <= 0 || center_col >= BOARD_LENGTH - 1) {
                can_rotate = false;
                break;
            }
            if (board[center_row][center_col - 1] != Unmarked || 
                board[center_row][center_col + 1] != Unmarked) {
                can_rotate = false;
                break;
            }
        }
    break;
    case Destroyer:
        if (orientation == 0) {
            if (center_row >= BOARD_LENGTH - 1) {
                can_rotate = false;
                break;
            }
            if (board[center_row + 1][center_col] != Unmarked) {
                can_rotate = false;
                break;
            }
        } else if (orientation == 1) {
            if (center_col >= BOARD_LENGTH - 1) {
                can_rotate = false;
                break;
            }
            if (board[center_row][center_col + 1] != Unmarked) {
                can_rotate = false;
                break;
            }
        }
    break;
    }

    if (!can_rotate) {
        std::cout << ANSI_RED_BACKGROUND << "\nCANNOT ROTATE THERE" << ANSI_RESET << '\n';
        std::cout << "Press any key to continue\n";
        get_keystroke();
        return;
    }

    switch(ship_type) {
    case Aircraft_Carrier:
        if (orientation == 0) {
            board[center_row][center_col - 1] = Unmarked;
            board[center_row][center_col - 2] = Unmarked;
            board[center_row][center_col + 1] = Unmarked;
            board[center_row][center_col + 2] = Unmarked;
            board[center_row - 1][center_col] = ship_type;
            board[center_row - 2][center_col] = ship_type;
            board[center_row + 1][center_col] = ship_type;
            board[center_row + 2][center_col] = ship_type;
        } else if (orientation == 1) {
            board[center_row - 1][center_col] = Unmarked;
            board[center_row - 2][center_col] = Unmarked;
            board[center_row + 1][center_col] = Unmarked;
            board[center_row + 2][center_col] = Unmarked;
            board[center_row][center_col - 1] = ship_type;
            board[center_row][center_col - 2] = ship_type;
            board[center_row][center_col + 1] = ship_type;
            board[center_row][center_col + 2] = ship_type;
        }
    break;
    case Battleship:
        if (orientation == 0) {
            board[center_row][center_col - 1] = Unmarked;
            board[center_row][center_col + 1] = Unmarked;
            board[center_row][center_col + 2] = Unmarked;
            board[center_row - 1][center_col] = ship_type;
            board[center_row + 1][center_col] = ship_type;
            board[center_row + 2][center_col] = ship_type;
        } else if (orientation == 1) {
            board[center_row - 1][center_col] = Unmarked;
            board[center_row + 1][center_col] = Unmarked;
            board[center_row + 2][center_col] = Unmarked;
            board[center_row][center_col - 1] = ship_type;
            board[center_row][center_col + 1] = ship_type;
            board[center_row][center_col + 2] = ship_type;
        }
    break;
    case Cruiser: // same as submarine
    case Submarine:
        if (orientation == 0) {
            board[center_row][center_col - 1] = Unmarked;
            board[center_row][center_col + 1] = Unmarked;
            board[center_row - 1][center_col] = ship_type;
            board[center_row + 1][center_col] = ship_type;
        } else if (orientation == 1) {
            board[center_row - 1][center_col] = Unmarked;
            board[center_row + 1][center_col] = Unmarked;
            board[center_row][center_col - 1] = ship_type;
            board[center_row][center_col + 1] = ship_type;
        }
    break;
    case Destroyer:
        if (orientation == 0) {
            board[center_row][center_col + 1] = Unmarked;
            board[center_row + 1][center_col] = ship_type;
        } else if (orientation == 1) {
            board[center_row + 1][center_col] = Unmarked;
            board[center_row][center_col + 1] = ship_type;
        }
    break;
    }
}

void place_ship(const Cell_State& ship_type, Cell_State (&board)[BOARD_LENGTH][BOARD_LENGTH]) {
    place_ship_randomly(ship_type, board);

    const std::string ship_name = get_ship_name(ship_type);

    Special_Key input;
    do {
        clear_terminal();

        std::cout << "Arrow Keys | Change position of " << ship_name << '\n'; // Manual alignment will do
        std::cout << "R          | Rotate " << ship_name << '\n';
        std::cout << "Escape     | Reset layout\n";
        std::cout << "Enter      | Confirm\n";
        std::cout << '\n';

        print_board(board);

        input = get_special_keystroke();

        switch(input) {
            case Special_Key::Not_Recognized:
                continue;
                break;
            case Special_Key::Escape:
                init_board(board);
                place_ship_randomly(ship_type, board);
                break;
            case Special_Key::Right_Arrow:
                move_ship({0, 1}, ship_type, board);
                break;
            case Special_Key::Left_Arrow:
                move_ship({0, -1}, ship_type, board);
                break;
            case Special_Key::Down_Arrow:
                move_ship({1, 0}, ship_type, board);
                break;
            case Special_Key::Up_Arrow:
                move_ship({-1, 0}, ship_type, board);
                break;
            case Special_Key::R:
                rotate_ship(ship_type, board);
                break;
        }

    } while(input != Special_Key::Enter); // temp exit condition for now, may need to change this

}

void handle_play_against_human() {
    std::cout << "PLAYER 1, CHOOSE YOUR SHIP LAYOUT\n";
    std::cout << '\n';

    const std::vector<std::string> options = { // There is a better way to align the strings but this will do
        "Place Aircraft Carrier (Length 5)",
        "Place Battleship       (Length 4)",
        "Place Cruiser          (Length 3)",
        "Place Submarine        (Length 3)",
        "Place Destroyer        (Length 2)",
        "Reset Layout",
        "Back",
    };
    print_options(options);
    std::cout << '\n';

    print_board(player_1_defending_board);

    switch(get_option_selected(options)) {
        case 1:
            place_ship(Aircraft_Carrier, player_1_defending_board);
            break;
        case 2:
            place_ship(Battleship, player_1_defending_board);
            break;
        case 3:
            place_ship(Cruiser, player_1_defending_board);
            break;
        case 4:
            place_ship(Submarine, player_1_defending_board);
            break;
        case 5:
            place_ship(Destroyer, player_1_defending_board);
            break;
        case 6:
            init_board(player_1_defending_board);
            break;
        case 7:
            init_board(player_1_defending_board);
            menu.pop();
            break;
    }
}

void handle_menu() {
    clear_terminal();

    switch(menu.top()) {
    case Menu::Start:
        handle_start();
        break;
    case Menu::Play_Against_Human:
        handle_play_against_human();
        break;
    case Menu::Play_Against_Bot: 
        break;
    }
}

int main() {

    menu.push(Menu::Start);

    clear_terminal();
    while(true) {
        handle_menu();
        // check win? -- act no that will prolly happen somewhere else
    }
}
