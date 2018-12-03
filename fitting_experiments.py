import os
import sys
from itertools import product

#
# Generate experiment scripts to fit sin, exp, and ln and tune hyperparams.
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
srun -n {nodes} run.out -m {mutation_rate} -c {crossover_rate} -s ${{SLURM_ARRAY_TASK_ID}} -f {func} -p 250 -g 10000 -o ./{output}/

scontrol show job ${{SLURM_JOB_ID}}
"""

try:
    email, buyin, output = sys.argv[1], sys.argv[2], sys.argv[3]
except IndexError:
    print("Must provide email, buy-in group name, and output directory.")

functions = [1, 2, 3]
mutation_rate = [0.001, 0.005, 0.01, 0.05, 0.10, 0.15, 0.2]
crossover_rate = [0.5, 0.6, 0.7, 0.8]
map_to_name = {1: "log", 2: "exp", 3: "sin"}


for func in functions:
    for m, cr in product(mutation_rate, crossover_rate):
        name = map_to_name[func] + "m{}cr{}".format(str(m).replace(".", "_"), str(cr).replace(".", "_"))
        this_output = os.path.join(output, name)
        os.makedirs(this_output, exist_ok=True)

        with open(name + ".sb", "w") as fptr:
            fptr.write(base_str.format(
                email=email,
                name=name,
                buyin=buyin,
                output=this_output,
                nodes=2,
                tasks=14,
                nested=2,
                func=func,
                mutation_rate=m,
                crossover_rate=cr
            ))
