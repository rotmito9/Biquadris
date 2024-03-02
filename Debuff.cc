#include "Biquadris.h"
#include "GameState.h"

Debuff::Debuff(std::unique_ptr<Player> p) : p{std::move(p)} {}
Debuff::~Debuff() {}

std::unique_ptr<Player> Debuff::removeDebuff(Biquadris::Debuffs debuff) {
  if (this->debuffType() == debuff) {
    auto b = std::move(p);
    return b;
  } else {
    return p->removeDebuff(debuff);
  }
}

void Debuff::disableRandom(std::string file) {
  auto b = getBase();
  b->disableRandom(file);
  return;
}

void Debuff::enableRandom() {
  auto b = getBase();
  b->enableRandom();
  return;
}

PlayerState *Debuff::getBase() {
  if (p->debuffType() == Biquadris::NIL) {
    return dynamic_cast<PlayerState *>(p.get());
  } else {
    return p->getBase();
  }
}

void Debuff::moveCommand(Biquadris::Commands cmd) {
  p->moveCommand(cmd);
  return;
}

int Debuff::updateBoard() { return p->updateBoard(); }

bool Debuff::increaseLevel() { return p->increaseLevel(); }

bool Debuff::decreaseLevel() { return p->decreaseLevel(); }

void Debuff::replaceBlock(Biquadris::BlockType b) {
  p->replaceBlock(b);
  return;
}

int Debuff::getScore() { return p->getScore(); }

int Debuff::getLevel() { return p->getLevel(); }

std::pair<Biquadris::BlockType, int> Debuff::getNextBlock() const {
  return p->getNextBlock();
}

Biquadris::Board Debuff::getBoard() const { return p->getBoard(); }

Blind::Blind(std::unique_ptr<Player> p) : Debuff{std::move(p)} {}

Blind::~Blind() {}

Biquadris::Board Blind::getBoard() const {
  auto b = p->getBoard();

  // Draw '?' sprites on the specified area
  // For example, covering columns 3 to 9 and rows 3 to 12 with '?'
  for (int y = 3; y <= 12; ++y) {
    for (int x = 3; x <= 9; ++x) {
      b[x][y] = '?';
    }
  }

  // draw '?' sprites on the area specified
  return b;
}

Biquadris::Debuffs Blind::debuffType() const { return Biquadris::BLIND; }

Heavy::Heavy(std::unique_ptr<Player> p) : Debuff{std::move(p)} {}

Heavy::~Heavy() {}

void Heavy::moveCommand(Biquadris::Commands cmd) {
  p->moveCommand(cmd);
  auto b = this->getBase();

  b->CurrentBlock->move(Block::DOWN);

  return;
}

Biquadris::Debuffs Heavy::debuffType() const { return Biquadris::HEAVY; }
