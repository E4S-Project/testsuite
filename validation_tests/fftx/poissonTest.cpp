// #include "fftx3.hpp"
// #include "fftx3utilities.h"
// #include "fftx_mddft_cpu_public.h"
// #include "fftx_imddft_cpu_public.h"
// #include "imddft.fftx.precompile.hpp"
// #include "mddft.fftx.precompile.hpp"

#include "fftx3.hpp"
#include "interface.hpp"
#include "transformlib.hpp"


void GetCmdLineArgumenti(int argc, const char** argv, const char* name, int* rtn)
{
  size_t len = strlen(name);
  for(int i=1; i<argc; i+=2)
    {
      if(strcmp(argv[i]+1,name) ==0)
        {
         *rtn = atoi(argv[i+1]);
         //  std::cout<<name<<"="<<" "<<*rtn<<std::endl;
          break;
        }
    }
}

void initialize(fftx::array_t<3, std::complex<double> > a_input,int nx)
{
  double r0 = .25;
  double dx = 1.0/nx;
  forall([r0,dx](std::complex<double>(&v),const fftx::point_t<3>& p)
         {
           double radius = 0.;
           for (int dir = 0; dir < 3; dir++)
             {
               radius += pow(p[dir]*dx - .5,2);
             }
           radius = sqrt(radius);
           if (radius < r0)
             {
               v = pow(cos(.5*M_PI*radius/r0),6);
             }
           else
             {
               v = 0.;
             }
         }, a_input);
}

double maxLapError(std::complex<double> *rhs,std::complex<double> *output,int nx)
{
  // This computes the max norm of L^h(output) - (rhs - mean(rhs)).
  // It should be comparable to roundoff.
  
  // Compute the mean, max norm of the rhs.

  double dx = 1.0/nx;
  double errmax = 0.;
  double mean = 0.;
  double max = 0.;
  for (int k = 1;k < nx;k++)
    {
      for (int j = 1;j < nx;j++)
        {
          for (int i = 1;i < nx; i++)
            {
              int p0 = i   + j*nx + k*nx*nx;
              mean = mean + rhs[p0].real();
              if (fabs(rhs[p0].real()) > max) max = fabs(rhs[p0].real()); 
            }
        }
    }
  mean /= (nx*nx*nx);
  // Compute the max norm error.
  for (int k = 1;k < nx-1;k++)
    {
      for (int j = 1;j < nx-1;j++)
        {
          for (int i = 1;i < nx-1; i++)
            {
              int p0 = i   + j*nx + k*nx*nx;
              int p1 = i-1 + j*nx + k*nx*nx;
              int p2 = i+1 + j*nx + k*nx*nx;
              int p3 = i + (j-1)*nx + k*nx*nx;
              int p4 = i + (j+1)*nx + k*nx*nx;
              int p5 = i + j*nx + (k-1)*nx*nx;
              int p6 = i + j*nx + (k+1)*nx*nx;
              double lap = (-output[p0].real()*6
                            +output[p1].real()
                            +output[p2].real()
                            +output[p3].real()
                            +output[p4].real()
                            +output[p5].real()
                            +output[p6].real())/(dx*dx);
              double err = fabs(lap - (rhs[p0].real() - mean));
              if (err > errmax) errmax = err;
            }
        }
    }
  return errmax/max;
}
         
int main(int argc, char* argv[])
{
  // Setting up simple example of Poisson solver where the
  // input and output data is allocated by the user.
  
  int nx = 128;
  /* -------------------- */
  /* command-line parameters */
  /* -------------------- */
  GetCmdLineArgumenti(argc, (const char**)argv, "nx", &nx);
  int ny = nx;
  int nz = nx;
  int sz = nx*ny*nz;

  std::cout << "command line input:" << std::endl;
  std::cout << argv[0] << " -nx " << nx  << std::endl;
  // Allocate user input data, output data. These pointers could also have been passed in 
  // from a user application calling FFTX ffts.
  
  std::complex<double> *userInputData = new std::complex<double>[sz];
  std::complex<double> *userOutputData = new std::complex<double>[sz];
  std::complex<double> *symbol = new std::complex<double>[sz];

  // FFTX description of problem domain,
  fftx::point_t<3> sz3d = {nx,ny,nz};
  fftx::box_t<3> domain(fftx::point_t<3>({{1, 1, 1}}),
                        fftx::point_t<3>({{sz3d[0],sz3d[1],sz3d[2]}}));
  
  // create FFTX views into userInputData, userOutputData.
  fftx::array_t<3, std::complex<double> >
    in(fftx::global_ptr<std::complex<double> >(userInputData),domain);
  fftx::array_t<3, std::complex<double> >
    out(fftx::global_ptr<std::complex<double> >(userOutputData),domain);

  // allocate fftx::array_t to hold the intermediate data.
  // fftx::array_t<3, std::complex<double> > mid(domain);
  std::complex<double> *midData = new std::complex<double>[sz];
  fftx::array_t<3, std::complex<double> >
    mid(fftx::global_ptr<std::complex<double> >(midData),domain);

  // Initialize input data.
  initialize(in,nx);
  // Apply forward FFT, with timers.  
  // fftx::mddft<3> forward3d(sz3d);
  // forward3d.transform(in,mid);
  std::vector<void*> args{midData, userInputData, symbol};
  std::vector<int> sizes{nx,ny,nz};

  MDDFTProblem mdp(args, sizes, "mddft");
  mdp.transform();
  // Multiply mid by 1/(symbol of laplacian).
  
  double dx = 1.0/nx;
  forall([dx](std::complex<double>(&v),const fftx::point_t<3>& p)
         {
           double sym = -6.;
           for (int dir = 0;dir < 3;dir++)
             {
               sym += 2*cos(2*M_PI*(p[dir]-1)*dx);
             }
           sym /=dx*dx;
           int normp = p[0]*p[0] + p[1]*p[1] + p[2]*p[2];                    
           if (normp > 3)
             {v /= sym;}
           else
             {v = 0.;} // Removing the mean charge to enforce solvability.
         },
         mid);
  
  std::vector<void*> args1{userOutputData, midData, symbol};
  IMDDFTProblem imdp(args1, sizes, "imddft");
  imdp.transform();
  // compute inverse FFT, with timers.
  // fftx::imddft<3> inverse3d(sz3d); 
  // inverse3d.transform(mid,out);
  
    // scale to normalize FFT.
  double scale = 1.0/(nx*ny*nz);  
  forall([scale](std::complex<double>(&v),const fftx::point_t<3>& p)
         {v*=scale;},out);
  
    // Check correctness of data in user arrays. In exact arithmetic, the error is zero.
    // Typically, we lose 2-4 digits of precision, depending on the size of the problem.
  
  double error = maxLapError(userInputData,userOutputData,nx);
  std::cout << "max error = " << error << std::endl;
  
  delete [] userInputData;
  delete [] userOutputData;
  delete [] midData;
  return 0;
}
