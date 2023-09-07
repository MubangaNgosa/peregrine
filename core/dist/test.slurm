#!/bin/bash
#SBATCH --job-name=jobPoolWorkerJob  # Job name
#SBATCH --nodes=6                    # Total number of nodes (1 for jobPool and 5 for workers)
#SBATCH --ntasks-per-node=1          # Number of tasks (processes) per node
#SBATCH --cpus-per-task=1            # CPU cores per task
#SBATCH --mem=5GB                    # Memory per node
#SBATCH --time=00:10:00              # Time limit hrs:min:sec
#SBATCH --output=jobPoolWorker_%j.log # Standard output and error log

# Load any modules or software you need
# module load ...

# Run jobPool on the first node
srun --nodes=1 --ntasks=1 timeout 60 /path/to/jobPool &

# Sleep for a short time to ensure jobPool is set up before workers start
sleep 10

# Run 5 worker instances on the other nodes, each with a max runtime of 1 minute
for i in {1..5}; do
    srun --nodes=1 --ntasks=1 timeout 60 /path/to/worker &
done

# Wait for all background processes to finish
wait
