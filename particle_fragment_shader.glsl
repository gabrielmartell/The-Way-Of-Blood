// Source code of fragment shader
#version 130

// Attributes passed from the vertex shader
in vec4 color_interp;
in vec2 uv_interp;

// Texture sampler
uniform sampler2D onetex;
uniform int type;
uniform float time;

void main()
{
    // Sample texture
    vec4 color = texture2D(onetex, uv_interp);

    // Assign color to fragment
    if (type == 0){
        color.rgb = vec3(0.1,0.1,0.1) * color_interp.r;
        gl_FragColor = vec4(color.r, color.g, color.b, color.a);
    } else if (type == 3){
        color.rgb = vec3(0.1,0.1,0.0) * color_interp.r;
        gl_FragColor = vec4(color.r, color.g, color.b, color.a);
    } else if (type == 2){
        color.rgb = vec3(1.0,0.1,0.0) * color_interp.r;
        gl_FragColor = vec4(color.r, color.g, color.b, color.a);
  
    } else {
        color.rgb = vec3(10.0,0.1,0.1) * color_interp.r;
        gl_FragColor = vec4(color.r, color.g, color.b, color.a);
    }

    // Check for transparency
    if(color.a < 1.0)
    {
         discard;
    }
}
