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
sys.path.insert(1, os.path.dirname(HERE)+ '/plopper')
from plopper import Plopper
nparams = 12

cs = CS.ConfigurationSpace(seed=1234)

p0= CSH.CategoricalHyperparameter(name='p0', choices=["#pragma clang loop(jl2) pack array(A) allocate(malloc)", "#pragma clang loop(jl2) pack array(B) allocate(malloc)", " "], default_value=' ')
p1= CSH.CategoricalHyperparameter(name='p1', choices=["#pragma clang loop(il1) pack array(A) allocate(malloc)", "#pragma clang loop(il1) pack array(B) allocate(malloc)", " "], default_value=' ')
p2= CSH.CategoricalHyperparameter(name='p2', choices=["#pragma clang loop(il1,jl1,kl1,il2,jl2) interchange permutation(jl1,kl1,il1,jl2,il2)", " "], default_value=' ')

p3= CSH.CategoricalHyperparameter(name='p3', choices=["#pragma clang loop(jm2) pack array(tmp) allocate(malloc)", "#pragma clang loop(jm2) pack array(C) allocate(malloc)", " "], default_value=' ')
p4= CSH.CategoricalHyperparameter(name='p4', choices=["#pragma clang loop(im1) pack array(tmp) allocate(malloc)", "#pragma clang loop(im1) pack array(C) allocate(malloc)", " "], default_value=' ')
p5= CSH.CategoricalHyperparameter(name='p5', choices=["#pragma clang loop(im1,jm1,km1,im2,jm2) interchange permutation(jm1,km1,im1,jm2,im2)", " "], default_value=' ')

p6= CSH.OrdinalHyperparameter(name='p6', sequence=['4','8','16','20','32','50','64','80','96','100','128'], default_value='96')
p7= CSH.OrdinalHyperparameter(name='p7', sequence=['4','8','16','20','32','50','64','80','100','128','2048'], default_value='2048')
p8= CSH.OrdinalHyperparameter(name='p8', sequence=['4','8','16','20','32','50','64','80','100','128','256'], default_value='256')

p9= CSH.OrdinalHyperparameter(name='p9', sequence=['4','8','16','20','32','50','64','80','96','100','128'], default_value='96')
p10= CSH.OrdinalHyperparameter(name='p10', sequence=['4','8','16','20','32','50','64','80','100','128','2048'], default_value='2048')
p11= CSH.OrdinalHyperparameter(name='p11', sequence=['4','8','16','20','32','50','64','80','100','128','256'], default_value='256')

cs.add_hyperparameters([p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11])

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

x1=['p0','p1','p2','p3','p4','p5', 'p6', 'p7', 'p8', 'p9', 'p10', 'p11']

def myobj(point: dict):

  def plopper_func(x):
    x = np.asarray_chkfinite(x)  # ValueError if any NaN or Inf
    values = [ point[x1[i]] for i in range( len( x1 ) )]
#    value = [point[x1[0]],point[x1[1]],point[x1[2]],point[x1[3]],point[x1[4]],point[x1[5]]]
#    print('VALUES:',point[x1[0]])
    print('VALUES:',point)
    params = ["P0","P1","P2","P3","P4","P5", "P6", "P7", "P8", "P9", "P10", "P11"]

    result = obj.findRuntime(value, params)
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
