// Function definitions (not found in the header) for 
// the Rndm, Hist, Vec4 and Particle classes, and for
// some global functions.

#include "Basics.h"
#include "ThePEG/Repository/CurrentGenerator.h"
#include "ThePEG/Repository/EventGenerator.h"
#include "ThePEG/Repository/UseRandom.h"
#include "ThePEG/PDT/ParticleData.h"

using namespace Pythia7;
using namespace Shower;

//**************************************************************************

// Rndm class.
// This class handles random number generation according to the
// Marsaglia-Zaman algorithm

//*********

// Definitions of static variables. 

bool Rndm::initrndm = false;
bool Rndm::savgauss = false;
long Rndm::i97, Rndm::j97;
double Rndm::u[97], Rndm::c, Rndm::cd, Rndm::cm, Rndm::save;

//*********

// Initialize, normally at construction or in first call.

void Rndm::init(long inseed) {
  // Pick seed in convenient way.
  long seed = inseed;
  if(inseed < 0) {seed = 19780503;}
  else if (inseed == 0) {seed = time(0);}

  // Unpack seed.
  long ij = (seed/30082) % 31329;
  long kl = seed % 30082;
  long i = (ij/177) % 177 + 2;
  long j = ij % 177 + 2;
  long k = (kl/169) % 178 + 1;
  long l =  kl % 169;

  // Initialize random number array.
  for (long ii=0; ii<97; ++ii) {
    double s = 0.;
    double t = 0.5;
    for (long jj=0; jj<48; ++jj) {
      long m = (( (i*j)%179 )*k) % 179;
      i = j;
      j = k;
      k = m;
      l = (53*l+1) % 169;
      if ( (l*m) % 64 >= 32) s += t;
      t *= 0.5;
    }
    u[ii] = s;
  }

  // Initialize other variables.
  double twom24 = 1.;
  for (long i24=0; i24<24; ++i24) twom24 *= 0.5;
  c = 362436. * twom24;
  cd = 7654321. * twom24;
  cm = 16777213. * twom24;
  i97 = 96;
  j97 = 32;

  // Finished.
  initrndm = true;
}

//*********

// Generate next random number uniformly between 0 and 1.

double Rndm::flat() {

  double ret = UseRandom::rnd();
  if ( ret > 0.0 ) return ret;

  double uni;

  // Ensure that already initialized.
  if (!initrndm) init(19780503); 

  // Find next random number and update saved state.
  do {
    uni = u[i97] - u[j97];
    if (uni < 0.) uni += 1.;
    u[i97] = uni;
    if (--i97 < 0) i97 = 96;
    if (--j97 < 0) j97 = 96;
    c -= cd;
    if (c < 0.) c += cm;
    uni -= c;
    if(uni < 0.) uni += 1.;
   } while (uni <= 0. || uni >= 1.);
  return uni;
}

//*********

// Generate random numbers according to exp(-x^2/2).

double Rndm::gauss() { 
  // Generate pair of Gaussian random numbers.
  if (!savgauss) {
    savgauss = true;
    double r = sqrt(-2. * log(flat()));
    double phi = 2. * M_PI * flat();
    save = r * sin(phi);
    return r * cos(phi);
  // Use saved element of pair.
  } else {
    savgauss = false;
    return save;
  }
} 

//*********

// Pick one option among  vector of (positive) probabilities.

long Rndm::pick(const vector<double>& vec) {
  double work = 0.;
  for (long i=0; i<long(vec.size()); ++i) work += vec[i];
  work *= flat();  
  long index = -1;
  do { work -= vec[++index]; } while (work > 0 && index < long(vec.size()));
  return index; 
}

//**************************************************************************

// Hist class.
// This class handles a single histogram at a time.

//*********

// Constants (possibly to be changed externally).
// Maximum number of lines a histogram can use at output.
long Hist::NLINES = 30;
// Tolerance in deviation of xmin and xmax between two histograms.
double Hist::TOLERANCE = 0.001;
// Small number to avoid division by zero.
double Hist::TINY = 1e-20;

//*********

// Book a histogram.

void Hist::book(string titlein, long nbinin, double xminin, 
  double xmaxin) {  
  title = titlein;
  nbin = nbinin; 
  if(nbinin < 1) nbin = 1; 
  if(nbinin > 100) nbin = 100;
  xmin = xminin;
  xmax = xmaxin;
  dx = (xmax - xmin)/nbin;
  res.resize(nbin);
  null();
}

//*********

// Set title of a histogram.

void Hist::name(string titlein) {title = titlein; }  

//*********

// Reset bin contents.

void Hist::null() {
  nfill = 0;
  under = 0.;
  inside = 0.;
  over = 0.;
  for (long ix=0; ix<nbin; ++ix) {res[ix] = 0.;}    
}

//*********

// Fill bin with weight.

void Hist::fill(double x, double w) {
  ++nfill;
  long ibin = long(floor((x - xmin)/dx));
  if (ibin < 0) {under += w; } 
  else if (ibin >= nbin) {over += w; } 
  else {inside += w; res[ibin] += w; }
}

//*********

// Print histogram contents as a table (e.g. for Gnuplot).

void Hist::table(ostream& os) const {
  os.setf(ios::scientific, ios::floatfield); os.precision(4); 
  for (long ix=0; ix<nbin; ++ix) {
    os.width(12); os << (xmin + (ix + 0.5) * dx) ;     
    os.width(12); os << res[ix] << "\n";     
  }
}

//*********

// Check whether another histogram has same size and limits.
bool Hist::sameSize(const Hist& h) const {
  if (nbin == h.nbin && abs(xmin - h.xmin) < Hist::TOLERANCE * dx &&
    abs(xmax - h.xmax) < Hist::TOLERANCE * dx) {return true;}
  else {return false;}
}  

//*********

// Add histogram to existing one.

Hist& Hist::operator+=(const Hist& h) {
  if (!sameSize(h)) return *this;
  nfill += h.nfill;
  under += h.under;
  inside += h.inside;
  over += h.over;
  for (long ix=0; ix<nbin; ++ix) {res[ix] += h.res[ix];}    
  return *this;
}

//*********

// Subtract histogram from existing one.

Hist& Hist::operator-=(const Hist& h) {
  if (!sameSize(h)) return *this;
  nfill += h.nfill;
  under -= h.under;
  inside -= h.inside;
  over -= h.over;
  for (long ix=0; ix<nbin; ++ix) {res[ix] -= h.res[ix];}    
  return *this;
}

//*********

// Multiply existing histogram by another one.

Hist& Hist::operator*=(const Hist& h) {
  if (!sameSize(h)) return *this;
  nfill += h.nfill;
  under *= h.under;
  inside *= h.inside;
  over *= h.over;
  for (long ix=0; ix<nbin; ++ix) {res[ix] *= h.res[ix];}    
  return *this;
}

//*********

// Divide existing histogram by another one.

Hist& Hist::operator/=(const Hist& h) {
  if (!sameSize(h)) return *this;
  nfill += h.nfill;
  if (abs(h.under) < Hist::TINY) under = 0.; 
  else under /= h.under;
  if (abs(h.inside) < Hist::TINY) inside = 0.; 
  else inside /= h.inside;
  if (abs(h.over) < Hist::TINY) over = 0.; 
  else over /= h.over;
  for (long ix=0; ix<nbin; ++ix) {
    if (abs(h.res[ix]) < Hist::TINY) res[ix] = 0.; 
    else res[ix] /= h.res[ix]; 
  }    
  return *this;
}

//*********

// Add constant offset to histogram.

Hist& Hist::operator+=(double f) {
  under += f;
  inside += nbin * f;
  over -= f;
  for (long ix=0; ix<nbin; ++ix) {res[ix] += f;}    
  return *this;
}

//*********

// Subtract constant offset from histogram.

Hist& Hist::operator-=(double f) {
  under -= f;
  inside -= nbin * f;
  over -= f;
  for (long ix=0; ix<nbin; ++ix) {res[ix] -= f;}    
  return *this;
}

//*********

// Multiply histogram by constant

Hist& Hist::operator*=(double f) {
  under *= f;
  inside *= f;
  over *= f;
  for (long ix=0; ix<nbin; ++ix) {res[ix] *= f;}    
  return *this;
}

//*********

// Divide histogram by constant

Hist& Hist::operator/=(double f) {
  under /= f;
  inside /= f;
  over /= f;
  for (long ix=0; ix<nbin; ++ix) {res[ix] /= f;}    
  return *this;
}

//*********

// Implementation of operator overloading with friends.

Hist operator+(const Hist& h1, const Hist& h2) 
  {Hist h = h1; return h += h2;}

Hist operator+(const Hist& h1, double f) 
  {Hist h = h1; return h += f;}

Hist operator+(double f, const Hist& h1) 
  {Hist h = h1; return h += f;}

Hist operator-(const Hist& h1, const Hist& h2) 
  {Hist h = h1; return h -= h2;}

Hist operator-(const Hist& h1, double f) 
  {Hist h = h1; return h -= f;}

Hist Pythia7::Shower::operator-(double f, const Hist& h1) 
  {Hist h = h1; 
  h.under = f - h1.under; 
  h.inside = h1.nbin * f - h1.inside;
  h.over = f - h1.over;
  for (long ix=0; ix<h1.nbin; ++ix) {h.res[ix] = f - h1.res[ix];}
  return h;}

Hist operator*(const Hist& h1, const Hist& h2) 
  {Hist h = h1; return h *= h2;}

Hist operator*(const Hist& h1, double f) 
  {Hist h = h1; return h *= f;}

Hist operator*(double f, const Hist& h1) 
  {Hist h = h1; return h *= f;}

Hist operator/(const Hist& h1, const Hist& h2) 
  {Hist h = h1; return h /= h2;}

Hist operator/(const Hist& h1, double f) 
  {Hist h = h1; return h /= f;}

Hist Pythia7::Shower::operator/(double f, const Hist& h1) {Hist h = h1; 
  h.under = (abs(h1.under) < Hist::TINY) ? 0. :  f/h1.under; 
  h.inside = (abs(h1.inside) < Hist::TINY) ? 0. :  f/h1.inside; 
  h.over = (abs(h1.over) < Hist::TINY) ? 0. :  f/h1.over; 
  for (long ix=0; ix<h1.nbin; ++ix) {
    h.res[ix] = (abs(h1.res[ix]) < Hist::TINY) ? 0. : f/h1.res[ix]; 
  }    
  return h;}

//*********

// Print a histogram: also operator overloading with friend.

ostream& Pythia7::Shower::operator<<(ostream& os, const Hist& h) {
  if (h.nfill <= 0) return os;

  // Constants for printout. 
  const static double DYAC[] = {0.04, 0.05, 0.06, 0.08, 0.10, 
                                0.12, 0.15, 0.20, 0.25, 0.30};
  const static char NUMBER[] = {'0', '1', '2', '3', '4', '5', 
                                '6', '7', '8', '9', 'X' };

  // Write time and title.
  time_t t = time(0);
  char date[18];
  strftime(date,18,"20%Y-%m-%d %H:%M",localtime(&t));
  os << "\n\n  " << date << "       " << h.title << "\n\n";

  // Find minimum and maximum bin content
  double ymin = h.res[0];
  double ymax = h.res[0];
  for (long i=1; i<h.nbin; ++i) {
    if (h.res[i] < ymin) ymin = h.res[i];
    if (h.res[i] > ymax) ymax = h.res[i];
  } 

  // Determine scale and step size for y axis.
  if (ymax - ymin > Hist::NLINES * DYAC[0] * 1e-9) { 
    if (ymin > 0. && ymin < 0.1 * ymax) ymin = 0.;
    if (ymax < 0. && ymax > 0.1 * ymin) ymax = 0.;
    int ipowy = int(floor( log10(ymax - ymin) ));
    if (ymax - ymin < Hist::NLINES * DYAC[0] * pow(10.,ipowy)) 
      ipowy = ipowy - 1;
    if (ymax - ymin > Hist::NLINES * DYAC[9] * pow(10.,ipowy)) 
      ipowy = ipowy + 1;
    double nlinpow = Hist::NLINES * pow(10.,ipowy);
    double dely = DYAC[0];
    for (long idel=0; idel<9; ++idel) {
      if (ymax - ymin >= nlinpow * DYAC[idel]) dely = DYAC[idel+1];
    } 
    double dy = dely * pow(10.,ipowy);

    // Convert bin contents to integer form; fractional fill in top row.
    vector<long> row(h.nbin);
    vector<long> frac(h.nbin);
    for (long ix=0; ix<h.nbin ; ++ix) { 
      double cta = abs(h.res[ix]) / dy;
      row[ix] = long(cta + 0.95);
      if(h.res[ix] < 0.) row[ix] = - row[ix];
      frac[ix] = long(10. * (cta + 1.05 - floor(cta + 0.95)));
    } 
    long rowmin = long(abs(ymin)/dy + 0.95);
    if ( ymin < 0) rowmin = - rowmin;
    long rowmax = long(abs(ymax)/dy + 0.95);
    if ( ymax < 0) rowmax = - rowmax;

    // Print histogram row by row.
    os.setf(ios::fixed, ios::floatfield); os.precision(2); 
    for (long rownow = rowmax; rownow >= rowmin; rownow--) {
      if (rownow != 0) { 
        os << "  ";
        os.width(10); os << rownow*dely ;
        os << "*10^";
        os.width(2); os << ipowy << "  ";
        for (long ix=0; ix<h.nbin ; ++ix) { 
          if (rownow == row[ix]) {os << NUMBER[frac[ix]];}
          else if (rownow * (row[ix] - rownow) > 0) {os << NUMBER[10];}
          else {os << " ";}
        } os << "\n";
      }
    } os << "\n"; 

    // Print sign and value of bin contents
    double maxim = log10(max(ymax, -ymin));
    int ipowb = int(floor(maxim + 0.0001));
    os << "          Contents  ";
    for (long ix=0; ix<h.nbin ; ++ix) {
      if (h.res[ix] < - pow(10., ipowb-4)) {os << "-";}
      else {os << " ";} 
      row[ix] = long(abs(h.res[ix]) * pow(10.,3-ipowb) + 0.5);
    } os << "\n";
    for (int rownow = 3; rownow >= 0; rownow--) {
      os << "            *10^"; 
      os.width(2); os << ipowb+rownow-3 << "  "; 
      long mask = long( pow(10., rownow) + 0.5); 
      for (long ix=0; ix<h.nbin ; ++ix) {
        os << NUMBER[(row[ix] / mask) % 10];
      } os << "\n";
    } os << "\n";

    //Print sign and value of lower bin edge.
    maxim = log10(max(-h.xmin, h.xmax - h.dx));
    int ipowe = int(floor(maxim + 0.0001));
    os << "          Low edge  ";
    for (long ix=0; ix<h.nbin ; ++ix) {
      if (h.xmin + ix * h.dx < - pow(10., ipowe-3)) {os << "-";}
      else {os << " ";} 
      row[ix] = long(abs(h.xmin + ix * h.dx) * pow(10.,2-ipowe) + 0.5);
    } os << "\n";
    for (int rownow = 2; rownow >= 0; rownow--) {
      os << "            *10^"; 
      os.width(2); os << ipowe+rownow-2 << "  "; 
      long mask = long( pow(10., rownow) + 0.5); 
      for (long ix=0; ix<h.nbin ; ++ix) {
        os << NUMBER[(row[ix] / mask) % 10];
      } os << "\n";
    } os << "\n";
  }
 
  // Calculate and print statistics.
  double csum = 0.;
  double cxsum = 0.;
  double cxxsum = 0.;
  for (long ix=0; ix<h.nbin ; ++ix) {
    double cta = abs(h.res[ix]); 
    double x = h.xmin + (ix + 0.5) * h.dx;
    csum = csum + cta;
    cxsum = cxsum + cta * x;
    cxxsum = cxxsum + cta * x * x;
  }
  double xmean = cxsum / max(csum, Hist::TINY);
  double rms = sqrtpos( cxxsum / max(csum, Hist::TINY) 
    - xmean * xmean ); 
  os.setf(ios::scientific, ios::floatfield); os.precision(4); 
  os << "   Entries  ="; os.width(12); os << h.nfill ;
  os << "    Mean ="; os.width(12); os << xmean;
  os << "    Underflow ="; os.width(12); os << h.under;
  os << "    Low edge  ="; os.width(12); os << h.xmin;
  os << "\n";
  os << "   All chan ="; os.width(12); os << h.inside;
  os << "    Rms  ="; os.width(12); os << rms;
  os << "    Overflow  ="; os.width(12); os << h.over;
  os << "    High edge ="; os.width(12); os << h.xmax;
  os << "\n";

  return os;

}

//*********

// Print vector of histogram contents as a table (e.g. for Gnuplot).

namespace Pythia7 {
namespace Shower {
void table(const vector<Hist>& h, ostream& os) {
  
  // Check that common format for histograms in vector.
  for (long i = 1; i < long(h.size()); ++i) {  
    if (!h[0].sameSize(h[i])) return;
  }
 
  // Print histogram vector bin by bin, with mean x as first column.
  os.setf(ios::scientific, ios::floatfield); os.precision(4); 
  for (long ix=0; ix<h[0].nbin; ++ix) {
    os.width(12); os << (h[0].xmin + (ix + 0.5) * h[0].dx) ;     
    for (long i = 0; i < long(h.size()); ++i) { 
      os.width(12); os << h[i].res[ix];     
    } os << "\n";
  } 
}
}
}
//**************************************************************************

// Vec4 class.
// This class implements four-vectors, in energy-momentum space.
// (But could also be used to hold space-time four-vectors.)

//*********

// Rotation (simple).

void Vec4::rot(double theta, double phi) {
  double cthe = cos(theta); double sthe = sin(theta);
  double cphi = cos(phi); double sphi = sin(phi);
  double tmpx =  cthe * cphi * xx -    sphi * yy + sthe * cphi * zz;
  double tmpy =  cthe * sphi * xx +    cphi * yy + sthe * sphi * zz;
  double tmpz = -sthe *        xx +                cthe *        zz; 
  xx = tmpx; yy = tmpy; zz = tmpz;
}

//*********

// Azimuthal rotation phi around an arbitrary axis (nz, ny, nz).

void Vec4::rotaxis(double phi, double nx, double ny, double nz) {
  double norm = 1./sqrt(nx*nx + ny*ny + nz*nz);
  nx *= norm; ny *=norm; nz *=norm; 
  double cphi = cos(phi);  double sphi = sin(phi);
  double comb = (nx * xx + ny * yy + nz * zz) * (1. - cphi);
  double tmpx = cphi * xx + comb * nx + sphi * (ny * zz - nz * yy);
  double tmpy = cphi * yy + comb * ny + sphi * (nz * xx - nx * zz);
  double tmpz = cphi * zz + comb * nz + sphi * (nx * yy - ny * xx);
  xx = tmpx; yy = tmpy; zz = tmpz;
}

//*********

// Azimuthal rotation phi around an arbitrary (3-vector component of) axis.

void Vec4::rotaxis(double phi, const Vec4& n) {
  double nx = n.xx; double ny = n.yy; double nz = n.zz;
  double norm = 1./sqrt(nx*nx + ny*ny + nz*nz);
  nx *= norm; ny *=norm; nz *=norm; 
  double cphi = cos(phi);  double sphi = sin(phi);
  double comb = (nx * xx + ny * yy + nz * zz) * (1. - cphi);
  double tmpx = cphi * xx + comb * nx + sphi * (ny * zz - nz * yy);
  double tmpy = cphi * yy + comb * ny + sphi * (nz * xx - nx * zz);
  double tmpz = cphi * zz + comb * nz + sphi * (nx * yy - ny * xx);
  xx = tmpx; yy = tmpy; zz = tmpz;
}

//*********

// Boost (simple).

void Vec4::bst(double betaX, double betaY, double betaZ) {
  double beta2 = betaX*betaX + betaY*betaY + betaZ*betaZ;
  double gamma = 1./sqrt(1.-beta2);
  double prod1 = betaX * xx + betaY * yy + betaZ * zz;
  double prod2 = gamma * (gamma * prod1 / (1. + gamma) + tt);
  xx += prod2 * betaX;
  yy += prod2 * betaY;
  zz += prod2 * betaZ;
  tt = gamma * (tt + prod1);
}

//*********

// Boost given by a Vec4.

void Vec4::bst(const Vec4& vec) {
  double betaX = vec.xx/vec.tt;
  double betaY = vec.yy/vec.tt;
  double betaZ = vec.zz/vec.tt;
  double beta2 = betaX*betaX + betaY*betaY + betaZ*betaZ;
  double gamma = 1./sqrt(1.-beta2);
  double prod1 = betaX * xx + betaY * yy + betaZ * zz;
  double prod2 = gamma * (gamma * prod1 / (1. + gamma) + tt);
  xx += prod2 * betaX;
  yy += prod2 * betaY;
  zz += prod2 * betaZ;
  tt = gamma * (tt + prod1);
}

//*********

// Arbitrary combination of rotations and boosts defined by 4 * 4 matrix.

void Vec4::rotbst(const RotBstMatrix& M) {
  double x = xx; double y = yy; double z = zz; double t = tt; 
  tt = M.M[0][0] * t + M.M[0][1] * x + M.M[0][2] * y +  M.M[0][3] * z;
  xx = M.M[1][0] * t + M.M[1][1] * x + M.M[1][2] * y +  M.M[1][3] * z;
  yy = M.M[2][0] * t + M.M[2][1] * x + M.M[2][2] * y +  M.M[2][3] * z;
  zz = M.M[3][0] * t + M.M[3][1] * x + M.M[3][2] * y +  M.M[3][3] * z;
} 

//*********

// Implementation of other functions with friends.

namespace Pythia7 {
namespace Shower {
double dot3(const Vec4& v1, const Vec4& v2)
  {return v1.xx*v2.xx + v1.yy*v2.yy + v1.zz*v2.zz;} 

Vec4 cross3(const Vec4& v1, const Vec4& v2) {
  Vec4 v; v.xx = v1.yy * v2.zz - v1.zz * v2.yy;
  v.yy = v1.zz * v2.xx - v1.xx * v2.zz;
  v.zz = v1.xx * v2.yy - v1.yy * v2.xx; return v; }

double costheta(const Vec4& v1, const Vec4& v2) {
  double cthe = v1.xx*v2.xx + v1.yy*v2.yy + v1.zz*v2.zz
    / sqrt( (v1.xx*v1.xx + v1.yy*v1.yy + v1.zz*v1.zz) 
    * (v2.xx*v2.xx + v2.yy*v2.yy + v2.zz*v2.zz) );
  cthe = max(-1., min(1., cthe));
  return cthe; } 

double cosphi(const Vec4& v1, const Vec4& v2, const Vec4& n) {
  double nx = n.xx; double ny = n.yy; double nz = n.zz;
  double norm = 1. / sqrt(nx*nx + ny*ny + nz*nz);
  nx *= norm; ny *=norm; nz *=norm; 
  double v1s = v1.xx * v1.xx + v1.yy * v1.yy + v1.zz * v1.zz;
  double v2s = v2.xx * v2.xx + v2.yy * v2.yy + v2.zz * v2.zz;
  double v1v2 = v1.xx * v2.xx + v1.yy * v2.yy + v1.zz * v2.zz;
  double v1n = v1.xx * nx + v1.yy * ny + v1.zz * nz;
  double v2n = v2.xx * nx + v2.yy * ny + v2.zz * nz;
  double cphi = (v1v2 - v1n * v2n) / sqrt( max(1e-20, 
    (v1s - v1n*v1n) * (v2s - v2n*v2n) ));  
  cphi = max(-1., min(1., cphi));
  return cphi; }

}}

//*********

// Print a four-vector: also operator overloading with friend.

ostream& Pythia7::Shower::operator<<(ostream& os, const Vec4& v) {
  os.setf(ios::fixed, ios::floatfield); os.precision(3); 
  os.width(10); os << v.xx; os.width(10); os << v.yy;  
  os.width(10); os << v.zz; os.width(10); os << v.tt << "\n";
  return os;
}

//**************************************************************************

// RotBstMatrix class.
// This class implements 4 * 4 matrices that encode an arbitrary combination
// of rotations and boosts, that can be applied to Vec4 four-vectors.

//*********

// Rotate by polar angle theta and azimuthal angle phi.

void RotBstMatrix::rot(double theta, double phi) {
  double cthe = cos(theta); double sthe = sin(theta);
  double cphi = cos(phi); double sphi = sin(phi);
  double Mrot[4][4] = { 
    {1.,           0.,         0.,          0.}, 
    {0.,  cthe * cphi,     - sphi, sthe * cphi},
    {0.,  cthe * sphi,       cphi, sthe * sphi},
    {0., -sthe,                0., cthe       } };
  double Mtmp[4][4];
  for (long i = 0; i <4; ++i) { 
    for (long j = 0; j <4; ++j) {
      Mtmp[i][j] = M[i][j]; 
    } 
  } 
  for (long i = 0; i <4; ++i) { 
    for (long j = 0; j <4; ++j) {
      M[i][j] = Mrot[i][0] * Mtmp[0][j] + Mrot[i][1] * Mtmp[1][j]
        + Mrot[i][2] * Mtmp[2][j] + Mrot[i][3] * Mtmp[3][j]; 
    } 
  } 
}

//*********

// Rotate so that vector originally along z axis becomes parallel with p.

void RotBstMatrix::rot(const Vec4& p) {
  double theta = p.theta();
  double phi = p.phi();
  rot(0., -phi);
  rot(theta, phi);
}

//*********

// Boost with velocity vector (betaX, betaY, betaZ).

void RotBstMatrix::bst(double betaX, double betaY, double betaZ) {
  double gm = 1. / sqrtpos(1. - betaX*betaX - betaY*betaY - betaZ*betaZ);
  double gf = gm*gm / (1. + gm);
  double Mbst[4][4] = { 
    { gm,           gm*betaX,           gm*betaY,          gm*betaZ },
    { gm*betaX, 1. + gf*betaX*betaX, gf*betaX*betaY, gf*betaX*betaZ },
    { gm*betaY, gf*betaY*betaX, 1. + gf*betaY*betaY, gf*betaY*betaZ },
    { gm*betaZ, gf*betaZ*betaX, gf*betaZ*betaY, 1. + gf*betaZ*betaZ } };
  double Mtmp[4][4];
  for (long i = 0; i < 4; ++i) { 
    for (long j = 0; j < 4; ++j) {
      Mtmp[i][j] = M[i][j]; 
    } 
  } 
  for (long i = 0; i < 4; ++i) { 
    for (long j = 0; j < 4; ++j) {
      M[i][j] = Mbst[i][0] * Mtmp[0][j] + Mbst[i][1] * Mtmp[1][j]
        + Mbst[i][2] * Mtmp[2][j] + Mbst[i][3] * Mtmp[3][j]; 
    } 
  } 
}

//*********

// Boost so that vector originally at rest obtains same velocity as p.

void RotBstMatrix::bst(const Vec4& p) {
  double betaX = p.px() / p.e();  
  double betaY = p.py() / p.e();  
  double betaZ = p.pz() / p.e();  
  bst(betaX, betaY, betaZ);
}

//*********

// Boost so that vector originally with same velocity as p is brought to rest.

void RotBstMatrix::bstback(const Vec4& p) {
  double betaX = -p.px() / p.e();  
  double betaY = -p.py() / p.e();  
  double betaZ = -p.pz() / p.e();  
  bst(betaX, betaY, betaZ);
}

//*********

// Boost that transforms p1 to p2, where p1^2 = p2^2 is assumed.

void RotBstMatrix::bst(const Vec4& p1, const Vec4& p2) {
  double eSum = p1.e() + p2.e();
  double betaX = (p2.px() - p1.px()) / eSum;
  double betaY = (p2.py() - p1.py()) / eSum;
  double betaZ = (p2.pz() - p1.pz()) / eSum;
  double fac = 2. / (1. + betaX*betaX + betaY*betaY + betaZ*betaZ);
  betaX *= fac; betaY *= fac; betaZ *= fac;
  bst(betaX, betaY, betaZ);
}

//*********

// Combine existing rotation/boost matrix with another one.

void RotBstMatrix::rotbst(const RotBstMatrix& Mrb) {
  double Mtmp[4][4];
  for (long i = 0; i < 4; ++i) { 
    for (long j = 0; j < 4; ++j) {
      Mtmp[i][j] = M[i][j]; 
    } 
  } 
  for (long i = 0; i < 4; ++i) { 
    for (long j = 0; j < 4; ++j) {
      M[i][j] = Mrb.M[i][0] * Mtmp[0][j] + Mrb.M[i][1] * Mtmp[1][j]
        + Mrb.M[i][2] * Mtmp[2][j] + Mrb.M[i][3] * Mtmp[3][j]; 
    } 
  } 
}

//*********

// Invert the rotation and boost.

void RotBstMatrix::invert() {
  double Mtmp[4][4];
  for (long i = 0; i < 4; ++i) { 
    for (long j = 0; j < 4; ++j) {
      Mtmp[i][j] = M[i][j]; 
    } 
  } 
  for (long i = 0; i < 4; ++i) { 
    for (long j = 0; j < 4; ++j) {
      M[i][j] = ( (i == 0 && j > 0) || (i > 0 && j == 0) ) 
        ? - Mtmp[j][i] : Mtmp[j][i]; 
    } 
  } 
}

//*********

// Boost and rotation that transforms from p1 and p2 
// to their rest frame with p1 along +z axis.

void RotBstMatrix::toCMframe(const Vec4& p1, const Vec4& p2) {
  Vec4 pSum = p1 + p2; 
  pSum.flip();
  Vec4 dir = p1;
  dir.bst(pSum);
  double theta = dir.theta();
  double phi = dir.phi();
  bst(pSum);
  rot(0., -phi);
  rot(-theta, phi);
}

//*********

// Rotation and boost that transforms from rest frame of p1 and p2
// with p1 along +z axis to actual frame of p1 and p2. (Inverse of above.)

void RotBstMatrix::fromCMframe(const Vec4& p1, const Vec4& p2) {
  Vec4 pSum = p1 + p2;
  pSum.flip();
  Vec4 dir = p1;
  dir.bst(pSum);
  pSum.flip();
  double theta = dir.theta();
  double phi = dir.phi();
  rot(0., -phi);
  rot(theta, phi);
  bst(pSum);
}

//*********

// Reset to diagonal matrix.

void RotBstMatrix::reset() {
  for (long i = 0; i < 4; ++i) { 
    for (long j = 0; j < 4; ++j) {
      M[i][j] = (i==j) ? 1. : 0.; 
    } 
  } 
} 

//*********

// Print a rotation and boost matrix: operator overloading with friend.

ostream& Pythia7::Shower::operator<<(ostream& os, const RotBstMatrix& M) {
  os.setf(ios::fixed, ios::floatfield); os.precision(5);
  os << "    Rotation/boost matrix: \n"; 
  for (long i = 0; i <4; ++i) { 
    os.width(10); os << M.M[i][0]; os.width(10); os << M.M[i][1]; 
    os.width(10); os << M.M[i][2]; os.width(10); os << M.M[i][3] << "\n";
  }
  return os;
}

//**************************************************************************

// Particle class.
// This class holds info on a particle in general.

//*********

// Print a particle.

ostream& Pythia7::Shower::operator<<(ostream& os, const Particle& pt) {
  os.width(7); os << pt.idp; os.width(3); os << pt.statusp; 
  os.width(4); os << pt.mother1p; os.width(4); os << pt.mother2p; 
  os.width(3); os << pt.prevp;
  os.width(3); os << pt.colp; os.width(3); os << pt.anticolp; 
  os.setf(ios::fixed, ios::floatfield); os.precision(3); 
  os.width(10); os << pt.px(); os.width(10); os << pt.py();  
  os.width(10); os << pt.pz(); os.width(10); os << pt.e(); 
  os.width(10); os << pt.mp << endl;
  return os;
}


//**************************************************************************

// Function to give particle masses.
 
double Pythia7::Shower::Mass(long id) {

  if ( !CurrentGenerator::isVoid() ) {
    tcPDPtr p = CurrentGenerator::current().getParticleData(id);
    if (p) return p->mass()/GeV;
  }

  double m = 0.;
  long idAbs = abs(id);
  if (idAbs == 1 || idAbs == 2) m = 0.33;
  else if (idAbs == 3) m = 0.5;
  else if (idAbs == 4) m = 1.5;
  else if (idAbs == 5) m = 4.8;
  else if (idAbs == 11) m = 0.00051; 
  else if (idAbs == 13) m = 0.106; 
  else if (idAbs == 15) m = 1.777; 

  return m; 

} 

//*********

// Function to give spin of particle as 2 * s + 1.

long Pythia7::Shower::iSpin(long id) {
  
  if ( !CurrentGenerator::isVoid() ) {
    tcPDPtr p = CurrentGenerator::current().getParticleData(id);
    if (p) return abs(int(p->iSpin()));
  }

  // Preliminaries and default value.
  const long SUSY = 1000000;
  long idAbs = abs(id);
  long spin = 0;

  // Go through various known cases; could be expanded.
       if (idAbs > 0 && idAbs <= 18) spin = 2; 
  else if (idAbs > SUSY && idAbs <= SUSY+18) spin = 1;  
  else if (idAbs > 2*SUSY && idAbs <= 2*SUSY+18) spin = 1; 
  else if (idAbs == 21) spin = 3; 
  else if (idAbs >= 22 && idAbs <= 24) spin = 3; 
  else if (idAbs >= 32 && idAbs <= 34) spin = 3; 
  else if (idAbs == 25 || (idAbs >= 35 && idAbs <= 37) ) spin = 1;
  else if (idAbs == SUSY+21) spin = 2; 
  else if (idAbs >= SUSY+22 && idAbs <= SUSY+37) spin = 2;

  return spin;

}

//*********

// Function to give charge of particle, times 3 to make integer.

long Pythia7::Shower::iCharge(long id) {
  
  if ( !CurrentGenerator::isVoid() ) {
    tcPDPtr p = CurrentGenerator::current().getParticleData(id);
    if (p) return abs(int(p->iCharge()));
  }

  // Preliminaries and default value.
  const long SUSY = 1000000;
  long idAbs = abs(id) % SUSY;
  long charge = 0;

  // Go through various known cases; could be expanded.
       if (idAbs > 0 && idAbs <= 8 && idAbs%2 == 1) charge = 1; 
  else if (idAbs > 0 && idAbs <= 8) charge = 2;  
  else if (idAbs > 10 && idAbs <= 18 && idAbs%2 == 1) charge = 2; 
  else if (idAbs > 10 && idAbs <= 18) charge = 0;  
  else if (idAbs == 24 || idAbs == 34 || idAbs == 37) charge = 3; 

  return charge;

}

//*********

// Function to give colour of particle:
// 1 = singlet; 3 = triplet or antitriplet; 8 = octet.

long Pythia7::Shower::iColour(long id) {

  if ( !CurrentGenerator::isVoid() ) {
    tcPDPtr p = CurrentGenerator::current().getParticleData(id);
    if (p) return abs(int(p->iColour()));
  }

  // Preliminaries and default value.
  const long SUSY = 1000000;
  long idAbs = abs(id) % SUSY;
  long col = 0;

  // Go through various known cases; could be expanded.
       if (idAbs > 0 && idAbs <= 8) col = 3;
  else if (idAbs > 10 && idAbs <= 18) col = 1;
  else if (idAbs == 21) col = 8;
  else if (idAbs >= 22 && idAbs <= 37) col = 1; 

  return col;

}
