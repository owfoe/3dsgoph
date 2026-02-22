#include "HitBox.h"
#include "Constants.h"

class Raycast
{
public:
    Raycast(float height, float width) : hitbox(height + RC_diff_h, width + RC_diff_w * 2) {}
    HitBox hitbox;
};