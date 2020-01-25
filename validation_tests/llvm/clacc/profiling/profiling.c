#include <stdio.h>
#include <acc_prof.h>

/*
 clang -fPIC -shared -fopenacc -o profiling.so profiling.c
 LD_PRELOAD=./profiling.so ./test_clacc
 *
 more info: clacc/openmp/runtime/test
 */

unsigned int cnt = 0;
#define VERSION 0.1

void prof_inc( acc_prof_info* prof_info, acc_event_info* event_info, acc_api_info* api_info ){
  printf( "line %d: toto %d\n", __LINE__, cnt++ );
}
void acc_register_library(acc_prof_reg reg, acc_prof_reg unreg,
                          acc_prof_lookup lookup) {

  reg( acc_ev_device_init_start, &prof_inc, acc_reg );
  reg( acc_ev_device_init_end, &prof_inc, acc_reg );
  reg( acc_ev_device_shutdown_start, &prof_inc, acc_reg );
  reg( acc_ev_device_shutdown_end, &prof_inc, acc_reg );
  reg( acc_ev_runtime_shutdown, &prof_inc, acc_reg );
  reg( acc_ev_create, &prof_inc, acc_reg );
  reg( acc_ev_delete, &prof_inc, acc_reg );
  reg( acc_ev_alloc, &prof_inc, acc_reg );
  reg( acc_ev_free, &prof_inc, acc_reg );
  reg( acc_ev_enter_data_start, &prof_inc, acc_reg );
  reg( acc_ev_enter_data_end, &prof_inc, acc_reg );
  reg( acc_ev_exit_data_start, &prof_inc, acc_reg );
  reg( acc_ev_exit_data_end, &prof_inc, acc_reg );
  reg( acc_ev_compute_construct_start, &prof_inc, acc_reg );
  reg( acc_ev_compute_construct_end, &prof_inc, acc_reg );
  reg( acc_ev_enqueue_launch_start, &prof_inc, acc_reg );
  reg( acc_ev_enqueue_launch_end, &prof_inc, acc_reg );
  reg( acc_ev_enqueue_upload_start, &prof_inc, acc_reg );
  reg( acc_ev_enqueue_upload_end, &prof_inc, acc_reg );
  reg( acc_ev_enqueue_download_start, &prof_inc, acc_reg );
  reg( acc_ev_enqueue_download_end, &prof_inc, acc_reg );

}

  /* Unimplemented:
     
     acc_ev_wait_start
     acc_ev_wait_end
     acc_ev_update_start
     acc_ev_update_end
  */  

