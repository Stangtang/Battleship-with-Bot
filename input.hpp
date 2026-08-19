#pragma once

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

int get_keystroke();
Special_Key get_special_keystroke();
