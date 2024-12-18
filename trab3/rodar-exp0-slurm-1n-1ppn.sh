#!/bin/bash
#SBATCH --exclusive
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --job-name=exp1
echo -----------------------------------
echo -----------------------------------
echo -----------------------------------
echo ---------- ATENCAO: usar --exclusive na linha de comando
echo -----------------------------------

##### SBATCH --output=saida.out

echo "$0 rodando no host " hostname  
echo "$0 rodando no host " hostname >saida.txt

echo "SLURM_JOB_NAME: "	$SLURM_JOB_NAME	
echo "SLURM_NODELIST: " $SLURM_NODELIST 
echo "SLURM_JOB_NODELIST: " $SLURM_JOB_NODELIST
echo "SLURM_JOB_CPUS_PER_NODE: " $SLURM_JOB_CPUS_PER_NODE

# Load necessary modules (adjust paths as needed)
module load openmpi/4.1.4

# Run the MPI program
mpirun -np 1 ./multipart-mpi 8000000 1

echo "O tempo total dessa shell foi de" $SECONDS "segundos"
echo "SLURM_JOB_NAME: "	$SLURM_JOB_NAME	
echo "SLURM_NODELIST: " $SLURM_NODELIST 
echo "SLURM_JOB_NODELIST: " $SLURM_JOB_NODELIST
echo "SLURM_JOB_CPUS_PER_NODE: " $SLURM_JOB_CPUS_PER_NODE
#imprime infos do job slurm (e.g. TEMPO até aqui no fim do job)
squeue -j $SLURM_JOBID
