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

cs = CS.ConfigurationSpace(seed=1234)

p0= CSH.CategoricalHyperparameter(name='p0', choices=["it1,jt1,kt1", "jt1,kt1,it1", "kt1,it1,jt1", "kt1,jt1,it1", "it1,kt1,jt1", "jt1,it1,kt1"], default_value="it1,jt1,kt1")
p1= CSH.CategoricalHyperparameter(name='p1', choices=["it2,jt2,kt2", "jt2,kt2,it2", "kt2,it2,jt2", "kt2,jt2,it2", "it2,kt2,jt2", "jt2,it2,kt2"], default_value="it2,jt2,kt2")
p2= CSH.CategoricalHyperparameter(name='p2', choices=["it3,jt3,kt3", "jt3,kt3,it3", "kt3,it3,jt3", "kt3,jt3,it3", "it3,kt3,jt3", "jt3,it3,kt3"], default_value="it3,jt3,kt3")


seq = ['4','8','16','20','32','50','64','80','96','100','128','192','256', '384','512','768','1024','1536','2048']

p3 = CSH.OrdinalHyperparameter(name='p3', sequence=seq, default_value='96')
p4 = CSH.OrdinalHyperparameter(name='p4', sequence=seq, default_value='96')
p5 = CSH.OrdinalHyperparameter(name='p5', sequence=seq, default_value='96')

p6 = CSH.OrdinalHyperparameter(name='p6', sequence=seq, default_value='96')
p7 = CSH.OrdinalHyperparameter(name='p7', sequence=seq, default_value='96')
p8 = CSH.OrdinalHyperparameter(name='p8', sequence=seq, default_value='96')

p9 = CSH.OrdinalHyperparameter(name='p9', sequence=seq, default_value='96')
p10 = CSH.OrdinalHyperparameter(name='p10', sequence=seq, default_value='96')
p11 = CSH.OrdinalHyperparameter(name='p11', sequence=seq, default_value='96')


cs.add_hyperparameters([p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11])

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

x1=['p0','p1','p2', 'p3', 'p4', 'p5', 'p6', 'p7', 'p8', 'p9', 'p10', 'p11']

params = len( x1 )

def myobj(point: dict):

  def plopper_func(x):
    x = np.asarray_chkfinite(x)  # ValueError if any NaN or Inf
    values = [ point[k] for k in x1 ]
    print('VALUES:',point)
    params = ["P00","P01","P02","P3","P4","P5","P6","P7","P8","P9","P10","P11"]

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
