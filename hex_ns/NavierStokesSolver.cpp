#include <dolfin.h>
using namespace dolfin;

class NavierStokesSolver{
    public:
    NavierStokesSolver(Mesh& mesh){

    }

    void run_one_step(Function &u, Function &p);

    private:
    void set_f(Function &f);


}