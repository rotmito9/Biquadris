#include "Biquadris.h"
#include "GameState.h"
#include "Prompt.h"
#include <memory>

Biquadris::Biquadris(std::vector<std::string> files, int level)
    : players{}, CommandPrompts{}, files{files},
      CurrentPlayer{0}, HighScore{0}, winner{-1}, defaultLevel{level} {
  for (int i = 0; i < 2; ++i) {
    players.emplace_back(new PlayerState{i, defaultLevel, files[i]});
    CommandPrompts.emplace_back(new Prompt{i});
  }
}

// entry point for the game.
// if a player loses, we give them a chance to play again.
void Biquadris::startGame() {
  while (true) {
    try {
      runGame();
    } catch (GameOver &e) {
#ifdef TEST
      std::cout << "EXCEPTION : GAME OVER" << std::endl;
#endif
      winner = 1 - e.loser;
      if (CommandPrompts[e.loser]->gameOver()) {
        restartGame();
      } else {
        return;
      }
    } catch (Restart &e) {
      restartGame();
    }
  }
}

// Restart game for the current player
void Biquadris::restart() {
  players[CurrentPlayer] = std::unique_ptr<PlayerState>(
      new PlayerState{CurrentPlayer, defaultLevel, files[CurrentPlayer]});
  CommandPrompts[CurrentPlayer] =
      std::unique_ptr<Prompt>(new Prompt{CurrentPlayer});

  return;
}

// Restart game for the both player
void Biquadris::restartGame() {
  players.clear();
  CommandPrompts.clear();

  for (int i = 0; i < 2; ++i) {
    players.emplace_back(new PlayerState{i, defaultLevel, files[i]});
    CommandPrompts.emplace_back(new Prompt{i});
  }

  return;
}

void Biquadris::runGame() {
  std::string f;
  Biquadris::BlockType b;
  while (true) {
#ifdef TEST
    std::cout << "!!" << std::endl;
#endif
    notifyObservers();
#ifdef TEST
    std::cout << "!!" << std::endl;
#endif
    auto cmd = CommandPrompts[CurrentPlayer]->getCommand();
    switch (cmd) {
    case CROTATE:
    case CCROTATE:
      players[CurrentPlayer]->moveCommand(cmd);
      break;
    case LEFT:
    case RIGHT:
    case DOWN:
      try {
        players[CurrentPlayer]->moveCommand(cmd);
      } catch (BlockLanded &e) {
        CommandPrompts[CurrentPlayer]->flushBuffer();
        blockLanded();
      }
      break;
    case LVLUP:
      players[CurrentPlayer]->increaseLevel();
      break;
    case LVLDOWN:
      players[CurrentPlayer]->decreaseLevel();
      break;
    case NORANDOM:
      f = CommandPrompts[CurrentPlayer]->getString("Block file");
      players[CurrentPlayer]->disableRandom(f);
      break;
    case RANDOM:
      players[CurrentPlayer]->enableRandom();
      break;
    case SKIP:
      CurrentPlayer = (CurrentPlayer + 1) % 2;
      break;
#ifdef TEST
    case DEBUFF:
      debuff();
      break;
    case DBG:;
      break;
#endif /* TEST */
    case BLOCK:
      b = CommandPrompts[CurrentPlayer]->getBlock();
      players[CurrentPlayer]->replaceBlock(b);
      break;
    case SEQUENCE:
      f = CommandPrompts[CurrentPlayer]->getString("Command file");
      CommandPrompts[CurrentPlayer] =
          std::make_unique<Prompt>(CurrentPlayer, f);
      break;
    case RESTART:
      restart();
      break;
    }
  }

  return;
}

// invoked when a player ends their turn
void Biquadris::blockLanded() {
  int rows = players[CurrentPlayer]->updateBoard();

  auto score = players[CurrentPlayer]->getScore();

  if (score > HighScore) {
    HighScore = score;
  }

  // Blind should always be the first debuff
  if (players[CurrentPlayer]->debuffType() == BLIND) {
    players[CurrentPlayer] = players[CurrentPlayer]->removeDebuff(BLIND);
  }

  if (rows >= 2) {
    debuff();
  }

  CurrentPlayer = (CurrentPlayer + 1) % 2;

  return;
}

// TODO apply debuffs
void Biquadris::debuff() {
  auto d = CommandPrompts[CurrentPlayer]->getDebuff();
  std::unique_ptr<Player> p(nullptr);
  int opponent = (CurrentPlayer + 1) % 2;

  switch (d) {
  case HEAVY:
    players[opponent] =
        std::unique_ptr<Player>(new Heavy(std::move(players[opponent])));
    break;
  case BLIND:
    players[opponent] =
        std::unique_ptr<Player>(new Blind(std::move(players[opponent])));
    break;
  case NIL:
#ifdef TEST
    std::cout << "NIL" << std::endl;
#else
      ;
#endif /* TEST */
    break;
  case FORCE:
    auto b = CommandPrompts[CurrentPlayer]->getBlock();
    players[opponent]->replaceBlock(b);
    break;
  }
  return;
}

int Biquadris::getHighScore() const { return HighScore; }

int Biquadris::getWinner() const { return winner; }

std::vector<PlayerData> Biquadris::getPlayerData() const {
  return {{players[0]->getScore(), players[0]->getLevel(),
           players[0]->getNextBlock(), players[0]->getBoard()},
          {players[1]->getScore(), players[1]->getLevel(),
           players[1]->getNextBlock(), players[1]->getBoard()}};
}
