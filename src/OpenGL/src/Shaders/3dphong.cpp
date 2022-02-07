
const std::string vertex_3d_phong = R"SRC(
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 model; // only translation & rotation allowed here!
uniform mat4 view;
uniform mat4 projection;

varying vec3 Normal;
varying vec3 FragPos;

void main()
{
  gl_Position = projection * view * model * vec4(aPos, 1.0);
  Normal = mat3(model) * aNormal;
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
  vec3 reflection;
  if (dot(N, lightDir) < 0.0)
  {
    reflection = vec3(0.0, 0.0, 0.0);
  }
  else // light source on the right side
  {
    vec3 viewDir = normalize(-FragPos);
    reflection = reflectionColor * pow(max(0.0, 0.5*dot(reflect(-lightDir, N), viewDir)), reflectionStrength);
  }

  // Sum up the parts
  vec3 result = color * ( ambient + diffuse ) + reflection * 0.75;
  FragColor = vec4(result, 1.0);
}
)SRC";

shaders["3d_phong"] = std::make_tuple(vertex_3d_phong,fragment_3d_phong);


