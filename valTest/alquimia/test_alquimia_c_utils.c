/* -*-  mode: c; c-default-style: "google"; indent-tabs-mode: nil -*- */

/*
** Alquimia Copyright (c) 2013-2015, The Regents of the University of California, 
** through Lawrence Berkeley National Laboratory (subject to receipt of any 
** required approvals from the U.S. Dept. of Energy).  All rights reserved.
** 
** Alquimia is available under a BSD license. See LICENSE.txt for more
** information.
**
** If you have questions about your rights to use or distribute this software, 
** please contact Berkeley Lab's Technology Transfer and Intellectual Property 
** Management at TTD@lbl.gov referring to Alquimia (LBNL Ref. 2013-119).
** 
** NOTICE.  This software was developed under funding from the U.S. Department 
** of Energy.  As such, the U.S. Government has been granted for itself and 
** others acting on its behalf a paid-up, nonexclusive, irrevocable, worldwide 
** license in the Software to reproduce, prepare derivative works, and perform 
** publicly and display publicly.  Beginning five (5) years after the date 
** permission to assert copyright is obtained from the U.S. Department of Energy, 
** and subject to any subsequent five (5) year renewals, the U.S. Government is 
** granted for itself and others acting on its behalf a paid-up, nonexclusive, 
** irrevocable, worldwide license in the Software to reproduce, prepare derivative
** works, distribute copies to the public, perform publicly and display publicly, 
** and to permit others to do so.
** 
** Authors: Benjamin Andre <bandre@lbl.gov>
*/

// FIXME: We should use something other than ALQUIMIA_ASSERT to test predicates.

/*******************************************************************************
 **
 **  Unit tests for alquimia C utilities
 **
 *******************************************************************************/

#include "alquimia/alquimia_constants.h"
#include "alquimia/alquimia_memory.h"
#include "alquimia/alquimia_interface.h"
#include "alquimia/alquimia_util.h"
#include "alquimia/pflotran_alquimia_interface.h"
#include "alquimia/crunch_alquimia_interface.h"

void test_AlquimiaCaseInsensitiveStringCompare(void);
void test_AlquimiaVectors(void);
void test_AlquimiaNameIndexMapping(void);
void test_CreateAlquimiaInterface(void);

void test_AlquimiaCaseInsensitiveStringCompare(void) {
  char* str1;
  char* str2;
  
  str1 = (char*) calloc((unsigned int)kAlquimiaMaxStringLength, sizeof(char));
  str2 = (char*) calloc((unsigned int)kAlquimiaMaxStringLength, sizeof(char));
  
  strncpy(str1, "Hello", kAlquimiaMaxStringLength);
  strncpy(str2, "World", kAlquimiaMaxStringLength);

  ALQUIMIA_ASSERT(AlquimiaCaseInsensitiveStringCompare(str1, str2) == false);

  strncpy(str2, "Goodbye", kAlquimiaMaxStringLength);
  ALQUIMIA_ASSERT(AlquimiaCaseInsensitiveStringCompare(str1, str2) == false);

  strncpy(str2, "Hello", kAlquimiaMaxStringLength);
  ALQUIMIA_ASSERT(AlquimiaCaseInsensitiveStringCompare(str1, str2) == true);

  strncpy(str2, "hELLO", kAlquimiaMaxStringLength);
  ALQUIMIA_ASSERT(AlquimiaCaseInsensitiveStringCompare(str1, str2) == true);
}  /* end test_AlquimiaCaseInsensitiveStringCompare */

void test_AlquimiaVectors(void) {
  int size;
  AlquimiaVectorDouble dvector;
  AlquimiaVectorInt ivector;
  AlquimiaVectorString svector;

  size = -1;
  AllocateAlquimiaVectorDouble(size, &dvector);
  ALQUIMIA_ASSERT(dvector.size == 0);
  ALQUIMIA_ASSERT(dvector.data == NULL);

  size = 5;
  AllocateAlquimiaVectorDouble(size, &dvector);
  ALQUIMIA_ASSERT(dvector.size == size);
  ALQUIMIA_ASSERT(dvector.data != NULL);
  FreeAlquimiaVectorDouble(&dvector);

  size = -1;
  AllocateAlquimiaVectorInt(size, &ivector);
  ALQUIMIA_ASSERT(ivector.size == 0);
  ALQUIMIA_ASSERT(ivector.data == NULL);

  size = 5;
  AllocateAlquimiaVectorInt(size, &ivector);
  ALQUIMIA_ASSERT(ivector.size == size);
  ALQUIMIA_ASSERT(ivector.data != NULL);
  FreeAlquimiaVectorInt(&ivector);

  size = -1;
  AllocateAlquimiaVectorString(size, &svector);
  ALQUIMIA_ASSERT(svector.size == 0);
  ALQUIMIA_ASSERT(svector.data == NULL);

  size = 5;
  AllocateAlquimiaVectorString(size, &svector);
  ALQUIMIA_ASSERT(svector.size == size);
  ALQUIMIA_ASSERT(svector.data != NULL);
  FreeAlquimiaVectorString(&svector);
}  /* end test_AlquimiaVectors() */

void test_AlquimiaNameIndexMapping(void) {
  int i, id, size;
  char* name;
  AlquimiaVectorString names;

  name = (char*) calloc((unsigned int)kAlquimiaMaxStringLength, sizeof(char));
  size = 5;
  AllocateAlquimiaVectorString(size, &names);
  for (i = 0; i < size; ++i) {
    snprintf(names.data[i], kAlquimiaMaxStringLength, "name_%d", i+10);
  }
  /*PrintAlquimiaVectorString("names", &names); */

  strncpy(name, "foo", kAlquimiaMaxStringLength);
  AlquimiaFindIndexFromName(name, &names, &id);
  ALQUIMIA_ASSERT(id == -1);

  strncpy(name, "name_13", kAlquimiaMaxStringLength);
  AlquimiaFindIndexFromName(name, &names, &id);
  ALQUIMIA_ASSERT(id == 3);

  FreeAlquimiaVectorString(&names);
}  /* end test_AlquimiaNameIndexMapping() */

void test_CreateAlquimiaInterface(void) {
  AlquimiaEngineStatus status;
  AlquimiaInterface interface;
  char* name;

  AllocateAlquimiaEngineStatus(&status);
  name = (char*) calloc((unsigned int)kAlquimiaMaxStringLength, sizeof(char));

  strncpy(name, "junk", kAlquimiaMaxStringLength);
  CreateAlquimiaInterface(name, &interface, &status);
  ALQUIMIA_ASSERT(status.error == kAlquimiaErrorInvalidEngine);
  ALQUIMIA_ASSERT(interface.Setup == NULL);
  ALQUIMIA_ASSERT(interface.Shutdown == NULL);
  ALQUIMIA_ASSERT(interface.ProcessCondition == NULL);
  ALQUIMIA_ASSERT(interface.ReactionStepOperatorSplit == NULL);
  ALQUIMIA_ASSERT(interface.GetAuxiliaryOutput == NULL);
  ALQUIMIA_ASSERT(interface.GetProblemMetaData == NULL);

  strncpy(name, "pflotran", kAlquimiaMaxStringLength);
  CreateAlquimiaInterface(name, &interface, &status);
#if ALQUIMIA_HAVE_PFLOTRAN
  ALQUIMIA_ASSERT(status.error == kAlquimiaNoError);
  ALQUIMIA_ASSERT(interface.Setup == &pflotran_alquimia_setup);
  ALQUIMIA_ASSERT(interface.Shutdown == &pflotran_alquimia_shutdown);
  ALQUIMIA_ASSERT(interface.ProcessCondition == &pflotran_alquimia_processcondition);
  ALQUIMIA_ASSERT(interface.ReactionStepOperatorSplit == &pflotran_alquimia_reactionstepoperatorsplit);
  ALQUIMIA_ASSERT(interface.GetAuxiliaryOutput == &pflotran_alquimia_getauxiliaryoutput);
  ALQUIMIA_ASSERT(interface.GetProblemMetaData == &pflotran_alquimia_getproblemmetadata);
#else
  ALQUIMIA_ASSERT(status.error == kAlquimiaErrorInvalidEngine);
  ALQUIMIA_ASSERT(interface.Setup == NULL);
  ALQUIMIA_ASSERT(interface.Shutdown == NULL);
  ALQUIMIA_ASSERT(interface.ProcessCondition == NULL);
  ALQUIMIA_ASSERT(interface.ReactionStepOperatorSplit == NULL);
  ALQUIMIA_ASSERT(interface.GetAuxiliaryOutput == NULL);
  ALQUIMIA_ASSERT(interface.GetProblemMetaData == NULL);
#endif

  strncpy(name, "crunchflow", kAlquimiaMaxStringLength);
  CreateAlquimiaInterface(name, &interface, &status);
#if ALQUIMIA_HAVE_CRUNCHFLOW
  ALQUIMIA_ASSERT(status.error == kAlquimiaNoError);
  ALQUIMIA_ASSERT(interface.Setup == &crunch_alquimia_setup);
  ALQUIMIA_ASSERT(interface.Shutdown == &crunch_alquimia_shutdown);
  ALQUIMIA_ASSERT(interface.ProcessCondition == &crunch_alquimia_processcondition);
  ALQUIMIA_ASSERT(interface.ReactionStepOperatorSplit == &crunch_alquimia_reactionstepoperatorsplit);
  ALQUIMIA_ASSERT(interface.GetAuxiliaryOutput == &crunch_alquimia_getauxiliaryoutput);
  ALQUIMIA_ASSERT(interface.GetProblemMetaData == &crunch_alquimia_getproblemmetadata);
#else
  ALQUIMIA_ASSERT(status.error == kAlquimiaErrorInvalidEngine);
  ALQUIMIA_ASSERT(interface.Setup == NULL);
  ALQUIMIA_ASSERT(interface.Shutdown == NULL);
  ALQUIMIA_ASSERT(interface.ProcessCondition == NULL);
  ALQUIMIA_ASSERT(interface.ReactionStepOperatorSplit == NULL);
  ALQUIMIA_ASSERT(interface.GetAuxiliaryOutput == NULL);
  ALQUIMIA_ASSERT(interface.GetProblemMetaData == NULL);
#endif

}  /* end test_CreateAlquimiaInterface() */

int main(int argc, char** argv) {
  (void) argc;
  (void) argv;

  printf("Testing alquimia c utilites.\n");

  test_AlquimiaCaseInsensitiveStringCompare();
  test_AlquimiaVectors();
  test_AlquimiaNameIndexMapping();
  test_CreateAlquimiaInterface();

  printf("All tests passed.\n");
  return EXIT_SUCCESS;
}  /* end main() */
