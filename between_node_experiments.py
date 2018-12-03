import os
import sys

#
# Generate SLURM scripts for the between node experiments.
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
srun -n {nodes} run.out -m 0.01 -c 0.75 -s ${{SLURM_ARRAY_TASK_ID}} -f 4 -p 250 -g 10000 -o ./{output}/

scontrol show job ${{SLURM_JOB_ID}}
"""

try:
    email, buyin, output = sys.argv[1], sys.argv[2], sys.argv[3]
except IndexError:
    print("Must provide email, buy-in group name, and output directory.")

min_nodes = 2
max_nodes = 6

# Best tasks and thread combination from the thread experiment.
best_tasks = 28
best_nesting = 1

assert best_tasks * best_nesting <= 28

if best_tasks * best_nesting < 28:
    raise Warning("Not using maximum intel16 threads per node: {} could be 28.".format(best_tasks * best_nesting))

for nodes in range(min_nodes, max_nodes + 1):
    name = "n{}".format(nodes)
    this_output = os.path.join(output, name)
    os.makedirs(this_output, exist_ok=True)

    with open(name + ".sb", "w") as fptr:
        fptr.write(base_str.format(
            email=email,
            name=name,
            buyin=buyin,
            output=this_output,
            nodes=nodes,
            tasks=best_tasks,
            nested=best_nesting
        ))
