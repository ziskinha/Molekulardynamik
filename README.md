# Group E: Scientific Computing Molekulardynamik
Welcome to our Molecular Simulation Project! :)

## Build Instructions
To generate the build files use cmake:

```bash
mkdir build
cd build
cmake ..
```

To build the project run make (in the directory with the generated build files):

```bash
make
```

The build target is called `MolSim`.

## Usage
After building, you can run the simulation with the following command:
```bash
./MolSim filename end_time delta_time output_format
```
### Arguments:
- **filename** Name of the file from which the particle data is read
- **end_time** End time for the simulation (e.g., 10.0)
- **delta_time** Time step Δt (e.g., 0.01)
- **output_format** Defines output format: 0 for XYZ, 1 for VTK

## Doxygen Instructions
To generate Doxygen documentation for this project, run in the build directory:
```bash
make doc_doxygen
```

If you want to disable creating the Doxygen target (e.g., if Doxygen is not installed), run in the build directory:
```bash
cmake -DENABLE_DOXYGEN=OFF ..
```

