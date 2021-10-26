#pragma once

#include <../charStream/Caret.h>
#include <../charStream/CharStream.h>

#include <string>
#include <fstream>

namespace lexer {

/// A file stream wrapper which keeps track of line and column numbers in file.
class FileCharStream : public CharStream
{
public:
  /// Opens the given file for reading.
  explicit FileCharStream(std::string filename);

  // Pass through methods to std::ifstream:
  [[nodiscard]] char peek() override;

  [[nodiscard]] char get() override;

  [[nodiscard]] bool eof() const override;

private:
  const std::string mFilename;
  std::ifstream mIn;
};

/// A file stream wrapper which keeps track of line and column numbers in file.
class StrCharStream : public CharStream
{
public:
  /// Opens the given file for reading.
  explicit StrCharStream(std::string src);

  [[nodiscard]] char peek() override;

  [[nodiscard]] char get() override;

  [[nodiscard]] bool eof() const override;

private:
  const std::string mSrc;
  long mSrcPos = 0;
  bool mEof = false;
};

} // namespace lexer
