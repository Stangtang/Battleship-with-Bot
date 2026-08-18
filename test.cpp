#include <iostream>

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

        return Special_Key::Not_Recognized;
    }
#else
    #include <termios.h>
    #include <unistd.h>
    #include <cstdio>

    Key get_special_keystroke() {
        termios oldt, newt;

        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);

        tcsetattr(STDIN_FILENO, TCSANOW, &newt);

        int ch = getchar();

        Key result = Special_Key::Not_Recognized;

        if (ch == '\n' || ch == '\r') {
            result = Special_Key::Enter;
        } else if (ch == 'r' || ch == 'R') {
            result = Special_Key::R; 
        } else if (ch == 27) { // ESC sequence
            if (getchar() == '[') {
                switch (getchar()) {
                    case 'A': result = Special_Key::Up_Arrow;    break;
                    case 'B': result = Special_Key::Down_Arrow;  break;
                    case 'C': result = Special_Key::Right_Arrow; break;
                    case 'D': result = Special_Key::Left_Arrow;  break;
                }
            }
        }

        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

        return result;
    }
#endif

int main() {
    while (true) {
        switch(get_special_keystroke()) {
            case Special_Key::Enter:       std::cout << "ENTER KEY PRESSED!!\n";   break;
            case Special_Key::Up_Arrow:    std::cout << "UP ARROW PRESSED!!\n";    break;
            case Special_Key::Down_Arrow:  std::cout << "DOWN ARROW PRESSED!!\n";  break;
            case Special_Key::Right_Arrow: std::cout << "RIGHT ARROW PRESSED!!\n"; break;
            case Special_Key::Left_Arrow:  std::cout << "LEFT ARROW PRESSED!!\n";  break;
            case Special_Key::R:           std::cout << "R PRESSED!!\n";           break;
        }
    }
}