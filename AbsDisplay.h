#ifndef DISPLAY_H_
#define DISPLAY_H_
#include "Biquadris.h"
#include "Block.h"
#include "observer.h"

class Biquadris;

class AbsDisplay : public Observer {
protected:
  Biquadris *game;
  std::vector<Biquadris::Board> nextBlockBoards;
  std::vector<PlayerData> data;

  // not sure where to put this
  void clearNextBlockBoard() {
    for (auto &board : nextBlockBoards) {
      for (auto &row : board) {
        for (auto &elem : row) {
          elem = ' ';
        }
      }
    }
  }
  virtual void _displayInfo() = 0;
  virtual void _displayEdge() = 0;
  virtual void _displayBorder() = 0;
  virtual void _displayBoard() = 0;
  virtual void _displayNextBlock() = 0;
  virtual void _endingTasks() = 0;
  virtual void _displayHighscore() = 0;

  static const Biquadris::Board _nextBlockBoard;

public:
  AbsDisplay(Biquadris *game) // remove hardcoded values
      : game{game}, nextBlockBoards{_nextBlockBoard, _nextBlockBoard} {
    game->attach(this);
  }
  inline static const int MAXBLOCKSIZE = 4; // fits in 4x4 square (including I)
  inline static const int COLS = 11, ROWS = 15;
  void notify() {
    data = game->getPlayerData();
    _displayEdge();
    _displayHighscore();
    _displayBorder();
    _displayInfo();
    _displayBorder();
    _displayBoard();
#ifdef TEST
    std::cout << "boards drawn" << std::endl;
#endif
    _displayBorder();
#ifdef TEST
    std::cout << "boarder drawn" << std::endl;
#endif
    _displayNextBlock();
#ifdef TEST
    std::cout << "next blocks drawn" << std::endl;
#endif
    _displayEdge();
    _endingTasks();
  }
  virtual ~AbsDisplay(){};
};

#endif // DISPLAY_H_
