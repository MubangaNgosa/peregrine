#!/bin/bash
#SBATCH --job-name=test      # Job name
#SBATCH --nodes=1                  # Request one node
#SBATCH --ntasks=1                 # Number of tasks (processes)
#SBATCH --cpus-per-task=1          # CPU cores per task
#SBATCH --mem=1GB                  # Memory
#SBATCH --time=00:01:00            # Time limit hrs:min:sec
#SBATCH --output=jobPool_%j.log    # Standard output and error log

# Run worker
./worker
./worker
./worker

# Run jobPool
./jobPool
```