#pragma once

#include <string>
#include <fstream>

namespace lexer {

/// Represents a column and line position within a character stream.
class Caret
{
public:
  /// Returns the current line.
  /// The first line in a file will return 1.
  [[nodiscard]] long line() const { return mLine; }

  /// Returns the current column.
  /// New lines will restart the column position.
  /// The first column on a line will return 1.
  [[nodiscard]] long col() const { return mCol; }

  /// Advances the caret's position to the next line.
  /// Resets the column position.
  void nextLine();

  /// Advances the caret's position by one character.
  void nextChar();

  /// Return a string representation of ths position in the form line:col.
  std::string str();

private:
  long mLine = 1;
  long mCol = 1;
};

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
  [[nodiscard]] Caret pos() const { return mPos; }

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
