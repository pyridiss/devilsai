uniform sampler2D texture;
uniform vec3 luminosity;

void main()
{
    vec2 pixel = gl_TexCoord[0].st;
    vec3 color = vec3(texture2D(texture, pixel));

    vec3 luminosityCoeffecient = vec3(0.2125, 0.7154, 0.0721);
    vec3 averageLuminosity = vec3(0.5, 0.5, 0.5);

    vec3 intensity = vec3(dot(color, luminosityCoeffecient));

    vec3 satColor = mix(intensity, color, 1.);
    vec3 conColor = mix(averageLuminosity, satColor, 1.3);

    vec3 final = vec3(conColor.x * luminosity.x, conColor.y * luminosity.y, conColor.z * luminosity.z);

    gl_FragColor = vec4(final, 1);
}
