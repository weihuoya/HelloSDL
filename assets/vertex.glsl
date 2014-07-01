attribute vec4 a_position;
attribute vec2 a_texcoord;
attribute vec4 a_color;

uniform mat4 u_mvpmatrix;

varying vec4 v_color;
varying vec2 v_texcoord;

void main()
{
    v_color = a_color;
    gl_Position = u_mvpmatrix * a_position;
    v_texcoord = a_texcoord;
}
