#include "Prompt.h"

Prompt::TrieNode::TrieNode(std::string l)
    : WordCount{0}, isWord{false}, letter{l}, letters{} {}

Prompt::TrieNode::TrieNode(std::vector<std::string> dictionary)
    : WordCount{0}, isWord{false}, letter{}, letters{} {
  for (auto &c : dictionary) {
    this->registerWord(c);
  }
}

Prompt::TrieNode::~TrieNode() {}

int Prompt::TrieNode::idx(char c) const {
  if (c < 'A' || ('Z' < c && c < 'a') || 'z' < c) {
    throw INVALIDCHAR{};
  }

  if (c < 'a') {
    return c - 'A';
  }

  return c - 'a' + Alphabet;
}

std::string Prompt::TrieNode::letterAt(int i) const {
  char c;

  if (i < 26) {
    c = i + 'A';
    return {c};
  } else {
    c = i - Alphabet + 'a';
  }

  return {c};
}

void Prompt::TrieNode::updateWordCount() {
  WordCount = 0;

  if (this->isWord) {
    WordCount = 1;
  }

  for (int i = 0; i < ValidLetters; ++i) {
    if (letters[i]) {
      letters[i]->updateWordCount();
      WordCount += letters[i]->WordCount;
    }
  }

  return;
}

void Prompt::TrieNode::registerWord(const std::string &word) {
  if (word.length() == 0) {
    return;
  }

  int i = idx(word[0]);

  if (!letters[i]) {
    letters[i] = make_unique<TrieNode>(letterAt(i));
  }

  if (word.length() == 1) {
    letters[i]->isWord = true;
  }

  letters[i]->registerWord(word.substr(1));

  this->updateWordCount();

  return;
}

std::string Prompt::TrieNode::trailingLetters() const {
  for (int i = 0; i < ValidLetters; ++i) {
    if (letters[i]) {
      return letterAt(i) + letters[i]->trailingLetters();
    }
  }

  return "";
}

Prompt::TrieNode::result
Prompt::TrieNode::autoComplete(const std::string &word) const {
  auto tmp = this;

  for (const auto &c : word) {
    int i = idx(c);
    if (tmp->letters[i]) {
      tmp = tmp->letters[i].get();
    } else {
      return {UNKNOWN, ""};
    }
  }

  switch (tmp->WordCount) {
  case 0:
    return {UNKNOWN, ""};
  case 1:
    return {VALID, word + tmp->trailingLetters()};
  default:
    return {AMBIGUOUS, ""};
  }
}
