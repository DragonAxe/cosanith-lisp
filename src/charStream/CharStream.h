#pragma once

#include <Caret.h>

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
