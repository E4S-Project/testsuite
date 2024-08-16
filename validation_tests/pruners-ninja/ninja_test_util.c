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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

#include "ninja_test_util.h"

#define LEN (2)
int lpusec;
int anarray[LEN];
static void do_noise_work(int loops)
{ 
  int i;
  for (i = 0; i < loops; i++) {
    anarray[i % LEN] = 0;
  }
  return;
}

void init_noise()
{ 
  double start, end;
  double usec;
  int loops = 10 * 1000 * 1000 * 10;
  start = get_time();
  do_noise_work(loops);
  end   = get_time();
  usec = end * 1e6 - start * 1e6;
  lpusec = (int)(loops/usec);
  return;
}

void do_work(int usec)
{
  do_noise_work(lpusec * usec);
  return;
}

double get_time(void)
{
  double t;
  struct timeval tv;
  gettimeofday(&tv, NULL);
  t = ((double)(tv.tv_sec) + (double)(tv.tv_usec) * 0.001 * 0.001);
  return t;
}

int init_rand(int seed) 
{
  srand(seed);
  return 0;
}

int init_ndrand() 
{
  srand((int)(get_time() * 1000000));
  return 0;
}

int get_rand(int max)
{
  return rand() % max;
}

int get_hash(int original_val, int max) {
  return (original_val * original_val + original_val % 23) % max;
}
