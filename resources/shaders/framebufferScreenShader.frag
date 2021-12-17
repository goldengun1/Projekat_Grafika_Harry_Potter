#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform float exposure;

//post processing values
uniform bool blurr;

const float offset = 1.0 / 300.0;

void main()
{
    const float gamma = 0.5;

    if(!blurr){
        // normal rendering
        vec3 hdrColor = texture(screenTexture, TexCoords).rgb;
        vec3 result = vec3(1.0f) - exp(-hdrColor * exposure);
        result = pow(result, vec3(1.0/gamma));
        FragColor = vec4(result, 1.0);
    }
    else if(blurr){
        vec2 offsets[9] = vec2[](
        vec2(-offset, offset), // top/left
        vec2(0.0f, offset), // top-center
        vec2(offset, offset),
        vec2(-offset, 0.0f),
        vec2(0.0f, 0.0f),
        vec2(0.0f, offset),
        vec2(-offset, -offset),
        vec2(0.0f, -offset),
        vec2(offset, -offset)
        );

        float kernel[9] = float[](
        1.0 / 16, 2.0/16, 1.0/ 16,
        2.0 / 16, 4.0/16, 2.0/ 16,
        1.0 / 16, 2.0/16, 1.0/ 16
        );

        vec3 sampleTex[9];
        for (int i = 0; i < 9; ++i) {
            sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));
        }

        vec3 col = vec3(0.0);
        for (int i = 0; i < 9; ++i) {
            col += sampleTex[i] * kernel[i];
        }

        vec3 result = vec3(1.0f) - exp(-col * exposure);
        result = pow(result, vec3(1.0/gamma));
        FragColor = vec4(result, 1.0);
    }

}