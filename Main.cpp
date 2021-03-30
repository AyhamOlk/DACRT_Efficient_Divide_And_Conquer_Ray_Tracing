#define _USE_MATH_DEFINES

#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <vector>
#include <algorithm>
#include <exception>

#include "CommandLine.h"
#include "Image.h"
#include "Ray.h"
#include "Camera.h"
#include "Mesh.h"
#include "Scene.h"
#include "RayTracer.h"
#include "AABB.h"
#include "DACRT.h"

using namespace std;


/*void test_DACRT(){
    std::vector<MyTriangle> Triangles;
    Vec3f a = {1,1,1}, b = {2,2,2}, c = {1,2,1};
    Vec3f step = {1,0,0};

    for(int i=0;i<5;i++){
        Triangles.push_back(MyTriangle(a, b, c, 0));
        a += step;
        b += step;
        c += step;
    }

    AABB Volume({0,0,0}, {10,10,10});
    int k = 10;
    vector<Ray> Rays;
    Scene scene;
    DACRT_Algorithms algo(Volume,Triangles, Rays, scene);
    vector<BinStats> bins = algo.binningSubdivision(k);

    for(int i=0; i<bins.size(); i++) {
        std::cout<<"binsStats: "<<i<<" ";
        bins.at(i).print();
        std::cout<<"\n\n";
    }
}*/

void foo(AABB Volume, vector<MyTriangle> Triangles, vector<Ray> Rays, const Scene& scene, Image& image) {

}

// Usage: argv[0] [-o file.ppm]
int main (int argc, char ** argv) {

    // Parsing the command line arguments
    CommandLine args;
    if (argc > 1) {
        try {
            args.parse(argc, argv);
        } catch (const std::exception & e) {
            std::cerr << e.what() << std::endl;
            args.printUsage (argv[0]);
            exit(1);
        }
    }


    //Testing Cube binning
    /*  DACRT_Algorithms Algo;
      AABB cube(Vec3f{0,0,0}, Vec3f{20,20,20});
      vector<AABB> bins = Algo.binningSubdivision(cube, 20);

      for(auto it = bins.begin(); it != bins.end(); it++) {
          std::cout<<"bin min: "<<it->min <<", max: "<< it->max << " \n";
      }*/

    //test_DACRT();

    // Initialization

    //Image image (args.width (), args.height ());
    Image image (480, 270);


    Scene scene;
    Camera camera(Vec3f(0.f, 0.f, 1.f),
                  Vec3f(),
                  Vec3f(0.f, 1.f, 0.f),
                  60.f,
                  float(args.width()) / args.height());

    scene.camera() = camera;

    //LightSource lightSource(Vec3f(0,-2,-1), Vec3f(1,1,1), 20);
    //LightSource lightSource(Vec3f(1,1,2), Vec3f(1,1,1), 0.2);
    LightSource lightSource(Vec3f(0,0,3), Vec3f(1,1,1), 0.2);
    // Loading a mesh

    Mesh mesh;
    Mesh floor;
    try {
        //mesh.loadOFF("example.off");
        //mesh.loadOFF("highResFace.off");
        mesh.loadOFF("rhino.off");
        floor.loadOFF("floor.off");

    }
    catch (const std::exception & e) {
        std::cerr << e.what() << std::endl;
        exit(1);
    }




    scene.meshes ().push_back (mesh);
    scene.meshes().push_back(floor);

    // Rendering OLD
    /*RayTracer rayTracer;
    image.fillBackground ();
    std::cout << "Ray tracing: starts";
    rayTracer.render (scene, image);
    std::cout << "ends." << std::endl;
    image.savePPM (args.outputFilename ());*/



    //MY renderring
    size_t w = image.width();
    size_t h = image.height();
    //const Camera& camera = scene.camera();

    //auto& mesh = scene.meshes()[0];
    auto& P = mesh.vertexPositions();
    auto& T = mesh.indexedTriangles();


    //create rays and triangles and volumes
    //To fill AABB we loop over vertices and find the min min and max max
    Vec3f min = {1000,1000,1000};
    Vec3f max = {-1000,-1000,-1000};
    for(int i =0; i<P.size(); i++) {
        if(P.at(i)[0]<min[0]) min[0] = P.at(i)[0];
        if(P.at(i)[1]<min[1]) min[1] = P.at(i)[1];
        if(P.at(i)[2]<min[2]) min[2] = P.at(i)[2];

        if(P.at(i)[0]>max[0]) max[0] = P.at(i)[0];
        if(P.at(i)[1]>max[1]) max[1] = P.at(i)[1];
        if(P.at(i)[2]>max[2]) max[2] = P.at(i)[2];
    }
    std::cout<<"\nmin "<<min<<" max: "<<max;
    max = {1,1,1};
    min = {-1,-1,-1};
    AABB Volume(min, max);
    vector<MyTriangle> Triangles;
    vector<Ray> Rays;

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            Ray r = camera.rayAt (float (x) / w, 1.f - float (y) / h);
            r.x = x;
            r.y = y;
            Rays.push_back(r);
        }
    }

    //Checking the ray vec
    /*for(int i=0; i<Rays.size(); i++) {
        std::cout<<Rays.at(i).x<<" "<<Rays.at(i).y<<"\n";
    }*/

    std::cout<<"\nDONE with Rays";
    Vec3f A = {0,0,0};
    Vec3f B = {0,0,0};
    Vec3f C = {0,0,0};

    for(int f=0; f<T.size(); f++) {
        A[0] = P.at(T.at(f)[0])[0];
        A[1] = P.at(T.at(f)[0])[1];
        A[2] = P.at(T.at(f)[0])[2];

        B[0] = P.at(T.at(f)[1])[0];
        B[1] = P.at(T.at(f)[1])[1];
        B[2] = P.at(T.at(f)[1])[2];

        C[0] = P.at(T.at(f)[2])[0];
        C[1] = P.at(T.at(f)[2])[1];
        C[2] = P.at(T.at(f)[2])[2];


        Triangles.push_back(MyTriangle(A, B, C, f));

    }
    std::cout<<"\nDONE triangles";
    std::cout<<"\nTotal number of Rays: "<<Rays.size();
    std::cout<<"\nTotal number of Triangles: "<<Triangles.size()<<"\n\n\n";

    vector<MyTriangle> Triangles2;
    vector<Ray> Rays2;

    image.fillBackground ();
    DACRT_Algorithms algo(Volume, Triangles, Rays, scene, image);
    algo.DACRT(Volume, Triangles, Rays);
    std::cout<<"\n\nCounter "<<algo.globalCounter;

    std::cout<<"\nSaving Image";
    algo.image.savePPM ("DACRT.ppm");

//-----
    //Old COde;
    RayTracer rayTracer;
    //rayTracer.render (scene, image);
    //image.savePPM ("NO_DACRT.ppm");


    return 0;
}



//-----------------------------------------------------------------------------------------