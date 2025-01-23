#include "build_shapes.hpp"

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
    new_shape.InitializeEBO(indices, 4, GL_LINE_LOOP);

    return new_shape;
}

BasicShape GetCircle (VAOStruct vao, float radius, int points, glm::vec3 location) {
    float circ_dat[points*3]{};
    float angle = 0.0;
    for(int i = 0; i<points+1; i++) {
        circ_dat[i*3] = location.x + radius*cos(glm::radians(angle));
        circ_dat[i*3+1] = location.y + radius*sin(glm::radians(angle));
        circ_dat[i*3+2] = 0.0f;
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
    new_shape.InitializeEBO(indices, points, GL_LINE_LOOP);
  
    return new_shape;
}

BasicShape GetHull(VAOStruct vao, float width, float height, glm::vec3 location) {
    float vertices[] {
        location.x, location.y, location.z, //bottom left
        location.x+width, location.y, location.z, //bottom right
        location.x+width, location.y+height, location.z, //top right
        location.x+(width*0.5f), location.y+height+(height*0.5f), location.z, //top center
        location.x, location.y+height, location.z //top left
    };
    
    BasicShape new_shape;
    new_shape.Initialize(vao, vertices, sizeof(vertices), 5, GL_TRIANGLE_FAN);

    unsigned int indices[] = {0,1,2,3,4};
    // set the ebo for the shape here
    new_shape.InitializeEBO(indices, 5, GL_LINE_LOOP);

    return new_shape;
}
