#include "Vec3.h"

class Material {
public:
    Material(){

    }
    Vec3f evaluateColorResponse(const Vec3f & normal, const Vec3f & wi) const{
        // returns Lo(wo)
        Vec3f Li_wi = {0.4,0.4,0.4};
        float k_d = 2.5;
        float f_wi_wo = k_d/M_PI;
        Vec3f Lo_wo = Li_wi * f_wi_wo * dot(normal,wi);
        return Lo_wo;
    }
};
