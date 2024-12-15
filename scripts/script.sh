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
 
echo "Running perf profiling..."
perf record -g ../build/MolSim 

echo "Running gprof profiling..."

../build/MolSim
gprof ../build/MolSim ../build/gmon.out > profile_report_gprof.txt