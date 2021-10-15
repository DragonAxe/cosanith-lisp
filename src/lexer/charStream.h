#pragma once

#include <string>
#include <fstream>

namespace lexer {

/// Represents a file position.
class Caret
{
public:
    [[nodiscard]] long line() const {return mLine;}
    [[nodiscard]] long col() const {return mCol;}

    void nextLine();
    void nextChar();

    std::string str();

private:
    long mLine = 1;
    long mCol = 1;
};

class CharStream
{
public:
    // Virtual methods:

    virtual char peek() = 0;
    virtual char get() = 0;
    [[nodiscard]] virtual bool eof() const = 0;
    
    // Concrete methods:

    [[nodiscard]] Caret pos() const { return mPos; }

protected:
    Caret mPos;
};

/// A file stream wrapper which keeps track of line and column numbers in file.
class FileCharStream : public CharStream
{
public:
    /// Opens the given file for reading.
    explicit FileCharStream(std::string  filename);

    // Pass through methods to std::ifstream:
    char peek() override;
    char get() override;
    bool eof() const override;

private:
    const std::string mFilename;
    std::ifstream mIn;
};

/// A file stream wrapper which keeps track of line and column numbers in file.
class StrCharStream : public CharStream
{
public:
    /// Opens the given file for reading.
    explicit StrCharStream(std::string  src);

    char peek() override;
    char get() override;
    [[nodiscard]] bool eof() const override;

private:
    const std::string mSrc;
    long mSrcPos = 0;
    bool mEof = false;
};

} // namespace lexer
