#include "X11Display.h"
#include "window.h"

X11Display::X11Display(Biquadris *game)
    : AbsDisplay{game}, window{new Xwindow(540, 580)}, curCoord{
                                                           std::pair<int, int>(
                                                               0, 0)} {}

void X11Display::printBoard(
    const std::vector<Biquadris::Board>
        &boards) { // TODO: Clean this up to not newline at the end
  int cols = boards[0].size();
  int rows = boards[0][0].size();
  for (int y = rows - 4; y >= 0; --y) { // start 3 from top, is awkward
    curCoord.first += X11Display::PIXELS;
    for (long unsigned int i = 0; i < 2; ++i) { // hard coded value...
      for (int x = 0; x < AbsDisplay::COLS; ++x) {
        if (x >= cols) {
          window->fillRectangle(curCoord.first, curCoord.second,
                                X11Display::PIXELS, X11Display::PIXELS,
                                getColor(' '));
        } else {
          char sq = boards[i][x][y];
          if (sq == ' ') {
            window->fillRectangle(curCoord.first, curCoord.second,
                                  X11Display::PIXELS, X11Display::PIXELS,
                                  getColor(sq));
          } else {
            drawCell(getColor(sq));
          }
        }
        curCoord.first += X11Display::PIXELS;
      }
      curCoord.first += 3 * X11Display::PIXELS;
    }
    curCoord.first = 0;
    curCoord.second += X11Display::PIXELS;
  }
}

void X11Display::drawCell(int color) {
  window->fillRectangle(curCoord.first, curCoord.second, X11Display::PIXELS,
                        X11Display::PIXELS, color);
  window->fillRectangle(curCoord.first, curCoord.second, 1, X11Display::PIXELS,
                        Xwindow::Black);
  window->fillRectangle(curCoord.first, curCoord.second, X11Display::PIXELS, 1,
                        Xwindow::Black);
  window->fillRectangle(curCoord.first + X11Display::PIXELS - 1,
                        curCoord.second, 1, X11Display::PIXELS, Xwindow::Black);
  window->fillRectangle(curCoord.first,
                        curCoord.second + X11Display::PIXELS - 1,
                        X11Display::PIXELS, 1, Xwindow::Black);
}

void X11Display::_displayEdge() {
  for (long unsigned int i = 0; i < data.size(); ++i) {
    window->fillRectangle(
        curCoord.first, curCoord.second + X11Display::PIXELS / 2 - 1,
        (AbsDisplay::COLS + 2) * X11Display::PIXELS, X11Display::PIXELS / 5);
    curCoord.first += (AbsDisplay::COLS + 3) * X11Display::PIXELS;
  }
  curCoord.first = 0;
  curCoord.second += X11Display::PIXELS;
}

// TODO:
// think about further abstraction
void X11Display::_displayInfo() {
  /*
  int startx = curCoord.first;
  // draw borders
  for(long unsigned int i = 0; i < data.size(); ++i) {
      window->fillRectangle(curCoord.first, curCoord.second, X11Display::PIXELS,
  3*X11Display::PIXELS); curCoord.first +=
  (AbsDisplay::COLS+1)*X11Display::PIXELS; window->fillRectangle(curCoord.first,
  curCoord.second, X11Display::PIXELS, 3*X11Display::PIXELS); curCoord.first +=
  2*X11Display::PIXELS;
  }
  curCoord.first = startx;
  */
  curCoord.second += X11Display::PIXELS; // dont know if I want to keep this
  // display information
  window->fillRectangle(curCoord.first, curCoord.second,
                        AbsDisplay::COLS * X11Display::PIXELS,
                        3 * X11Display::PIXELS, Xwindow::White);
  curCoord.first += X11Display::PIXELS;
  for (long unsigned int i = 0; i < data.size(); ++i) {
    window->drawString(curCoord.first, curCoord.second,
                       "Player: " + std::to_string(i));
    curCoord.first += (AbsDisplay::COLS + 3) * X11Display::PIXELS;
  }
  curCoord.first =
      X11Display::PIXELS; // theres some abstraction possible here....
  curCoord.second += X11Display::PIXELS;
  for (long unsigned int i = 0; i < data.size(); ++i) {
    window->drawString(curCoord.first, curCoord.second,
                       "Level: " + std::to_string(data[i].level));
    curCoord.first += (AbsDisplay::COLS + 3) * X11Display::PIXELS;
  }
  curCoord.first = X11Display::PIXELS;
  curCoord.second += X11Display::PIXELS;
  for (long unsigned int i = 0; i < data.size(); ++i) {
    window->drawString(curCoord.first, curCoord.second,
                       "Score: " + std::to_string(data[i].score));
    curCoord.first += (AbsDisplay::COLS + 3) * X11Display::PIXELS;
  }
  curCoord.first = 0;
}

void X11Display::_displayBorder() {
  for (long unsigned int i = 0; i < data.size(); ++i) {
    window->fillRectangle(
        curCoord.first, curCoord.second + X11Display::PIXELS / 2 - 1,
        (AbsDisplay::COLS + 2) * X11Display::PIXELS, X11Display::PIXELS / 5);
    curCoord.first += (AbsDisplay::COLS + 3) * X11Display::PIXELS;
  }
  curCoord.first = 0;
  curCoord.second += X11Display::PIXELS;
}

void X11Display::_displayBoard() {
  std::vector<Biquadris::Board> boards;
  for (long unsigned int i = 0; i < data.size(); ++i) {
    boards.push_back(data[i].board);
  }
  printBoard(boards);
}

void X11Display::_displayNextBlock() {
  for (long unsigned int i = 0; i < data.size(); ++i) {
    auto nextBlock = Block::block(Block::coordinate{0, 0}, &nextBlockBoards[i],
                                  data[i].level, i, data[i].NextBlock.first);
    for (int j = 0; j < data[i].NextBlock.second; ++j) {
      nextBlock->move(Block::CROTATE);
    }
    nextBlockBoards[i] << nextBlock.get();
  }
  for (long unsigned int i = 0; i < data.size(); ++i) {
    window->drawString(curCoord.first + X11Display::PIXELS,
                       curCoord.second + X11Display::PIXELS, "Next Block: ");
    curCoord.first += (AbsDisplay::COLS + 3) * X11Display::PIXELS;
  }
  curCoord.first = 0;
  curCoord.second += X11Display::PIXELS;
  printBoard(nextBlockBoards);
  AbsDisplay::clearNextBlockBoard();
}

void X11Display::_displayHighscore() {
  curCoord.first += 220; // hardcoded value... dont think there is solution
  curCoord.second += 20;
  window->drawString(curCoord.first, curCoord.second,
                     "Highscore: " + std::to_string(game->getHighScore()));
  curCoord.first = 0;
}

void X11Display::_endingTasks() {
  curCoord.first = 0;
  curCoord.second = 0;
}

int X11Display::getColor(const char c) {
  switch (c) {
  case ' ':
    return Xwindow::White;
    break;
  case 'I':
    return Xwindow::Yellow;
    break;
  case 'J':
    return Xwindow::Orange;
    break;
  case 'O':
    return Xwindow::Magenta;
    break;
  case 'L':
    return Xwindow::Cyan;
    break;
  case 'S':
    return Xwindow::Green;
    break;
  case 'Z':
    return Xwindow::Red;
    break;
  case 'T':
    return Xwindow::Blue;
    break;
  case '*':
    return Xwindow::Brown;
    break;
  case '?':
    return Xwindow::Black;
    break;
  }
  return Xwindow::White;
}
