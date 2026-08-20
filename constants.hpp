#pragma once

static const unsigned int BOARD_LENGTH = 10;

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

enum Player {
  None,
  Player_1,
  Player_2,
  // Bot
};

enum Menu {
  Start,
  Play_Against_Human,
  Play_Against_Human_Ship_Placement,
  Player_Versus_Player_Game,
  Play_Against_Bot,
};
