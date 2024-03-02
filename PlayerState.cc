#include "Biquadris.h"
#include "GameState.h"

// XXX : this should not be reached
std::unique_ptr<Player> Player::removeDebuff(Biquadris::Debuffs debuff) {
  return nullptr;
}

Biquadris::Debuffs Player::debuffType() const { return Biquadris::NIL; }

PlayerState *Player::getBase() { return nullptr; }

// TODO: plz don't hardcode coordinates
PlayerState::PlayerState(int player, int level, std::string FileName)
    : player{player}, level{level}, score{0}, board{_board}, LandedBlocks{},
      CurrentBlock{nullptr}, random{true}, NextBlock{}, game{nullptr} {
  game = PlayerState::GenerateLevel(level, this, FileName);
  CurrentBlock =
      Block::block({0, 14}, &board, level, player, game->nextBlock().first);
  NextBlock = game->nextBlock();
}

// TODO: unique ptr so this should be irrelevant
PlayerState::~PlayerState() {}

void PlayerState::_moveCommand(Biquadris::Commands cmd) {
  game->moveBlock(cmd);
  return;
}

void PlayerState::moveCommand(Biquadris::Commands cmd) {
  _moveCommand(cmd);
  return;
}

void PlayerState::registerNewBlock() {
  // register new block
  // and rotate it
  CurrentBlock = Block::block({0, 14}, &board, level, player, NextBlock.first);
  for (int i = 0; i < NextBlock.second; ++i) {
    CurrentBlock->move(Block::CROTATE);
  }
  NextBlock = game->nextBlock();
}

int PlayerState::updateBoard() {
  board << CurrentBlock.get();
  LandedBlocks.emplace_back(std::move(CurrentBlock));
  auto r = game->updateBoard(board, LandedBlocks);
  registerNewBlock();
  return r;
}

// TODO : should we throw MAXLVL, LVL0 exceptions?
bool PlayerState::increaseLevel() {
  if (level == 5) {
    return false;
  }

  game = PlayerState::GenerateLevel(++level, this);
  return true;
}

bool PlayerState::decreaseLevel() {
  if (level == 0) {
    return false;
  }

  game = PlayerState::GenerateLevel(--level, this);
  return true;
}

// TODO:
// * smart pointer
// * randomize rotation ?
void PlayerState::replaceBlock(Biquadris::BlockType block) {
  auto ll = CurrentBlock->lowerLeft();

  CurrentBlock = Block::block(ll, &board, level, player, block);

  return;
}

void PlayerState::disableRandom(std::string file) {
  if (level < 3) {
    return;
  }

  random = false;
  game = std::unique_ptr<GameControl>(new NoRandom(std::move(game), file));
  NextBlock = game->nextBlock();
}

void PlayerState::enableRandom() {
  if (level < 3 || random) {
    return;
  }

  game = PlayerState::GenerateLevel(level, this);
  NextBlock = game->nextBlock();
}

int PlayerState::getScore() { return score; }

int PlayerState::getLevel() { return level; }

std::pair<Biquadris::BlockType, int> PlayerState::getNextBlock() const {
  return NextBlock;
}

Biquadris::Board PlayerState::_getBoard() const {
  Biquadris::Board tmp{board};
  tmp << CurrentBlock.get();
  return tmp;
}

Biquadris::Board PlayerState::getBoard() const { return _getBoard(); }
