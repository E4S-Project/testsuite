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
nparams = 8

cs = CS.ConfigurationSpace(seed=1234)
p0= CSH.CategoricalHyperparameter(name='p0', choices=["#pragma clang loop(ik2) pack array(A) allocate(malloc)", "#pragma clang loop(ik2) pack array(B) allocate(malloc)", " "], default_value=' ')

#p1 = CSH.CategoricalHyperparameter( name = "p1", choices=[''], default_value = '')

p1= CSH.CategoricalHyperparameter(name='p1', choices=["#pragma clang loop(jk2) pack array(A) allocate(malloc)", "#pragma clang loop(jk2) pack array(B) allocate(malloc)", " "], default_value=' ')

p2= CSH.CategoricalHyperparameter(name='p2', choices=["#pragma clang loop(im2) pack array(A) allocate(malloc)", "#pragma clang loop(im2) pack array(B) allocate(malloc)", " "], default_value=' ')

p3= CSH.CategoricalHyperparameter(name='p3', choices=["#pragma clang loop(jm2) pack array(A) allocate(malloc)", "#pragma clang loop(jm2) pack array(B) allocate(malloc)", " "], default_value=' ')

#p3 = CSH.CategoricalHyperparameter( name = "p3", choices=[''], default_value = '')

#TODO permutations
#p2= CSH.CategoricalHyperparameter(name='p2', choices=["#pragma clang loop(i1,j1,k1,i2,j2) interchange permutation(j1,k1,i1,j2,i2)", " "], default_value=' ')


p4= CSH.OrdinalHyperparameter(name='p4', sequence=['4','8','16','20','32','50','64','80','96','100','128'], default_value='96')
p5= CSH.OrdinalHyperparameter(name='p5', sequence=['4','8','16','20','32','50','64','80','96','100','128'], default_value='96')
p6= CSH.OrdinalHyperparameter(name='p6', sequence=['4','8','16','20','32','50','64','80','96','100','128'], default_value='96')
p7= CSH.OrdinalHyperparameter(name='p7', sequence=['4','8','16','20','32','50','64','80','96','100','128'], default_value='96')



#p5= CSH.OrdinalHyperparameter(name='p5', sequence=['4','8','16','20','32','50','64','80','100','128','256'], default_value='256')

cs.add_hyperparameters([p0, p1,p2,p3,p4,p5,p6,p7])

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

x1=['p0','p1','p2','p3','p4','p5','p6','p7']

def myobj(point: dict):

  def plopper_func(x):
    x = np.asarray_chkfinite(x)  # ValueError if any NaN or Inf
    value = [ point[k] for k in x1 ]
    print('VALUES:', value )
    params = ["P0","P1","P2","P3","P4","P5","P6","P7"]

    result = obj.findRuntime(value, params)
    print( "RESULT: ", result )
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
