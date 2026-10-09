import tensorflow as tf
import sys

sys_details = tf.sysconfig.get_build_info()
gpus = tf.config.list_physical_devices('GPU')

for i, d in enumerate(gpus):
  print(i, tf.config.experimental.get_device_details(d))

errors=[]
if not sys_details.get("is_rocm_build"):
    errors.append("TensorFlow was not built wit ROCm")

if errors:
    print("FAIL:", "; ".join(errors), file=sys.stderr)
    sys.exit(1)
else:
    print("Pass")

