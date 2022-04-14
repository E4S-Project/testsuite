#include <sicm_low.h>
#include <stdio.h>

int main() {
    sicm_device_list devs = sicm_init();

    for(unsigned i = 0; i < devs.count; i++) {
        sicm_device *dev = devs.devices[i];
        fprintf(stdout, "%u: NUMA node %d: %s (%zu KB)\n", i, dev->node, sicm_device_tag_str(dev->tag), dev->page_size);
    }

    sicm_fini();
}
