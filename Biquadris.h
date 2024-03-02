#ifndef BIQUADRIS_H_
#define BIQUADRIS_H_
#include <exception>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "subject.h"

class Player;
class GameState;
class Block;
class Prompt;
struct PlayerData;

class Biquadris : public Subject {
public:
  enum Commands {
    LEFT = 0,
    DOWN,
    RIGHT,
    CROTATE,
    CCROTATE,
    LVLUP,
    LVLDOWN,
    RESTART,
#ifdef TEST
    DBG,
    DEBUFF,
#endif /* TEST */
    NORANDOM,
    RANDOM,
    SKIP,
    SEQUENCE,
    BLOCK
  };

  enum Debuffs { BLIND, HEAVY, FORCE, NIL };
  enum BlockType { S = 0, Z, I, J, L, O, T, Dot };
  typedef std::vector<std::vector<char>> Board;

private:
  // TODO: use unique_ptr
  std::vector<std::unique_ptr<Player>> players;
  std::vector<std::unique_ptr<Prompt>> CommandPrompts;
  std::vector<std::string> files;
  int CurrentPlayer;
  int HighScore = 0;
  int winner = -1;
  int defaultLevel = 0;

  void blockLanded();
  void debuff();
  void restart();
  void restartGame();

public:
  Biquadris(std::vector<std::string> files, int level);

  void startGame();
  void runGame();

  // getters
  int getHighScore() const;
  int getWinner() const;
  std::vector<PlayerData> getPlayerData() const;

  ~Biquadris() = default;
};

struct PlayerData {
  int score;
  int level;
  std::pair<Biquadris::BlockType, int> NextBlock;
  Biquadris::Board board;
};

// TODO:
// plz refactor these in cohesive hierarchy

// exception: the block landed on the board
class BlockLanded {};

// exception: the game is over
class GameOver {
public:
  int loser;
};

// exception: game needs to be restarted
class Restart {};

// exception: turn off the game
class Off {};

#endif // BIQUADRIS_H_
