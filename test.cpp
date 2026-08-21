#include <iostream>
using namespace std;

int main() {
    cout << "\033[38;5;7m" << "Light Gray\n";
    cout << "\033[38;5;8m" << "Dark Gray\n";
    cout << "\033[38;5;94m" << "Dark Yellow\n";
    cout << "\033[0m" << "Reset\n";
}