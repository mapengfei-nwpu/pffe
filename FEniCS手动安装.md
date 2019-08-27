## FEniCS手动安装

FEniCS由python组件**FIAT**、**dijitso**、**UFL**、**FFC**和**C++/Python组件DOLFIN**构成。

mshr是可选组件。

DOLFIN是FEniCS的主要接口，C++和Python的接口都有。

FEniCS需要Python 3. 构建时需要CMake。推荐使用pip。

为了构建可选的Python接口ofDOLFIN和mshr，还需要pybind11：

```bash
PYBIND11_VERSION=2.2.3
wget -nc --quiet https://github.com/pybind/pybind11/archive/v${PYBIND11_VERSION}.tar.gz
tar -xf v${PYBIND11_VERSION}.tar.gz && cd pybind11-${PYBIND11_VERSION}
mkdir build && cd build && cmake -DPYBIND11_TEST=off .. && make install
```

It may be useful to add `cmake` flag `-DCMAKE_INSTALL_PREFIX=<prefix>` to install to a user location

### 稳定版

### Stable version

To install the Python components of FEniCS:
安装fenics的python组件

```
pip3 install fenics-ffc --upgrade
```
这会安装FFC和它的依赖。添加`--user` 或 `--prefix=<prefix>`可以将它安装到指定位置。
~~This will install FFC and its dependencies. It may be useful to add flag `--user` or `--prefix=<prefix>` to install to a user location.~~ To install DOLFIN, and optionally mshr and/or Python interface of DOLFIN/mshr:

```
FENICS_VERSION=$(python3 -c"import ffc; print(ffc.__version__)")
git clone --branch=$FENICS_VERSION https://bitbucket.org/fenics-project/dolfin
git clone --branch=$FENICS_VERSION https://bitbucket.org/fenics-project/mshr
mkdir dolfin/build && cd dolfin/build && cmake .. && make install && cd ../..
mkdir mshr/build   && cd mshr/build   && cmake .. && make install && cd ../..
cd dolfin/python && pip3 install . && cd ../..
cd mshr/python   && pip3 install . && cd ../..
```

It may be useful to add `cmake` flag `-DCMAKE_INSTALL_PREFIX=<prefix>` and `pip3` flag `--user` or `--prefix=<prefix>` to install to a user location.

See detailed instructions for
[DOLFIN](https://fenicsproject.org/docs/dolfin/dev/python/installation.html) 
[mshr](https://bitbucket.org/fenics-project/mshr/wiki/Installation)

## 更详细的DOLFIN安装

从源码安装DOLFIN
1. 支持C++11的编译器
2. 其他依赖：
    1.Boost, 有如下编译好的boost组建
        - filesystem
        - iostreams
        - program_options
        - timer
    2. cmake
    3. eigen3
    4. ffc
    5. pkg-config
    6. python 3 (通过构建系统使用)
    7. zlib
3. python接口需要的依赖：
    4. python3（包含头文件）
    5. pybind11
    6. numpy
    7. ply
4. 可选的：
    - HDF5 (with MPI support enabled if building DOLFIN with MPI support)
    - MPI
    - ParMETIS 1
    - PETSc (strongly recommended) 2
    - SCOTCH and PT-SCOTCH 1
    - SLEPc
    - Suitesparse 1
    - Trilinos
5. python可选的接口
    - matplotlib
    - mpi4py
    - petsc4py
    - slepc4py
1 、通过petsc的构建系统构建这些组建
2、Its is recommended to configuration with ParMETIS, PT-SCOTCH, MUMPS and Hypre using the --download-parmetis --download-ptscotch --download-suitesparse --download-mumps --download-hypre


