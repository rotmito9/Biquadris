#ifndef BLOCK_H_
#define BLOCK_H_
#include <utility>
#include <vector>

//#ifdef DBG
#include <iostream>
//#endif // DBG

#include "Biquadris.h"

class Block {
public:
  enum Movement { LEFT = 0, DOWN, RIGHT, CROTATE, CCROTATE };
  typedef std::pair<int, int> coordinate;
  typedef std::vector<coordinate> coordinates;

private:
  int player;
  int level;
  int lifespan;
  coordinates _coordinates;
  char sprite;
  const Biquadris::Board *board;

  void translate(Block::Movement m);
  void rotate(Block::Movement m);

  bool detectCollision();
  bool detectCollision(coordinates &s);

  // compute lower left corner

  coordinate lowerLeft(coordinates &coordinates);

  void crotate(coordinate &c);
  void crotate(coordinates &cs);
  void ccrotate(coordinate &cs);
  void ccrotate(coordinates &cs);

public:
  // throws if collision is detected while being created
  Block(const coordinate &base, const coordinates cs, char sprite,
        Biquadris::Board *board, int level, int player);

  // factory for block
  static std::unique_ptr<Block> block(const coordinate &base,
                                      Biquadris::Board *board, int level,
                                      int player, Biquadris::BlockType type);
  static int randomRotation();
  static Biquadris::BlockType stringToBlockType(std::string str);

  void move(Block::Movement m);
  virtual Biquadris::BlockType type() = 0;

  coordinate lowerLeft();
  int getLevel() const;
  coordinates getCoordinates() const;
  char getSprite() const;
  int &getLifespan();

  virtual ~Block();
};

Biquadris::Board &operator<<(Biquadris::Board &board, const Block *block);

// Some functions to help us with linear algebra
Block::coordinate operator-(const Block::coordinate &c);
Block::coordinate operator+(const Block::coordinate &c1,
                            const Block::coordinate &c2);
Block::coordinate &operator+=(Block::coordinate &c1,
                              const Block::coordinate &c2);
Block::coordinate operator-(const Block::coordinate &c1,
                            const Block::coordinate &c2);
Block::coordinates operator+(Block::coordinates cs, const Block::coordinate &c);
Block::coordinates &operator+=(Block::coordinates &cs,
                               const Block::coordinate &c);
Block::coordinates &operator-=(Block::coordinates &cs,
                               const Block::coordinate &c);

class IBlock : public Block {
public:
  const static coordinates points;

  IBlock(const coordinate &base, Biquadris::Board *board, int level,
         int player);
  Biquadris::BlockType type() override;
};

class JBlock : public Block {
public:
  const static coordinates points;

  JBlock(const coordinate &base, Biquadris::Board *board, int level,
         int player);
  Biquadris::BlockType type() override;
};

class LBlock : public Block {
public:
  const static coordinates points;

  LBlock(const coordinate &base, Biquadris::Board *board, int level,
         int player);
  Biquadris::BlockType type() override;
};

class OBlock : public Block {
public:
  const static coordinates points;

  OBlock(const coordinate &base, Biquadris::Board *board, int level,
         int player);
  Biquadris::BlockType type() override;
};

class SBlock : public Block {
public:
  const static coordinates points;

  SBlock(const coordinate &base, Biquadris::Board *board, int level,
         int player);
  Biquadris::BlockType type() override;
};

class ZBlock : public Block {
public:
  const static coordinates points;

  ZBlock(const coordinate &base, Biquadris::Board *board, int level,
         int player);
  Biquadris::BlockType type() override;
};

class TBlock : public Block {
public:
  const static coordinates points;

  TBlock(const coordinate &base, Biquadris::Board *board, int level,
         int player);
  Biquadris::BlockType type() override;
};

class DotBlock : public Block {
public:
  const static coordinates points;

  DotBlock(const coordinate &base, Biquadris::Board *board, int level,
           int player);
  Biquadris::BlockType type() override;
};

#endif // BLOCK_H_
