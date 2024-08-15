#!/bin/bash
. ./setup.sh
set -e
set -x
if [ -d "matthew" ]; then
echo "Matthew data already present"
else
wget https://www2.mmm.ucar.edu/wrf/TUTORIAL_DATA/matthew_1deg.tar.gz
tar -xf ./matthew_1deg.tar.gz

fi
ln -sf $WPS_ROOT/ungrib/Variable_Tables/Vtable.GFS Vtable
link_grib.csh ./matthew/fnl
ungrib.exe  

cp $WPS_ROOT/geogrid/GEOGRID.TBL .
wget https://www2.mmm.ucar.edu/wrf/src/wps_files/geog_low_res_mandatory.tar.gz
tar -xf ./geog_low_res_mandatory.tar.gz
geogrid.exe
ncdump -h ./geo_em.d01.nc

cp $WPS_ROOT/metgrid/METGRID.TBL .
metgrid.exe
ncdump -h ./met_em.d01.2016-10-06_00:00:00.nc
