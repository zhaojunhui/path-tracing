#include <sstream>
#include "eigen3/Eigen/Dense"
#include "vector3.h"

class AABB
{
public:
    void Init(const Eigen::Vector3d* controlPoints);
    double Distance();   
    bool Intersect(const Eigen::Vector3d &ray_O, const Eigen::Vector3d &ray_V);
private:    
    Eigen::Vector3d maxp, minp;
};