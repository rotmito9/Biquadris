#include <cstdlib>
#include <fstream>
#include <sstream>

#include "Biquadris.h"
#include "Block.h"
#include "GameState.h"

GameControl::~GameControl(){};

void GameControl::moveBlock(Biquadris::Commands cmd) {
  _moveBlock(cmd);
  return;
}

int GameControl::updateBoard(
    Biquadris::Board &board,
    std::vector<std::unique_ptr<Block>> &LandedBlocks) {
  return _updateBoard(board, LandedBlocks);
}

std::pair<Biquadris::BlockType, int> GameControl::nextBlock() {
  return _nextBlock();
}

// Level Factory
std::unique_ptr<GameControl>
PlayerState::GenerateLevel(int level, PlayerState *ps, std::string FileName) {
  switch (level) {
  case 0:
    return std::make_unique<Level0>(ps, FileName);
  case 1:
    return std::make_unique<Level1>(ps);
  case 2:
    return std::make_unique<Level2>(ps);
  case 3:
    return std::make_unique<Level3>(ps);
  case 4:
    return std::make_unique<Level4>(ps);
  case 5:
    return std::make_unique<Level5>(ps);
  }

  // should not be reached
  return nullptr;
}

NoRandom::NoRandom(std::unique_ptr<GameControl> g, std::string file)
    : g{std::move(g)}, BlockFile{file}, BlockStream{} {
  std::string l;
  std::ifstream f{BlockFile};
  std::getline(f, l);
  BlockStream = std::istringstream{l};
}

NoRandom::~NoRandom() {}

void NoRandom::_moveBlock(Biquadris::Commands cmd) { g->moveBlock(cmd); }

int NoRandom::_updateBoard(Biquadris::Board &board,
                           std::vector<std::unique_ptr<Block>> &LandedBlocks) {
  return g->updateBoard(board, LandedBlocks);
}

std::pair<Biquadris::BlockType, int> NoRandom::_nextBlock() {
  std::string b;

  if (BlockStream.eof()) {
    std::string l;
    std::ifstream f{BlockFile};
    std::getline(f, l);
    BlockStream = std::istringstream{l};
  }

  BlockStream >> b;

  auto block = Block::stringToBlockType(b);

  return {block, Block::randomRotation()};
}

// Level 0 : predetermined sequence of blocks
// TODO : plz refactor me

Level0::Level0(PlayerState *ps, std::string FileName)
    : BlockFile{FileName}, BlockStream{}, ps{ps} {
  std::string l;
  std::ifstream f{BlockFile};
  std::getline(f, l);
  BlockStream = std::istringstream{l};
}

// if we want to do error handling we can throw UNKNOWN_BLOCK here
std::pair<Biquadris::BlockType, int> Level0::_nextBlock() {
  std::string b;

  if (BlockStream.eof()) {
    std::string l;
    std::ifstream f{BlockFile};
    std::getline(f, l);
    BlockStream = std::istringstream{l};
  }

  BlockStream >> b;

  auto block = Block::stringToBlockType(b);

  return {block, Block::randomRotation()};
}

void Level0::_moveBlock(Biquadris::Commands cmd) {
  ps->CurrentBlock->move(static_cast<Block::Movement>(cmd));
}

// FIXME: this is just a sketch plz refactor it to helper functions etc
// compute score, delete score and register the next block as the current block.

int Level0::_updateBoard(Biquadris::Board &board,
                         std::vector<std::unique_ptr<Block>> &LandedBlocks) {
  int lines = 0;

  // TODO: don't hard code this
  bool full;
  std::vector<int> FullRows;
  for (int y = 0; y < 15; ++y) {
    full = true;
    for (int x = 0; x < 11; ++x) {
      if (board[x][y] == ' ') {
        full = false;
        break;
      }
    }
    if (full) {
      ++lines;
      FullRows.emplace_back(y);
    }
  }

  if (lines != 0) {
    ps->score += (lines + ps->level) * (lines + ps->level);
  }

  // N^3 is bad but there shouldn't be that many blocks and rows methinks
  // we could use suffix tree but that feels like an overkill for now?
  for (auto b = LandedBlocks.begin(); b != LandedBlocks.end();) {
    int deletedSprites = 0;
    auto cs = (*b)->getCoordinates();
    int s = cs.size();
    for (const auto &c : cs) {
      for (const auto &r : FullRows) {
        if (c.second == r) {
          ++deletedSprites;
        }
      }
    }

    if (deletedSprites > 0) {
      if (deletedSprites == s) {
        ps->score += ((*b)->getLevel() + 1) * ((*b)->getLevel() + 1);
      }
      b = LandedBlocks.erase(
          b); // isnt there something where we have to refresh the iterator

    } else {
      ++b;
    }
  }

  // Again not my proudest moment but it should be rare to clear
  // more than two rows imo

  int deleted = 0;

  for (auto r : FullRows) {
    for (int i = r - deleted; i < 15 - deleted; ++i) {
      for (int x = 0; x < 11; ++x) {
        board[x][i] = board[x][i + 1];
      }
    }

    // XXX: I am cheap and dirty
    // I think it makes more sense to do it this way then
    // drawing registered blocks manually all the time
    for (auto &b : LandedBlocks) {
      try {
        b->move(Block::DOWN);
      } catch (BlockLanded &e) {
#ifdef TEST
        std::cout << "Hack :p" << std::endl;
#endif /* TEST */
      }
    }

    ++deleted;
  }

  return lines;
}

// Level 1
// rng biased to friendlier blocks

Level1::Level1(PlayerState *ps) : Level0{ps} {}

std::pair<Biquadris::BlockType, int> Level1::_nextBlock() {
  auto r = Block::randomRotation();
  switch (rand() % 12) {
  case 0:
    return {Biquadris::S, r};
  case 1:
    return {Biquadris::Z, r};
  default:
    int a = rand() % 6;
    return {static_cast<Biquadris::BlockType>(std::rand() % (a + 2)), r};
  }
}

// Level 2
// unbiased rng

Level2::Level2(PlayerState *ps) : Level0{ps} {}

std::pair<Biquadris::BlockType, int> Level2::_nextBlock() {
  return {static_cast<Biquadris::BlockType>(
              std::rand() % (static_cast<int>(Biquadris::BlockType::T) + 1)),
          Block::randomRotation()};
}

// TODO:
// Level 3
// Less friendly rng
// Blocks are heavy so issue down command at the end directly

Level3::Level3(PlayerState *ps) : Level0{ps} {}

void Level3::_moveBlock(Biquadris::Commands cmd) {
  ps->CurrentBlock->move(static_cast<Block::Movement>(cmd));
  ps->CurrentBlock->move(Block::DOWN);

  return;
}

std::pair<Biquadris::BlockType, int> Level3::_nextBlock() {
  auto r = Block::randomRotation();
  switch (rand() % 9) {
  case 0:
  case 1:
    return {Biquadris::S, r};
  case 2:
  case 3:
    return {Biquadris::Z, r};
  default:
    int a = rand() % 6;
    return {static_cast<Biquadris::BlockType>(std::rand() % (a + 2)), r};
  }
}

// Level 4
// same rng as lvl3
// Punish player when they fail to clear row.

Level4::Level4(PlayerState *ps) : Level3{ps}, BlockCount{0} {}

int Level4::_updateBoard(Biquadris::Board &board,
                         std::vector<std::unique_ptr<Block>> &LandedBlocks) {
  BlockCount = (BlockCount + 1) % 5;

  int r = Level0::_updateBoard(board, LandedBlocks);

  if (r == 0 && BlockCount == 0) {
    auto b =
        Block::block({5, 14}, &board, ps->level, ps->player, Biquadris::Dot);
    try {
      while (true) {
        b->move(Block::DOWN);
      }
    } catch (BlockLanded &e) {
      board << b.get();
      r = Level0::_updateBoard(board, LandedBlocks);
    }
  }

  return r;
}

// Level 5
// same rng as level 3
// same penalty block mechanics of level 4
// Adds the clearing blocks if not gone for 10 blocks
Level5::Level5(PlayerState *ps) : Level4{ps} {}

int Level5::_updateBoard(Biquadris::Board &board,
                         std::vector<std::unique_ptr<Block>> &LandedBlocks) {
  int r = Level4::_updateBoard(board, LandedBlocks);

  for (auto it = LandedBlocks.begin(); it != LandedBlocks.end();) {
    int &lifespan = (*it)->getLifespan();
    if (lifespan == 0) {
      for (auto &c : (*it)->getCoordinates()) {
        board[c.first][c.second] = ' ';
      }
      it = LandedBlocks.erase(it);
    } else {
      if (lifespan > 0) {
        --lifespan;
      }
      ++it;
    }
  }

  return r;
}
