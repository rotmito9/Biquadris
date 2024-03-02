#ifndef PROMPT_H_
#define PROMPT_H_
#include <deque>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "Biquadris.h"

class Prompt {
  // Adapted from a2q2
  // TODO:
  // * smart pointers
  // * Big five
  class TrieNode {
  public:
    static const int Alphabet = 26;
    static const int ValidLetters = Alphabet * 2;
    enum ResultCode { VALID = 0, UNKNOWN, AMBIGUOUS };
    typedef std::pair<ResultCode, std::string> result;

  private:
    int WordCount;
    bool isWord;
    std::string letter;
    std::unique_ptr<TrieNode> letters[ValidLetters];

    int idx(char c) const;
    std::string letterAt(int i) const;
    void updateWordCount();
    std::string trailingLetters() const;

  public:
    TrieNode(std::string l);
    TrieNode(std::vector<std::string> dictionary);

    void registerWord(const std::string &word);
    result autoComplete(const std::string &word) const;

    ~TrieNode();
  };

  TrieNode CommandDictionary;
  TrieNode DebuffDictionary;

  int player;

  std::deque<Biquadris::Commands> buffer;

  std::map<std::string, std::deque<Biquadris::Commands>> commands;
  std::map<std::string, Biquadris::Debuffs> debuffs;

  Biquadris::BlockType b = Biquadris::Dot;

  std::ifstream a;
  std::istream &in = a;
  std::ostream &out;

  void promptUser(std::string msg);

public:
  Prompt(int player);
  Prompt(int player, std::string file);
  Prompt(std::fstream replayfile);

  // should we do some error handling and throw?
  void setCommandStream(std::string file);

  Biquadris::Commands getCommand();
  Biquadris::Debuffs getDebuff();
  Biquadris::BlockType getBlock();
  std::string getString(std::string msg);
  bool gameOver();

  void flushBuffer();

  ~Prompt() = default;
};

static std::map<std::string, std::deque<Biquadris::Commands>> DefaultCommands =
    {{"left", {Biquadris::LEFT}},
     {"down", {Biquadris::DOWN}},
     {"right", {Biquadris::RIGHT}},
     {"clockwise", {Biquadris::CROTATE}},
     {"counterclockwise", {Biquadris::CCROTATE}},
     // XXX : not my proudest moment but just to show how macros are done xD
     {"drop",
      {Biquadris::DOWN, Biquadris::DOWN, Biquadris::DOWN, Biquadris::DOWN,
       Biquadris::DOWN, Biquadris::DOWN, Biquadris::DOWN, Biquadris::DOWN,
       Biquadris::DOWN, Biquadris::DOWN, Biquadris::DOWN, Biquadris::DOWN,
       Biquadris::DOWN, Biquadris::DOWN, Biquadris::DOWN}},
     {"levelup", {Biquadris::LVLUP}},
     {"leveldown", {Biquadris::LVLDOWN}},
     {"norandom", {Biquadris::NORANDOM}},
     {"random", {Biquadris::RANDOM}},
     {"sequence", {Biquadris::SEQUENCE}},
     {"skip", {Biquadris::SKIP}},
#ifdef TEST
     {"debug", {Biquadris::DBG}},
     {"debuff", {Biquadris::DEBUFF}},
#endif /* TEST */
     {"I", {Biquadris::BLOCK}},
     {"J", {Biquadris::BLOCK}},
     {"L", {Biquadris::BLOCK}},
     {"S", {Biquadris::BLOCK}},
     {"Z", {Biquadris::BLOCK}},
     {"T", {Biquadris::BLOCK}},
     {"O", {Biquadris::BLOCK}},
     {"restart", {Biquadris::RESTART}}};

static std::map<std::string, Biquadris::Debuffs> DefaultDebuffs = {
    {"heavy", Biquadris::HEAVY},
    {"blind", Biquadris::BLIND},
    {"force", Biquadris::FORCE}};

static std::vector<std::string> DefaultCommandDictionary = {
    "left",  "down",     "right",     "clockwise", "counterclockwise",
    "drop",  "levelup",  "leveldown", "norandom",  "random",
    "skip",  "sequence", "I",         "J",         "L",
#ifdef TEST
    "debug", "debuff",
#endif /* TEST */
    "S",     "Z",        "T",         "O",         "restart"};

static std::vector<std::string> DefaultDebuffDictionary = {"blind", "heavy",
                                                           "force"};

// Command may only spelled with lower case or upper case letters.
class INVALIDCHAR {};

#endif // PROMPT_H_
