#!/bin/bash
. ./setup.sh
set -e
set -x

#cp -r $WRF_ROOT/test/em_b_wave/* .

ln -sf ../wps/met_em.d01.2016-10* .

real.exe

ln -sf $WRF_ROOT/run/CAMtr_volume_mixing_ratio .
ln -sf $WRF_ROOT/run/*.TBL .
ln -sf $WRF_ROOT/run/ozone* .
ln -sf $WRF_ROOT/run/RRTMG* .
time ${TEST_RUN} wrf.exe
ncdump -h wrfout_d01_2016-10-06_00:00:00
ncdump -v Times wrfout_d01_2016-10-06_00:00:00 
