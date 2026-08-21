#include <algorithm>
#include <cctype>
#include <iostream>
#include <random>
#include <stack>
#include <string>
#include <utility>
// #include <unordered_map>
#include <vector>

#include "ansi_codes.hpp"
#include "constants.hpp"
#include "input.hpp"

std::stack<Menu> menu;
// SAVE FOR LATER
// std::unordered_map<Cell_State, std::vector<std::pair<int, int>>>
//     player_one_ship_placements, player_two_ship_placements;
Cell_State player_1_defending_board[BOARD_LENGTH][BOARD_LENGTH] = {};
Cell_State player_2_defending_board[BOARD_LENGTH][BOARD_LENGTH] = {};
Cell_State player_1_attacking_board[BOARD_LENGTH][BOARD_LENGTH] = {};
Cell_State player_2_attacking_board[BOARD_LENGTH][BOARD_LENGTH] = {};
Player curr_player = None;
std::random_device main_rd;

// SAVE FOR LATER
// void init_ship_placements(std::unordered_map<Cell_State, std::vector<std::pair<int, int>>> (&ship_placements)) {
//   ship_placements.clear();
// }

void init_board(Cell_State (*board)[BOARD_LENGTH]) {
    for (unsigned int i = 0; i < BOARD_LENGTH; i++) {
        for (unsigned int j = 0; j < BOARD_LENGTH; j++) {
            board[i][j] = Cell_State::Unmarked;
        }
    }
}

void clear_terminal() {
    std::cout << ANSI_CLEAR_TERMINAL;
}

int get_random_number_inclusive(const int& bound_1, const int& bound_2) {
    const int lower_bound = std::min(bound_1, bound_2);
    const int upper_bound = std::max(bound_1, bound_2);
    std::uniform_int_distribution<int> dist(lower_bound, upper_bound);
    const int random_number = dist(main_rd);
    return random_number;
}

auto to_upper = [](std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
        [](unsigned char c) {
            return std::toupper(c);
        }
    );
    return s;
};

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
        "Play Against Bot (WIP)",
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

void print_board(const Cell_State (*board)[BOARD_LENGTH]) {
    for (unsigned int row = 0; row < BOARD_LENGTH; row++) {
        for (unsigned int col = 0; col < BOARD_LENGTH; col++) {
            switch(board[row][col]) {
            case Cell_State::Unmarked:
                std::cout << '-';
                break;
            case Cell_State::Miss:
                std::cout << 'O';
                break;
            case Cell_State::Hit:
                std::cout << 'X';
                break;
            case Cell_State::Aircraft_Carrier:
                std::cout << PRINT_AIRCRAFT_CARRIER_WITH_COLOR;
                break;
            case Cell_State::Battleship:
                std::cout << PRINT_BATTLESHIP_WITH_COLOR;
                break;
            case Cell_State::Cruiser:
                std::cout << PRINT_CRUISER_WITH_COLOR;
                break;
            case Cell_State::Submarine:
                std::cout << PRINT_SUBMARINE_WITH_COLOR;
                break;
            case Cell_State::Destroyer:
                std::cout << PRINT_DESTROYER_WITH_COLOR;
                break;
            }
            std::cout << ' ';
        }
        std::cout << '\n';
    }
}

void print_ship_color_legend() {
    std::cout << "SHIP COLOR LEGEND\n";
    std::cout << PRINT_AIRCRAFT_CARRIER_WITH_COLOR << " : Aircraft Carrier\n";
    std::cout << PRINT_BATTLESHIP_WITH_COLOR << " : Battleship\n";
    std::cout << PRINT_CRUISER_WITH_COLOR << " : Cruiser\n";
    std::cout << PRINT_SUBMARINE_WITH_COLOR << " : Submarine\n";
    std::cout << PRINT_DESTROYER_WITH_COLOR << " : Destroyer\n";
}

void print_enemy_seas_legend() {
    std::cout << "ENEMY SEAS LEGEND\n";
    std::cout << "- : Unmarked\n";
    std::cout << "O : Miss\n";
    std::cout << "X : Hit\n";
}

bool is_index_in_bounds(const int& index) {
    return index >= 0 && index < BOARD_LENGTH;
}

bool is_position_in_bounds(const int& row, const int& col) {
    return is_index_in_bounds(row) && is_index_in_bounds(col);
}

bool are_cell_and_neighbors_unmarked(const int& row, const int& col, const Cell_State (*board)[BOARD_LENGTH]) {
    for (int i = row - 1; i <= row + 1; i++) {
        for (int j = col - 1; j <= col + 1; j++) {
            if (board[i][j] != Cell_State::Unmarked || !is_position_in_bounds(row, col)) {
                return false;
            }
        }
    }
    return true;
}

void remove_ship(const Cell_State& ship_type, Cell_State (*board)[BOARD_LENGTH]) { 
    for (unsigned int i = 0; i < BOARD_LENGTH; i++) {
        for (unsigned int j = 0; j < BOARD_LENGTH; j++) {
            if (board[i][j] == ship_type) {
                board[i][j] = Cell_State::Unmarked;
            }
        }
    }
};

void place_ship_randomly(const Cell_State& ship_type, Cell_State (*board)[BOARD_LENGTH]) {
    unsigned int ship_length;
    switch (ship_type) {
    case Cell_State::Aircraft_Carrier: ship_length = 5; break;
    case Cell_State::Battleship:       ship_length = 4; break;
    case Cell_State::Cruiser:          ship_length = 3; break;
    case Cell_State::Submarine:        ship_length = 3; break;
    case Cell_State::Destroyer:        ship_length = 2; break;
    default: return;
    }

    while (true) { // there should be no case where there is no valid placement for a ship
        const unsigned int direction = get_random_number_inclusive(1, 2);
        if (direction == 1) { // horizontal
            const unsigned int row = get_random_number_inclusive(0, BOARD_LENGTH - 1);
            const unsigned int col = get_random_number_inclusive(0, BOARD_LENGTH - 1 - ship_length + 1);

            bool is_placement_valid = true;
            for (unsigned int i = 0; i < ship_length; i++) {
                if (!are_cell_and_neighbors_unmarked(row, col + i, board)) {
                    is_placement_valid = false;
                }
            }
            
            if (!is_placement_valid) {
                continue;
            }

            for (unsigned int i = 0; i < ship_length; i++) {
                board[row][col + i] = ship_type;
            }
            
            break;
        } else if (direction == 2) { // vertical
            const unsigned int row = get_random_number_inclusive(0, BOARD_LENGTH - 1 - ship_length + 1);
            const unsigned int col = get_random_number_inclusive(0, BOARD_LENGTH - 1);

            bool is_placement_valid = true;
            for (unsigned int i = 0; i < ship_length; i++) {
                if (!are_cell_and_neighbors_unmarked(row + i, col, board)) {
                    is_placement_valid = false;
                }
            }

            if (!is_placement_valid) {
                continue;
            }

            for (unsigned int i = 0; i < ship_length; i++) {
                board[row + i][col] = ship_type;
            }

            break;
        }
    }
}

std::string get_ship_text(const Cell_State& ship_type) {
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

std::string get_player_text(const Player& player) {
    switch(player) {
        case Player_1: return "Player 1";
        case Player_2: return "Player 2";
        // add bot case?
    }
}

void move_ship(const int (&direction)[2], const Cell_State& ship_type, Cell_State (*board)[BOARD_LENGTH]) {
    const int delta_row = direction[0];
    const int delta_col = direction[1];

    bool can_move = true;
    for (unsigned int row = 0; row < BOARD_LENGTH && can_move; row++) {
        for (unsigned int col = 0; col < BOARD_LENGTH && can_move; col++) {
            if (board[row][col] == ship_type) {
                const int new_row = row + delta_row;
                const int new_col = col + delta_col;

                if (!is_position_in_bounds(new_row, new_col)) {
                    can_move = false;
                    break;
                }

                if (board[new_row][new_col] != Cell_State::Unmarked && board[new_row][new_col] != ship_type) {
                    can_move = false;
                    break;
                }
            }
        }
    }

    if (!can_move) {
        std::cout << ANSI_RED_BACKGROUND << "\nCANNOT MOVE THERE" << ANSI_RESET << '\n';
        std::cout << "Press Any Key to Continue\n";
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

void rotate_ship(const Cell_State& ship_type, Cell_State (*board)[BOARD_LENGTH]) {
    unsigned int cells_until_center_of_rotation;
    switch(ship_type) {
        case Aircraft_Carrier: cells_until_center_of_rotation = 3; break;
        case Battleship:       cells_until_center_of_rotation = 2; break;
        case Cruiser:          cells_until_center_of_rotation = 2; break;
        case Submarine:        cells_until_center_of_rotation = 2; break;
        case Destroyer:        cells_until_center_of_rotation = 1; break;
        default: return;
    }

    unsigned int center_row;
    unsigned int center_col;
    bool orientation; // 0 is horizontal, 1 is vertical
    unsigned int ship_cells_seen = 0;
    bool center_found = false;
    for (unsigned int row = 0; row < BOARD_LENGTH && !center_found; row++) {
        for (unsigned int col = 0; col < BOARD_LENGTH; col++) {
            if (board[row][col] == ship_type) {
                ship_cells_seen++;
                if (ship_cells_seen == cells_until_center_of_rotation) {
                    center_row = row;
                    center_col = col;
                    center_found = true;
                    if (col == 0) {
                        orientation = 1;
                        break;
                    }
                    if (col == BOARD_LENGTH - 1) {
                        orientation = 1;
                        break;
                    }
                    if (board[row][col - 1] == board[row][col] || board[row][col + 1] == board[row][col]) {
                        orientation = 0;
                        break;
                    }
                    orientation = 1;
                    break;
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
    case Cruiser:
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
            if (center_row == BOARD_LENGTH - 1) {
                can_rotate = false;
                break;
            }
            if (board[center_row + 1][center_col] != Unmarked) {
                can_rotate = false;
                break;
            }
        } else if (orientation == 1) {
            if (center_col == BOARD_LENGTH - 1) {
                can_rotate = false;
                break;
            }
            if (board[center_row][center_col + 1] != Unmarked) {
                can_rotate = false;
                break;
            }
        }
    break;
    default: return;
    }

    if (!can_rotate) {
        std::cout << ANSI_RED_BACKGROUND << "\nCANNOT ROTATE THERE" << ANSI_RESET << '\n';
        std::cout << "Press Any Key to Continue\n";
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
    case Cruiser:
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
    default: return;
    }
}

void place_ship(const Cell_State& ship_type, Cell_State (*board)[BOARD_LENGTH]) {
    place_ship_randomly(ship_type, board);

    const std::string ship_name = get_ship_text(ship_type);

    Special_Key input;
    do {
        clear_terminal();

        std::cout << "PLACING " << to_upper(ship_name) << " FOR " << to_upper(get_player_text(curr_player)) << "\n";
        std::cout << '\n';

        std::cout << "Arrow Keys | Change Position of " << ship_name << '\n';
        std::cout << "R          | Rotate " << ship_name << '\n';
        std::cout << "Escape     | Reset Layout\n";
        std::cout << "Enter      | Confirm\n";
        std::cout << '\n';

        print_ship_color_legend();
        std::cout << '\n';

        print_board(board);

        input = get_special_keystroke();
        switch(input) {
            case Special_Key::Not_Recognized:
                continue;
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
            default: break;
        }
    } while(input != Special_Key::Enter);
}

bool are_all_ships_placed(const Cell_State (*board)[BOARD_LENGTH]) {
    const Cell_State ships[] = {
        Aircraft_Carrier,
        Battleship,
        Cruiser,
        Submarine,
        Destroyer
    };
    for (Cell_State ship : ships) {
        bool found = false;
        for (int i = 0; i < BOARD_LENGTH && !found; i++) {
            for (int j = 0; j < BOARD_LENGTH; j++) {
                if (board[i][j] == ship) {
                    found = true;
                    break;
                }
            }
        }
        if (!found) {
            return false;
        }
    }
    return true;
}

void handle_play_against_human_ship_placement() {
    const std::string curr_player_text = get_player_text(curr_player);

    std::cout << to_upper(curr_player_text) << ", CHOOSE YOUR SHIP LAYOUT\n";
    std::cout << '\n';

    const std::vector<std::string> options = {
        "Place " + std::string(ANSI_AIRCRAFT_CARRIER_COLOR) + "Aircraft Carrier\e[0m (Length 5)",
        "Place " + std::string(ANSI_BATTLESHIP_COLOR) + "Battleship\e[0m       (Length 4)",
        "Place " + std::string(ANSI_CRUISER_COLOR) + "Cruiser\e[0m          (Length 3)",
        "Place " + std::string(ANSI_SUBMARINE_COLOR) + "Submarine\e[0m        (Length 3)",
        "Place " + std::string(ANSI_DESTROYER_COLOR) + "Destroyer\e[0m        (Length 2)",
        "Reset Layout",
        "Confirm Layout",
        "Back",
    };
    print_options(options);
    std::cout << '\n';

    print_ship_color_legend();
    std::cout << '\n';

    Cell_State (*curr_player_defending_board)[BOARD_LENGTH];
    switch (curr_player) {
        case Player_1:
            curr_player_defending_board = player_1_defending_board;
            break;
        case Player_2:
            curr_player_defending_board = player_2_defending_board;
            break;
    }

    print_board(curr_player_defending_board);

    switch(get_option_selected(options)) {
        case 1:
            remove_ship(Aircraft_Carrier, curr_player_defending_board);
            place_ship(Aircraft_Carrier, curr_player_defending_board);
            break;
        case 2:
            remove_ship(Battleship, curr_player_defending_board);
            place_ship(Battleship, curr_player_defending_board);
            break;
        case 3:
            remove_ship(Cruiser, curr_player_defending_board);
            place_ship(Cruiser, curr_player_defending_board);
            break;
        case 4:
            remove_ship(Submarine, curr_player_defending_board);
            place_ship(Submarine, curr_player_defending_board);
            break;
        case 5:
            remove_ship(Destroyer, curr_player_defending_board);
            place_ship(Destroyer, curr_player_defending_board);
            break;
        case 6:
            init_board(curr_player_defending_board);
            break;
        case 7:
            if (!are_all_ships_placed(curr_player_defending_board)) {
                std::cout << ANSI_RED_BACKGROUND << "\nCANNOT CONFIRM LAYOUT: NOT ALL SHIPS PLACED" << ANSI_RESET << '\n';
                std::cout << "Press Any Key to Continue\n";
                get_keystroke();
                break;
            }
            menu.pop();
            break;
        case 8:
            init_board(curr_player_defending_board);
            menu.pop();
            break;
    }
}

void handle_play_against_human() {
    if (!are_all_ships_placed(player_1_defending_board)) {
        curr_player = Player_1;
        menu.push(Menu::Play_Against_Human_Ship_Placement);
    } else if (!are_all_ships_placed(player_2_defending_board)) {
        curr_player = Player_2;
        menu.push(Menu::Play_Against_Human_Ship_Placement);
    } else {
        curr_player = None;
        menu.push(Menu::Player_Versus_Player_Game);
    }
}

Player get_first_player() {
    std::cout << "SHIP PLACEMENT COMPLETE\n\n";
    std::cout << "Who Goes First?\n";
    const std::vector<std::string> options = {
        "Player 1",
        "Player 2",
    };
    print_options(options);
    switch(get_option_selected(options)) {
        case 1: return Player_1;
        case 2: return Player_2;
    }
}

void view_enemy_seas(const Cell_State (*curr_player_attacking_board)[BOARD_LENGTH], const std::string& enemy_player_text) {
    clear_terminal();

    std::cout << "VIEWING ENEMY (" << to_upper(enemy_player_text) << "'S) SEAS\n";
    std::cout << '\n';
    
    const std::vector<std::string> options = {
        "Back",
    };
    print_options(options);
    std::cout << '\n';

    print_enemy_seas_legend();
    std::cout << '\n';
    std::cout << "ENEMY SEAS\n";
    print_board(curr_player_attacking_board);

    switch (get_option_selected(options)) {
    case 1: return;
    }
}

void view_your_seas(const Cell_State (*curr_player_defending_board)[BOARD_LENGTH], const std::string curr_player_text) {
    clear_terminal();

    std::cout << "VIEWING YOUR (" << to_upper(curr_player_text) << "'S) SEAS\n";
    std::cout << '\n';
    
    const std::vector<std::string> options = {
        "Back",
    };
    print_options(options);
    std::cout << '\n';

    print_ship_color_legend();
    std::cout << '\n';
    std::cout << "YOUR SEAS\n";
    print_board(curr_player_defending_board);

    switch (get_option_selected(options)) {
    case 1: return;
    }
}

void print_attacking_board_with_selected_cell(Cell_State (*board)[BOARD_LENGTH], unsigned int selected_row, unsigned int selected_col) {
    for (unsigned int row = 0; row < BOARD_LENGTH; row++) {
        for (unsigned int col = 0; col < BOARD_LENGTH; col++) {
            if (row == selected_row && col == selected_col) {
                std::cout << HIGHLIGHT_SELECTED_CELL_BACKGROUND_COLOR;
            }
            switch(board[row][col]) {
            case Cell_State::Unmarked:
                std::cout << '-';
                break;
            case Cell_State::Miss:
                std::cout << 'O';
                break;
            case Cell_State::Hit:
                std::cout << 'X';
                break;
            }
            std::cout << ANSI_RESET << ' ';
        }
        std::cout << '\n';
    }
}

void notify_move_selection_out_of_bounds() {
    std::cout << ANSI_RED_BACKGROUND << "\nCANNOT SELECT THERE: OUT OF BOUNDS" << ANSI_RESET << '\n';
    std::cout << "Press Any Key to Continue\n";
    get_keystroke();
}

void attack(Cell_State (*curr_player_attacking_board)[BOARD_LENGTH], const Cell_State (*enemy_player_defending_board)[BOARD_LENGTH], const std::string& curr_player_text, const std::string& enemy_player_text) {
    unsigned int selected_row = get_random_number_inclusive(0, BOARD_LENGTH - 1);
    unsigned int selected_col = get_random_number_inclusive(0, BOARD_LENGTH - 1);

    bool cell_selected = false;
    Special_Key input;
    do {
        clear_terminal();
        std::cout << "ATTACKING ENEMY (" << to_upper(enemy_player_text) << "'S) SEAS\n";
        std::cout << '\n';

        std::cout << "Arrow Keys | Change Selected Cell\n";
        std::cout << "Enter      | Confirm\n";
        std::cout << "Escape     | Back\n";
        std::cout << '\n';

        std::cout << HIGHLIGHT_SELECTED_CELL_BACKGROUND_COLOR << ' ' << ANSI_RESET << " : Selected Cell\n";
        std::cout << '\n';

        print_attacking_board_with_selected_cell(curr_player_attacking_board, selected_row, selected_col);

        input = get_special_keystroke();
        switch(input) {
            case Not_Recognized: 
                continue;
            case Escape:
                return;
            case Right_Arrow:
                if (is_index_in_bounds(selected_col + 1)) {
                    selected_col++;
                    break;
                }
                notify_move_selection_out_of_bounds();
                break;
            case Left_Arrow:
                if (is_index_in_bounds(selected_col - 1)) {
                    selected_col--;
                    break;
                }
                notify_move_selection_out_of_bounds();
                break;
            case Down_Arrow:
                if (is_index_in_bounds(selected_row + 1)) {
                    selected_row++;
                    break;
                }
                notify_move_selection_out_of_bounds();
                break;
            case Up_Arrow:
                if (is_index_in_bounds(selected_row - 1)) {
                    selected_row--;
                    break;
                }
                notify_move_selection_out_of_bounds();
                break;
            case Enter:
                if (curr_player_attacking_board[selected_row][selected_col] != Unmarked) {
                    std::string cell_state_text;
                    if (curr_player_attacking_board[selected_row][selected_col] == Hit) {
                        cell_state_text = "HIT";
                    } else if (curr_player_attacking_board[selected_row][selected_col] == Miss) {
                        cell_state_text = "MISS";
                    }
                    std::cout << ANSI_RED_BACKGROUND << "\nCANNOT ATTACK THERE: ALREADY A " << cell_state_text << ANSI_RESET << '\n';
                    std::cout << "Press Any Key to Continue\n";
                    get_keystroke();
                    break;
                }

                cell_selected = true;
                break;
        }
    } while(!cell_selected);

    if (enemy_player_defending_board[selected_row][selected_col] == Unmarked) {
        curr_player_attacking_board[selected_row][selected_col] = Miss;
        
        clear_terminal();
        std::cout << "ENEMY (" << to_upper(enemy_player_text) << "'S) SEAS\n";
        print_board(curr_player_attacking_board);
        std::cout << ANSI_BRIGHT_BLUE_FOREGROUND << "\nIT WAS A MISS!" << ANSI_RESET << '\n';
        std::cout << "Press Any Key to Continue\n";
        get_keystroke();

        if (curr_player == Player_1) {
            curr_player = Player_2;
        } else {
            curr_player = Player_1;
        }

        return;
    }

    curr_player_attacking_board[selected_row][selected_col] = Hit;

    clear_terminal();
    std::cout << "ENEMY (" << to_upper(enemy_player_text) << "'S) SEAS\n";
    print_board(curr_player_attacking_board);
    std::cout << ANSI_LIGHT_GREEN_FOREGROUND << "\nIT WAS A HIT!" << ANSI_RESET << '\n';
    std::cout << "Press Any Key to Continue\n";
    get_keystroke();

    unsigned int num_ships_hit = 0;
    for (unsigned int i = 0; i < BOARD_LENGTH; i++) {
        for (unsigned int j = 0; j < BOARD_LENGTH; j++) {
            if (curr_player_attacking_board[i][j] == Hit) {
                num_ships_hit++;
            }
        }
    }

    if (num_ships_hit == 5 + 4 + 3 + 3 + 2) {
        clear_terminal();
        std::cout << ANSI_LIGHT_YELLOW_FOREGROUND << to_upper(curr_player_text) << " IS THE WINNER" << ANSI_RESET << '\n';
        std::cout << '\n';

        std::cout << curr_player_text << "'s Attacks\n";
        print_board(curr_player_attacking_board);
        std::cout << '\n';

        std::cout << "Press Any Key to Return to Main Menu\n";
        get_keystroke();

        std::stack<Menu> new_menu;
        new_menu.push(Menu::Start);
        menu = new_menu;
        init_board(player_1_defending_board);
        init_board(player_2_defending_board);
        init_board(player_1_attacking_board);
        init_board(player_2_attacking_board);
        return;
    }

    attack(curr_player_attacking_board, enemy_player_defending_board, curr_player_text, enemy_player_text);
}

void handle_player_versus_player_game() {
    if (curr_player == None) {
        curr_player = get_first_player();
        return;
    }

    const std::string curr_player_text = get_player_text(curr_player);
    std::string enemy_player_text;
    Cell_State (*curr_player_defending_board)[BOARD_LENGTH];
    Cell_State (*curr_player_attacking_board)[BOARD_LENGTH];
    Cell_State (*enemy_player_defending_board)[BOARD_LENGTH];
    switch (curr_player) {
        case Player_1:
            curr_player_defending_board = player_1_defending_board;
            curr_player_attacking_board = player_1_attacking_board;
            enemy_player_defending_board = player_2_defending_board;
            enemy_player_text = get_player_text(Player_2);
            break;
        case Player_2:
            curr_player_defending_board = player_2_defending_board;
            curr_player_attacking_board = player_2_attacking_board;
            enemy_player_defending_board = player_1_defending_board;
            enemy_player_text = get_player_text(Player_1);
            break;
    }

    std::cout << to_upper(curr_player_text) << "'S TURN\n";
    std::cout << '\n';

    const std::vector<std::string> options = {
        "View Enemy (" + enemy_player_text + "'s) Seas",
        "View Your (" + curr_player_text + "'s) Seas",
        "Attack",
    };
    print_options(options);
    std::cout << '\n';

    std::cout << "ENEMY SEAS\n";
    print_board(curr_player_attacking_board);
    std::cout << '\n';
    
    std::cout << "YOUR SEAS\n";
    print_board(curr_player_defending_board);

    switch(get_option_selected(options)) {
        case 1:
            view_enemy_seas(curr_player_attacking_board, enemy_player_text);
            return;
        case 2:
            view_your_seas(curr_player_defending_board, curr_player_text);
            return;
        case 3:
            attack(curr_player_attacking_board, enemy_player_defending_board, curr_player_text, enemy_player_text);
            return;
    }
}

void handle_menu() {
    clear_terminal();

    switch(menu.top()) {
    case Start:
        handle_start();
        break;
    case Play_Against_Human:
        handle_play_against_human();
        break;
    case Play_Against_Human_Ship_Placement:
        handle_play_against_human_ship_placement();
        break;
    case Player_Versus_Player_Game:
        handle_player_versus_player_game();
        break;
    case Play_Against_Bot:
        // not implemented yet
        break;
    }
}

int main() {
    menu.push(Menu::Start);
    clear_terminal();

    while(true) {
        handle_menu();
    }
}
