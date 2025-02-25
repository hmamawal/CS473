#ifndef PLAYER_HPP
#define PLAYER_HPP

class Player {
public:
    Player(float x = 0.0f, float y = 0.0f, float angle = 0.0f, float speed = 0.5f);

    float getX() const;
    float getY() const;
    float getAngle() const;
    float getSpeed() const;

    void setX(float x);
    void setY(float y);
    void setAngle(float angle);
    void setSpeed(float speed);

    // Add this:
    void render(float swingAngle, unsigned int shaderProgram, unsigned int VAO, unsigned int texture) const;

private:
    float x, y;
    float angle;
    float speed;
};

#endif // PLAYER_HPP
