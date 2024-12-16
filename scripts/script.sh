#!/bin/bash
#SBATCH -J profiling_molSim
#SBATCH -o ./%x.%j.%N.out
#SBATCH -D ./
#SBATCH --get-user-env
#SBATCH --clusters=cm4
#SBATCH --partition=cm4_tiny
#SBATCH --qos=cm4_tiny
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=112
#SBATCH --export=NONE
#SBATCH --time=08:00:00

module load gcc/13.2.0
module load xerces-c/3.2.1
module load slurm_setup

# Ensure /usr/bin is in PATH
export PATH=/usr/bin:$PATH

# Check the environment for debugging
echo "PATH: $PATH"
echo "Current working directory: $(pwd)"

    echo "Running perf profiling..."
    /usr/bin/perf record -g ../build/MolSim

echo "Running gprof profiling..."
../build/MolSim
if [[ -f ../build/gmon.out ]]; then
    gprof ../build/MolSim ../build/gmon.out > profile_report_gprof.txt
    echo "gprof profiling completed. Output saved to profile_report_gprof.txt."
else
    echo "Error: gmon.out not found. Ensure your program is compiled with '-pg' for gprof."
    exit 1
fi
