#include <upcxx/upcxx.hpp>
#include <cassert>

//SNIPPET
upcxx::future<> add_accumulate(upcxx::global_ptr<double> remote_dst, 
                               double *buf, std::size_t buf_len) {
  return upcxx::rpc(remote_dst.where(),
    [](const upcxx::global_ptr<double>& dst, const upcxx::view<double>& buf_in_rpc) {
      // Traverse `buf_in_rpc` like a container, adding each element to the
      // corresponding element in dst. Views fulfill most of the container
      // contract: begin, end, size, and if the element type is trivial, even operator[].
      double *local_dst = dst.local();
      std::size_t index = 0;
      for(double x : buf_in_rpc) {
        local_dst[index++] += x;
      }
    },
    remote_dst, upcxx::make_view(buf, buf + buf_len));
}
//SNIPPET

int main() {
  upcxx::init();

  const int nranks = upcxx::rank_n();
  const int me = upcxx::rank_me();
  const int next = (me + 1) % nranks;
  const int prev = (me + nranks - 1) % nranks;

  int N = 1024;

  double *incr = new double[N];
  upcxx::global_ptr<double> recv_buff = upcxx::new_array<double>(N);
  assert(incr && recv_buff);
  memset(recv_buff.local(), 0x00, N * sizeof(double));
  for (int i = 0; i < N; i++) {
    incr[i] = me;
  }

  upcxx::dist_object<upcxx::global_ptr<double>> dobj(recv_buff);
  upcxx::global_ptr<double> target = dobj.fetch(next).wait();

  add_accumulate(target, incr, N).wait();
  
  upcxx::barrier();

  for (int i = 0; i < N; i++) {
    assert(recv_buff.local()[i] == prev);
  }

  upcxx::barrier();

  if(upcxx::rank_me() == 0)
    std::cout << "SUCCESS" << std::endl;

  delete[] incr;
  upcxx::delete_array(recv_buff);
  
  upcxx::finalize();
  return 0;
}
