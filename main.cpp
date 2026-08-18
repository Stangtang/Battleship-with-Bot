#include <cstdint>
#include <cstdlib>
#include <iostream>
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

std::stack<Menu> curr_menu;
Cell_State player_1_self_view_grid[BOARD_LENGTH][BOARD_LENGTH] = {};
Cell_State player_1_opp_view_grid[BOARD_LENGTH][BOARD_LENGTH] = {};
Cell_State player_2_self_view_grid[BOARD_LENGTH][BOARD_LENGTH] = {};
Cell_State player_2_opp_view_grid[BOARD_LENGTH][BOARD_LENGTH] = {};

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

std::uint8_t print_options_and_get_option_selected(const std::vector<std::string>& options) {
    int key_pressed;
    do {
        clear_terminal();
    
        print_options(options);

        key_pressed = get_keystroke();
    } while (!is_option_selection_input_valid(key_pressed, options.size()));

    const char key_pressed_char = static_cast<char>(key_pressed);

    return (key_pressed_char - '1' + 1);
}

void handle_start(std::stack<Menu>& menu) {
    std::cout << "PRESS NUMBER KEY TO MAKE SELECTION\n";

    const std::vector<std::string> options = {
        "Play against bot",
        "Play against another human"
    };
    switch(print_options_and_get_option_selected(options)) {
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

void handle_play_against_human(std::stack<Menu>& menu) {
    std::cout << "PLAYER 1, CHOOSE YOUR SHIP LAYOUT\n";
    std::cout << '\n';

    const std::vector<std::string> options = {
        "Place Aircraft Carrier (Length 5)",
        "Place Battleship (Length 4)",
        "Place Cruiser (Length 3)",
        "Place Submarine (Length 3)",
        "Place Destroyer (Length 2)",
        "Back"
    };
    
    print_options(options);
    std::cout << '\n';
    print_board(player_1_self_view_grid);
    switch(get_option_selected(options)) {
        case 1:
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
            menu.pop();
            break;
    }
}

void handle_menu(std::stack<Menu>& menu) {
    clear_terminal();

    switch(menu.top()) {
    case Menu::Start:
        handle_start(menu);
        break;
    case Menu::Play_Against_Human:
        handle_play_against_human(menu);
    }
}

int main() {
    curr_menu.push(Menu::Start);

    clear_terminal();
    while(true) {
        handle_menu(curr_menu);
        // check win? -- act no that will prolly happen somewhere else
    }
}
