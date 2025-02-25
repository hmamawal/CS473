#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform float uZoom;  // <--- new uniform controlling zoom

out vec2 TexCoord;

void main()
{
    // Flip the y-coordinate
    vec2 flippedPos = vec2(aPos.x, 600.0 - aPos.y);

    // Scale by the zoom factor (zoom > 1.0 => bigger, < 1.0 => smaller).
    // If you want "zoom in" to actually make objects look bigger, you can invert it.
    flippedPos *= uZoom;

    // Same normalization from (0..800,0..600) to Clip Space
    gl_Position = vec4(flippedPos / vec2(800.0, 600.0) * 2.0 - 1.0, 0.0, 1.0);
    TexCoord = aTexCoord;
}
