#pragma once

#include <CharStream.h>

#include <string>

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
