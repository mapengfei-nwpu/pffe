#ifndef __HEXBOXMESH_H
#define __HEXBOXMESH_H

#include <array>
#include <cstddef>
#include <dolfin.h>


namespace dolfin
{
  class HexBoxMesh : public Mesh
  {
  public:
      HexBoxMesh(const std::array<Point,2 >& p, std::array<std::size_t, 3> n):_point(p), _n(n)
      { build_hex(*this, p, n); }
      size_t hex_search(Point pc);
  private:
      const std::array<Point, 2> _point;
      const std::array<std::size_t, 3> _n;
      // Build mesh
      static void build_hex(Mesh& mesh, const std::array<Point, 2>& p, std::array<std::size_t, 3> n);
  };
}

#endif