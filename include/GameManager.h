#pragma once
class GameManager {
private:
int a;
public:
    GameManager(int state);
    void init();
    void exit();
    void update(int& s);
    void draw();
    long long getTime();

};