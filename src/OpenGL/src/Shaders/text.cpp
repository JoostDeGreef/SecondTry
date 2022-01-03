
const std::string vertex_text = R"SRC(
#version 330 core

layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 projection;

void main()
{
    gl_Position = projection * model * vec4(vertex.xy, 0.0, 1.0);
    TexCoords = vertex.zw;
}
)SRC";

const std::string fragment_text = R"SRC(
#version 330 core

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D text;
uniform vec3 color;

void main()
{ 
  vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
  FragColor = vec4(color, 1.0) * sampled;
}
)SRC";

shaders["text"] = std::make_tuple(vertex_text,fragment_text);
