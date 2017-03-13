uniform mat4 mmat, vmat;
uniform mat4 projmat;
attribute vec3 vPosition;
attribute vec2 vTexture;
varying vec4 mpos;
varying vec3 vsoNormal;
varying vec2 vsoTexture;
void main() {
    mpos = mmat * vec4(vPosition, 1.0);
    vsoNormal = vPosition;
    gl_Position = projmat * vmat * mpos;
    vsoTexture = vTexture;
}