#include <dolfin.h>
#include <vector>
#include <iostream>
#include <cmath>
#include <limits>

size_t hex_index(dolfin::Point p2, const std::array<dolfin::Point,2 >& p, std::array<std::size_t, 3> n){
    // interplate from solid to fluid.
    // find a random point in which cell.
    size_t nx = n[0];
    size_t ny = n[1];
    size_t nz = n[2];
    dolfin::Point p0=p[0];
    dolfin::Point p1=p[1];

    const double x = p2.x();
    const double y = p2.y();
    const double z = p2.z();
    const double x0 = std::min(p0.x(), p1.x());
    const double x1 = std::max(p0.x(), p1.x());
    const double y0 = std::min(p0.y(), p1.y());
    const double y1 = std::max(p0.y(), p1.y());
    const double z0 = std::min(p0.z(), p1.z());
    const double z1 = std::max(p0.z(), p1.z());
    
    unsigned int out_range = std::numeric_limits<unsigned int>::max();
    if(x<x0 || x>x1 || y<y0 || y>y1 || z<z0 || z>z1) return out_range;

    size_t ix = static_cast<size_t>((x-x0)/((x1-x0)/nx));
    size_t iy = static_cast<size_t>((y-y0)/((y1-y0)/ny));
    size_t iz = static_cast<size_t>((z-z0)/((z1-z0)/nz));

    size_t cell_index = iz*ny*nx + iy*nx + ix;
    return cell_index;
}

void hex_index_test(){
    // two extreme point of the Box.
    dolfin::Point p0(-1, 3, -1);
    dolfin::Point p1( 2, 0,  2);
    //nx, ny, nz are Numbers of cells in each direction.
    size_t nx = 8, ny = 8, nz = 8;
    auto mesh = dolfin::BoxMesh::create({p0, p1}, {nx, ny, nz}, dolfin::CellType::Type::hexahedron);
    // dolfin::File mesh_file("hexahedron.pvd");
    // mesh_file << mesh;
    dolfin::Point pc(1.2, 9, 1.8);
    size_t i = hex_index(pc,{p0, p1}, {nx, ny, nz});

    if (i == std::numeric_limits<unsigned int>::max()) {
        std::cout<<pc.str()<<"> is out of range!"<<std::endl;
    } else {
        dolfin::MeshEntity e(mesh, 3, i);
        dolfin::Point p2 = e.midpoint();
        std::cout<<"the index:"<<i<<std::endl;
        std::cout<<"distance to the midpoint:" << (pc-p2).str() << std::endl;
    }
}
int main(){
    hex_index_test();
    
    return 0;

}