import vllm
from vllm.platforms import current_platform

print("vLLM import successful. vLLM version is: ", vllm.__version__)
print("vLLM found device type: ", current_platform.device_type)
