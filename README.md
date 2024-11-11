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
./MolSim <input_file> <duration> <delta_t> <num_frames> <output_format>
./MolSim -h | --help
./MolSim -f
./MolSim -b
./MolSim -v
```
### Arguments:
- **input_file** Name of the file to read particle data from. Should end in .txt
- **duration** Simulation duration (e.g., 10.0)
- **delta_t** Time step delta_t (e.g., 0.01)
- **num_frames** Number of Frames saved (e.g. 500)
- **output_format** Output format: either 'XYZ' or 'VTK'

### Flags
- **-h, --help** Show this help message and exit
- **-f** Delete all contents of the output folder before writing
- **-b** Benchmark the simulation (output_format and output_folder optional)
- **-v** Verbose: log additional information

## Doxygen Instructions
To generate Doxygen documentation for this project, run in the build directory:
```bash
make doc_doxygen
```

If you want to disable creating the Doxygen target (e.g., if Doxygen is not installed), run in the build directory:
```bash
cmake -DENABLE_DOXYGEN=OFF ..
```

