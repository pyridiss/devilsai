uniform sampler2D texture;
uniform vec2 textureSize;

void main()
{
    vec2 pixel = gl_TexCoord[0].xy;

    vec4 color             = texture2D(texture, pixel);
    vec4 colorLeft         = texture2D(texture, vec2(pixel.x - 1.0/textureSize.x, pixel.y));
    vec4 colorRight        = texture2D(texture, vec2(pixel.x + 1.0/textureSize.x, pixel.y));
    vec4 colorTop          = texture2D(texture, vec2(pixel.x - 1.0/textureSize.x, pixel.y - 1.0/textureSize.y));
    vec4 colorBottom       = texture2D(texture, vec2(pixel.x                    , pixel.y + 1.0/textureSize.y));
    vec4 colorLeftLeft     = texture2D(texture, vec2(pixel.x - 2.0/textureSize.x, pixel.y));
    vec4 colorLeftTop      = texture2D(texture, vec2(pixel.x - 1.0/textureSize.x, pixel.y - 1.0/textureSize.y));
    vec4 colorLeftBottom   = texture2D(texture, vec2(pixel.x - 1.0/textureSize.x, pixel.y + 1.0/textureSize.y));
    vec4 colorRightRight   = texture2D(texture, vec2(pixel.x + 2.0/textureSize.x, pixel.y));
    vec4 colorRightTop     = texture2D(texture, vec2(pixel.x + 1.0/textureSize.x, pixel.y - 1.0/textureSize.y));
    vec4 colorRightBottom  = texture2D(texture, vec2(pixel.x + 1.0/textureSize.x, pixel.y + 1.0/textureSize.y));
    vec4 colorTopTop       = texture2D(texture, vec2(pixel.x                    , pixel.y - 2.0/textureSize.y));
    vec4 colorBottomBottom = texture2D(texture, vec2(pixel.x                    , pixel.y + 2.0/textureSize.y));

    gl_FragColor = 1.0/15.0 * (color + colorLeft + colorRight + colorTop + colorBottom + colorLeftLeft + colorLeftTop
                               + colorLeftBottom + colorRightRight + colorRightTop + colorRightBottom + colorTopTop
                               + colorBottomBottom);
}
