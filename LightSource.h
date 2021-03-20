#include <iostream>

class LightSource {
public:
    LightSource(Vec3f p = {0,0,0}, Vec3f c = {1,1,1}, float i = 0.5){
        pos = p;
        color = c;
        intensity = i;
    }
    Vec3f pos;
    Vec3f color;
    float intensity;
};
