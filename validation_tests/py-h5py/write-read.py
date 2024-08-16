import numpy as np
import h5py

d1 = np.random.random(size = (1000,20))
d2 = np.random.random(size = (1000,200))

print( d1.shape, d2.shape)

hf = h5py.File('data.h5', 'w')

hf.create_dataset('dataset_1', data=d1)
hf.create_dataset('dataset_2', data=d2)

hf.close()

hf = h5py.File('data.h5', 'r')

print(hf.keys())

n1 = hf.get('dataset_1')
print(n1)

n1 = np.array(n1)
print(n1.shape)

hf.close()
