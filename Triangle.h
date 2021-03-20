//
// Created by Ayham on 05.03.21.
//
#include "Vec3.h"
#include "AABB.h"
#include <iostream>
#ifndef MYRAYTRACER_TRIANGLE_H
#define MYRAYTRACER_TRIANGLE_H


//MyTriangle used in DACRT
class MyTriangle{
public:
    Vec3f a, b, c;
    Vec3f center;
    size_t triangleIndex;
    AABB aabb = AABB(Vec3f{0,0,0}, Vec3f{0,0,0});

    inline MyTriangle(Vec3f &A, Vec3f &B, Vec3f &C, size_t triangleIndex) : a(A), b(B), c(C) {
        center = get_center();
        aabb = get_AABB();
        this->triangleIndex = triangleIndex;
    }

    Vec3f get_center(){
        Vec3f center = {0.,0.,0.};
        for(int i=0;i<3;i++)
            center[i] = (a[i]+b[i]+c[i])/3.;
        return center;
    }

    AABB get_AABB(){
        //aabb = AABB(Vec3f{0,0,0}, Vec3f{0,0,0});
        for(int i=0;i<3;i++){
            aabb.min[i] = std::min(std::min(a[i], b[i]), c[i]);
            aabb.max[i] = std::max(std::max(a[i], b[i]), c[i]);
        }
        return aabb;
    }

    size_t getFaceIndex() {
        return triangleIndex;
    }
};

#endif //MYRAYTRACER_TRIANGLE_H
