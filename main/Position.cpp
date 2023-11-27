#include "Position.h"

Position::Position() 
{ 
}

Position::Position(int cx, int cy) 
{ 
    x = cx;
    y = cy;
}

Position::~Position() 
{ 
}

int Position::getX()
{
    return x;
}

int Position::getY()
{
    return y;
}
