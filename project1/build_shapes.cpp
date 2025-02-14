#include "build_shapes.hpp"
#include "stb_image_implementation.hpp"

unsigned int GetTexture (std::string file_path) {
    unsigned int texture;
    glGenTextures(1,&texture);
    glBindTexture(GL_TEXTURE_2D,texture);

    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    unsigned char *data = stbi_load(file_path.c_str(), &width, &height, &nrChannels, 0);
    unsigned int type_channels = GL_RGB;
    if (nrChannels > 3) {
        type_channels = GL_RGBA;
    }
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, type_channels, width, height, 0, type_channels, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture: " << file_path << std::endl;
    }
    stbi_image_free(data);
    return texture;
}

BasicShape GetTriangle (VAOStruct vao,float size, glm::vec3 location) {
    float tri_vert[] {
        -1.0,-1.0,0.0,//bottom left vertex
        1.0,-1.0,0.0,  //bottom right vertex
        0.0,1.0,0.0  //top vertex
    };
    for (int i = 0; i < 3; i++) {
        tri_vert[3*i] = size * tri_vert[3*i] + location.x;
        tri_vert[3*i+1] = size * tri_vert[3*i+1] + location.y;
        tri_vert[3*i+2] = size * tri_vert[3*i+2] + location.z;
    }
    BasicShape new_shape;
    new_shape.Initialize(vao,tri_vert,sizeof(tri_vert),3,GL_TRIANGLES);

    unsigned int indices[] = {0,1,2};
    //Set the EBO for the shape here.
    new_shape.InitializeEBO(indices,sizeof(indices),3);

    return new_shape;
}

BasicShape GetTexturedTriangle (VAOStruct vao,glm::vec3 bottom_left, 
                                      glm::vec3 bottom_right, glm::vec3 top) {
    float tri_vert[] {
       bottom_left.x,bottom_left.y,bottom_left.z, 0.0, 0.0, //bottom left vertex
       bottom_right.x,bottom_right.y,bottom_right.z, 1.0, 0.0,  //bottom right vertex
       top.x,top.y,top.z, (top.x - bottom_left.x)/(bottom_right.x-bottom_left.x),1.0  //top vertex
    };
    
    
    BasicShape new_shape;
    new_shape.Initialize(vao,tri_vert,sizeof(tri_vert),3,GL_TRIANGLES);

    unsigned int indices[] = {0,1,2};
    //Set the EBO for the shape here.
    new_shape.InitializeEBO(indices,sizeof(indices),3);


    return new_shape;
}

BasicShape GetCircle (VAOStruct vao, float radius, int points, glm::vec3 location, bool is_textured) {
    int vals_per_vertex = 3;
    if (is_textured) {
        vals_per_vertex = 5;
    }
    //float circ_texture_dat[points*5]{};
    float circ_dat[points*vals_per_vertex]{};
    float angle = 0.0;
    for(int i = 0; i<points+1; i++) {
        circ_dat[i*vals_per_vertex] = location.x + radius*cos(glm::radians(angle));
        circ_dat[i*vals_per_vertex+1] = location.y + radius*sin(glm::radians(angle));
        circ_dat[i*vals_per_vertex+2] = 0.0f;
        if (is_textured) {
            circ_dat[i*vals_per_vertex+3] = 0.5f + 0.5f*cos(glm::radians(angle));
            circ_dat[i*vals_per_vertex+4] = 0.5f + 0.5f*sin(glm::radians(angle));
        }
        angle+=360.0f/points;
    }
    
    BasicShape new_shape;
    new_shape.Initialize(vao,circ_dat,sizeof(circ_dat),points,GL_TRIANGLE_FAN);

    //for use with the EBO
    unsigned int indices[points]{};
    for(int i = 0; i<points; i++)
    {
        indices[i] = i;
    }
    //set the ebo for the shape here
    new_shape.InitializeEBO(indices,sizeof(indices),points);
  
    return new_shape;
}

//Creates and returns a BasicShape hull given a VAO and a location
BasicShape GetTexturedHull (VAOStruct vao, glm::vec3 lower_left, float width, float height){
    float total_height = height + 0.15f;
    
    float vertices[] {
        lower_left.x,lower_left.y,0.0, 0.0, 0.0,//lower left
        lower_left.x+width, lower_left.y, 0.0, 1.0, 0.0, //lower right
        lower_left.x+width, lower_left.y+height,0.0, 1.0, 1.0-0.15f, //upper right
        lower_left.x+0.9f*width, lower_left.y+height+0.05f,0.0f, 0.9f, 1.0f-0.10f,//points of the front, right to left
        lower_left.x+0.8f*width, lower_left.y+height+0.09f,0.0f, 0.8f, 1.0f-0.06f,
        lower_left.x+0.7f*width, lower_left.y+height+0.11f,0.0f, 0.7f, 1.0f-0.04f,
        lower_left.x+0.6f*width, lower_left.y+height+0.13f,0.0f, 0.6f, 1.0f-0.02f,
        lower_left.x+0.5f*width, lower_left.y+height+0.15f,0.0f, 0.5f, 1.0f,
        lower_left.x+0.4f*width, lower_left.y+height+0.13f,0.0f, 0.4f, 1.0f-0.02f,
        lower_left.x+0.3f*width, lower_left.y+height+0.11f,0.0f, 0.3f, 1.0f-0.04f,
        lower_left.x+0.2f*width, lower_left.y+height+0.09f,0.0f, 0.2f, 1.0f-0.06f,
        lower_left.x+0.1f*width, lower_left.y+height+0.05f,0.0f, 0.1f, 1.0f-0.10f,
        lower_left.x,lower_left.y+height,0.0, 0.0f, 1.0-0.15f//upper left
    };

    unsigned int indices [] {
        0,1,2,3,4,5,6,7,8,9,10,11,12
    };
    BasicShape new_shape;
    new_shape.Initialize(vao,vertices,sizeof(vertices),13,GL_TRIANGLE_FAN);
    new_shape.InitializeEBO(indices,sizeof(indices),13);
    return new_shape;

}

//Creates and returns a BasicShape hull given a VAO and a location
BasicShape GetHull (VAOStruct vao, glm::vec3 lower_left, float width, float height){
    float vertices[] {
        lower_left.x,lower_left.y,0.0, //lower left
        lower_left.x+width, lower_left.y, 0.0, //lower right
        lower_left.x+width, lower_left.y+height,0.0, //upper right
        lower_left.x+0.9f*width, lower_left.y+height+0.05f,0.0f, //points of the front, right to left
        lower_left.x+0.8f*width, lower_left.y+height+0.09f,0.0f,
        lower_left.x+0.7f*width, lower_left.y+height+0.11f,0.0f,
        lower_left.x+0.6f*width, lower_left.y+height+0.13f,0.0f,
        lower_left.x+0.5f*width, lower_left.y+height+0.15f,0.0f,
        lower_left.x+0.4f*width, lower_left.y+height+0.13f,0.0f,
        lower_left.x+0.3f*width, lower_left.y+height+0.11f,0.0f,
        lower_left.x+0.2f*width, lower_left.y+height+0.09f,0.0f,
        lower_left.x+0.1f*width, lower_left.y+height+0.05f,0.0f,
        lower_left.x,lower_left.y+height,0.0//upper left
    };

    unsigned int indices [] {
        0,1,2,3,4,5,6,7,8,9,10,11,12
    };
    BasicShape new_shape;
    new_shape.Initialize(vao,vertices,sizeof(vertices),13,GL_TRIANGLE_FAN);
    new_shape.InitializeEBO(indices,sizeof(indices),13);
    return new_shape;

}

BasicShape GetRectangle(VAOStruct vao, glm::vec3 lower_left, float width, float height) {
    float vertices[] {
        lower_left.x,lower_left.y,0.0, //lower left
        lower_left.x+width, lower_left.y, 0.0, //lower right
        lower_left.x+width, lower_left.y+height,0.0, //upper right
        lower_left.x,lower_left.y+height,0.0//upper left
    };
    
    unsigned int indices[]{
        0,1,2,3
    };
    
    BasicShape new_shape;
    new_shape.Initialize(vao,vertices,sizeof(vertices),4,GL_TRIANGLE_FAN);
    new_shape.InitializeEBO(indices,sizeof(indices),4);
    return new_shape;
}

BasicShape GetTextureRectangle (VAOStruct vao, glm::vec3 lower_left, float width, float height, float texture_size) {
    float vertices[] {
        lower_left.x,lower_left.y,0.0, 0.0,0.0,//lower left
        lower_left.x+width, lower_left.y, 0.0, texture_size,0.0,//lower right
        lower_left.x+width, lower_left.y+height,0.0, texture_size,texture_size,//upper right
        lower_left.x,lower_left.y+height,0.0,0.0,texture_size//upper left
    };
    
    unsigned int indices[]{
        0,1,2,3
    };
    
    BasicShape new_shape;
    new_shape.Initialize(vao,vertices,sizeof(vertices),4,GL_TRIANGLE_FAN);
    new_shape.InitializeEBO(indices,sizeof(indices),4);
    return new_shape;
}