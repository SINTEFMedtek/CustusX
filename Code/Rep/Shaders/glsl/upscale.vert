const vec2 madd=vec2(0.5,0.5);
varying vec2 textureCoord;
void main() {
   gl_TexCoord[0].xy = gl_Vertex.xy*madd+madd; // scale vertex attribute to [0-1] range
   gl_Position = vec4(gl_Vertex.xy,0.0,1.0);
}
