#include<stdexcept>
#include<iostream>
void dolfin_assert(bool a){
    if (!a)throw std::runtime_error("It is wrong!haha~");
    else std::cout<<"OK"<<std::endl;
}

int main(){
    dolfin_assert(true);
    return 0;
}