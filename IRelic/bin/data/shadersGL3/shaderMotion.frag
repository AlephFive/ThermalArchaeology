#version 150

// these are our textures
uniform sampler2DRect tex0;
uniform sampler2DRect motionTex;

// this comes from the vertex shader
in vec2 texCoordVarying;

// this is the output of the fragment shader
out vec4 outputColor;

void main()
{
    // get rgba from existTex
    vec4 src = texture(tex0, texCoordVarying).rgba;

    // get rgba from motiontex
    vec4 motion = texture(motionTex, texCoordVarying).rgba;
    if(src.r>motion.r){outputColor=src;}
    else{outputColor=motion;}
    
    //mix the rgb from tex0 with the alpha of the mask
    
}

