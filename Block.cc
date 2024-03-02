#include "Block.h"
#include "Biquadris.h"
#include <stdexcept>

// ctor for the blocks
// throws if block cannot be placed on the board
// the lifespan field should not be modified under level 5
Block::Block(const coordinate &base, const coordinates cs, char sprite,
             Biquadris::Board *board, int level, int player)
    : player{player}, level{level}, lifespan{10},
      _coordinates{cs + base}, sprite{sprite}, board{board} {
  if ((rand() % 100) > 20) {
    lifespan = -1;
  } else {
#ifdef TEST
    std::cout << "Ephemeral block!" << std::endl;
#endif /* TEST */
    if (level == 5) {
      this->sprite = '!';
    }
  }
  if (detectCollision()) {
    throw GameOver();
  }
}

// dtor for the blocks
Block::~Block() {}

// block factory
std::unique_ptr<Block> Block::block(const coordinate &base,
                                    Biquadris::Board *board, int level,
                                    int player, Biquadris::BlockType type) {
  switch (type) {
  case Biquadris::I:
    return std::make_unique<IBlock>(base, board, level, player);
  case Biquadris::J:
    return std::make_unique<JBlock>(base, board, level, player);
  case Biquadris::L:
    return std::make_unique<LBlock>(base, board, level, player);
  case Biquadris::O:
    return std::make_unique<OBlock>(base, board, level, player);
  case Biquadris::S:
    return std::make_unique<SBlock>(base, board, level, player);
  case Biquadris::Z:
    return std::make_unique<ZBlock>(base, board, level, player);
  case Biquadris::T:
    return std::make_unique<TBlock>(base, board, level, player);
  case Biquadris::Dot:
    return std::make_unique<DotBlock>(base, board, level, player);
  }

  // should not be reached
  return {nullptr};
}

// defined in main
extern int RandomizeRotation;

// randomize block orientation
int Block::randomRotation() { return (rand() * RandomizeRotation) % 4; }

// convert string to BlockType
Biquadris::BlockType Block::stringToBlockType(std::string str) {
  Biquadris::BlockType block;
  if (str == "I") {
    block = Biquadris::I;
  } else if (str == "J") {
    block = Biquadris::J;
  } else if (str == "L") {
    block = Biquadris::L;
  } else if (str == "O") {
    block = Biquadris::O;
  } else if (str == "S") {
    block = Biquadris::S;
  } else if (str == "Z") {
    block = Biquadris::Z;
  } else {
    block = Biquadris::T;
  }

  return block;
}

// handles translation and rotation of block
void Block::move(Block::Movement m) {
  switch (m) {
  case Block::LEFT:
  case Block::DOWN:
  case Block::RIGHT:
    translate(m);
    break;
  case Block::CROTATE:
  case Block::CCROTATE:
    rotate(m);
    break;
  }
  return;
}

// Perform translation of block
// throws if block has been landed on the board
void Block::translate(Block::Movement m) {
  coordinates tmp = _coordinates;

  switch (m) {
  case Movement::LEFT:
    tmp += {-1, 0};
    break;
  case Movement::RIGHT:
    tmp += {1, 0};
    break;
  case Movement::DOWN:
    tmp += {0, -1};
    break;
  // should not be reached
  case Movement::CROTATE:
  case Movement::CCROTATE:
#ifdef TEST
    std::cout << "ERROR: translate should not handle rotation!!!" << std::endl;
#endif /* TEST */
    break;
  }

  // ignores command and make player waste turn for making invalid move
  // when the block is landed on the board, throws so board gets updated.
  if (detectCollision(tmp)) {
    if (m == Movement::DOWN) {
      throw BlockLanded();
    }
    return;
  }

  std::swap(tmp, this->_coordinates);

  return;
}

// rotate the block, preserving the lower left coordinates
void Block::rotate(Block::Movement m) {
  coordinate ll1 = lowerLeft();
  coordinates cs = _coordinates;

  if (this->type() == Biquadris::O)
    return;

  cs -= ll1;

  if (m == Movement::CROTATE) {
    crotate(cs);
  } else if (m == Movement::CCROTATE) {
    ccrotate(cs);
  }

  coordinate ll2 = lowerLeft(cs);

  cs += ll1 - ll2;

  if (!detectCollision(cs)) {
    std::swap(_coordinates, cs);
  }

  return;
}

Block::coordinate Block::lowerLeft() { return lowerLeft(this->_coordinates); }

Block::coordinate Block::lowerLeft(coordinates &cs) {
  coordinate m = cs[0];

  for (auto &p : cs) {
    if (m.first > p.first) {
      m.first = p.first;
    }
    if (m.second > p.second) {
      m.second = p.second;
    }
  }

  return m;
}

bool Block::detectCollision() { return detectCollision(this->_coordinates); }

bool Block::detectCollision(coordinates &s) {
  for (auto &c : s) {
    try {
      if (board->at(c.first).at(c.second) != ' ') {
        return true;
      }

    } catch (std::out_of_range &e) {
      return true;
    }
  }

  return false;
}

void Block::crotate(coordinate &c) {
  std::swap(c.first, c.second);
  c.first *= -1;

  return;
}

void Block::crotate(coordinates &cs) {
  for (auto &c : cs) {
    crotate(c);
  }

  return;
}

void Block::ccrotate(coordinate &c) {
  std::swap(c.first, c.second);
  c.second *= -1;

  return;
}

void Block::ccrotate(coordinates &cs) {
  for (auto &c : cs) {
    ccrotate(c);
  }

  return;
}

Block::coordinate operator-(const Block::coordinate &c) {
  return {-c.first, -c.second};
}

Block::coordinate operator+(const Block::coordinate &c1,
                            const Block::coordinate &c2) {
  return {c1.first + c2.first, c1.second + c2.second};
}

Block::coordinate &operator+=(Block::coordinate &c1,
                              const Block::coordinate &c2) {
  c1 = c1 + c2;
  return c1;
}

Block::coordinate operator-(const Block::coordinate &c1,
                            const Block::coordinate &c2) {
  return {c1.first - c2.first, c1.second - c2.second};
}

Block::coordinates operator+(Block::coordinates cs,
                             const Block::coordinate &c) {
  for (auto &e : cs) {
    e += c;
  }

  return cs;
}

Block::coordinates &operator+=(Block::coordinates &cs,
                               const Block::coordinate &c) {
  cs = cs + c;
  return cs;
}

Block::coordinates &operator-=(Block::coordinates &cs,
                               const Block::coordinate &c) {
  cs = cs + -c;
  return cs;
}

Biquadris::Board &operator<<(Biquadris::Board &board, const Block *block) {
  for (auto &s : block->getCoordinates()) {
    board[s.first][s.second] = block->getSprite();
  }

  return board;
}

// getters
int Block::getLevel() const { return level; }

int &Block::getLifespan() { return lifespan; }

Block::coordinates Block::getCoordinates() const { return _coordinates; }

char Block::getSprite() const { return sprite; }

// I block
const Block::coordinates IBlock::points = {{0, 0}, {0, 1}, {0, 2}, {0, 3}};

IBlock::IBlock(const coordinate &base, Biquadris::Board *board, int level,
               int player)
    : Block{base, points, 'I', board, level, player} {}

Biquadris::BlockType IBlock::type() { return Biquadris::I; }

// J block
const Block::coordinates JBlock::points = {{0, 0}, {1, 0}, {1, 1}, {1, 2}};

JBlock::JBlock(const coordinate &base, Biquadris::Board *board, int level,
               int player)
    : Block{base, points, 'J', board, level, player} {}

Biquadris::BlockType JBlock::type() { return Biquadris::J; }

// L block
const Block::coordinates LBlock::points = {{0, 0}, {0, 1}, {0, 2}, {1, 0}};

LBlock::LBlock(const coordinate &base, Biquadris::Board *board, int level,
               int player)
    : Block{base, points, 'L', board, level, player} {}

Biquadris::BlockType LBlock::type() { return Biquadris::L; }

// O block
const Block::coordinates OBlock::points = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};

OBlock::OBlock(const coordinate &base, Biquadris::Board *board, int level,
               int player)
    : Block{base, points, 'O', board, level, player} {}

Biquadris::BlockType OBlock::type() { return Biquadris::O; }

// S block
const Block::coordinates SBlock::points = {{0, 0}, {1, 0}, {1, 1}, {2, 1}};

SBlock::SBlock(const coordinate &base, Biquadris::Board *board, int level,
               int player)
    : Block{base, points, 'S', board, level, player} {}

Biquadris::BlockType SBlock::type() { return Biquadris::S; }

// Z block
const Block::coordinates ZBlock::points = {{0, 1}, {1, 1}, {1, 0}, {2, 0}};

ZBlock::ZBlock(const coordinate &base, Biquadris::Board *board, int level,
               int player)
    : Block{base, points, 'Z', board, level, player} {}

Biquadris::BlockType ZBlock::type() { return Biquadris::Z; }

// T block
const Block::coordinates TBlock::points = {{0, 1}, {1, 1}, {1, 0}, {2, 1}};

TBlock::TBlock(const coordinate &base, Biquadris::Board *board, int level,
               int player)
    : Block{base, points, 'T', board, level, player} {}

Biquadris::BlockType TBlock::type() { return Biquadris::T; }

// Dot block (for level 4)
const Block::coordinates DotBlock::points = {{0, 0}};

DotBlock::DotBlock(const coordinate &base, Biquadris::Board *board, int level,
                   int player)
    : Block{base, points, '*', board, level, player} {}

Biquadris::BlockType DotBlock::type() { return Biquadris::Dot; }
