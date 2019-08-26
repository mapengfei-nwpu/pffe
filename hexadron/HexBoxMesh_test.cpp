


#include "HexBoxMesh.h"

int main(){
    dolfin::Point p0(-1, 3, -1);
    dolfin::Point p1( 2, 0,  2);
    dolfin::HexBoxMesh mesh({p0,p1},{8,8,8});
    dolfin::File mesh_file("hexahedron.pvd");
    mesh_file << mesh;
    dolfin::Point pc(-0.9, 0, -1);
    size_t i = mesh.hex_search(pc);

    if (i == std::numeric_limits<unsigned int>::max()) {
        std::cout<<pc.str()<<"> is out of range!"<<std::endl;
    } else {
        dolfin::MeshEntity e(mesh, 3, i);
        dolfin::Point p2 = e.midpoint();
        std::cout<<"the cell index:"<<i<<std::endl;
        std::cout<<"distance to the midpoint:" << (pc-p2).str() << std::endl;
    }
}