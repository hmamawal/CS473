// ResourceManager.hpp
#ifndef RESOURCE_MANAGER_HPP
#define RESOURCE_MANAGER_HPP

#include <string>
#include <map>


// ... any other needed includes

class ResourceManager {
    public:
        static unsigned int loadTexture(const char* path);
        unsigned int CreateShaderProgram(const std::string& vertexShader, const std::string& fragmentShader);
    private:
        static unsigned int compileShader(unsigned int type, const std::string& source);
};
#endif // RESOURCE_MANAGER_HPP