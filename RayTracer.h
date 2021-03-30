#pragma once

#include <random>
#include <cmath>
#include <algorithm>
#include <limits>

#include "Vec3.h"
#include "Image.h"
#include "Camera.h"
#include "Scene.h"
#include "Material.h"
#include "Triangle.h"
//#include "DACRT.h"
#define ROW 480
#define COLUMN 270
using namespace std;

class RayTracer {
public:
    RayTracer () {}
    virtual ~RayTracer() {}

    // Returns true if an intersection found, filling the barycentric coordinate {u,v} and the distance d in this case.
    inline bool rayTrace (const Ray & ray,
                          const Scene & scene,
                          size_t & meshIndex,
                          size_t & triangleIndex,
                          float & u,
                          float & v,
                          float & d) {
        const auto& meshes = scene.meshes();
        float closest = std::numeric_limits<float>::max();
        bool intersectionFound = false;
        for (size_t mIndex = 0; mIndex < meshes.size(); mIndex++) {
            const auto& P = meshes[mIndex].vertexPositions();
            const auto& T = meshes[mIndex].indexedTriangles();
            for (size_t tIndex = 0; tIndex < T.size(); tIndex++) {
                const Vec3i& triangle = T[tIndex];
                float ut, vt, dt;
                if (ray.triangleIntersect(P[triangle[0]], P[triangle[1]], P[triangle[2]], ut, vt, dt) == true) {
                    if (dt > 0.f && dt < closest) {
                        intersectionFound = true;
                        closest = dt;
                        meshIndex = mIndex;
                        triangleIndex = tIndex;
                        u = ut;
                        v = vt;
                        d = dt;
                    }
                }
            }
        }
        return intersectionFound;
    }

    // Returns the color response associated to point defined as a barycentric interpolation over a triangle of a mesh in the scene.
    inline Vec3f shade (Scene scene, size_t meshIndex, size_t triangleIndex, float u, float v) {
        const auto& mesh = scene.meshes()[meshIndex];
        const auto& P = mesh.vertexPositions();
        const auto& N = mesh.vertexNormals();
        const Vec3i& triangle = mesh.indexedTriangles()[triangleIndex];
        Vec3f hitNormal = normalize((1.f - u - v) * N[triangle[0]] + u * N[triangle[1]] + v * N[triangle[2]]);

        // the starting point vector
        Vec3f L_pos = scene.light_source().pos;
        Vec3f hitPoint = normalize((1.f - u - v) * P[triangle[0]] + u * P[triangle[1]] + v * P[triangle[2]]);

        //Other way
        Vec3f wi = hitPoint-L_pos;
        Material mat;
        Vec3f wo = mat.evaluateColorResponse(hitNormal, wi);
        return wo;

        // No shading so far, only normal visualization
        //return hitNormal/2.f;
        //return Vec3f(0.5f, 0.5f, 0.5f) + hitNormal / 2.f;
    }


    // Fills the image pixels with color response which are ray traced in the scene
    inline void render (const Scene& scene, Image& image) {
        size_t w = image.width();
        size_t h = image.height();
        const Camera& camera = scene.camera();
        for (int y = 0; y < h; y++) {
            static int progress = 0;
            progress++;
            if (progress % 10 == 0)
                std::cout << ".";
#pragma omp parallel for
            for (int x = 0; x < w; x++) {
                Vec3f colorResponse;
                Ray ray = camera.rayAt (float (x) / w, 1.f - float (y) / h);
                size_t meshIndex, triangleIndex;
                float u, v, d;
                bool intersectionFound = rayTrace (ray, scene, meshIndex, triangleIndex, u, v, d);
                if (intersectionFound && d > 0.f)
                    image (x,y) = shade (scene, meshIndex, triangleIndex, u, v);

            }
        }
    }








    //Our adapted render function
    //TODO ray sampling is needed here
    inline void myRender (const Scene& scene, Image& image, vector<Ray> Rays, int &counter, bool (&visited)[ROW][COLUMN]) {
        size_t w = image.width();
        size_t h = image.height();
        const Camera& camera = scene.camera();

#pragma omp parallel for
        for(int i = 0; i<Rays.size(); i++) {
            if(visited[Rays.at(i).x][Rays.at(i).y])
                continue;
            visited[Rays.at(i).x][Rays.at(i).y] = true;
            float u, v, d;
            size_t meshIndex, triangleIndex;
            Ray ray = camera.rayAt (float (Rays.at(i).x) / w, 1.f - float (Rays.at(i).y) / h);
            bool intersectionFound = rayTrace (ray, scene, meshIndex, triangleIndex, u, v, d);
            if (intersectionFound && d > 0.f && meshIndex==0){

                image (Rays.at(i).x, Rays.at(i).y) = shade (scene, meshIndex, triangleIndex, u, v);

                counter++;

            }
            if (intersectionFound && meshIndex==1){

                image (Rays.at(i).x, Rays.at(i).y) = {1,0,0};
                /*Ray ray_to_source(ray.m_origin + d*ray.m_direction, normalize(- scene.m_light_source.pos + ray.m_origin + d*ray.m_direction));
                bool inter2 = rayTrace (ray_to_source, scene, meshIndex, triangleIndex, u, v, d);
                if(inter2 && meshIndex==0) {
                    image (Rays.at(i).x, Rays.at(i).y) = {0.2,0.3,0.2};
                    //std::cout<<"Intersecton \t";
                }
                else {
                    image (Rays.at(i).x, Rays.at(i).y) = {1,0,0};
                    //std::cout<<"NO intersecton \t";
                }*/

            }
        }
        //image.savePPM ("DACRT2");
    }





    //------------------------------------------------------------------------------------------




    inline bool rayTrace_skip (const Ray & ray,
                               const Scene & scene,
                               size_t & meshIndex,
                               size_t & triangleIndex,
                               float & u,
                               float & v,
                               float & d, vector<MyTriangle> Triangles) {

        const auto& meshes = scene.meshes();
        float closest = std::numeric_limits<float>::max();
        bool intersectionFound = false;
        for (size_t mIndex = 0; mIndex < meshes.size(); mIndex++) {
            //int mIndex = 0;
            const auto& P = meshes[mIndex].vertexPositions();
            const auto& T = meshes[mIndex].indexedTriangles();
            for (size_t tIndex = 0; tIndex < Triangles.size(); tIndex++) {
                const Vec3i& triangle = T[Triangles.at(tIndex).triangleIndex];
                float ut, vt, dt;
                if (ray.triangleIntersect(P[triangle[0]], P[triangle[1]], P[triangle[2]], ut, vt, dt) == true) {
                    if (dt > 0.f && dt < closest) {
                        intersectionFound = true;
                        closest = dt;
                        meshIndex = mIndex;
                        triangleIndex = Triangles.at(tIndex).triangleIndex;
                        u = ut;
                        v = vt;
                        d = dt;
                        //return true;
                    }
                }
            }
          }
        return intersectionFound;
    }



    //Our adapted render function
    //TODO ray sampling is needed here
    inline void myRender_skip (const Scene& scene, Image& image, vector<Ray> Rays, int &counter, bool (&visited)[ROW][COLUMN], vector<MyTriangle> Triangles) {
        size_t w = image.width();
        size_t h = image.height();
        const Camera& camera = scene.camera();

#pragma omp parallel for
        for(int i = 0; i<Rays.size(); i++) {
           /* if(visited[Rays.at(i).x][Rays.at(i).y]) {

                continue;
            }*/
            visited[Rays.at(i).x][Rays.at(i).y] = true;
            counter++;

            float u, v, d;
            size_t meshIndex, triangleIndex;

            //check

            float u2, v2, d2;
            size_t meshIndex2, triangleIndex2;


            //Ray ray = camera.rayAt (float (Rays.at(i).x) / w, 1.f - float (Rays.at(i).y) / h);
            bool intersectionFound = rayTrace_skip(Rays.at(i), scene, meshIndex, triangleIndex, u, v, d, Triangles);
            //intersectionFound = rayTrace (Rays.at(i), scene, meshIndex2, triangleIndex2, u2, v2, d2);
            /*if((meshIndex != meshIndex || triangleIndex != triangleIndex2) && intersectionFound) {

                bool inside = false;
                for(int i =0; i <Triangles.size(); i++) {
                    if(triangleIndex2 == Triangles.at(i).triangleIndex) {
                        inside = true;
                        break;
                    }
                }
                std::cout<<"\nTriangle index: "<<triangleIndex<<" correct: "<<triangleIndex2<< " --- is inside: "<<inside;
            }*/

            if (intersectionFound && d > 0.f ){

                //image (Rays.at(i).x, Rays.at(i).y) = {1,0,0};
                image (Rays.at(i).x, Rays.at(i).y) = shade (scene, meshIndex, triangleIndex, u, v);



            }
            /*if (intersectionFound && meshIndex==1){


                Ray ray_to_source(ray.m_origin + d*ray.m_direction, normalize(- scene.m_light_source.pos + ray.m_origin + d*ray.m_direction));
                bool inter2 = rayTrace (ray_to_source, scene, meshIndex, triangleIndex, u, v, d);
                if(inter2 && meshIndex==0) {
                    image (Rays.at(i).x, Rays.at(i).y) = {0.2,0.3,0.2};
                    //std::cout<<"Intersecton \t";
                }
                else {
                    image (Rays.at(i).x, Rays.at(i).y) = {1,0,0};
                    //std::cout<<"NO intersecton \t";
                }

            }*/
        }

    }
};