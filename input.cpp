#include "input.hpp"

#ifdef _WIN32
    #include <conio.h>

    int get_keystroke() {
        return _getch();
    }

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
    #include <cstdio>
    #include <sys/select.h>
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

    Special_Key get_special_keystroke() {
        termios oldt, newt;

        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;

        newt.c_lflag &= ~(ICANON | ECHO);

        tcsetattr(STDIN_FILENO, TCSANOW, &newt);

        char ch;
        if (read(STDIN_FILENO, &ch, 1) != 1) {
            tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
            return Special_Key::Not_Recognized;
        }

        Special_Key result = Special_Key::Not_Recognized;

        if (ch == '\n' || ch == '\r') {
            result = Special_Key::Enter;
        }
        else if (ch == 'r' || ch == 'R') {
            result = Special_Key::R;
        }
        else if (ch == 27) { // esc
            fd_set set;
            FD_ZERO(&set);
            FD_SET(STDIN_FILENO, &set);

            timeval timeout{};
            timeout.tv_sec = 0;
            timeout.tv_usec = 50000; // 50 ms

            int ready = select(STDIN_FILENO + 1,
                               &set,
                               nullptr,
                               nullptr,
                               &timeout);

            if (ready <= 0) {
                // No additional bytes arrived: treat as escape key
                result = Special_Key::Escape;
            }
            else {
                char seq[2];

                if (read(STDIN_FILENO, &seq[0], 1) == 1 &&
                    read(STDIN_FILENO, &seq[1], 1) == 1) {
                    if (seq[0] == '[') {
                        switch (seq[1]) {
                            case 'A': result = Special_Key::Up_Arrow;    break;
                            case 'B': result = Special_Key::Down_Arrow;  break;
                            case 'C': result = Special_Key::Right_Arrow; break;
                            case 'D': result = Special_Key::Left_Arrow;  break;
                            default:  result = Special_Key::Not_Recognized;
                        }
                    }
                    else {
                        result = Special_Key::Escape;
                    }
                }
                else {
                    result = Special_Key::Escape;
                }
            }
        }

        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        return result;
    }
#endif
