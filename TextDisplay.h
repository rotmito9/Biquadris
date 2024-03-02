#ifndef TEXTDISPLAY_H_
#define TEXTDISPLAY_H_
#include <iostream>

#include "AbsDisplay.h"
#include "Biquadris.h"
#include "Block.h"

class Biquadris;

class TextDisplay : public AbsDisplay {
  std::ostream &out = std::cout;

  void _displayInfo() override;
  void _displayEdge() override;
  void _displayBorder() override;
  void _displayBoard() override;
  void _displayNextBlock() override;
  void _endingTasks() override;
  void _displayHighscore() override;

public:
  TextDisplay(Biquadris *game);

  friend std::ostream &operator<<(std::ostream &out,
                                  const std::vector<Biquadris::Board> &boards);
};
inline std::string repeatString(std::string s, int n);

#endif // TEXT_DISPLAY_H_
