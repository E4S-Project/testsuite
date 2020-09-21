#include <cstdint>
#include <cstdlib>
#include <cstddef>

#include <upcxx/upcxx.hpp>

template<typename T>
struct sallocator   {
  using value_type = T;
  sallocator() {}
  template<typename U>
  sallocator(const sallocator<U> &other) {}
  T* allocate(std::size_t n) const {
    if (T* result = upcxx::allocate<T>(n).local()) {
      return result;
      
    }
    throw std::bad_alloc();
  }
  void deallocate(T* ptr, std::size_t n) const {
    upcxx::deallocate(ptr);
  }
};

template<typename T, typename U>
bool operator==(const sallocator<T>& lhs, const sallocator<U>& rhs) {
  return true;
}

template<typename T, typename U>
bool operator!=(const sallocator<T>& lhs, const sallocator<U>& rhs) {
  return false;
}
using namespace upcxx;
using namespace std;

#include "non-contig.hpp"

struct particle_t {
  float x, y, z, vx, vy, vz, charge;
  uint64_t id;
};

typedef float spatch_t[sdim[2]][sdim[1]][sdim[0]];
typedef float dpatch_t[ddim[2]][ddim[1]][ddim[0]];

typedef std::vector<particle_t,sallocator<particle_t>> uvector;

static uvector* dParticles;

int main() {

  init();
  
  intrank_t me = rank_me();
  intrank_t n =  rank_n();
  intrank_t nebrHi = (me + 1) % n;
  intrank_t nebrLo = (me + n - 1) % n;

  spatch_t* myPatchSPtr = (spatch_t*)allocate(sizeof(spatch_t));
  dist_object<global_ptr<float> > smesh(to_global_ptr<float>((float*)myPatchSPtr));
  dpatch_t* myPatchDPtr = (dpatch_t*)allocate(sizeof(dpatch_t));
  dist_object<global_ptr<float> > dmesh(to_global_ptr<float>((float*)myPatchDPtr));
  
  future<global_ptr<float>> dgpf = dmesh.fetch(nebrHi);
  future<global_ptr<float>> sgpf = smesh.fetch(nebrLo);

  when_all(dgpf, sgpf).wait();
 
  global_ptr<float> d_gp=dgpf.result();
  global_ptr<float> s_gp=sgpf.result();
  {     //   memory transfer done as an rput_strided
    float*  src_base = (*smesh).local(); //raw pointer to local src patch
    global_ptr<float> dst_base = d_gp; // global_ptr to memory location on destination rank
    future<> f1 = rput_strided_example(src_base, dst_base);
    f1.wait();
  }
  {     //   memory transfer done as an rget_strided
    float*  dst_base = (*dmesh).local(); //raw pointer to local destination patch
    global_ptr<float> src_base = s_gp; // global_ptr to memory location on src rank
    future<> f1 = rget_strided_example(src_base, dst_base);
    f1.wait();
  }

  {
    uvector sParticles(240);
    //size the landing vector and return the global_ptr
    future<global_ptr<particle_t> > hiVectorF = rpc(nebrHi, [](){
        dParticles = new uvector(38); 
        return to_global_ptr<particle_t>(&((dParticles)->front()));});
 
    hiVectorF.wait();
    
    particle_t* srcP=&((sParticles).front());
    global_ptr<particle_t> destP=hiVectorF.result();

#include "non-contig2.hpp"

    // ok for src and dest and sParictles to now go out of scope
  }
  barrier();
  if(me==0)
    std::cout<<"SUCCESS"<<std::endl;
  
  delete dParticles;
  finalize();
  
  return 0;
}

