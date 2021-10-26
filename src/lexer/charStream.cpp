#include <charStream.h>

#include <utility>

namespace lexer {

//
// FileCharStream -------------------------------------------------------------
//

FileCharStream::FileCharStream(std::string filename) : mFilename(std::move(filename))
{
    mIn.open(mFilename);
}

char FileCharStream::peek()
{
    return mIn.peek();
}

char FileCharStream::get()
{
    // Keep track of line and column in file.
    // Check eof before get so eof token can point just beyond end of file.
    if (!mIn.eof()) {
        mPos.nextChar();
    }
    
    const char c = mIn.get();

    if (c == '\n') {
        mPos.nextLine();
    }

    return c;
}

bool FileCharStream::eof() const
{
    return mIn.eof();
}

//
// StrCharStream --------------------------------------------------------------
//

StrCharStream::StrCharStream(std::string  src) : mSrc(std::move(src))
{}

char StrCharStream::peek()
{
    if (mSrcPos < mSrc.size()) {
        return mSrc[mSrcPos];
    } else {
        mEof = true;
        return '\0';
    }
}

char StrCharStream::get()
{
    if (mSrcPos < mSrc.size()) {
        mPos.nextChar();
        const char c = mSrc[mSrcPos];
        mSrcPos++;
        if (c == '\n') {
            mPos.nextLine();
        }
        return c;
    } else {
        mEof = true;
        return '\0';
    }
}

bool StrCharStream::eof() const
{
    return mEof;
}

} // namespace lexer
