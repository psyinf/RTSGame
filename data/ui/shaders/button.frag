varying vec4 color;

void main()
{
    vec4 c = clamp(color, 0.0, 1.0);

    gl_FragColor = c;
}
