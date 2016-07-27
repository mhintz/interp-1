#version 330

in vec3 fWorldPosition;
in vec3 fNormal;
in vec4 fColor;

uniform vec3 uCameraPosition;

out vec4 FragColor;

void main() {
  vec3 toCamera = normalize(uCameraPosition - fWorldPosition);
  vec3 normal = normalize(fNormal);

  float dotValue = abs(dot(normal, toCamera));
  dotValue = fract(dotValue * 3.0);

  // vec3 partialColor = fract(fColor.xyz + vec3(dotValue / 3.0, dotValue / 2.0, dotValue / 1.0));
  vec3 partialColor = fract(fColor.xyz + vec3(dotValue));
  // vec3 partialColor = vec3(dotValue);

  FragColor = vec4(partialColor, fColor.a);
}
