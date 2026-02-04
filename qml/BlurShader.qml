import QtQuick 2.0

ShaderEffect
{
    objectName: "BlurShader"
    property int radius: 12
    property var src
    property var pixelStep: Qt.vector2d(1/src.width, 1/src.height)


    blending: false
    fragmentShader: "
        layout(location = 0) in vec2 qt_TexCoord0;
        layout(location = 0) out vec4 fragColor;

        layout(std140, binding = 0) uniform buf {
            mat4 qt_Matrix;
            float qt_Opacity;
            vec2 pixelStep;
            int radius;
        };
        layout(binding = 1) uniform sampler2D src;

        void main(void)
        {
            vec3 sum = vec3(0.0, 0.0, 0.0);
            for(int x = -radius; x <= radius; ++x) {
                for(int y = -radius; y <= radius; ++y) {
                    vec2 c = qt_TexCoord0 + vec2(x, y) * pixelStep;
                    sum += texture(src, c).rgb;
                }
            }

            fragColor = vec4(sum / ((radius*2 + 1) * (radius*2 + 1)), 1.0) * qt_Opacity;
        }
    "
}
