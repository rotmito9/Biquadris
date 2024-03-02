#include <iostream>
#include <string>

#include "Biquadris.h"
#include "GameState.h"
#include "Prompt.h"
#include "TextDisplay.h"
#include "X11Display.h"
#include "window.h"

using namespace std;

// global flag that controls block rng
int RandomizeRotation = 1;

// TODO parse arguments
int main(int argc, char *argv[]) {
  std::string f1 = "sequence1.txt", f2 = "sequence2.txt";
  int level = 0;
  bool TextOnly = false;

  for (int i = 1; i < argc; ++i) {
    std::string cmd = argv[i];

    if (cmd == "-scriptfile1") {
      f1 = argv[i + 1];
      ++i;
    } else if (cmd == "-scriptfile2") {
      f2 = argv[i + 1];
      ++i;
    } else if (cmd == "-seed") {
      srand(stoi(argv[i + 1]));
      ++i;
    } else if (cmd == "-startlevel") {
      level = stoi(argv[i + 1]);
      ++i;
    } else if (cmd == "-text") {
      TextOnly = true;
    } else if (cmd == "-fixedrotation") {
      RandomizeRotation = 0;
    } else {
      std::cerr << "Invalid Command Line Argument " << cmd << std::endl;
    }
  }

  auto game = Biquadris{{f1, f2}, level};

  auto text = std::make_unique<TextDisplay>(&game);

  std::unique_ptr<X11Display> x11;

  if (!TextOnly) {
    x11 = std::make_unique<X11Display>(&game);
  }

  try {
    game.startGame();
  } catch (Off &e) {
    std::cout << std::endl << "Thank you for playing :D" << std::endl;
  }

  return 0;
}
