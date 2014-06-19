varying mediump vec4 v_color;
varying mediump vec2 v_texcoord;

uniform sampler2D u_texture;

void main()
{
    //gl_FragColor = v_color;
    gl_FragColor = v_color * texture2D(u_texture, v_texcoord);
}

