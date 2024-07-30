/* ==========================NINJA:LICENSE==========================================   
  Copyright (c) 2016, Lawrence Livermore National Security, LLC.                     
  Produced at the Lawrence Livermore National Laboratory.                            
                                                                                    
  Written by Kento Sato, kento@llnl.gov. LLNL-CODE-713637.                           
  All rights reserved.                                                               
                                                                                    
  This file is part of NINJA. For details, see https://github.com/PRUNER/NINJA      
  Please also see the LICENSE.TXT file for our notice and the LGPL.                      
                                                                                    
  This program is free software; you can redistribute it and/or modify it under the 
  terms of the GNU General Public License (as published by the Free Software         
  Foundation) version 2.1 dated February 1999.                                       
                                                                                    
  This program is distributed in the hope that it will be useful, but WITHOUT ANY    
  WARRANTY; without even the IMPLIED WARRANTY OF MERCHANTABILITY or                  
  FITNESS FOR A PARTICULAR PURPOSE. See the terms and conditions of the GNU          
  General Public License for more details.                                           
                                                                                    
  You should have received a copy of the GNU Lesser General Public License along     
  with this program; if not, write to the Free Software Foundation, Inc., 59 Temple 
  Place, Suite 330, Boston, MA 02111-1307 USA                                 
  ============================NINJA:LICENSE========================================= */

#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>


#include "mpi.h"
#include "ninja_test_util.h"

int my_rank;


int main(int argc, char **argv)
{
  int size, vrank;
  int length;
  double s, e, ss, ee, time;
  int *send, *recv;
  int i, j;
  int repeat = 1;
  int first = 0;

  MPI_Request send_req, recv_req;

  s = get_time();

  MPI_Init(&argc, &argv);
  e = get_time();


  /* if (argc < 1) { */
  /*   printf("./pingpong <send size(Bytes)>\n"); */
  /*   exit(1); */
  /* } */
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &vrank);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);




  if (vrank == first) {
    fprintf(stderr, "Init time: %f\n", e - s);
  }


  int max = 134217728;
  //  for (i = 0, length=1024 * 1024 * 1024; i <= 20; i++, length = length / 2)
  for (i = 0, length=64; i <= 17; i++, length = length * 2)
    //  for (i = 0, length=1; i <= 17; i++, length = length )
    //    for (i = 17, length=max; i > 0; i--, length = length / 2)
  {    
    if (vrank != 0 && vrank != 1) break;
    //    length = 1024 << 2;
    send = (int*)malloc(length);
    memset(send, 0, length);
    recv = (int*)malloc(length);
    memset(recv, 0, length);

    if (length <= 1024) {
      repeat = 500;
    } else if (length <= 838860) {
      repeat = 50;
    } else {
      repeat = 10;
    }
    repeat = repeat * 10;
    s = get_time();
    for (j = 0; j < repeat; j++) {

      if (vrank == first) {
	//	ss = get_dtime();
	MPI_Isend(send, length / sizeof(int), MPI_INT,  (first + 1) % size, j, MPI_COMM_WORLD, &send_req);
	//	ee = get_dtime();
	//      fprintf(stderr, "  %f\n", ee - ss);
	//	ss = get_dtime();
	//	fprintf(stderr, "Wait send: %d %d\n", j, length);
	//	ee = get_dtime();
	//	fprintf(stderr, ":0: send  %f (%f)\n", ee - ss, ee);
	//			ss = get_dtime();
	//	fprintf(stderr, "Recv\n");
	//	mst_test_dbg_print("isend req: %p", send_req);
	//	fprintf(stderr, "rank 0: IsGend: %d\n", j);
	MPI_Wait(&send_req, MPI_STATUS_IGNORE);
	//	fprintf(stderr, "rank 0: complte Send: %d; length: %d\n", j, length);
	MPI_Irecv(recv, length / sizeof(int), MPI_INT,  (first + 1) % size, j, MPI_COMM_WORLD, &recv_req);
	MPI_Wait(&recv_req, MPI_STATUS_IGNORE);
	//	fprintf(stderr, "rank 0: complte Recv: %d; length: %d\n", j, length);
	//	fprintf(stderr, "rank 0: complte Recv: %d\n", j);
	//	ee = get_dtime();
	//	fprintf(stderr, ":0: recv  %f\n", ee - ss);
	//	fprintf(stderr, "total  %f\n", ee - ss);
	//	tu_verify_int(recv, length/ sizeof(int),  (first + 1) % size);      
	//	if (j % 10 == 0) fprintf(stderr, "%d: %f\n", j, ee - ss);
	//	fprintf(stderr, "%d(%f) bytesx2  %f usec %f MB/sec \n", length, length/(1000.0 * 1000.0), (e - s) * 1000 * 1000 , (length/(500.0 * 1000.0)) / (e - s), recv[0]);
      } else {
	MPI_Irecv(recv, length / sizeof(int), MPI_INT, first, j, MPI_COMM_WORLD, &recv_req);
	MPI_Wait(&recv_req, MPI_STATUS_IGNORE);
	//	fprintf(stderr, "rank 1: complte Recv: %d; length: %d\n", j, length);
	MPI_Isend(send, length / sizeof(int), MPI_INT, first, j, MPI_COMM_WORLD, &send_req);
	//	fprintf(stderr, "rank 1: Isend: %d\n", j);
	MPI_Wait(&send_req, MPI_STATUS_IGNORE);
	//	fprintf(stderr, "rank 1: complte Send: %d; length: %d: requ: %p\n", j, length, send_req);
      }
    }
    e = get_time();

    if (vrank == first) {
      time = (e - s)/repeat;
      fprintf(stderr, ":0: %d(%f) bytesx2  %f usec %f MB/sec val:%d (r:%d): %f\n", length, length/(1000.0 * 1000.0), time * 1000 * 1000 , (length/(500.0 * 1000.0)) / time, recv[0], repeat, length/1000.0/1000/1000/3);
      //      fdmi_err("invalide val");
    }
    free(send);
    free(recv);
  }
  MPI_Finalize();

  return 0;
}
