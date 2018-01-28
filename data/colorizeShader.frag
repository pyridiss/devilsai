uniform sampler2D texture;
uniform vec3 red;
uniform vec3 green;
uniform vec3 blue;

void main()
{
    vec2 pixel = gl_TexCoord[0].st;
    vec3 color = vec3(texture2D(texture, pixel));

    vec3 final = vec3(color.x * red.x + color.y * green.x + color.z * blue.x,
                      color.x * red.y + color.y * green.y + color.z * blue.y,
                      color.x * red.z + color.y * green.z + color.z * blue.z);

    gl_FragColor = vec4(final, texture2D(texture, pixel).a);
}
