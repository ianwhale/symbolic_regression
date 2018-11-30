import os
import sys

#
# Generate the between thread and nesting scaling experiments.
#
base_str = """#!/bin/bash
#SBATCH --mail-user={email}
#SBATCH --mail-type=FAIL
#SBATCH --time=01:00:00
#SBATCH --ntasks-per-node=28
#SBATCH --nodes=1
#SBATCH --mem=96G
#SBATCH --job-name={name}
#SBATCH --array=0-9
#SBATCH --account={buyin}
#SBATCH --output={output}/%x_%A_%a.out

cd ${{SLURM_SUBMIT_DIR}}

export OMP_NUM_THREADS={tasks},{nested}
./run.out -m 0.01 -c 0.75 -s ${{SLURM_ARRAY_TASK_ID}} -f 4 -p 250 -g 10000 -o ./{output}/

scontrol show job ${{SLURM_JOB_ID}}
"""

try:
    email, buyin, output = sys.argv[1], sys.argv[2], sys.argv[3]
except IndexError:
    print("Must provide email, buy-in group name, and output directory.")

max_threads = 28

# 1,28; 2,14; 4,7; 7,4; 14,2; 28,1;
thread_combos = [(i, 28 // i) for i in [1, 2, 4]]
thread_combos += [(j, i) for i, j in thread_combos]

for tasks, nested in thread_combos:
    name = "t{}_ns{}".format(tasks, nested)
    this_output = os.path.join(output, name)
    os.makedirs(this_output, exist_ok=True)

    with open(name + ".sb", "w") as fptr:
        fptr.write(base_str.format(
            email=email,
            name=name,
            buyin=buyin,
            output=this_output,
            tasks=tasks,
            nested=nested
        ))
