#include <iostream>

#include "AbsDisplay.h"
#include "Biquadris.h"
#include "Block.h"
#include "TextDisplay.h"

TextDisplay::TextDisplay(Biquadris *game) : AbsDisplay{game} {}

std::ostream &operator<<(std::ostream &out,
                         const std::vector<Biquadris::Board> &boards) {
  // TODO: Clean this up to not newline at the end
  int cols = boards[0].size();
  int rows = boards[0][0].size();
  for (int y = rows - 4; y >= 0; --y) {         // start 3 from top, is awkward
    for (long unsigned int i = 0; i < 2; ++i) { // hard coded value...
      out << "|";
      for (int x = 0; x < AbsDisplay::COLS; ++x) {
        if (x >= cols) {
          out << ' ';
        } else {
          out << boards[i][x][y];
        }
      }
      out << "|";
    }
    out << std::endl;
  }
  return out;
}

void TextDisplay::_displayEdge() {
  for (long unsigned int i = 0; i < data.size(); ++i) {
    out << "+" << repeatString("-", AbsDisplay::COLS) << "+";
  }
  out << std::endl;
}

void TextDisplay::_displayInfo() {
  for (long unsigned int i = 0; i < data.size(); ++i) {
    out << "|Player:" << repeatString(" ", AbsDisplay::COLS - 7 - i / 10 - 1)
        << i << "|";
  }
  out << std::endl;
  for (long unsigned int i = 0; i < data.size(); ++i) {
    out << "|Level:"
        << repeatString(" ", AbsDisplay::COLS - 6 - data[i].level / 10 - 1)
        << data[i].level << "|";
  }
  out << std::endl;
  for (long unsigned int i = 0; i < data.size(); ++i) {
    out << "|Score:"
        << repeatString(" ", AbsDisplay::COLS - 6 - data[i].score / 10 - 1)
        << data[i].score << "|";
  }
  out << std::endl;
}

void TextDisplay::_displayBorder() {
  for (long unsigned int i = 0; i < data.size(); ++i) {
    out << "|" << repeatString("-", AbsDisplay::COLS) << "|";
  }
  out << std::endl;
}

void TextDisplay::_displayBoard() {
  std::vector<Biquadris::Board> boards;

#ifdef TEST
  std::cout << "TextDisplay : _displayBoard" << std::endl;
#endif

  for (long unsigned int i = 0; i < data.size(); ++i) {
    boards.push_back(data[i].board);
  }
  out << boards;

#ifdef TEST
  std::cout << "TextDisplay : _displayBoard displayed" << std::endl;
#endif
}

void TextDisplay::_displayNextBlock() {
#ifdef TEST
  std::cout << "TextDisplay : _displayNextBlock" << std::endl;
#endif

  for (long unsigned int i = 0; i < data.size(); ++i) {
#ifdef TEST
    std::cout << i << std::endl;
#endif
    auto nextBlock = Block::block(Block::coordinate{0, 0}, &nextBlockBoards[i],
                                  data[i].level, i, data[i].NextBlock.first);
#ifdef TEST
    std::cout << "TextDisplay : _displayNextBlock : nextBlock ctored"
              << std::endl;
#endif

    for (int j = 0; j < data[i].NextBlock.second; ++j) {
      nextBlock->move(Block::CROTATE);
    }
#ifdef TEST
    std::cout << "TextDisplay : _displayNextBlock : nextBlock rotated"
              << std::endl;
#endif

    nextBlockBoards[i] << nextBlock.get();

#ifdef TEST
    std::cout << "TextDisplay : _displayNextBlock : nextBlock drawn on board"
              << std::endl;
#endif
  }
  for (long unsigned int i = 0; i < data.size(); ++i) {
    out << "|Next Block:" << repeatString(" ", TextDisplay::COLS - 11) << "|";
  }
  out << std::endl << nextBlockBoards;
#ifdef TEST
  std::cout << "TextDisplay : _displayNextBlock calling clear" << std::endl;
#endif

  AbsDisplay::clearNextBlockBoard();
#ifdef TEST
  std::cout << "TextDisplay : _displayNextBlock cleared" << std::endl;
#endif
}

void TextDisplay::_endingTasks() {}

void TextDisplay::_displayHighscore() {
  std::string score = "Highscore: " + std::to_string(game->getHighScore());
  int len = score.length();
  int spaces = (AbsDisplay::COLS*2 + 4 - len)/2;
  out << "|" << repeatString(" ", spaces) 
      << score 
      << repeatString(" ", AbsDisplay::COLS*2 + 2 - len - spaces) 
      << "|" << std::endl;
}

std::string repeatString(std::string s, int n) {
  std::string ans = "";
  for (int i = 0; i < n; ++i) {
    ans += s;
  }
  return ans;
}
