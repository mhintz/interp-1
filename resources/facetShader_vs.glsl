#version 330

in vec3 ciPosition;
in vec4 ciColor;

uniform mat4 ciModelViewProjection;

out vec4 fColor;

void main() {
  gl_Position = ciModelViewProjection * vec4(ciPosition, 1);
  fColor = ciColor;
}
