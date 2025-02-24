#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

// Settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
const unsigned int TILE_SIZE = 20;

// At global scope or near the top of main.cpp:
static float legSwingAngle = 0.0f;      // Current rotation offset for the legs
static float legSwingDirection = 1.0f;  // 1 means increasing angle, -1 means decreasing
static bool  isWalking = false;         // Track if the player is currently moving
const float LEG_SWING_SPEED = 2.0f; // Adjust this value to change the speed

// For ducks and foxes, we still use a simple block of size 10:
const float ENTITY_SIZE = 10.0f;

// --- New Composite Player Constants ---
// We now assume the player's visual is 10 units wide and 25 units tall
// (with 5 for legs, 10 for body, and roughly 10 for the head's diameter).
const float PLAYER_WIDTH = 10.0f;
const float PLAYER_HEIGHT = 25.0f;
const float PLAYER_SPEED = 0.50f;

// Maximum hearts the player can have.
const int MAX_HEARTS = 3;

#endif // CONSTANTS_HPP