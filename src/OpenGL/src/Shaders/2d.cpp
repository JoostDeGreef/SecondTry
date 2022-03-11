
const std::string vertex_2d = R"SRC(
#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 projection;

void main()
{
  gl_Position = projection * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);
}
)SRC";

const std::string fragment_2d = R"SRC(
#version 330 core

uniform vec3 color;
out vec4 FragColor;

void main()
{
  FragColor = vec4(color.rgb, 1.0f);
}
)SRC";

const std::vector<OpenGL::Shader::Uniforms> uniforms_2d = 
{
  OpenGL::Shader::Uniforms::model,
  OpenGL::Shader::Uniforms::projection,
  OpenGL::Shader::Uniforms::color,
};

shaders["2d"] = std::make_tuple(vertex_2d,fragment_2d,uniforms_2d);
