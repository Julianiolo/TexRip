#version 110

//https://stackoverflow.com/questions/33018652/how-to-use-opengl-to-emulate-opencvs-warpperspective-functionality-perspective

in vec2 fragTexCoord;
in vec4 vertColor;
in vec4 fragColor;

uniform sampler2D inputImageTexture;

uniform sampler2D invMatTex;
uniform float width;
uniform float height;
uniform vec4 bgColor;

void main()
{
    float ind = fragColor.x;
    mat3 inverseHomographyMatrix;
    inverseHomographyMatrix[0] = texture2D(invMatTex,vec2(ind,.5/3)).xyz;
    inverseHomographyMatrix[1] = texture2D(invMatTex,vec2(ind,1.5/3)).xyz;
    inverseHomographyMatrix[2] = texture2D(invMatTex,vec2(ind,2.5/3)).xyz;
    
    
    vec3 frameCoordinate = vec3(fragTexCoord.x * width, fragTexCoord.y * height, 1.0); 
    vec3 trans = inverseHomographyMatrix * frameCoordinate; 
    vec2 coords = vec2(trans.x / width, trans.y / height) / trans.z;

    // Sample the texture if we're mapping within the image, otherwise set color to bgColor
    if (coords.x >= 0.0 && coords.x <= 1.0 && coords.y >= 0.0 && coords.y <= 1.0) {
        gl_FragColor = texture2D(inputImageTexture, coords);
    } else {
        gl_FragColor = bgColor;
    }
}