#pragma once

#include <string>
#include <fstream>

namespace lexer {

/// Represents a file position.
class Caret
{
public:
    long line() const {return mLine;}
    long col() const {return mCol;}

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
    virtual bool eof() const = 0;
    
    // Concrete methods:

    Caret pos() const { return mPos; }

protected:
    Caret mPos;
};

/// A file stream wrapper which keeps track of line and column numbers in file.
class FileCharStream : public CharStream
{
public:
    /// Opens the given file for reading.
    explicit FileCharStream(const std::string filename);

    // Pass through methods to std::ifstream:

    char peek() override;
    char get() override;
    bool eof() const override;

    // Additional methods:

    Caret pos() const;

private:
    const std::string mFilename;
    std::ifstream mIn;
};

/// A file stream wrapper which keeps track of line and column numbers in file.
class StrCharStream : public CharStream
{
public:
    /// Opens the given file for reading.
    explicit StrCharStream(const std::string src);

    // Pass through methods to std::ifstream:

    char peek() override;
    char get() override;
    bool eof() const override;

    // Additional methods:

    Caret pos() const;

private:
    const std::string mSrc;
    long mSrcPos = 0;
    bool mEof;
};

} // namespace lexer
