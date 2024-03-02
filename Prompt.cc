#include "Prompt.h"
#include "Biquadris.h"
#include "Block.h"
#include <fstream>

Prompt::Prompt(int player)
    : CommandDictionary{DefaultCommandDictionary},
      DebuffDictionary{DefaultDebuffDictionary}, player{player}, buffer{},
      commands{DefaultCommands}, debuffs{DefaultDebuffs}, in{std::cin},
      out{std::cout} {}

Prompt::Prompt(int player, std::string msg)
    : CommandDictionary{DefaultCommandDictionary},
      DebuffDictionary{DefaultDebuffDictionary}, player{player}, buffer{},
      commands{DefaultCommands}, debuffs{DefaultDebuffs}, a{std::ifstream{msg}},
      in{a}, out{std::cout} {}

void Prompt::flushBuffer() {
  buffer = {};
  return;
}

void Prompt::promptUser(std::string msg) {
  out << "P" << player << "|" << msg << ":";
  return;
}

Biquadris::Commands Prompt::getCommand() {
  int count = 1;
  std::string tmp;
  Biquadris::Commands cmd;

  while (buffer.empty()) {
    promptUser("Command");
    in >> count;

    if (in.eof()) {
      throw Off{};
    }

    if (in.fail()) {
      in.clear();
      count = 1;
      in >> tmp;
    } else {
      in >> tmp;
    }

    TrieNode::result cmd;

    try {
      cmd = CommandDictionary.autoComplete(tmp);
    } catch (INVALIDCHAR &e) {
      out << "Please enter alphanumeric characters only.\n";
    }

    switch (cmd.first) {
    case TrieNode::UNKNOWN:
      out << "Unknown command\n";
      break;
    case TrieNode::AMBIGUOUS:
      out << "Ambiguous command\n";
      break;
    case TrieNode::VALID:
      auto a = commands[cmd.second][0];
      if (a == Biquadris::BLOCK) {
        b = Block::stringToBlockType(cmd.second);
      }

      for (int i = 0; i < count; ++i) {
        buffer.insert(buffer.end(), commands[cmd.second].begin(),
                      commands[cmd.second].end());
      }
      break;
    }
  }

  cmd = buffer.front();
  buffer.pop_front();

  return cmd;
}

bool Prompt::gameOver() {
  std::string answer;

  out << "Player" << player << ", You lost :(" << std::endl
      << "Play again? (Y)es / (N)o : ";

  if (in >> answer) {
    if (answer == "Y") {
      return true;
    }
  }

  return false;
}

Biquadris::Debuffs Prompt::getDebuff() {
  std::string tmp;

  while (true) {
    promptUser("DEBUFF");

    in >> tmp;

    if (in.eof()) {
      throw Off{};
    }

    TrieNode::result cmd;

    try {
      cmd = DebuffDictionary.autoComplete(tmp);
    } catch (INVALIDCHAR &e) {
      cmd.second = TrieNode::UNKNOWN;
    }

    switch (cmd.first) {
    case TrieNode::UNKNOWN:
      out << "Unknown command\n";
      break;
    case TrieNode::AMBIGUOUS:
      out << "Ambiguous command\n";
      break;
    case TrieNode::VALID:
      return debuffs[cmd.second];
      // TODO:
      // throw?
    }
  }
}

Biquadris::BlockType Prompt::getBlock() {
  Biquadris::BlockType ret;
  if (b == Biquadris::Dot) {
    promptUser("Choose Block:");

    std::string input;

    while (true) {
      in >> input;

      if (in.eof()) {
        throw Off{};
      }

      if (input == "I") {
        b = Biquadris::I;
        break;
      } else if (input == "J") {
        b = Biquadris::J;
        break;
      } else if (input == "L") {
        b = Biquadris::L;
        break;
      } else if (input == "O") {
        b = Biquadris::O;
        break;
      } else if (input == "S") {
        b = Biquadris::S;
        break;
      } else if (input == "Z") {
        b = Biquadris::Z;
        break;
      } else if (input == "T") {
        b = Biquadris::T;
        break;
      } else {
        out << "Wrong Block Type!" << std::endl;
      }
    }
  }

  ret = b;
  b = Biquadris::Dot;

  return ret;
}

std::string Prompt::getString(std::string msg) {
  std::string ret;

  promptUser(msg);
  in >> ret;

  return ret;
}
