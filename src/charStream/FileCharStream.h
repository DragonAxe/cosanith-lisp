#pragma once

#include <CharStream.h>

#include <string>
#include <fstream>

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
