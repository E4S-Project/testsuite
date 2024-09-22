/*******************************************************************************
 * Copyright 2019 UChicago Argonne, LLC.
 * (c.f. AUTHORS, LICENSE)
 *
 * This file is part of the AML project.
 * For more info, see https://xgitlab.cels.anl.gov/argo/aml
 *
 * SPDX-License-Identifier: BSD-3-Clause
 ******************************************************************************/
#include <aml.h>
#include <stdio.h>
int
main(int argc, char **argv)
{
	if (aml_init(&argc, &argv) != AML_SUCCESS)
		return 1;
	// Detect if header match the library ABI.
	if ((aml_version_major != AML_VERSION_MAJOR) ||
	    (aml_version_minor != AML_VERSION_MINOR)) {
		fprintf(stderr, "AML version mismatch\n");
		return 1;
	}
	aml_finalize();
	fprintf(stderr, "Hello world!\n");
	return 0;
}
