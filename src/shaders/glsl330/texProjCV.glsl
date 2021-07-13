#version 330

//https://stackoverflow.com/questions/33018652/how-to-use-opengl-to-emulate-opencvs-warpperspective-functionality-perspective

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 vertColor;
in vec4 fragColor;

//varying vec2 textureCoordinate;

uniform sampler2D inputImageTexture;

// NOTE: you will need to pass the INVERSE of the homography matrix, as well as 
// the width and height of your image as uniforms!
//uniform mat3 inverseHomographyMatrix;
uniform sampler2D invMatTex;
uniform float width;
uniform float height;
//uniform float numOfRecs;
uniform vec4 bgColor;

void test(){
    //gl_FragColor = vec4(coords,0,1.0f);
    //fragColor = vec4(fragTexCoord,0,1.0f);
    //fragColor = texture(inputImageTexture,fragTexCoord);
    //gl_FragColor = vec4(mod(fragTexCoord*5,1),0,1.0f);
}

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

    // Sample the texture if we're mapping within the image, otherwise set color to black
    if (coords.x >= 0.0 && coords.x <= 1.0 && coords.y >= 0.0 && coords.y <= 1.0) {
        gl_FragColor = texture2D(inputImageTexture, coords);
    } else {
        //gl_FragColor = vec4(0.0,0.0,0.0,0.0);
        //gl_FragColor = vec4(0.0,0.0,0.0,1.0);
        //gl_FragColor = vec4(1.0,0.0,1.0,1.0);
        gl_FragColor = bgColor;
    }
}

/*
// Texture coordinates will run [0,1],[0,1];
   // Convert to "real world" coordinates
   //vec3 frameCoordinate = vec3(textureCoordinate.x * width, textureCoordinate.y * height, 1.0);
   vec3 frameCoordinate = vec3(fragTexCoord.x * width, fragTexCoord.y * height, 1.0);

   // Determine what 'z' is
   vec3 m = inverseHomographyMatrix[0][2] * frameCoordinate;
   float zed = 1.0 / (m.x + m.y + m.z);
   frameCoordinate = frameCoordinate * zed;

   // Determine translated x and y coordinates
   float xTrans = inverseHomographyMatrix[0][0][0] * frameCoordinate.x + inverseHomographyMatrix[0][0][1] * frameCoordinate.y + inverseHomographyMatrix[0][0][2] * frameCoordinate.z;
   float yTrans = inverseHomographyMatrix[0][1][0] * frameCoordinate.x + inverseHomographyMatrix[0][1][1] * frameCoordinate.y + inverseHomographyMatrix[0][1][2] * frameCoordinate.z;

   // Normalize back to [0,1],[0,1] space
   vec2 coords = vec2(xTrans / width, yTrans / height);

   //test();

   //fragColor = texture2D(inputImageTexture,fragTexCoord);
*/