//
// Created by ghosto on 3/5/21.
//

#ifndef MYRAYTRACER_BINSTATS_H
#define MYRAYTRACER_BINSTATS_H

class BinStats {
public:
    int index; //index x,y,z where we are following the cut
    Vec3f pointCut; //the point where we make the cut


    //Save indices is more efficient
    std::vector<int> R_triangles_indices;
    std::vector<int> L_triangles_indices;

    //VL and VR
    AABB R_volume;
    AABB L_volume;

    //check for empty volume
    bool isLeftEmpty = false;
    bool isRightEmpty = false;


    //ratios
    float L_alpha;
    float R_alpha;

    
    void print() {
        std::cout<<"\nIndex: "<<index<<"\n";
        std::cout<<"\nPointCut: "<<pointCut<<"\n";

        std::cout<<"\nR_triang size: "<<R_triangles_indices.size()<<"\n";
        std::cout<<"\nL_triang size: "<<L_triangles_indices.size()<<"\n";

        R_volume.print();
        std::cout<<"\n";
        L_volume.print();
        std::cout<<"\n";
    }

    //later add alpha left and alpha and more stats
    BinStats(Vec3f point, int index) { //AABB parentVolume, vector<MyTriangle> Triangles
        pointCut = point;
        this->index = index;
    }

    void computeStats(AABB volume, std::vector<MyTriangle> Triangles) {
        int counter = 0;

        //Filling the left and right triangle indices
        for(auto it=Triangles.begin(); it!=Triangles.end(); it++) {
            if(it->center[index] < pointCut[index]) {
                L_triangles_indices.push_back(counter);
            }
            else {
                R_triangles_indices.push_back(counter);
            }
            counter++;
        }

        //Filling the left and right Volumes based on triangles

        Vec3f min_min(0,0,0);
        Vec3f max_max(0,0,0);
        bool first = true;
        for(auto it=L_triangles_indices.begin(); it!=L_triangles_indices.end(); it++) {
            if(first){
                min_min = Triangles.at(*it).aabb.min;
                max_max = Triangles.at(*it).aabb.max;
                first = false;
            }
            //min min
            if(Triangles.at(*it).aabb.min[0] < min_min[0])
                min_min[0] = Triangles.at(*it).aabb.min[0];

            if(Triangles.at(*it).aabb.min[1] < min_min[1])
                min_min[1] = Triangles.at(*it).aabb.min[1];

            if(Triangles.at(*it).aabb.min[2] < min_min[2])
                min_min[2] = Triangles.at(*it).aabb.min[2];

            //max max
            if(Triangles.at(*it).aabb.max[0] > max_max[0])
                max_max[0] = Triangles.at(*it).aabb.max[0];

            if(Triangles.at(*it).aabb.max[1] > max_max[1])
                max_max[1] = Triangles.at(*it).aabb.max[1];

            if(Triangles.at(*it).aabb.max[2] > max_max[2])
                max_max[2] = Triangles.at(*it).aabb.max[2];

        }
        if(L_triangles_indices.size()!= 0){
            L_volume = AABB(min_min, max_max);
        }
        else{
            //std::cout<<"\n\n ZERO volume and triangles encountered \n\n";
            isLeftEmpty = true;
        }

        first = true; //to reset it
        //Right Side
        for(auto it=R_triangles_indices.begin(); it!=R_triangles_indices.end(); it++) {
            if(first){
                min_min = Triangles.at(*it).aabb.min;
                max_max = Triangles.at(*it).aabb.max;
                first = false;
            }
            //min min
            if(Triangles.at(*it).aabb.min[0] < min_min[0])
                min_min[0] = Triangles.at(*it).aabb.min[0];

            if(Triangles.at(*it).aabb.min[1] < min_min[1])
                min_min[1] = Triangles.at(*it).aabb.min[1];

            if(Triangles.at(*it).aabb.min[2] < min_min[2])
                min_min[2] = Triangles.at(*it).aabb.min[2];

            //max max
            if(Triangles.at(*it).aabb.max[0] > max_max[0])
                max_max[0] = Triangles.at(*it).aabb.max[0];

            if(Triangles.at(*it).aabb.max[1] > max_max[1])
                max_max[1] = Triangles.at(*it).aabb.max[1];

            if(Triangles.at(*it).aabb.max[2] > max_max[2])
                max_max[2] = Triangles.at(*it).aabb.max[2];

        }
        if(R_triangles_indices.size()!= 0){
            R_volume = AABB(min_min, max_max);
        }
        else{
            //std::cout<<"\n\n ZERO volume and triangles encountered \n\n";
            isRightEmpty = true;
        }
        //TODO we have a problem with cases where we dont have triangles on one of the sizes,  <do we care actually?
    }



};
#endif //MYRAYTRACER_BINSTATS_H
