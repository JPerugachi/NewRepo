#version 330 core
in vec3 ourColor;
in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform int useTexture2; // 0 for texture1, 1 for texture2

void main() {
    vec4 texColor;
    if (useTexture2 == 1) {
        texColor = texture(texture2, TexCoord);
    } else {
        texColor = texture(texture1, TexCoord);
    }
    FragColor = texColor;
}
