#include <upcxx/upcxx.hpp>
#include <iostream>
#if !UPCXX_CUDA_ENABLED
#error "This example requires UPC++ to be built with CUDA support."
#endif
using namespace std;
using namespace upcxx; 

int main() {
  upcxx::init();

  std::size_t segsize = 4*1024*1024; // 4MB
  auto gpu_device = upcxx::cuda_device( 0 ); // open device 0
  auto gpu_alloc = // alloc GPU segment
       upcxx::device_allocator<upcxx::cuda_device>(gpu_device, segsize); 

  // alloc an array of 1024 doubles on GPU and host
  global_ptr<double,memory_kind::cuda_device> gpu_array = gpu_alloc.allocate<double>(1024);
  global_ptr<double> host_array1 = upcxx::new_array<double>(1024);
  global_ptr<double> host_array2 = upcxx::new_array<double>(1024);


  double *h1 = host_array1.local();
  double *h2 = host_array2.local();
  for (int i=0; i< 1024; i++) h1[i] = i; //initialize h1

  //SNIPPET
  dist_object<global_ptr<double,memory_kind::cuda_device>> dobj(gpu_array);
  int neighbor = (rank_me() + 1) % rank_n();
  global_ptr<double,memory_kind::cuda_device> other_gpu_array = dobj.fetch(neighbor).wait();

  // copy data from local host memory to remote GPU
  upcxx::copy(host_array1, other_gpu_array, 1024).wait();
  // copy data back from remote GPU to local host memory
  upcxx::copy(other_gpu_array, host_array2, 1024).wait();

  upcxx::barrier();
  //SNIPPET

  int nerrs = 0;
  for (int i=0; i< 1024; i++){
    if (h1[i] != h2[i]){
      if (nerrs < 10) cout << "Error at element " << i << endl;
      nerrs++;
    }
  }
  if (nerrs) cout << "Failure/ERROR: " << nerrs << " errors detected" << endl;
  else cout << "Success/SUCCESS" << endl;

  gpu_alloc.deallocate(gpu_array);
  upcxx::delete_array(host_array1);
  upcxx::delete_array(host_array2);

  gpu_device.destroy();
  upcxx::finalize();
}
