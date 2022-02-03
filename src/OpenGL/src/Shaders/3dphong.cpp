
const std::string vertex_3d_phong = R"SRC(
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

varying vec3 Normal;
varying vec3 FragPos;

void main()
{
  // todo: this is constant for all vertices, move the calculation out of the shader
  gl_Position = projection * view * model * vec4(aPos, 1.0);
  Normal = vec3(model * vec4(aNormal, 1.0));
  FragPos = vec3(model * vec4(aPos, 1.0));
}
)SRC";

const std::string fragment_3d_phong = R"SRC(
#version 330 core

uniform vec3 color;
uniform vec3 lightPos; 
uniform vec3 lightColor;
uniform float ambientStrength;
uniform vec3 ambientColor;
uniform float reflectionStrength;
uniform vec3 reflectionColor;

varying vec3 Normal;
varying vec3 FragPos;

out vec4 FragColor;

void main()
{
  vec3 N = normalize(Normal);

  // Ambient term
  vec3 ambient = ambientStrength * ambientColor;

  // Diffuse term
  vec3 lightDir = normalize(lightPos - FragPos); 
  vec3 diffuse = lightColor * max(dot(N, lightDir), 0.0);  

  // Specular term
  vec3 reflection = normalize(-reflect(lightDir,N));
  vec3 specular = reflectionColor * pow(max(dot(reflection,normalize(-FragPos)),0.0),0.3*reflectionStrength);

  // Sum up the parts
  vec3 result = color * ( ambient + diffuse ) + specular;
  FragColor = vec4(result, 1.0);
}
)SRC";

shaders["3d_phong"] = std::make_tuple(vertex_3d_phong,fragment_3d_phong);


// const std::string vertex_3d_phong = R"SRC(
// #version 330 core

// layout (location = 0) in vec3 aPos;
// layout (location = 1) in vec3 aNormal;

// uniform mat4 model;
// uniform mat4 view;
// uniform mat4 projection;

// out vec3 Normal;
// out vec3 FragPos;

// void main()
// {
//   // todo: this is constant for all vertices, move the calculation out of the shader
//   gl_Position = projection * view * model * vec4(aPos, 1.0);
//   Normal = vec3(model * vec4(aNormal, 1.0));
//   FragPos = vec3(model * vec4(aPos, 1.0));
// }
// )SRC";

// const std::string fragment_3d_phong = R"SRC(
// #version 330 core

// uniform vec3 color;
// uniform vec3 lightPos; 
// uniform vec3 lightColor;
// // TODO: add ambientStrength & ambientColor

// in vec3 Normal;
// in vec3 FragPos;

// out vec4 FragColor;

// void main()
// {
//   float ambientStrength = 0.1;
//   vec3 ambient = ambientStrength * lightColor;

//   vec3 norm = normalize(Normal);
//   vec3 lightDir = normalize(lightPos - FragPos); 
//   float diff = max(dot(norm, lightDir), 0.0);  
//   vec3 diffuse = diff * lightColor;
//   vec3 result = (ambient + diffuse) * color;
//   FragColor = vec4(result, 1.0);
// }
// )SRC";

// shaders["3d_phong"] = std::make_tuple(vertex_3d_phong,fragment_3d_phong);
