attribute vec4 a_position;
attribute vec2 a_texcoord;
attribute vec4 a_color;
uniform mat4 a_projection;

varying vec4 v_color;
varying vec2 v_texcoord;

void main()
{
    v_color = a_color;
    gl_Position = a_position * a_projection;
    //gl_Position = a_position;
    v_texcoord = a_texcoord;
}
