#pragma once

#include <../charStream/Caret.h>

#include <string>
#include <fstream>

namespace lexer {

/// A virtual class representing a stream of characters from some source.
class CharStream
{
public:
  // Virtual methods:

  /// Look at the next character in the stream, but don't advance the stream's
  /// position. Successive calls to peek will return the same character.
  [[nodiscard]] virtual char peek() = 0;

  /// Get the next character from the stream and advance the stream's position
  /// to the next character.
  [[nodiscard]] virtual char get() = 0;

  /// Returns true if end of file has been reached. This function only reports
  /// the stream state as set by the most recent I/O operation; it does not
  /// examine the associated data source.
  [[nodiscard]] virtual bool eof() const = 0;

  // Concrete methods:

  /// Get the stream's current position.
  [[nodiscard]] Caret pos() const
  { return mPos; }

protected:
  Caret mPos;
};

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
