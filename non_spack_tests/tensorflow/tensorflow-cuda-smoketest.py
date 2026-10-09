import tensorflow as tf
import sys

sys_details = tf.sysconfig.get_build_info()
gpus = tf.config.list_physical_devices('GPU')

for i, d in enumerate(gpus):
  print(i, tf.config.experimental.get_device_details(d))

errors=[]
if not sys_details.get("is_cuda_build"):
    errors.append("TensorFlow was not built wit CUDA")

if errors:
    print("FAIL:", "; ".join(errors), file=sys.stderr)
    sys.exit(1)
else:
    print(sys_details["cuda_compute_capabilities"])
