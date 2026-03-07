/*!\file effet.fs
 *
 * \brief rendu avec effet sur texture 2D.
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr 
 * \date May 14 2018
 */
#version 330
uniform sampler2D tex;
in vec2 vsoTexCoord;

out vec4 fragColor;
uniform float temps;    

float rand(float v){
    return fract(sin(v * 12.9898) + 43758.5453123);
}

float trunc2(float v,float num) {
    return floor(v * num) / num;
}

const float kSameWidthLinesWidth = 16.0; 
const float kSameWidthChangeTimesPerSec = 4.0; 
const float kChangeTimesPerSec = 24.0; 
const float kThreshold = 0.4;    
const float kStrength = 0.5;
const float kGlitchBlendFactor = 1.0;




uniform vec3      iResolution;  
//uniform float     iTime;        
uniform float     iTimeDelta;   
uniform float     iFrameRate;   
uniform int       iFrame;       
uniform float     iChannelTime[4];
uniform vec3      iChannelResolution[4];
uniform vec4      iMouse;               
//uniform samplerXX iChannel0..3;         
uniform vec4      iDate;                
uniform float     iSampleRate;          


void main(void) {
  vec2 uv = vsoTexCoord;
    float v = rand(floor(uv.y * kSameWidthLinesWidth) + rand(floor(temps * kSameWidthChangeTimesPerSec)));
    float changeSeed = floor(temps * kChangeTimesPerSec * v);
    float yTrunc = uv.y + changeSeed;
    float randV = 0.25 * rand(trunc2(yTrunc,12.0));
    randV += 0.25 * rand(trunc2(yTrunc,6.0));
    randV += 0.25 * rand(trunc2(yTrunc,11.0));
    randV += 0.25 * rand(trunc2(yTrunc,1.5));
    randV = randV * 2.0 - 1.0;
    randV = sign(randV) * clamp((abs(randV) - kThreshold),0.0,1.0) * kStrength;
    vec2 glitchUV = abs(uv + vec2(0.1 * randV,0.0));
    vec4 originColor = texture(tex,uv);
    float r = texture(tex,uv + vec2(randV,0.0)).r;
    float g = texture(tex,uv - vec2(randV,0.0)).g;
    float b = texture(tex,uv ).b;
    vec4 glitchColor = vec4(r,g,b,1.0);
    
    if(uv.x < 0.9){
        fragColor = mix(originColor,glitchColor,kGlitchBlendFactor);
    }
    else {
         float v = randV * 40.0;
         fragColor = vec4(v,v,v,1.0);
    }
}
