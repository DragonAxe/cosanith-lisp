#include <Caret.h>

void Caret::nextLine()
{
  mCol = 0;
  mLine++;
}

void Caret::nextChar()
{
  mCol++;
}

std::string Caret::str() const
{
  return "" + std::to_string(mLine) + ":" + std::to_string(mCol);
}
