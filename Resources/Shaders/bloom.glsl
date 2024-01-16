#version 330 core
layout(location = 0) in vec4 a_quad;

out vec2 uvs;

void main() 
{
	gl_Position = vec4(a_quad.x, a_quad.y, 0.0, 1.0);
	uvs = vec2(a_quad.z, a_quad.w);
}

++VERTEX++

#version 330 core
out vec4 out_fragment;
in vec2 uvs;

const float WEIGHTS[5] = float[] (0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162);

uniform sampler2D u_brightnessMap;
uniform bool u_horizontalPass;
uniform int u_frameHeight;
uniform int u_frameWidth;

void main() 
{             
    vec3 color = texture(u_brightnessMap, uvs).rgb * WEIGHTS[0]; 

    // vec2 texelSize = 1.0 / textureSize(u_brightnessMap, 0); with opengl 4.x

    vec2 texelSize = 1.0 / vec2(u_frameWidth, u_frameHeight);

    if(u_horizontalPass) 
    {
        for(int i = 1; i < 5; ++i)
        {
            color += texture(u_brightnessMap, uvs + vec2(texelSize.x * i, 0.0)).rgb * WEIGHTS[i];
            color += texture(u_brightnessMap, uvs - vec2(texelSize.x * i, 0.0)).rgb * WEIGHTS[i];
        }
    }
    else 
    {
        for(int i = 1; i < 5; ++i) 
        {
            color += texture(u_brightnessMap, uvs + vec2(0.0, texelSize.y * i)).rgb * WEIGHTS[i];
            color += texture(u_brightnessMap, uvs - vec2(0.0, texelSize.y * i)).rgb * WEIGHTS[i];
        }
    }  
    out_fragment = vec4(color, 1.0);      
}

++FRAGMENT++