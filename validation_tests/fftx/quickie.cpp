#include <stdio.h>
#include <stdlib.h>

#define NUM_BITS    12

//  static int factors[] = { 1, 3, 5, 9, 15, 25, 27, 45, 75, 125, 225, 375, 675, 1125, 3375 };
//  static int factors[] = { 3375, 1125, 675, 375, 225, 125, 75, 45, 27, 25, 15, 9, 5, 3, 1 };
//	static int factors[] = { 1875, 625, 375, 125, 75, 25, 15, 5, 3, 1 };
//  static int factors[] = { 1400, 700, 350, 280, 200, 175, 140, 100, 70, 56, 50, 40, 35, 28, 25, 20, 14, 10, 8, 7, 5, 4, 2, 1 };
//  static int factors[] = { 20480, 10240, 5120, 4096, 2560, 2048, 1280, 1024, 640, 512, 320, 256, 160, 128, 80, 64, 40, 32, 20, 16, 10, 8, 5, 4, 2, 1 };
static int factors[200];

int main ( int argc, char *argv[] )
{
	//  get the number to factorize
	int numblocks;
	if ( argc >= 2 )
		numblocks = atoi ( argv[1] );
	else {
		numblocks = 1400;
		printf ( "Defaulting to numblocks = 1400\n" );
	}

	int ii, xx, fpos = 0;
	printf ( "##  factors of %d are:\n##  ( ", numblocks );
	for ( ii = 1; ii <= numblocks; ii++ ) {
		xx = numblocks / ii;
		if ( xx * ii == numblocks ) {
			printf ( "%d ", xx );
			factors[fpos++] = xx;
		}
	}
	printf ( " )\n" );
	
	int XBLOCK, YBLOCK, ZBLOCK;
	int jj, kk, prod;

	//  printf ( "Data for max blocks = %d\n", maxfact );
	printf ( "##  XBLOCK\tYBLOCK\tZBLOCK\n" );   //   \tXSCALE\tYSCALE\tZSCALE\n" );

	for ( ii = 0; ii < fpos; ii++ ) {
		for ( jj = 0; jj < fpos; jj++ ) {
			for ( kk = 0; kk < fpos; kk++ ) {
				prod = factors[ii] * factors[jj] * factors[kk];
				if ( prod == numblocks ) {
					printf ( "%d\t%d\t%d\n", factors[ii], factors[jj], factors[kk] );
				}
			}				
		}
	}

#if 0
	int numbits, numblocks;
	if ( argc >= 2 )
		numbits = atoi ( argv[1] );
	else
		numbits = NUM_BITS;
	numblocks = ( 1 << numbits );
	
	int XSCALE, YSCALE, ZSCALE;
	printf ( "Data for NUM BITS = %d and NUM BLOCKS = %d\n", numbits, numblocks );
	printf ( "XBLOCK\tYBLOCK\tZBLOCK\tXSCALE\tYSCALE\tZSCALE\n" );
	for ( int idx1 = 0; idx1 <= numbits; idx1++ ) {
		for ( int idx2 = 0; idx2 <= numbits; idx2++ ) {
			for ( int idx3 = 0; idx3 <= numbits; idx3++ ) {
				XBLOCK = ( 1 << idx1 );
				YBLOCK = ( 1 << idx2 );
				ZBLOCK = ( 1 << idx3 );
				//  printf ( "X/Y/Z block = %d %d %d, product = %d\n", XBLOCK, YBLOCK, ZBLOCK, ( XBLOCK * YBLOCK * ZBLOCK ) );
				if ( XBLOCK * YBLOCK * ZBLOCK != numblocks )  continue;
				XSCALE = YBLOCK;
				YSCALE = 1;
				ZSCALE = (XBLOCK * YBLOCK);
				printf ( "%d\t%d\t%d\t%d\t%d\t%d\n", XBLOCK, YBLOCK, ZBLOCK, XSCALE, YSCALE, ZSCALE );
			}
		}
	}
#endif
	
	exit (0);
}
