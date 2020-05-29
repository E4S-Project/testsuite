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
#nparams = 8

cs = CS.ConfigurationSpace(seed=1234)


p0= CSH.CategoricalHyperparameter(name='p0', choices=["#warning \"hello\"", "" ], default_value='')
p1= CSH.CategoricalHyperparameter(name='p1', choices=["#warning \"hello\"", "" ], default_value='')
p2= CSH.CategoricalHyperparameter(name='p2', choices=["#warning \"hello\"", "" ], default_value='')
p3= CSH.CategoricalHyperparameter(name='p3', choices=["#warning \"hello\"", "" ], default_value='')

'''
p0= CSH.CategoricalHyperparameter(name='p0', choices=["#pragma clang loop(i102) pack array(A) allocate(malloc)", "" ], default_value='')
p1= CSH.CategoricalHyperparameter(name='p1', choices=["#pragma clang loop(i102) pack array(B) allocate(malloc)", "" ], default_value='')
p2= CSH.CategoricalHyperparameter(name='p2', choices=["#pragma clang loop(i102) pack array(X) allocate(malloc)", ""], default_value='')

p3= CSH.CategoricalHyperparameter(name='p3', choices=["#pragma clang loop(i11) pack array(B) allocate(malloc)", ""], default_value='')
'''
#pragma clang loop(i11) pack array(X) allocate(malloc)  


#TODO permutations
#p2= CSH.CategoricalHyperparameter(name='p2', choices=["#pragma clang loop(i1,j1,k1,i2,j2) interchange permutation(j1,k1,i1,j2,i2)", " "], default_value=' ')

p4= CSH.OrdinalHyperparameter(name='p4', sequence=['4','8','16','20','32','50','64','80','96','100','128'], default_value='96')
p5= CSH.OrdinalHyperparameter(name='p5', sequence=['4','8','16','20','32','50','64','80','96','100','128'], default_value='96')
"""
l2= CSH.OrdinalHyperparameter(name='l2', sequence=['4','8','16','20','32','50','64','80','96','100','128'], default_value='96')
l3= CSH.OrdinalHyperparameter(name='l3', sequence=['4','8','16','20','32','50','64','80','96','100','128'], default_value='96')

l4= CSH.OrdinalHyperparameter(name='l4', sequence=['4','8','16','20','32','50','64','80','96','100','128'], default_value='96')
l5= CSH.OrdinalHyperparameter(name='l5', sequence=['4','8','16','20','32','50','64','80','96','100','128'], default_value='96')
l6= CSH.OrdinalHyperparameter(name='l6', sequence=['4','8','16','20','32','50','64','80','96','100','128'], default_value='96')
l7= CSH.OrdinalHyperparameter(name='l7', sequence=['4','8','16','20','32','50','64','80','96','100','128'], default_value='96')
"""

#cs.add_hyperparameters([p0,p1,p2,p3,l0,l1,l2,l3,l4,l5,l6,l7])
cs.add_hyperparameters([p0,p1,p2,p3,p4,p5])
#cs.add_hyperparameters([p0,p1,p2,p3])

#cond1 = CS.InCondition(p1, p0, ['#pragma clang loop(j2) pack array(A) allocate(malloc)'])
#cs.add_condition(cond1)

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

#x1=['p0','p1','p2','p3','l0','l1','l2','l3','l4','l5','l6','l7']
x1=['p0','p1','p2','p3','p4','p5']
#x1=['p0','p1','p2','p3']

nparams = len( x1 )

def myobj(point: dict):

  def plopper_func(x):
    x = np.asarray_chkfinite(x)  # ValueError if any NaN or Inf
    values = [ point[k] for k in x1 ]

    print('VALUES:',point[x1[0]])
#    params = ["P0","P1","P2","P3","L0","L1","L2","L3","L4","L5","L6","L7"]
    params = ["P0","P1","P2","P3","L0","L1"]
#    params = ["P0","P1","P2","P3"]

    result = obj.findRuntime(values, params)
#    print( "RESULT: ", result )
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

#Problem.checkcfg()
