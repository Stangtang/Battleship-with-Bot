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

enum Key {
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

    Key get_special_key() {
        int ch = _getch();

        if (ch == '\r') {
            return Key::Enter;
        }

        if (ch == 'r' || ch == 'R') {
            return Key::R;
        }

        if (ch == 0 || ch == 224) {
            switch (_getch()) {
                case 72: return Key::Up_Arrow;
                case 80: return Key::Down_Arrow;
                case 75: return Key::Left_Arrow;
                case 77: return Key::Right_Arrow;
            }
        }

        return Key::Not_Recognized;
    }
#else
    #include <termios.h>
    #include <unistd.h>
    #include <cstdio>

    Key get_special_key() {
        termios oldt, newt;

        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);

        tcsetattr(STDIN_FILENO, TCSANOW, &newt);

        int ch = getchar();

        Key result = Key::Not_Recognized;

        if (ch == '\n' || ch == '\r') {
            result = Key::Enter;
        } else if (ch == 'r' || ch == 'R') {
            result = Key::R; 
        } else if (ch == 27) { // ESC sequence
            if (getchar() == '[') {
                switch (getchar()) {
                    case 'A': result = Key::Up_Arrow;    break;
                    case 'B': result = Key::Down_Arrow;  break;
                    case 'C': result = Key::Right_Arrow; break;
                    case 'D': result = Key::Left_Arrow;  break;
                }
            }
        }

        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

        return result;
    }
#endif

int main() {
    while (true) {
        switch(get_special_key()) {
            case Key::Enter:       std::cout << "ENTER KEY PRESSED!!\n";   break;
            case Key::Up_Arrow:    std::cout << "UP ARROW PRESSED!!\n";    break;
            case Key::Down_Arrow:  std::cout << "DOWN ARROW PRESSED!!\n";  break;
            case Key::Right_Arrow: std::cout << "RIGHT ARROW PRESSED!!\n"; break;
            case Key::Left_Arrow:  std::cout << "LEFT ARROW PRESSED!!\n";  break;
            case Key::R:           std::cout << "R PRESSED!!\n";           break;
        }
    }
}