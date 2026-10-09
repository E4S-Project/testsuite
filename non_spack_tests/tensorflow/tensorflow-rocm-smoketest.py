import tensorflow as tf

for i, d in enumerate(tf.config.list_physical_devices('GPU')):
  print(i, tf.config.experimental.get_device_details(d))
sys_details = tf.sysconfig.get_build_info()
if "is_rocm_build" in sys_details:
  print(sys_details["is_rocm_build"])
