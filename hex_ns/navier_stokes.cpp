
// Begin demo

#include <dolfin.h>
#include "TentativeVelocity.h"
#include "PressureUpdate.h"
#include "VelocityUpdate.h"

const double lenth = 10.0;
const double width = 1.0;

using namespace dolfin;

// Define noslip domain
class NoslipDomain : public SubDomain
{
  bool inside(const Array<double>& x, bool on_boundary) const
  {
    return (on_boundary && (x[1] < DOLFIN_EPS_LARGE || 
    x[1] > 1.0 - DOLFIN_EPS_LARGE|| x[2] < DOLFIN_EPS_LARGE
    || x[2] > 1.0 - DOLFIN_EPS_LARGE));
  }
};

// Define inflow domain
class InflowDomain : public SubDomain
{
  bool inside(const Array<double>& x, bool on_boundary) const
  { return x[0] < DOLFIN_EPS; }
};

// Define inflow domain
class OutflowDomain : public SubDomain
{
  bool inside(const Array<double>& x, bool on_boundary) const
  { return x[0] > 1.0 - DOLFIN_EPS; }
};

// Define pressure boundary value at inflow
class InflowPressure : public Expression
{
public:

  // Constructor
  InflowPressure() : t(0) {}

  // Evaluate pressure at inflow
  void eval(Array<double>& values, const Array<double>& x) const
  { values[0] = sin(3.0*t); }

  // Current time
  double t;

};

int main()
{
  // Print log messages only from the root process in parallel
  parameters["std_out_all_processes"] = false;

  // Load mesh from file
  // auto mesh = std::make_shared<Mesh>("../lshape.xml.gz");
  auto _mesh = BoxMesh::create({Point(0,0,0),Point(1,1,1)},{16,16,16},CellType::Type::hexahedron);
  auto mesh = std::make_shared<Mesh>(_mesh);

  // Create function spaces
  auto V = std::make_shared<VelocityUpdate::FunctionSpace>(mesh);
  auto Q = std::make_shared<PressureUpdate::FunctionSpace>(mesh);

  // Set parameter values
  double dt = 0.01;
  double T = 3;

  // Define values for boundary conditions
  auto p_in = std::make_shared<InflowPressure>();
  auto zero = std::make_shared<Constant>(0.0);
  auto zero_vector = std::make_shared<Constant>(0.0, 0.0, 0.0);

  // Define subdomains for boundary conditions
  auto noslip_domain = std::make_shared<NoslipDomain>();
  auto inflow_domain = std::make_shared<InflowDomain>();
  auto outflow_domain = std::make_shared<OutflowDomain>() ;

  // Define boundary conditions
  DirichletBC noslip(V, zero_vector, noslip_domain);
  DirichletBC inflow(Q, p_in, inflow_domain);
  DirichletBC outflow(Q, zero, outflow_domain);
  std::vector<DirichletBC*> bcu = {&noslip};
  std::vector<DirichletBC*> bcp = {{&inflow, &outflow}};

  // Create functions
  auto u0 = std::make_shared<Function>(V);
  auto u1 = std::make_shared<Function>(V);
  auto p1 = std::make_shared<Function>(Q);

  // Create coefficients
  auto k = std::make_shared<Constant>(dt);
  auto f = std::make_shared<Constant>(0, 0, 0);

  // Create forms
  TentativeVelocity::BilinearForm a1(V, V);
  TentativeVelocity::LinearForm L1(V);
  PressureUpdate::BilinearForm a2(Q, Q);
  PressureUpdate::LinearForm L2(Q);
  VelocityUpdate::BilinearForm a3(V, V);
  VelocityUpdate::LinearForm L3(V);

  // Set coefficients
  a1.k = k; L1.k = k; L1.u0 = u0; L1.f = f;
  L2.k = k; L2.u1 = u1;
  L3.k = k; L3.u1 = u1; L3.p1 = p1;

  // Assemble matrices
  Matrix A1, A2, A3;
  assemble(A1, a1);
  assemble(A2, a2);
  assemble(A3, a3);

  // Create vectors
  Vector b1, b2, b3;

  // Use amg preconditioner if available
  const std::string prec(has_krylov_solver_preconditioner("amg") ? "amg" : "default");

  // Create files for storing solution
  File ufile("results/velocity.pvd");
  File pfile("results/pressure.pvd");

  // Time-stepping
  double t = dt;
  while (t < T + DOLFIN_EPS)
  {
    // Update pressure boundary condition
    p_in->t = t;

    // Compute tentative velocity step
    begin("Computing tentative velocity");
    assemble(b1, L1);
    for (std::size_t i = 0; i < bcu.size(); i++)
      bcu[i]->apply(A1, b1);
    solve(A1, *u1->vector(), b1, "gmres", "default");
    end();

    // Pressure correction
    begin("Computing pressure correction");
    assemble(b2, L2);
    for (std::size_t i = 0; i < bcp.size(); i++)
    {
      bcp[i]->apply(A2, b2);
      bcp[i]->apply(*p1->vector());
    }
    solve(A2, *p1->vector(), b2, "bicgstab", prec);
    end();

    // Velocity correction
    begin("Computing velocity correction");
    assemble(b3, L3);
    for (std::size_t i = 0; i < bcu.size(); i++)
      bcu[i]->apply(A3, b3);
    solve(A3, *u1->vector(), b3, "gmres", "default");
    end();

    // Save to file
    ufile << *u1;
    pfile << *p1;

    // Move to next time step
    *u0 = *u1;
    t += dt;
    cout << "t = " << t << endl;
  }

  return 0;
}
