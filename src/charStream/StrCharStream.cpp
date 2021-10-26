#include <StrCharStream.h>

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
