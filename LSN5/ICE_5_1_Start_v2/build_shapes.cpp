#include "build_shapes.hpp"

BasicShape GetTriangle(VAOStruct vao, float size, glm::vec3 location) {
    float tri_vert[] {
        -1.0, -1.0, 0.0, // bottom left vertex
        1.0, -1.0, 0.0,  // bottom right vertex
        0.0, 1.0, 0.0    // top vertex
    };
    for (int i = 0; i < 3; i++) {
        tri_vert[3 * i] = size * tri_vert[3 * i] + location.x;
        tri_vert[3 * i + 1] = size * tri_vert[3 * i + 1] + location.y;
        tri_vert[3 * i + 2] = size * tri_vert[3 * i + 2] + location.z;
    }
    BasicShape new_shape;
    new_shape.Initialize(vao, tri_vert, sizeof(tri_vert), 3, GL_TRIANGLES);

    unsigned int indices[] = { 0, 1, 2 };
    new_shape.InitializeEBO(indices, sizeof(indices), 3, GL_TRIANGLES);

    return new_shape;
}

BasicShape GetCircle(VAOStruct vao, float radius, int points, glm::vec3 location) {
    float circ_dat[points * 3]{};
    float angle = 0.0;
    for (int i = 0; i < points + 1; i++) {
        circ_dat[i * 3] = location.x + radius * cos(glm::radians(angle));
        circ_dat[i * 3 + 1] = location.y + radius * sin(glm::radians(angle));
        circ_dat[i * 3 + 2] = 0.0f;
        angle += 360.0f / points;
    }

    BasicShape new_shape;
    new_shape.Initialize(vao, circ_dat, sizeof(circ_dat), points, GL_TRIANGLE_FAN);

    unsigned int indices[points]{};
    for (int i = 0; i < points; i++) {
        indices[i] = i;
    }
    new_shape.InitializeEBO(indices, sizeof(indices), points, GL_LINE_LOOP);

    return new_shape;
}

BasicShape GetShipHull(VAOStruct vao, glm::vec3 location) {
    float hull_vert[] {
        -0.5, -0.5, 0.0, // bottom left vertex
        0.5, -0.5, 0.0,  // bottom right vertex
        0.3, 0.0, 0.0,   // top right vertex
        -0.3, 0.0, 0.0   // top left vertex
    };
    for (int i = 0; i < 4; i++) {
        hull_vert[3 * i] += location.x;
        hull_vert[3 * i + 1] += location.y;
        hull_vert[3 * i + 2] += location.z;
    }
    BasicShape new_shape;
    new_shape.Initialize(vao, hull_vert, sizeof(hull_vert), 4, GL_TRIANGLE_FAN);

    unsigned int indices[] = { 0, 1, 2, 3 };
    new_shape.InitializeEBO(indices, sizeof(indices), 4, GL_TRIANGLE_FAN);

    return new_shape;
}

BasicShape GetWindshield(VAOStruct vao, glm::vec3 location) {
    float windshield_vert[] {
        -0.2, 0.0, 0.0, // bottom left vertex
        0.2, 0.0, 0.0,  // bottom right vertex
        0.1, 0.3, 0.0,  // top right vertex
        -0.1, 0.3, 0.0  // top left vertex
    };
    for (int i = 0; i < 4; i++) {
        windshield_vert[3 * i] += location.x;
        windshield_vert[3 * i + 1] += location.y;
        windshield_vert[3 * i + 2] += location.z;
    }
    BasicShape new_shape;
    new_shape.Initialize(vao, windshield_vert, sizeof(windshield_vert), 4, GL_TRIANGLE_FAN);

    unsigned int indices[] = { 0, 1, 2, 3 };
    new_shape.InitializeEBO(indices, sizeof(indices), 4, GL_TRIANGLE_FAN);

    return new_shape;
}

BasicShape GetCannon(VAOStruct vao, glm::vec3 location) {
    float cannon_vert[] {
        -0.05, -0.1, 0.0, // bottom left vertex
        0.05, -0.1, 0.0,  // bottom right vertex
        0.05, 0.1, 0.0,   // top right vertex
        -0.05, 0.1, 0.0   // top left vertex
    };
    for (int i = 0; i < 4; i++) {
        cannon_vert[3 * i] += location.x;
        cannon_vert[3 * i + 1] += location.y;
        cannon_vert[3 * i + 2] += location.z;
    }
    BasicShape new_shape;
    new_shape.Initialize(vao, cannon_vert, sizeof(cannon_vert), 4, GL_TRIANGLE_FAN);

    unsigned int indices[] = { 0, 1, 2, 3 };
    new_shape.InitializeEBO(indices, sizeof(indices), 4, GL_TRIANGLE_FAN);

    return new_shape;
}
