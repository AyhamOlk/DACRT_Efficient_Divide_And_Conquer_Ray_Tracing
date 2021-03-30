//
// Created by ghosto on 3/5/21.
//
#pragma once
#include "AABB.h"
#include<vector>
#include "Triangle.h"
#include "BinStats.h"
#include "RayTracer.h"
#include <bits/stdc++.h>
#include <bits/stdc++.h>

using namespace std;
#ifndef MYRAYTRACER_DACRT_H
#define MYRAYTRACER_DACRT_H

#endif //MYRAYTRACER_DACRT_H

//COSTS
const float Ct = 1;
const float Ci = 1;

const int RAYS_THRESHOLD = 30;
const int TRIANGLES_THRESHOLD = 30;
const int NB_SUBDIVISIONS = 10;

class DACRT_Algorithms {
public:
    //AABB Volume;
    //vector<Ray> Rays;
    //vector<MyTriangle> Triangles;
    Scene scene;
    RayTracer rayTracer;
    Image image;
    int globalCounter = 0;
    bool debug = false;

    #define ROW 480
    #define COLUMN 270
    //bool grid[580][270];
    bool visited [ROW][COLUMN];
    DACRT_Algorithms(AABB Volume, vector<MyTriangle> Triangles, vector<Ray> Rays, const Scene& scene, Image& image ) { //add Rays vector<Ray> Rays in parameters
        //this->Volume = Volume;
        //this->Rays = Rays;
        //this->Triangles = Triangles;
        this->scene = scene;
        this->image = image;
        for(int i=0; i<ROW; i++) {
            for(int j=0; j<COLUMN; j++) {
                visited[i][j] = false;
            }
        }
    }

    //The recursive function that is similar to the code in class
    void DACRT(AABB Volume, vector<MyTriangle> Triangles, vector<Ray> allRays) {
        if(debug)
            std::cout<<"\n\n\n                  DACRT called with "<<Triangles.size()<<" Trinagles and "<<allRays.size()<<" Rays";
        int Nr = allRays.size();
        int Nt = Triangles.size();
        int Ns = 0;
        int k = NB_SUBDIVISIONS;


        //Need to sample if nb of rays?1000
        vector<Ray> Rays;

        if(Nr>1000) {
            //need to sample
            Ns = 100;
            vector<int> picks = pick(Nr-1, Ns);
            for(int i =0; i<picks.size(); i++) {
                Rays.push_back(allRays.at(picks.at(i)));
            }
        }
        else {
            Rays = allRays;
            Ns = Nr;
        }


        //Base condition
        if(Nr < RAYS_THRESHOLD || Nt < TRIANGLES_THRESHOLD) {
            if(debug)
                std::cout<<"\nCalling Naiive Render";
            //rayTracer.myRender_skip(scene, image, allRays, globalCounter, visited, Triangles);
            rayTracer.myRender(scene, image, allRays, globalCounter, visited);
            return;
            //TODO Need to call NAIIVE RT which is the old ray tracer code
        }

        //Finding the stats of the bins
        vector<BinStats> bins = binningSubdivision(Volume, Triangles, k);
        if(debug)
            std::cout<<"\nDone binning";
        //Inintialzing vectors
        vector<int> cLeft, cRight, nLeft, nRight;

        for(int i=0; i<k; i++) {
            cLeft.push_back(0);
            cRight.push_back(0);
            nLeft.push_back(0);
            nRight.push_back(0);
        }

        //Looping over rays and filling vectors
        for(int i = 0; i<Rays.size(); i++) {
            intersectRayWithVolumes(Rays.at(i), bins, cLeft, cRight, nLeft, nRight);
        }
        if(debug)
            std::cout<<"\nFilled parameters";
        /*for(int i=0; i<k; i++) {
            std::cout<<"\n"<<"cLeft: "<<cLeft.at(i)<<" cRight : "<<cRight.at(i)<<" nLeft : "<<nLeft.at(i)<<" nRight : "<<nRight.at(i);
            std::cout<<"\n";
        }*/
        //Cost function division
        float Cmin = MAXFLOAT;
        int i_min = 0;
        float C = 0;

        for(int i=0; i<k; i++) {
            bins.at(i).L_alpha = float(cLeft.at(i))/float(Ns + 0.001);
            bins.at(i).R_alpha = float(cRight.at(i))/float(Ns + 0.001);

            //cost function calculation
            C = Ct + Ci*(bins.at(i).L_alpha*bins.at(i).L_triangles_indices.size() +
                    bins.at(i).R_alpha*bins.at(i).R_triangles_indices.size());
            if(debug)
                std::cout<<"    C is "<<C;
            //bins.at(i).print();
            if(C <= Cmin) {
                Cmin = C;
                i_min = i;
            }
        }
        if(debug)
            std::cout<<"\nCost function calculated where Cmin is "<<Cmin<<" and jmin is"<<i_min;
        //Traversal order
        AABB firstChildVolume;
        vector<MyTriangle> firstChildTriangles;
        float firstAlpha = 0;

        AABB secondChildVolume;
        vector<MyTriangle> secondChildTriangles;
        float secondAlpha = 0;


        //TODO remove triangle structure and use indices instead maybe?
        if(nLeft.at(i_min) >= nRight.at(i_min)) {

            //first order
            firstChildVolume = bins.at(i_min).L_volume;

            for(auto it = bins.at(i_min).L_triangles_indices.begin(); it != bins.at(i_min).L_triangles_indices.end(); it++) {
                firstChildTriangles.push_back(Triangles.at(*it));
            }
            firstAlpha = bins.at(i_min).L_alpha;

            //setting second order
            secondChildVolume = bins.at(i_min).R_volume;

            for(auto it = bins.at(i_min).R_triangles_indices.begin(); it != bins.at(i_min).R_triangles_indices.end(); it++) {
                secondChildTriangles.push_back(Triangles.at(*it));
            }
            secondAlpha = bins.at(i_min).R_alpha;
        }
        else {
            firstChildVolume = bins.at(i_min).R_volume;

            for(auto it = bins.at(i_min).R_triangles_indices.begin(); it != bins.at(i_min).R_triangles_indices.end(); it++) {
                firstChildTriangles.push_back(Triangles.at(*it));
            }
            firstAlpha = bins.at(i_min).R_alpha;

            //setting second order
            secondChildVolume = bins.at(i_min).L_volume;

            for(auto it = bins.at(i_min).L_triangles_indices.begin(); it != bins.at(i_min).L_triangles_indices.end(); it++) {
                secondChildTriangles.push_back(Triangles.at(*it));
            }
            secondAlpha = bins.at(i_min).L_alpha;
        }
        if(debug)
            std::cout<<"\nTraversal order found";


        //Ray filtering


        vector<Ray> R0;
        vector<Ray> R1;
        float a,b;

        if(firstAlpha>0.5 ){ //approximation

            if(debug)
                std::cout<<"\nSkip filterring not applied";
            DACRT(firstChildVolume, firstChildTriangles, allRays);
        }
        else{
            for(int i=0; i<allRays.size(); i++) {
                if(firstChildVolume.intersect(allRays.at(i),a,b)) {
                    R0.push_back(allRays.at(i));
                }
            }

            if(debug)
                std::cout<<"\nSkip filterring applied";
            DACRT(firstChildVolume, firstChildTriangles, R0);
        }

        //second child
        if(secondAlpha>0.5 ){ //approximation

            if(debug)
                std::cout<<"\nSkip filterring not applied";
            DACRT(secondChildVolume, secondChildTriangles, allRays);
        }
        else{
            for(int i=0; i<allRays.size(); i++) {
                if(secondChildVolume.intersect(allRays.at(i),a,b)) {
                    R1.push_back(allRays.at(i));
                }
            }
            if(debug)
                std::cout<<"\nSkip filterring applied";
            DACRT(secondChildVolume, secondChildTriangles, R1);
        }

    }

    void intersectRayWithVolumes(Ray &ray, vector<BinStats> &bins, vector<int> &cLeft, vector<int> &cRight, vector<int> &nLeft, vector<int> &nRight) {
        float entryDistance = 0;
        float exitDistance = 0;
        float distanceLeft;
        float distanceRight;
        for(int i=0; i<bins.size(); i++) {
            distanceLeft = MAXFLOAT;
            distanceRight = MAXFLOAT;

            if(bins.at(i).L_volume.intersect(ray, entryDistance, exitDistance)) {//TODO need to check if empty or dont care?
                cLeft.at(i)++;
                distanceLeft = entryDistance;
            }
            /*if(debug)
                std::cout<<"\n done for left";
*/

            if(bins.at(i).R_volume.intersect(ray, entryDistance, exitDistance)) {//TODO need to check if empty or dont care?
                cRight.at(i)++;
                distanceRight = entryDistance;
            }
  /*          if(debug)
                std::cout<<"\n done for right";
*/
            if(distanceLeft < distanceRight) {
                nLeft.at(i)++;
            }
            else{
                nRight.at(i)++;
            }

        }
    }

    //Divides the volume into k bins
    vector<BinStats> binningSubdivision(AABB Volume, vector<MyTriangle> Triangles, int k) {
        vector<BinStats> bins;

        float length_x = abs(Volume.min[0] - Volume.max[0]);
        float length_y = abs(Volume.min[1] - Volume.max[1]);
        float length_z = abs(Volume.min[2] - Volume.max[2]);

        //subdividing over the x value for now //TODO TAKE max Length later
        float length = length_x ;
        int index = 0;

        if(length_y > length){
            length = length_y;
            index = 1;
        }
        if(length_z > length) {
            length = length_z;
            index = 2;
        }

        float jump = length/k;

        //jumping over longest axis found
        Vec3f step = {0, 0, 0};
        step[index] = jump;

        Vec3f min_counter = Volume.min;

        for(int i=0; i<k; i++) {
            BinStats b = BinStats(min_counter, index);
            b.computeStats(Volume, Triangles);
            bins.push_back(b);
            min_counter += step;
        }

        return bins;
    }


    //Sampling stuff
    std::vector<int> pick(int N, int k) {
        std::random_device rd;
        std::mt19937 gen(rd());

        std::unordered_set<int> elems = pickSet(N, k, gen);

        // ok, now we have a set of k elements. but now
        // it's in a [unknown] deterministic order.
        // so we have to shuffle it:

        std::vector<int> result(elems.begin(), elems.end());
        std::shuffle(result.begin(), result.end(), gen);
        return result;
    }

    std::unordered_set<int> pickSet(int N, int k, std::mt19937& gen)
    {
        std::uniform_int_distribution<> dis(1, N);
        std::unordered_set<int> elems;

        while (elems.size() < k) {
            elems.insert(dis(gen));
        }

        return elems;
    }

};
