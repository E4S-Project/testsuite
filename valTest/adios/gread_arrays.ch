s = adios_selection_writeblock (rank);
adios_schedule_read (fp, s, "var_double_2Darray", 0, 1, t);
adios_schedule_read (fp, s, "var_int_1Darray", 0, 1, p);
adios_perform_reads (fp, 1);
adios_selection_delete (s);
