#ifndef PLAYERSTATE_H_
#define PLAYERSTATE_H_
#include <iostream>
#include <sstream>
#include <utility>
#include <vector>

#include "Biquadris.h"
#include "Block.h"

class GameControl;
class PlayerState;
class Level0;
class Level3;
class Level4;
class Heavy;

// Abstract class for PlayerState
class Player {
public:
  virtual void moveCommand(Biquadris::Commands cmd) = 0;
  virtual int updateBoard() = 0;
  virtual bool increaseLevel() = 0;
  virtual bool decreaseLevel() = 0;

  virtual std::unique_ptr<Player> removeDebuff(Biquadris::Debuffs debuff);
  virtual Biquadris::Debuffs debuffType() const;
  virtual PlayerState *getBase();
  virtual void replaceBlock(Biquadris::BlockType block) = 0;

  virtual void disableRandom(std::string file) = 0;
  virtual void enableRandom() = 0;

  virtual int getScore() = 0;
  virtual int getLevel() = 0;
  virtual std::pair<Biquadris::BlockType, int> getNextBlock() const = 0;
  virtual Biquadris::Board getBoard() const = 0;

  Player() = default;
  virtual ~Player() = default;
};

class PlayerState : public Player {
  // TODO: unique_ptr
  const int player = 0;
  int level;
  int score = 0;
  Biquadris::Board board;
  std::vector<std::unique_ptr<Block>> LandedBlocks;
  std::unique_ptr<Block> CurrentBlock;
  bool random = true;
  std::pair<Biquadris::BlockType, int> NextBlock;
  std::unique_ptr<GameControl> game;

  static std::unique_ptr<GameControl> GenerateLevel(int level, PlayerState *ps,
                                                    std::string FileName = "");

  friend Level0;
  friend Level3;
  friend Level4;
  friend Heavy;

  Biquadris::Board _getBoard() const;
  void _moveCommand(Biquadris::Commands cmd);
  void registerNewBlock();

public:
  PlayerState(int player, int level, std::string FileName);
  virtual ~PlayerState();

  void moveCommand(Biquadris::Commands cmd) override;
  // returns number of rows cleared
  // throws when someone lost the game
  int updateBoard() override;
  bool increaseLevel() override;
  bool decreaseLevel() override;
  void replaceBlock(Biquadris::BlockType block) override;
  void disableRandom(std::string file) override;
  void enableRandom() override;

  int getScore() override;
  int getLevel() override;
  std::pair<Biquadris::BlockType, int> getNextBlock() const override;
  Biquadris::Board getBoard() const override;
};

static const Biquadris::Board _board = {
    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
     ' ', ' ', ' '},
    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
     ' ', ' ', ' '},
    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
     ' ', ' ', ' '},
    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
     ' ', ' ', ' '},
    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
     ' ', ' ', ' '},
    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
     ' ', ' ', ' '},
    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
     ' ', ' ', ' '},
    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
     ' ', ' ', ' '},
    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
     ' ', ' ', ' '},
    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
     ' ', ' ', ' '},
    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
     ' ', ' ', ' '}};

// TODO:
// * smart pointer
class Debuff : public Player {
protected:
  // owns
  std::unique_ptr<Player> p;

public:
  Debuff(std::unique_ptr<Player> p);
  virtual ~Debuff() = 0;

  void moveCommand(Biquadris::Commands cmd) override;
  int updateBoard() override;
  bool increaseLevel() override;
  bool decreaseLevel() override;
  void replaceBlock(Biquadris::BlockType b) override;

  std::unique_ptr<Player> removeDebuff(Biquadris::Debuffs debuff) override;
  void disableRandom(std::string file) override;
  void enableRandom() override;

  PlayerState *getBase() override;
  int getScore() override;
  int getLevel() override;
  std::pair<Biquadris::BlockType, int> getNextBlock() const override;
  Biquadris::Board getBoard() const override;
};

class Blind : public Debuff {
public:
  Blind(std::unique_ptr<Player> p);
  ~Blind();

  Biquadris::Board getBoard() const override;
  Biquadris::Debuffs debuffType() const override;
};

class Heavy : public Debuff {
public:
  Heavy(std::unique_ptr<Player> p);
  ~Heavy();

  void moveCommand(Biquadris::Commands) override;
  Biquadris::Debuffs debuffType() const override;
};

// Abstract class for Level
class GameControl {
protected:
  // these two may throw
  virtual void _moveBlock(Biquadris::Commands) = 0;
  virtual int
  _updateBoard(Biquadris::Board &board,
               std::vector<std::unique_ptr<Block>> &LandedBlocks) = 0;
  virtual std::pair<Biquadris::BlockType, int> _nextBlock() = 0;

public:
  void moveBlock(Biquadris::Commands cmd);
  // returns number of rows cleared
  // throws if someone lost
  int updateBoard(Biquadris::Board &board,
                  std::vector<std::unique_ptr<Block>> &LandedBlocks);

  void setCurrentBlock(Biquadris::BlockType);
  std::pair<Biquadris::BlockType, int> nextBlock();

  virtual ~GameControl();
};

// NoRanodm Decorator for Level3 and Level4
// TODO: smart pointer
class NoRandom : public GameControl {
  // owns
  std::unique_ptr<GameControl> g;

  std::string BlockFile;
  std::istringstream BlockStream;

protected:
  void _moveBlock(Biquadris::Commands) override;
  int _updateBoard(Biquadris::Board &board,
                   std::vector<std::unique_ptr<Block>> &LandedBlocks) override;
  std::pair<Biquadris::BlockType, int> _nextBlock() override;

public:
  // should ctor fail when file cannot be opened?
  NoRandom(std::unique_ptr<GameControl> g, std::string file);

  virtual ~NoRandom();
};

class Level0 : public GameControl {
  std::string BlockFile;
  std::istringstream BlockStream;

protected:
  // "has" so raw ptr
  // do we need this anymore?
  PlayerState *ps;

  void _moveBlock(Biquadris::Commands) override;
  int _updateBoard(Biquadris::Board &board,
                   std::vector<std::unique_ptr<Block>> &LandedBlocks) override;
  virtual std::pair<Biquadris::BlockType, int> _nextBlock() override;

public:
  Level0(PlayerState *ps, std::string file = "");

  void setBlockFile(std::string file);

  ~Level0() = default;
};

class Level1 : public Level0 {
protected:
  std::pair<Biquadris::BlockType, int> _nextBlock() override;

public:
  Level1(PlayerState *ps);
};

class Level2 : public Level0 {
protected:
  std::pair<Biquadris::BlockType, int> _nextBlock() override;

public:
  Level2(PlayerState *ps);
};

class Level3 : public Level0 {
protected:
  void _moveBlock(Biquadris::Commands) override;
  virtual std::pair<Biquadris::BlockType, int> _nextBlock() override;

public:
  Level3(PlayerState *ps);
};

class Level4 : public Level3 {
  int BlockCount = 0; // mod 5

protected:
  int _updateBoard(Biquadris::Board &board,
                   std::vector<std::unique_ptr<Block>> &LandedBlocks) override;

public:
  Level4(PlayerState *ps);
};

class Level5 : public Level4 {
protected:
  int _updateBoard(Biquadris::Board &board,
                   std::vector<std::unique_ptr<Block>> &LandedBlocks) override;

public:
  Level5(PlayerState *ps);
};

#endif // PLAYERSTATE_H_
