
const std::string vertex_2d = R"SRC(
#version 120
attribute vec2 coord2d;
void main(void) {
  gl_Position = vec4(coord2d, 0.0, 1.0);
}
)SRC";

const std::string fragment_2d = R"SRC(
#version 120
void main(void) {
  gl_FragColor[0] = 0.0;
  gl_FragColor[1] = 0.0;
  gl_FragColor[2] = 1.0;
}
)SRC";

shaders["2d"] = std::make_tuple(vertex_2d,fragment_2d);
