#pragma once

#include <string>

/// Represents a column and line position within a character stream.
class Caret
{
public:
  /// Returns the current line.
  /// The first line in a file will return 1.
  [[nodiscard]] long line() const
  { return mLine; }

  /// Returns the current column.
  /// New lines will restart the column position.
  /// The first column on a line will return 1.
  [[nodiscard]] long col() const
  { return mCol; }

  /// Advances the caret's position to the next line.
  /// Resets the column position.
  void nextLine();

  /// Advances the caret's position by one character.
  void nextChar();

  /// Return a string representation of ths position in the form line:col.
  std::string str() const;

private:
  long mLine = 1;
  long mCol = 1;
};
