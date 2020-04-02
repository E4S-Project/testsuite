#include <stdio.h>
#include <acc_prof.h>

/*
 clang -fPIC -shared -fopenacc -o profiling.so profiling.c
 LD_PRELOAD=./profiling.so ./test_clacc
 */

unsigned int cnt = 0;
#define VERSION 0.1

typedef struct { int event; char name[256]; } my_event_t;

my_event_t eventmap[] = {
  { acc_ev_device_init_start, "acc_ev_device_init_start" },
  { acc_ev_device_init_end, "acc_ev_device_init_end" },
  { acc_ev_device_shutdown_start, "acc_ev_device_shutdown_start" },
  { acc_ev_device_shutdown_end, "acc_ev_device_shutdown_end" },
  { acc_ev_runtime_shutdown, "acc_ev_runtime_shutdown" }, 
  { acc_ev_enter_data_start, "acc_ev_enter_data_start" },
  { acc_ev_enter_data_end, "acc_ev_enter_data_end" },
  { acc_ev_exit_data_start, "acc_ev_exit_data_start" },
  { acc_ev_exit_data_end, "acc_ev_exit_data_end" },
  { acc_ev_enqueue_launch_start, "acc_ev_enqueue_launch_start" },
  { acc_ev_enqueue_launch_end, "acc_ev_enqueue_launch_end" },
  { acc_ev_enqueue_upload_start, "acc_ev_enqueue_upload_start" },
  { acc_ev_enqueue_upload_end, "acc_ev_enqueue_upload_end" },
  { acc_ev_enqueue_download_start, "acc_ev_enqueue_download_start" },
  { acc_ev_enqueue_download_end, "acc_ev_enqueue_download_end" },
  { acc_ev_compute_construct_start, "acc_ev_compute_construct_start" },
  { acc_ev_compute_construct_end, "acc_ev_compute_construct_end" },
  { acc_ev_create, "acc_ev_create" },
  { acc_ev_delete, "acc_ev_delete" },
  { acc_ev_alloc, "acc_ev_alloc" },
  { acc_ev_free, "acc_ev_free" },
  { acc_ev_wait_start, "acc_ev_wait_start" },
  { acc_ev_wait_end, "acc_ev_wait_end" },
  { acc_ev_update_start, "acc_ev_update_start" },
  { acc_ev_update_end, "acc_ev_update_end" },

  //  { acc_ev_done, "acc_ev_done" },

  { acc_ev_none, "acc_ev_none" },
  {-1, ""},
};

char* eventToName( int eventtype ){
  int i = 0;
  /* Highly suboptimal, having C++ maps would help there */
  while( eventmap[i].event != -1 ) {
    if( eventmap[i].event == eventtype ) return eventmap[i].name;
    i++;
  }
  return "";
}

void prof_inc( acc_prof_info* prof_info, acc_event_info* event_info, acc_api_info* api_info ){
  printf( "Event type %d (%s) file %s line %d func name %s acc version %d\n", 
	  prof_info->event_type, eventToName( prof_info->event_type), 
	  prof_info->src_file, prof_info->line_no, prof_info->func_name, 
	  prof_info->version );
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


