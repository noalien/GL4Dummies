precision mediump float;
varying vec3 vsoNormal;
varying vec4 mpos;
varying vec2 vsoTexture;
uniform mat4 tinv_mmat;
uniform vec3 lum_pos;
uniform sampler2D mTex;
uniform int sky;
float myClamp(float v, float min, float max) {
    if(v > max) return max;
    if(v < min) return min;
    return v;
}
void main() {
    if(sky == 0) {
        float intensite;
        vec3 nnorm = normalize((tinv_mmat * vec4(vsoNormal, 0.0)).xyz);
        vec3 L = normalize(mpos.xyz - lum_pos);
        intensite = myClamp(dot(-L, nnorm), 0.0, 1.0);
        gl_FragColor = (0.3 + 1.0 * intensite) * texture2D(mTex, vsoTexture).bgra;
    } else
        gl_FragColor = 0.1 * texture2D(mTex, vsoTexture).bgra;
}