#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <random>
#include <stack>
#include <vector>

#define BOARD_LENGTH 10

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
        "Play against bot",
        "Play against another human"
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

void place_ship(const Cell_State& ship_type, const Cell_State (&board)[BOARD_LENGTH][BOARD_LENGTH]) {
    Special_Key input;
    do {
        clear_terminal();
        
        std::cout << "USE ARROW KEYS TO CHANGE POSITION OF SHIP\n";
        std::cout << "USE R TO ROTATE SHIP\n";
        std::cout << "PRESS ENTER TO CONFIRM\n";
        std::cout << '\n';
        const std::vector<std::string> options = {
            "Back",
        };
        print_options(options);
        std::cout << '\n';

        print_board(board);

        input = get_special_keystroke();

        switch(input) {
            case Special_Key::Not_Recognized:
                continue;
            case Special_Key::Right_Arrow:
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
            player_1_defending_board[BOARD_LENGTH][BOARD_LENGTH] = {};
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
