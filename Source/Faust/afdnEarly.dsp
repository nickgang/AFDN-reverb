//======================Ambisonics Feedback Delay Network (AFDN) Reverberator========================
//========================================================================================

import("stdfaust.lib");

//--------------------------------`afdnEarly0`---------------------------------
// Early reflection section of Ambisonics FDN reverb, build on top of
// standard Faust function `afdnrev0`.
//
// #### Usage
//
// ```
// <1,2,4,...,N signals> <:
// afdnrev0(MAXDELAY,delays,BBSO,freqs,durs,loopgainmax,nonl) :>
// <1,2,4,...,N signals>
// ```
//
// Where:
//
// * `N`: 2, 4, 8, ...  (power of 2)
// * `MAXDELAY`: power of 2 at least as large as longest delay-line length
// * `delays`: N delay lines, N a power of 2, lengths perferably coprime
// * `BBSO`: odd positive integer = order of bandsplit desired at freqs
// * `freqs`: NB-1 crossover frequencies separating desired frequency bands
// * `durs`: NB decay times (t60) desired for the various bands
// * `loopgainmax`: scalar gain between 0 and 1 used to "squelch" the reverb
// * `nonl`: nonlinearity (0 to 0.999..., 0 being linear)
//
// #### Reference
//
// <https://ccrma.stanford.edu/~jos/pasp/FDN_Reverberation.html>
// <https://github.com/nickgang/AFDN-reverb>
//------------------------------------------------------------
afdnEarly0(MAXDELAY, delays, BBSO, freqs, durs, loopgainmax, nonl)
  = (si.bus(N) : delaylines(N)) : (delayfilters(N,freqs,durs) :
    feedbackmatrix(N))
with {
  N = ba.count(delays);
  NB = ba.count(durs);
//assert(count(freqs)+1==NB);
  delayval(i) = ba.take(i+1,delays);
  dlmax(i) = MAXDELAY; // must hardwire this from argument for now
//dlmax(i) = 2^max(1,nextpow2(delayval(i))) // try when slider min/max is known
//           with { nextpow2(x) = ceil(log(x)/log(2.0)); };
// -1 is for feedback delay:
  delaylines(N) = par(i,N,(de.delay(dlmax(i),(delayval(i)-1))));
  delayfilters(N,freqs,durs) = par(i,N,filter(i,freqs,durs));
  feedbackmatrix(N) = bhadamard(N);
  vbutterfly(n) = si.bus(n) <: (si.bus(n):>bus(n/2)) , ((si.bus(n/2),(si.bus(n/2):par(i,n/2,*(-1)))) :> si.bus(n/2));
  bhadamard(2) = si.bus(2) <: +,-;
  bhadamard(n) = si.bus(n) <: (si.bus(n):>si.bus(n/2)) , ((si.bus(n/2),(si.bus(n/2):par(i,n/2,*(-1)))) :> si.bus(n/2))
                 : (bhadamard(n/2) , bhadamard(n/2));

  // Experimental nonlinearities:
  // nonlinallpass = apnl(nonl,-nonl);
  // s = nonl*PI;
  // nonlinallpass(x) = allpassnn(3,(s*x,s*x*x,s*x*x*x)); // filters.lib
     nonlinallpass = _; // disabled by default (rather expensive)

  filter(i,freqs,durs) = fi.filterbank(BBSO,freqs) : par(j,NB,*(g(j,i)))
                         :> *(loopgainmax) / sqrt(N) : nonlinallpass
  with {
    dur(j) = ba.take(j+1,durs);
    n60(j) = dur(j)*ma.SR; // decay time in samples
    g(j,i) = exp(-3.0*log(10.0)*delayval(i)/n60(j));
        // ~ 1.0 - 6.91*delayval(i)/(SR*dur(j)); // valid for large dur(j)
  };
};
