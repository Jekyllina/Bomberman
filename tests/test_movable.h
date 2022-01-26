#include "clove.h"
#include "../bomberman.h"

CLOVE_TEST(MovableFixPositionX)
{
    level_t level;
    int32_t map[] = {0, 0, 0, 0};
    level_init(&level, 4, 4, 8, map);
    movable_t movable = {1, 0, 4, 4};  //4x4 movable on a 8x8 grid
    move_on_level(&level, &movable, -10, 0);
    CLOVE_FLOAT_EQ(movable.x, 0);
}

CLOVE_TEST(MovableFixPositionY)
{
    level_t level;
    int32_t map[] = {0, 0, 0, 0};
    level_init(&level, 4, 4, 8, map);
    movable_t movable = {1, 0, 4, 4};  //4x4 movable on a 8x8 grid
    move_on_level(&level, &movable, -10, -30);
    CLOVE_FLOAT_EQ(movable.y, 0);
}

CLOVE_TEST(MovableFixPositionWidth)
{
    level_t level;
    int32_t map[] = {0, 0, 0, 0};
    level_init(&level, 4, 4, 8, map);
    movable_t movable = {1, 0, 4, 4};  //4x4 movable on a 8x8 grid
    move_on_level(&level, &movable, 50, 0);
    CLOVE_FLOAT_EQ(movable.x, 28);
}

CLOVE_TEST(MovableFixPositionHeight)
{
    level_t level;
    int32_t map[] = {0, 0, 0, 0};
    level_init(&level, 4, 4, 8, map);
    movable_t movable = {1, 0, 4, 4};  //4x4 movable on a 8x8 grid
    move_on_level(&level, &movable, 0, 100);
    CLOVE_FLOAT_EQ(movable.y, 28);
}

CLOVE_TEST(MovableWallHitOnFeetMovingRight)
{
    level_t level;
    int32_t map[] = {0, 0x101, 0, 0};
    level_init(&level, 2, 2, 64, map);
    movable_t movable = {1, 1, 32, 32};  
    move_on_level(&level, &movable, 80, 0);
    CLOVE_FLOAT_EQ(movable.x, 32);
}

CLOVE_TEST(MovableWallHitOnNeckMovingRight)
{
    level_t level;
    int32_t map[] = {0, 0x101, 0, 0};
    level_init(&level, 2, 2, 64, map);
    movable_t movable = {1, 50, 32, 32};  
    move_on_level(&level, &movable, 80, 0);
    CLOVE_FLOAT_EQ(movable.x, 32);
}

CLOVE_TEST(MovableWallHitOnFeetMovingLeft)
{
    level_t level;
    int32_t map[] = {0x101, 0, 0, 0};
    level_init(&level, 2, 2, 64, map);
    movable_t movable = {70, 1, 32, 32};  
    move_on_level(&level, &movable, -30, 0);
    CLOVE_FLOAT_EQ(movable.x, 64);
}

CLOVE_TEST(MovableWallHitOnNeckMovingLeft)
{
    level_t level;
    int32_t map[] = {0x101, 0, 0, 0};
    level_init(&level, 2, 2, 64, map);
    movable_t movable = {70, 50, 32, 32};  
    move_on_level(&level, &movable, -30, 0);
    CLOVE_FLOAT_EQ(movable.x, 64);
}

CLOVE_SUITE(MovableSuite)
{
    CLOVE_SUITE_TESTS(
        MovableFixPositionX,
        MovableFixPositionY,
        MovableFixPositionWidth,
        MovableFixPositionHeight,
        MovableWallHitOnFeetMovingRight,
        MovableWallHitOnNeckMovingRight,
        MovableWallHitOnFeetMovingLeft,
        MovableWallHitOnNeckMovingLeft
    );
}