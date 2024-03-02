#ifndef X11_DISPLAY_H_
#define X11_DISPLAY_H_
#include "AbsDisplay.h"
#include "Biquadris.h"
#include "Block.h"
#include "observer.h"
#include "window.h"

class Biquadris;

class X11Display : public AbsDisplay {
  std::unique_ptr<Xwindow> window;
  std::pair<int, int> curCoord;

  void printBoard(const std::vector<Biquadris::Board> &boards);
  int getColor(const char c);
  void drawCell(int color);

  void _displayInfo() override;
  void _displayEdge() override;
  void _displayBorder() override;
  void _displayBoard() override;
  void _displayNextBlock() override;
  void _endingTasks() override;
  void _displayHighscore() override;

public:
  inline static int PIXELS = 20;
  X11Display(Biquadris *game);
  ~X11Display() = default;
};

#endif // X11_DISPLAY_H_
