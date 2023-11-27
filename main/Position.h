#ifndef POSITION
#define POSITION

class Position {
private:
    int x;
    int y;
public:
    Position();
    Position(int cx, int cy);
    ~Position();
    int getX();
    int getY();
};

#endif