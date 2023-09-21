## Coherent I/O format for OpenFOAM
The new "coherent" OpenFOAM file format is designed for input/output (I/O) on high performance parallel file systems. The developments were conducted in a reduced version of foam-extend 4.1 comprising the core `foam` and `finiteVolume` libraries.

### Installation
For the reduced version the same steps apply as for foam-extend-4.1, see
https://openfoamwiki.net/index.php/Installation/Linux/foam-extend-4.1.

In particular, use `Allwmake.firstInstall` to download and install the dependency to ADIOS2 in the ThirdParty folder.

```
source etc/bashrc [WM_MPLIB=XXX MPI_ROOT=/path/to/mpi]
Allwmake.firstInstall
```

Sourcing the bashrc for a second time enables usage of ADIOS2 tools like `bpls`.

### Usage
Check out the `lid-driven-cavity-3D` case in the `tutorials` folder. The following workflow works straightforward:

```
blockMesh
[decomposePar]
mpirun -n X icoFoam -parallel
```

Note that `decomposePar` actually is optional because a *naive* decomposition can be used during start-up phase of `icoFoam`. Also, if the number of processors in `decomposeParDict` does not match the number of ranks in `mpirun -n X`, the *naive* decomposition is used. Hence, `decomposePar` becomes optional and restarts on arbitrary number of MPI ranks is possible.

After the first run the time folders contain a ADIOS2 bp-file. The data can be observed using the command line tool `bpls`.

```
bpls 0.001/data.bp/
  double   U/internalField                     {3000000}
  double   p/internalField                     {1000000}
  double   phi/boundaryField/movingWall/value  {10000}
  double   phi/internalField                   {2970000}
```

Please also refer to https://adios2.readthedocs.io/en/v2.9.1/ about viewing and extracting data from ADIOS2 files.

If the `system/controlDict` sets `writeBulkData yes`, the the data of all time steps is stored in a case-global ADIOS2 bp-file. The time steps share one common `data.bp` file in the case folder. In that case, asynchronous output can be activate in the `system/config.xml`:

```
<parameter key="AsyncOpen" value="true"/>
<parameter key="AsyncWrite" value="true"/>
```

The activated asynchronous output into the case-global data file can hide the overheads of the I/O latencies.

#### Contributors
The work has been carried out in Task 3.4 — Parallel I/O — of the exaFOAM project.
Participating partners (partner in **bold** is the task lead): **HLRS**, Wikki GmbH

HLRS: Gregor Weiss, Flavio Galeazzo, and Andreas Ruopp

Wikki GmbH: Sergey Lesnik and Henrik Rusche

#### Acknowledgment
This application has been developed as part of the exaFOAM Project
https://www.exafoam.eu, which has received funding from the European
High-Performance Computing Joint Undertaking (JU) under grant agreement No
956416. The JU receives support from the European Union's Horizon 2020 research
and innovation programme and France, Germany, Italy, Croatia, Spain, Greece,
and Portugal.
