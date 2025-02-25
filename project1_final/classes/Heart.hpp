#ifndef HEART_HPP
#define HEART_HPP

class Heart {
public:
    Heart(float x = 0.0f, float y = 0.0f, bool isFull = true);

    float getX() const;
    float getY() const;
    bool isFull() const;

    void setX(float x);
    void setY(float y);
    void setFull(bool isFull);

    void render(unsigned int shaderProgram, unsigned int VAO, unsigned int fullTexture, unsigned int emptyTexture) const;

private:
    float x, y;
    bool full;
};

#endif // HEART_HPP