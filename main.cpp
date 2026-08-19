#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <random>
#include <stack>
#include <utility>
#include <vector>

#define BOARD_LENGTH 10

#define ANSI_RESET "\033[0m"
#define ANSI_RED_BACKGROUND "\033[41m"
#define ANSI_BRIGHTER_RED_BACKGROUND "\033[101m"

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

void clear_terminal()
{
#ifdef _WIN32
    std::system("cls");
#else
    std::system("clear");
#endif
}

#ifdef _WIN32
    #include <conio.h>

    int get_keystroke() {
        return _getch();
    }
#else
    #include <termios.h>
    #include <unistd.h>

    int get_keystroke() {
        struct termios oldt, newt;
        int ch;

        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;

        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);

        ch = getchar();

        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

        return ch;
    }
#endif

enum Special_Key {
    Not_Recognized,
    Enter,
    Up_Arrow,
    Down_Arrow,
    Left_Arrow,
    Right_Arrow,
    R,
    Escape,
};

#ifdef _WIN32
    #include <conio.h>

    Special_Key get_special_keystroke() {
        int ch = _getch();

        if (ch == '\r') {
            return Special_Key::Enter;
        }

        if (ch == 'r' || ch == 'R') {
            return Special_Key::R;
        }

        if (ch == 0 || ch == 224) {
            switch (_getch()) {
                case 72: return Special_Key::Up_Arrow;
                case 80: return Special_Key::Down_Arrow;
                case 75: return Special_Key::Left_Arrow;
                case 77: return Special_Key::Right_Arrow;
            }
        }

        if (ch == 27) {
            return Special_Key::Escape;
        }

        return Special_Key::Not_Recognized;
    }
#else
    #include <termios.h>
    #include <unistd.h>
    #include <cstdio>
    #include <sys/select.h>

    Special_Key get_special_keystroke() {
        termios oldt, newt;

        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);

        tcsetattr(STDIN_FILENO, TCSANOW, &newt);

        int ch = getchar();

        Special_Key result = Special_Key::Not_Recognized;

        if (ch == '\n' || ch == '\r') {
            result = Special_Key::Enter;
        } else if (ch == 'r' || ch == 'R') {
            result = Special_Key::R;
        } else if (ch == 27) { // Escape sequence
            fd_set set;
            FD_ZERO(&set);
            FD_SET(STDIN_FILENO, &set);

            timeval timeout{};
            timeout.tv_sec = 0;
            timeout.tv_usec = 0;

            if (select(STDIN_FILENO + 1, &set, nullptr, nullptr, &timeout) == 0) {
                result = Special_Key::Escape;
            } else {
                if (getchar() == '[') {
                    switch (getchar()) {
                        case 'A': result = Special_Key::Up_Arrow;    break;
                        case 'B': result = Special_Key::Down_Arrow;  break;
                        case 'C': result = Special_Key::Right_Arrow; break;
                        case 'D': result = Special_Key::Left_Arrow;  break;
                    }
                }
            }
        }

        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

        return result;
    }
#endif

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

bool are_cell_and_neighbors_unmarked(const int& row, const int& col, Cell_State (&board)[BOARD_LENGTH][BOARD_LENGTH]) {
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
        "Back",
    };
    print_options(options);
    std::cout << '\n';

    print_board(player_1_defending_board);

    switch(get_option_selected(options)) {
        case 1:
            place_ship(Cell_State::Aircraft_Carrier, player_1_defending_board);
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        case 6:
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
