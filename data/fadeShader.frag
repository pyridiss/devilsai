uniform sampler2D texture;
uniform float value;

void main()
{
    float alpha = min(texture2D(texture, gl_TexCoord[0].st).a, value);
    gl_FragColor = vec4(vec3(texture2D(texture, gl_TexCoord[0].st)), alpha);
}
