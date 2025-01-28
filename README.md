# Group E: Scientific Computing Molekulardynamik
Welcome to our Molecular Simulation Project! :)

This project is part of the practical course "Scientific Computing (PSE) Molekulardynamik" at TUM during the winter 
semester 2024/25.

[![Continuous Integration](https://github.com/ziskinha/Molekulardynamik/actions/workflows/ContinuousIntegration.yml/badge.svg)](https://github.com/ziskinha/Molekulardynamik/actions/workflows/ContinuousIntegration.yml)

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
After building, you can run the simulation with the following commands:
```bash
./MolSim <input_file> <output_format>
./MolSim -h | --help
```
### Arguments:
- **input_file** XML or TXT file with input parameters important for the simulation.
- **output_format** Output format: either 'XYZ' or 'VTK'

### Flags
- **-h, --help** Show this help message and exit
- **-f** Delete all contents of the output folder before writing
- **-b** Benchmark the simulation (output_format and output_folder optional)

## Logging Instructions
If no log level is set, the default log level used is info.  
The log level must be configured before compiling the code. To set the desired log level, run in the build directory:
```bash
cmake -DSPDLOG_LEVEL=<desired level>
```
Available log levels are:
- **trace** Extreamly fine grained information
- **debug** Debug information
- **info** General information 
- **warn** Potential issues
- **error** Occuring errors
- **critical** Servere issues
- **off** Disables logging

## Doxygen Instructions
To generate Doxygen documentation for this project, run in the build directory:
```bash
make doc_doxygen
```

If you want to disable creating the Doxygen target (e.g., if Doxygen is not installed), run in the build directory:
```bash
cmake -DENABLE_DOXYGEN=OFF ..
```

## Parallelization Instructions
The simulation can also be run using one of two parallelization strategies. 
To enable parallelization, the program must be compiled with the OpenMP flag:
```bash
cmake .. -DCMAKE_CXX_FLAGS="-fopenmp"
```
The parallelization strategy can be selected via the XML configuration file as follows:
```bash
    <parameters>
        [...]
        <parallel_strategy>STRATEGY</parallel_strategy>
    </parameters>
```
Where **STRATEGY** can be one of the following options:
- **CELL_LOCK** Locks the linked cells in a way that allows the force calculation to be performed in parallel.
- **SPATIAL_DECOMPOSITION** Divides the simulation space so that the force calculation can be performed in parallel.
- **NONE** No parallelization.
