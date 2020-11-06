	

import numpy as np
from libensemble.libE import libE
from generator import gen_random_sample
from simulator import sim_find_sine
from libensemble.tools import add_unique_random_streams

nworkers = 4
libE_specs = {'nworkers': nworkers, 'comms': 'local'}



gen_specs = {'gen_f': gen_random_sample,   # Our generator function
             'out': [('x', float, (1,))],  # gen_f output (name, type, size)
             'user': {
                'lower': np.array([-3]),   # lower boundary for random sampling
                'upper': np.array([3]),    # upper boundary for random sampling
                'gen_batch_size': 5        # number of x's gen_f generates per call
                }
             }

sim_specs = {'sim_f': sim_find_sine,       # Our simulator function
             'in': ['x'],                  # Input field names. 'x' from gen_f output
             'out': [('y', float)]}        # sim_f output. 'y' = sine('x')
persis_info = add_unique_random_streams({}, nworkers+1) # Worker numbers start at 1

exit_criteria = {'sim_max': 80}           # Stop libEnsemble after 80 simulations



H, persis_info, flag = libE(sim_specs, gen_specs, exit_criteria, persis_info,
                            libE_specs=libE_specs)

print([i for i in H.dtype.fields])  # (optional) to visualize our history array
print(H)

