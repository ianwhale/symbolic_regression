import os
import sys

#
# Generate SLURM scripts for the weak scaling experiments.
#
base_str = """#!/bin/bash
#SBATCH --mail-user={email}
#SBATCH --mail-type=FAIL
#SBATCH --time=01:00:00
#SBATCH --ntasks-per-node=28
#SBATCH --nodes={nodes}
#SBATCH --mem=96G
#SBATCH --job-name={name}
#SBATCH --array=0-9
#SBATCH --account={buyin}
#SBATCH --output={output}/%x_%A_%a.out

cd ${{SLURM_SUBMIT_DIR}}

export OMP_NUM_THREADS={tasks},{nested}
srun -n {nodes} run.out -m 0.01 -c 0.75 -s ${{SLURM_ARRAY_TASK_ID}} -f 4 -p {population_size} -g 10000 -o ./{output}/

scontrol show job ${{SLURM_JOB_ID}}
"""

try:
    email, buyin, output = sys.argv[1], sys.argv[2], sys.argv[3]
except IndexError:
    print("Must provide email, buy-in group name, and output directory.")

min_scale = 1
max_scale = 6

for i in range(min_scale, max_scale + 1):
    name = "s{}".format(i)
    this_output = os.path.join(output, name)
    os.makedirs(this_output, exist_ok=True)

    with open(name + ".sb", "w") as fptr:
        fptr.write(base_str.format(
            email=email,
            name=name,
            buyin=buyin,
            output=this_output,
            nodes=i,
            tasks=28,
            nested=1,
            population_size= i * 28
        ))
