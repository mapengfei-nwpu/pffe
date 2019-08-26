
#include <cmath>
#include <boost/multi_array.hpp>
#include "HexBoxMesh.h"

using namespace dolfin;

void HexBoxMesh::build_hex(Mesh& mesh, const std::array<Point, 2>& p,
                        std::array<std::size_t, 3> n)
{
  // Receive mesh according to parallel policy
  if (MPI::is_receiver(mesh.mpi_comm()))
  {
    MeshPartitioning::build_distributed_mesh(mesh);
    return;
  }

  // Extract data
  const Point& p0 = p[0];
  const Point& p1 = p[1];
  const std::size_t nx = n[0];
  const std::size_t ny = n[1];
  const std::size_t nz = n[2];

  // Extract minimum and maximum coordinates
  const double x0 = std::min(p0.x(), p1.x());
  const double x1 = std::max(p0.x(), p1.x());
  const double y0 = std::min(p0.y(), p1.y());
  const double y1 = std::max(p0.y(), p1.y());
  const double z0 = std::min(p0.z(), p1.z());
  const double z1 = std::max(p0.z(), p1.z());

  MeshEditor editor;
  editor.open(mesh, CellType::Type::hexahedron, 3, 3);

  // Create vertices and cells:
  editor.init_vertices_global((nx + 1)*(ny + 1)*(nz + 1), (nx + 1)*(ny + 1)*(nz + 1));
  editor.init_cells_global(nx*ny*nz, nx*ny*nz);

  // Storage for vertices
  std::vector<double> x(3);

  const double a = x0;
  const double b = x1;
  const double c = y0;
  const double d = y1;
  const double e = z0;
  const double f = z1;

  // Create main vertices:
  std::size_t vertex = 0;
  for (std::size_t iz = 0; iz <= nz; iz++)
  {
    x[2] = e + ((static_cast<double>(iz))*(f - e)/static_cast<double>(nz));
    for (std::size_t iy = 0; iy <= ny; iy++)
    {
      x[1] = c + ((static_cast<double>(iy))*(d - c)/static_cast<double>(ny));
      for (std::size_t ix = 0; ix <= nx; ix++)
      {
        x[0] = a + ((static_cast<double>(ix))*(b - a)/static_cast<double>(nx));
        editor.add_vertex(vertex, x);
        vertex++;
      }
    }
  }

  // Create cuboids
  std::size_t cell = 0;
  std::vector<std::size_t> v(8);
  for (std::size_t iz = 0; iz < nz; iz++)
  {
    for (std::size_t iy = 0; iy < ny; iy++)
    {
      for (std::size_t ix = 0; ix < nx; ix++)
      {
        v[0] = (iz*(ny + 1)*(nx + 1) + iy*(nx + 1) + ix;
        v[1] = v[0] + 1;
        v[2] = v[0] + (nx + 1);
        v[3] = v[1] + (nx + 1);
        v[4] = v[0] + (nx + 1)*(ny + 1);
        v[5] = v[1] + (nx + 1)*(ny + 1);
        v[6] = v[2] + (nx + 1)*(ny + 1);
        v[7] = v[3] + (nx + 1)*(ny + 1);
        editor.add_cell(cell, v);
        ++cell;
      }
    }
  }

  // Close mesh editor
  editor.close();

  // Broadcast mesh according to parallel policy
  if (MPI::is_broadcaster(mesh.mpi_comm()))
  {
    MeshPartitioning::build_distributed_mesh(mesh);
    return;
  }
}
//-----------------------------------------------------------------------------
size_t HexBoxMesh::hex_search(Point pc){
    // interplate from solid to fluid.
    // find a random point in which cell.
    size_t nx = _n[0];
    size_t ny = _n[1];
    size_t nz = _n[2];
    Point p0=_point[0];
    Point p1=_point[1];

    const double x = pc.x();
    const double y = pc.y();
    const double z = pc.z();
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