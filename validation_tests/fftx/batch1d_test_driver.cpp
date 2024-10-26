#include <stdio.h>

#include "fftx3.hpp"
#include "interface.hpp"
#include "transformlib.hpp"

#include <stdlib.h>
#include <string.h>
#include <strings.h>

#if defined(FFTX_HIP)
#define GPU_STR "rocfft"
#else
#define GPU_STR "cufft"
#endif

#if defined ( PRINTDEBUG )
#define DEBUGOUT 1
#else
#define DEBUGOUT 0
#endif

static int FFTlen, Nbatch, K, K_adj;
static int RdStrType = 0, WrStrType = 0;

//  Build the input buffer for a transform and copy it to the device:
//  The flags listed control whether real or complex data is generated and whether a full cube
//  or half cube is required (based on the transform type).
//
//    host_X:     local (host) buffer, allocated before calling build buffer
//    X:          location on device to which buffer is copied
//    genData:    True => generate new (random) data, False => reuse existing data
//    genComplex: True => generate Complex values, False => generate real values
//    useHalfDim: True => Use half dimension (for R2C), False => use full dimension

static void buildInputBuffer ( double *host_X, double *X, bool genData, bool genComplex, bool useHalfDim )
{
    int KK = ( useHalfDim ) ? ((FFTlen/2)+1) : FFTlen;

    if ( genData ) {                    // generate a new data input buffer
        for (int m = 0; m < KK; m++) {
            for (int n = 0; n < Nbatch; n++) {
                if ( genComplex ) {
                    host_X[(n + m*Nbatch)*2 + 0] = 1 - ((double) rand()) / (double) (RAND_MAX/2);
                    host_X[(n + m*Nbatch)*2 + 1] = 1 - ((double) rand()) / (double) (RAND_MAX/2);
                }
                else {
                    host_X[(n + m*Nbatch)] = 1 - ((double) rand()) / (double) (RAND_MAX/2);
                }
            }
        }
    }

    unsigned int nbytes = KK * Nbatch * sizeof(double);
    if ( genComplex ) nbytes *= 2;
    DEVICE_MEM_COPY ( X, host_X, nbytes, MEM_COPY_HOST_TO_DEVICE);
    DEVICE_CHECK_ERROR ( DEVICE_GET_LAST_ERROR () );
    return;
}


static bool printIterTimes = false;

static void checkOutputBuffers ( double *Y, double *cufft_Y, bool isR2C, bool xfmdir )
{
    int datasz;
    bool compCplx = true;                //  compare complex buffers

    K_adj = (FFTlen / 2) + 1;
    if ( isR2C ) {
        //  real (double) to complex
        if ( xfmdir ) {
            //  output buffers are complex, dims ((FFTlen/2)+1) * Nbatch
            datasz = K_adj * Nbatch * 2;
        }
        else {
            //  out buffers are real (double), dims FFTlen * Nbatch
            datasz = FFTlen * Nbatch;
            compCplx = false;
        }
    }
    else {
        //  complex to complex, dims FFTlen * Nbatch
        datasz = FFTlen * Nbatch * 2;
    }

    printf ( "Batch DFT { %d, %d, %d, %d }\t%s\t%s\t", FFTlen, Nbatch, RdStrType, WrStrType,
             ( isR2C ) ? "PRDFTBAT" : "DFTBAT",
             ( xfmdir ) ? "(Forward)" : "(Inverse)" );
    fflush ( stdout );

    double *tmp_Y       = new double [ datasz ];
    double *tmp_cufft_Y = new double [ datasz ];
    DEVICE_MEM_COPY ( tmp_Y,             Y, datasz * sizeof(double), MEM_COPY_DEVICE_TO_HOST );
    DEVICE_MEM_COPY ( tmp_cufft_Y, cufft_Y, datasz * sizeof(double), MEM_COPY_DEVICE_TO_HOST );

    bool correct = true;
    double maxdelta = 0.0;

    for ( int m = 0; m < FFTlen; m++ ) {
        for ( int n = 0; n < Nbatch; n++ ) {
            if ( compCplx ) {
                DEVICE_FFT_DOUBLECOMPLEX *host_Y       = (DEVICE_FFT_DOUBLECOMPLEX *) tmp_Y;
                DEVICE_FFT_DOUBLECOMPLEX *host_cufft_Y = (DEVICE_FFT_DOUBLECOMPLEX *) tmp_cufft_Y;
                DEVICE_FFT_DOUBLECOMPLEX s = host_Y      [n + m*Nbatch];
                DEVICE_FFT_DOUBLECOMPLEX c = host_cufft_Y[n + m*Nbatch];

                bool elem_correct = ( (abs(s.x - c.x) < 1e-7) && (abs(s.y - c.y) < 1e-7) );
                maxdelta = maxdelta < (double)(abs(s.x -c.x)) ? (double)(abs(s.x -c.x)) : maxdelta ;
                maxdelta = maxdelta < (double)(abs(s.y -c.y)) ? (double)(abs(s.y -c.y)) : maxdelta ;
                correct &= elem_correct;
            }
            else {
                double *host_Y = tmp_Y, *host_cufft_Y = tmp_cufft_Y;
                double deltar = abs ( host_Y[(n + m*Nbatch)] - host_cufft_Y[(n + m*Nbatch)] );
                bool   elem_correct = ( deltar < 1e-7 );
                maxdelta = maxdelta < deltar ? deltar : maxdelta ;
                correct &= elem_correct;
            }
        }
    }

    printf ( "Correct: %s\tMax delta = %E\t\t##PICKME##\n", (correct ? "True" : "False"), maxdelta );
    fflush ( stdout );

    delete[] tmp_Y;
    delete[] tmp_cufft_Y;

    return;
}


//  Todo:  Fix up array shapes/sizes for R2C/C2R

static int NUM_ITERS = 100;

template<class T>
static void    run_transform ( fftx::point_t<4> curr, bool isR2C, bool xfmdir, T p )
{
    if ( DEBUGOUT)     std::cout << "Entered run_transform: name = " << p.name <<  std::endl;
    DEVICE_EVENT_T start, stop, custart, custop;
    DEVICE_EVENT_CREATE ( &start );
    DEVICE_EVENT_CREATE ( &stop );
    DEVICE_EVENT_CREATE ( &custart );
    DEVICE_EVENT_CREATE ( &custop );

    double *X, *Y;
    int iters = NUM_ITERS + 10;

    FFTlen = curr.x[0], Nbatch = curr.x[1], RdStrType = curr.x[2], WrStrType = curr.x[3];
    K_adj = FFTlen / 2 + 1;
    double *host_X;
    DEVICE_FFT_DOUBLEREAL *cufft_Y;

    if ( isR2C && xfmdir ) {
        //  When is real-2-complex and xfmdir (i.e., forward) input is real (double) of dims FFTlen * Nbatch
        //  and the output array is (complex) of dims ((FFTlen / 2) + 1) * Nbatch
        DEVICE_MALLOC ( &X,       ( FFTlen * Nbatch * sizeof(DEVICE_FFT_DOUBLEREAL) ) );
        DEVICE_MALLOC ( &Y,       ( K_adj  * Nbatch * sizeof(DEVICE_FFT_DOUBLECOMPLEX) ) );
        DEVICE_MALLOC ( &cufft_Y, ( K_adj  * Nbatch * sizeof(DEVICE_FFT_DOUBLECOMPLEX) ) );
        host_X = new double[ FFTlen * Nbatch ];
    }
    else if ( isR2C && !xfmdir ) {
        //  When is real-2-complex and !xfmdir (i.e., inverse) input is complex of dims ((FFTlen/2) + 1) * Nbatch
        //  and the output array is (double) of dims FFTlen * Nbatch
        DEVICE_MALLOC ( &X,       ( K_adj  * Nbatch * sizeof(DEVICE_FFT_DOUBLECOMPLEX) ) );
        DEVICE_MALLOC ( &Y,       ( FFTlen * Nbatch * sizeof(DEVICE_FFT_DOUBLEREAL) ) );
        DEVICE_MALLOC ( &cufft_Y, ( FFTlen * Nbatch * sizeof(DEVICE_FFT_DOUBLEREAL) ) );
        host_X = new double[ K_adj * Nbatch * 2];
    }
    else {
        // complex-2-complex: input and output are complex of dims FFTlen * Nbatch
        DEVICE_MALLOC ( &X,       ( FFTlen * Nbatch * sizeof(DEVICE_FFT_DOUBLECOMPLEX) ) );
        DEVICE_MALLOC ( &Y,       ( FFTlen * Nbatch * sizeof(DEVICE_FFT_DOUBLECOMPLEX) ) );
        DEVICE_MALLOC ( &cufft_Y, ( FFTlen * Nbatch * sizeof(DEVICE_FFT_DOUBLECOMPLEX) ) );
        host_X = new double[ FFTlen * Nbatch * 2];
    }

    if ( DEBUGOUT) std::cout << "malloced memory" << std::endl;
    //  want to run and time: 1st iteration; then N iterations
    //  Report 1st time, and average of N further iterations
    float *milliseconds   = new float[iters];
    float *cumilliseconds = new float[iters];
    bool check_buff = true;

    // set up data in input buffer: gen data = true,
    // gen complex = true if !isR2C or (isR2C and inverse direction); false otherwise
    // use full K dim = false when (isR2C and inverse direction); true otherwise

    buildInputBuffer ( host_X, X, true,                         /* generate data */
                       ( !isR2C || ( isR2C && !xfmdir ) ),      /* Complex data when !isR2C or isR2C AND inverse xfm */
                       ( isR2C && !xfmdir ) );                  /* Use half dimension when isR2C AND inverse xfm */

    if ( DEBUGOUT) std::cout << "Setup to run transform" << std::endl;
    std::vector<int> sizes { FFTlen, Nbatch, RdStrType, WrStrType };
    #if defined FFTX_HIP
        std::vector<void*> args { Y, X };
    #else
        std::vector<void*> args { &Y, &X };
    #endif
    p.setSizes ( sizes );
    p.setArgs ( args );

    for ( int ii = 0; ii < iters; ii++ ) {
        //  Call the main transform function
        p.transform();
        milliseconds[ii] = p.getTime();

        /* #ifdef USE_DIFF_DATA */
        /*         buildInputBuffer ( host_X, X, true, */
        /*                            ( !isR2C || ( isR2C && !xfmdir ) ), */
        /*                            ( isR2C && !xfmdir ) ); */
        /* #else */
        /*         buildInputBuffer ( host_X, X, false, */
        /*                            ( !isR2C || ( isR2C && !xfmdir ) ), */
        /*                            ( isR2C && !xfmdir ) ); */
        /* #endif */
    }

    if ( check_buff ) {
        //  Setup a plan to run the transform using cu or roc fft
        DEVICE_FFT_HANDLE plan;
        DEVICE_FFT_RESULT res;
        DEVICE_FFT_TYPE   xfmtype = ( !isR2C ) ? DEVICE_FFT_Z2Z : ( xfmdir ) ? DEVICE_FFT_D2Z : DEVICE_FFT_Z2D ;
        DEVICE_EVENT_CREATE ( &custart );
        DEVICE_EVENT_CREATE ( &custop );
        float *devmilliseconds = new float[iters];
        float *invdevmilliseconds = new float[iters];

        //  Plan setup is different based on the read/write stride types
        if ( RdStrType == 0 && WrStrType == 0) {
            if ( DEBUGOUT ) std::cout << "APAR, APAR" << std::endl;
            res = DEVICE_FFT_PLAN_MANY ( &plan, 1, &FFTlen,             // plan, rank, n,
                                         &FFTlen,   1,  FFTlen,         // iembed, istride, idist,
                                         &FFTlen,   1,  FFTlen,         // oembed, ostride, odist,
                                         xfmtype, Nbatch );             // type and batch
        }
        else if ( RdStrType == 0 && WrStrType == 1 ) { 
            if ( DEBUGOUT ) std::cout << "APAR, AVEC" << std::endl;
            res = DEVICE_FFT_PLAN_MANY ( &plan, 1, &FFTlen,             // plan, rank, n,
                                         &FFTlen,   1,  FFTlen,         // iembed, istride, idist,
                                         &FFTlen,   Nbatch,  1,         // oembed, ostride, odist,
                                         xfmtype, Nbatch );             // type and batch
        }
        else if ( RdStrType == 1 && WrStrType == 0 ) {
            if ( DEBUGOUT ) std::cout << "AVEC, APAR" << std::endl;
            res = DEVICE_FFT_PLAN_MANY ( &plan, 1, &FFTlen,             // plan, rank, n,
                                         &FFTlen,   Nbatch,  1,         // iembed, istride, idist,
                                         &FFTlen,   1,  FFTlen,         // oembed, ostride, odist,
                                         xfmtype, Nbatch );             // type and batch
        }
        else {
            if ( DEBUGOUT ) std::cout << "AVEC, AVEC" << std::endl;
            res = DEVICE_FFT_PLAN_MANY ( &plan, 1, &FFTlen,             // plan, rank, n,
                                         &FFTlen,   Nbatch,  1,         // iembed, istride, idist,
                                         &FFTlen,   Nbatch,  1,         // oembed, ostride, odist,
                                         xfmtype, Nbatch );             // type and batch
        }

        if ( res != DEVICE_FFT_SUCCESS ) {
            printf ( "Create DEVICE_FFT_PLAN_MANY failed with error code %d ... skip buffer check\n", res );
            fflush ( stdout );
            check_buff = false;
        }
        if ( DEBUGOUT) std::cout << "Created device fft plan: " << p.name <<  std::endl;

        for ( int ii = 0; ii < iters; ii++ ) {
            //  Run the device fft plan using the same input data as the Spiral case
            DEVICE_EVENT_RECORD ( custart );
            if ( !isR2C ) {
                res = DEVICE_FFT_EXECZ2Z ( plan,
                                           (DEVICE_FFT_DOUBLECOMPLEX *) X,
                                           (DEVICE_FFT_DOUBLECOMPLEX *) cufft_Y,
                                           ( xfmdir ) ? DEVICE_FFT_FORWARD : DEVICE_FFT_INVERSE );
            }
            else {
                if ( xfmdir )
                    res = DEVICE_FFT_EXECD2Z ( plan,
                                               (DEVICE_FFT_DOUBLEREAL *) X,
                                               (DEVICE_FFT_DOUBLECOMPLEX *) cufft_Y );
                else
                    res = DEVICE_FFT_EXECZ2D ( plan,
                                               (DEVICE_FFT_DOUBLECOMPLEX *) X,
                                               (DEVICE_FFT_DOUBLEREAL *) cufft_Y );
            }

            if ( res != DEVICE_FFT_SUCCESS) {
                printf ( "Launch DEVICE_FFT_EXEC failed with error code %d ... skip buffer check\n", res );
                fflush ( stdout );
                check_buff = false;
                break;
            }
            DEVICE_EVENT_RECORD ( custop );
            DEVICE_EVENT_SYNCHRONIZE ( custop );
            DEVICE_EVENT_ELAPSED_TIME ( &cumilliseconds[ii], custart, custop );

            //  if ( isR2C && !xfmdir ) {
            if ( isR2C  ) {
                //  Input buffer is over-written / corrupted when doing IMDPRDFT (CUDA); both fwd & inv (HIP)
              #ifdef USE_DIFF_DATA
                buildInputBuffer ( host_X, X, true,
                                   ( !isR2C || ( isR2C && !xfmdir ) ),
                                   ( isR2C && !xfmdir ) );
              #else
                buildInputBuffer ( host_X, X, false,
                                   ( !isR2C || ( isR2C && !xfmdir ) ),
                                   ( isR2C && !xfmdir ) );
              #endif
            }
        }
    }
    DEVICE_SYNCHRONIZE ();

    //  check cufft/rocfft and FFTX got same results
    if ( check_buff ) checkOutputBuffers ( Y, (double *)cufft_Y, isR2C, xfmdir );

    printf("%f\tms (SPIRAL) vs\t%f\tms (%s),\t\tFIRST iteration\t##PICKME## \n",
           milliseconds[0], cumilliseconds[0], GPU_STR);
    fflush ( stdout );

    //  compute averages (ignore first 10 iterations)
    float cumulSpiral = 0.0, cumulHip = 0.0;
    float minSpiral = 1e6, maxSpiral = -1e6, minroc = 1e6, maxroc = -1e6;
    for ( int ii = 10; ii < iters; ii++ ) {
        cumulSpiral += milliseconds[ii];
        cumulHip    += cumilliseconds[ii];
        minSpiral = ( milliseconds[ii] < minSpiral ) ? milliseconds[ii] : minSpiral ;
        maxSpiral = ( milliseconds[ii] > maxSpiral ) ? milliseconds[ii] : maxSpiral ;
        minroc = ( cumilliseconds[ii] < minroc ) ? cumilliseconds[ii] : minroc ;
        maxroc = ( cumilliseconds[ii] > maxroc ) ? cumilliseconds[ii] : maxroc ;
        if ( printIterTimes )             // print the times for each iteration
            printf ( "Iteration: %d\t%f\tms (SPIRAL) vs\t%f\tms (%s)\n", ii, milliseconds[ii], cumilliseconds[ii], GPU_STR );
    }
    printf("%f\tms (SPIRAL) vs\t%f\tms (%s), AVERAGE over %d iterations (range: 11 - %d) ##PICKME## \n",
           cumulSpiral / NUM_ITERS, cumulHip / NUM_ITERS, GPU_STR, NUM_ITERS, (10 + NUM_ITERS) );
    printf("%f\tms (min SPIRAL) \t%f\tms (max SPIRAL)\t%f\tms (min %s) \t%f\tms (max %s)\n",
           minSpiral, maxSpiral, minroc, GPU_STR, maxroc, GPU_STR );
    fflush ( stdout );

    DEVICE_FREE ( X );
    DEVICE_FREE ( Y );
    DEVICE_FREE ( cufft_Y );
    delete[] host_X;
    delete[] milliseconds;
    delete[] cumilliseconds;

    return;
}


int main( int argc, char** argv)
{
    //  Test to time transforms involving nbatch * fftlen (1D fft) on a GPU [CUDA or HIP]
    bool oneshot = false;
    bool runbatdft = true, runibatdft = true, runbatprdft = true, runibatprdft = true;
    char *prog = argv[0];

    int baz = 0;
    while ( argc > 1 && argv[1][0] == '-' ) {
        switch ( argv[1][1] ) {
        case 'i':
            argv++, argc--;
            NUM_ITERS = atoi ( argv[1] );
            break;

        case 'p':                     //  print times for each iteration
            printIterTimes = true;
            break;

        case 's':
            baz = 0;
            argv++, argc--;
            FFTlen = atoi ( argv[1] );
            while ( argv[1][baz] != 'x' ) baz++;
            baz++ ;
            Nbatch = atoi ( & argv[1][baz] );
            oneshot = true;
            break;

        case 'r':
            baz = 0;
            argv++, argc--;
            RdStrType = atoi ( argv[1] );
            while ( argv[1][baz] != 'x' ) baz++;
            baz++ ;
            WrStrType = atoi ( & argv[1][baz] );
            break;

        case 't':
            argv++, argc--;
            runbatdft = false, runibatdft = false, runbatprdft = false, runibatprdft = false;
            if ( strcasecmp ( argv[1], "dftbat" ) == 0 )    runbatdft    = true;
            if ( strcasecmp ( argv[1], "idftbat" ) == 0 )   runibatdft   = true;
            if ( strcasecmp ( argv[1], "prdftbat" ) == 0 )  runbatprdft  = true;
            if ( strcasecmp ( argv[1], "iprdftbat" ) == 0 ) runibatprdft = true;
            break;

        case 'h':
            printf ( "Usage: %s: [ -i iterations ] [ -s MMxNN ] [ -r rd_x_wr [ -t transform ] [ -p ]\n", argv[0] );
            printf ( "One -t option is permitted (name is case insensitive): -t { dftbat | idftbat | prdftbat | iprdftbat\n" );
            printf ( "-p prints the time for each individual iteration\n" );
            exit (0);

        default:
            printf ( "%s: unknown argument: %s ... ignored\n", prog, argv[1] );
        }
        argv++, argc--;
    }

    int iloop = 0;
    int iters = NUM_ITERS + 10;

    char buf[100];
    if ( oneshot ) sprintf ( buf, "for size: %dx%d, Read/Write stride types: %dx%d, ", FFTlen, Nbatch, RdStrType, WrStrType );
    printf ( "%s: Measure %d iterations, %s", prog, iters, (oneshot) ? buf : "for all sizes, " );
    if ( runbatdft && runibatdft && runbatprdft && runibatprdft ) {
        sprintf ( buf, "for all transforms, " );
    }
    else {
        if ( runbatdft ) sprintf ( buf, "for transform: DFTBAT, " );
        if ( runibatdft ) sprintf ( buf, "for transform: IDFTBAT, " );
        if ( runbatprdft ) sprintf ( buf, "for transform: PRDFTBAT, " );
        if ( runibatprdft ) sprintf ( buf, "for transform: IPRDFTBAT, " );
    }
    printf ( "\n" );

    fftx::point_t<4> *xfmspec, curr;

    xfmspec = fftx_dftbat_QuerySizes ();
    if (xfmspec == NULL) {
        printf ( "%s: Failed to get list of available sizes from DFTBAT library\n", prog );
        exit (-1);
    }

    if ( oneshot ) {
        for ( iloop = 0; ; iloop++ ) {
            if ( xfmspec[iloop].x[0] == 0 && xfmspec[iloop].x[1] == 0 && xfmspec[iloop].x[2] == 0 && xfmspec[iloop].x[3] == 0 ) {
                //  requested size is not in library, print message & let RTC generate code
                printf ( "%s: Batch DFT { %d, %d, %d, %d } not found in library ... perform RTC to generate code\n",
                         prog, FFTlen, Nbatch, RdStrType, WrStrType );
                break;
            }
            if ( xfmspec[iloop].x[0] == FFTlen && xfmspec[iloop].x[1] == Nbatch &&
                 xfmspec[iloop].x[2] == RdStrType && xfmspec[iloop].x[3] == WrStrType ) {
                break;
            }
        }
    }

#if defined(FFTX_HIP)
    //  setup the library
    rocfft_setup();
#endif

    bool isR2C;

    for ( /* iloop is initialized */ ; ; iloop++ ) {
        curr = xfmspec[iloop];
        if ( curr.x[0] == 0 && curr.x[1] == 0 && curr.x[2] == 0 && curr.x[3] == 0 ) {
            if ( !oneshot ) break;        // no more to loop thru - break out
            curr.x[0] = FFTlen; curr.x[1] = Nbatch; curr.x[2] = RdStrType; curr.x[3] = WrStrType;
            //  oneshot = true;                // end after processing this size
        }

        printf ( "Batch DFT { %d, %d, %d, %d } ... processing\n", curr.x[0], curr.x[1], curr.x[2], curr.x[3]);

        isR2C = false;            //  do complex-2-complex first
        BATCH1DDFTProblem batp ( "dftbat" );
        IBATCH1DDFTProblem ibatp ( "idftbat" );

        if ( runbatdft  ) run_transform<BATCH1DDFTProblem>  ( curr, isR2C, true,  batp );
        if ( runibatdft ) run_transform<IBATCH1DDFTProblem> ( curr, isR2C, false, ibatp );

        isR2C = true;            //  do R2C & C2R
        BATCH1DPRDFTProblem batpr ( "prdftbat" );
        IBATCH1DPRDFTProblem ibatpr ( "iprdftbat" );

        // if ( runbatprdft  ) run_transform<BATCH1DPRDFTProblem>  ( curr, isR2C, true,  batpr );
        // if ( runibatprdft ) run_transform<IBATCH1DPRDFTProblem> ( curr, isR2C, false, ibatpr );

        if ( oneshot ) break;
    }

#if defined(FFTX_HIP)
    //  cleanup the library
    rocfft_cleanup();
#endif

}
