import numpy as np
from numpy import abs, cos, exp, mean, pi, prod, sin, sqrt, sum
from autotune import TuningProblem
from autotune.space import *
import os
import sys
import time
import json
import math

import ConfigSpace as CS
import ConfigSpace.hyperparameters as CSH
from skopt.space import Real, Integer, Categorical

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(1, os.path.dirname(HERE)+ '/tools')
from plopper import Plopper
params = 3

cs = CS.ConfigurationSpace(seed=1234)

#p0= CSH.CategoricalHyperparameter(name='p0', choices=["//#pragma clang loop(jl2) pack array(A) allocate(malloc)", " "], default_value=' ')
#p1= CSH.CategoricalHyperparameter(name='p1', choices=["//#pragma clang loop(il2) pack array(A) allocate(malloc)", " "], default_value=' ')

p0= CSH.OrdinalHyperparameter(name='p0', sequence=['4','8','16','20','32','50','64','80','96','100','128'], default_value='96')
p1= CSH.OrdinalHyperparameter(name='p1', sequence=['4','8','16','20','32','50','64','80','100','128','2048'], default_value='2048')
p2= CSH.OrdinalHyperparameter(name='p2', sequence=['4','8','16','20','32','50','64','80','100','128','256'], default_value='256')

#cs.add_hyperparameters([p0, p1, p2, p3, p4])
cs.add_hyperparameters([p0, p1, p2])

# problem space
task_space = None

input_space = cs

output_space = Space([
     Real(0.0, inf, name="time")
])

dir_path = os.path.dirname(os.path.realpath(__file__))
kernel_idx = dir_path.rfind('/')
kernel = dir_path[kernel_idx+1:]
obj = Plopper(dir_path+'/mmp.c',dir_path)

#x1=['p0','p1', 'p2', 'p3', 'p4']
x1=['p0','p1', 'p2']


def myobj(point: dict):

  def plopper_func(x):
    x = np.asarray_chkfinite(x)  # ValueError if any NaN or Inf
    values = [ point[k] for k in x1 ]
    print('VALUES:',point)
#    params = ["P0","P1","P6","P7","P8"]
    params = ["P6","P7","P8"]

    result = obj.findRuntime(values, params)
    print( 'RESULT:', result )
    return result

  x = np.array([point[f'p{i}'] for i in range(len(point))])
  results = plopper_func(x)
  print('OUTPUT:%f',results)

  return results

Problem = TuningProblem(
    task_space=None,
    input_space=input_space,
    output_space=output_space,
    objective=myobj,
    constraints=None,
    model=None
    )
