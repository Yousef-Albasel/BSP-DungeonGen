#include "Mesh.h"
class Torch {
    Mesh torchMesh;
    int x, y; // Position of the torch in the level grid
    float lightIntensity; // Intensity of the torch light
    float lightRadius; // Radius of the torch light effect
    

    public:
    Torch(const Mesh& mesh, int posX, int posY) : torchMesh(mesh), x(posX), y(posY) {}


};