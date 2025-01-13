#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include <glm/vec3.hpp>        // For glm::vec3
#include <glm/geometric.hpp>   // For glm::distance
#include <glm/common.hpp>      // For common GLSL functions like abs, clamp, etc.


void ModifiedHello(std::string name)
{
    std::cout << "Hello " << name << std::endl;
}

std::string GetWord()
{
    std::string input;
    std::cout << "Enter a word: ";
    std::cin >> input;
    return input;
}

float GetDistance(int *array1, int *array2, int size)
{
    float distance = 0.0;
    for (int i = 0; i < size; ++i)
    {
        distance += pow(array1[i] - array2[i], 2);
    }
    return sqrt(distance);
}

float GetDistance(const std::vector<int>& vec1, const std::vector<int>& vec2)
{
    float distance = 0.0;
    int size = vec1.size();
    for (int i = 0; i < size; ++i)
    {
        distance += pow(vec1[i] - vec2[i], 2);
    }
    return sqrt(distance);
}

// Distance function using glm::vec3
float GetDistance(const glm::vec3& pointA, const glm::vec3& pointB) {
    return glm::distance(pointA, pointB);
}

int main()
{
    std::cout << "Hello World" << std::endl;
    std::string word = GetWord();
    ModifiedHello(word);

    int array1[] = {1, 2, 3};
    int array2[] = {4, 5, 6};
    int size = sizeof(array1) / sizeof(array1[0]);
    float distance = GetDistance(array1, array2, size);
    std::cout << "Distance (arrays): " << distance << std::endl;

    std::vector<int> vec1 = {1, 2, 3};
    std::vector<int> vec2 = {4, 5, 6};
    distance = GetDistance(vec1, vec2);
    std::cout << "Distance (vectors): " << distance << std::endl;

    // Using glm::vec3
    glm::vec3 pointA(1.0f, 2.0f, 3.0f);
    glm::vec3 pointB(4.0f, 5.0f, 6.0f);
    distance = GetDistance(pointA, pointB);
    std::cout << "Distance (glm::vec3): " << distance << std::endl;
}
