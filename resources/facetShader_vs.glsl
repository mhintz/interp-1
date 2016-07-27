#version 330

in vec3 ciPosition;
in vec3 ciNormal;
in vec4 ciColor;

uniform mat4 ciModelViewProjection;
uniform mat4 ciModelMatrix;
uniform mat3 ciNormalMatrix;

out vec3 fWorldPosition;
out vec3 fNormal;
out vec4 fColor;

void main() {
  gl_Position = ciModelViewProjection * vec4(ciPosition, 1);
  fWorldPosition = (ciModelMatrix * vec4(ciPosition, 1)).xyz;
  fNormal = ciNormalMatrix * ciNormal;
  fColor = ciColor;
}
