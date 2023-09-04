// Source code of fragment shader
#version 130

// Attributes passed from the vertex shader
in vec4 color_interp;
in vec2 uv_interp;

// Texture sampler
uniform sampler2D onetex;
uniform int object_deceased;
uniform int isBackground;

void main()
{    

    // Sample texture
    vec4 color = texture2D(onetex, uv_interp);

    // If its the background set its tiling
    if(isBackground == 1){
        color = texture2D(onetex, vec2(uv_interp.x/0.1, uv_interp.y/0.1));
    }

    // Assign color to fragment
    gl_FragColor = vec4(color.r, color.g, color.b, color.a);

    // Set the color to grayscale if its deceased
    if(object_deceased == 1){
        gl_FragColor = vec4(color.r, color.r, color.r, color.a);
    }

    // Check for transparency
    if(color.a < 1.0)
    {
        discard;
    }    
}