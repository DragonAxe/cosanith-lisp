#include <FileCharStream.h>


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
