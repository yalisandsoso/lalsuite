/************************************ <lalVerbatim file="GeneratePulsarSignalHV">
Author: Prix, Reinhard
$Id$
************************************* </lalVerbatim> */

/********************************************************** <lalLaTeX>
\section{Header \texttt{GeneratePulsarSignal.h}}
\label{s:GeneratePulsarSignal.h}

Header file for GeneratePulsarSignal.

\subsection*{Synopsis}
\begin{verbatim}
#include "GeneratePulsarSignal.h"
\end{verbatim}

\noindent 

******************************************************* </lalLaTeX> */

#ifndef _GENERATEPULSARSIGNAL_H  /* Double-include protection. */
#define _GENERATEPULSARSIGNAL_H

#include <lal/LALDatatypes.h>
#include <lal/DetectorSite.h>
#include <lal/GenerateSpinOrbitCW.h>
#include <lal/Date.h>
#include <lal/LALBarycenter.h>

/* C++ protection. */
/* #ifdef  __cplusplus    */
/* extern "C" { */
/* #endif */

NRCSID( PULSARSIGNALH, "$Id$");

/********************************************************** <lalLaTeX>
\subsection*{Error codes}
</lalLaTeX>
***************************************************** <lalErrTable> */
#define PULSARSIGNALH_ENULL 		1
#define PULSARSIGNALH_ENONULL		2
#define PULSARSIGNALH_EMEM		3
#define PULSARSIGNALH_ESAMPLING		4
#define PULSARSIGNALH_ESSBCONVERT	5
#define PULSARSIGNALH_ESYS		6

#define PULSARSIGNALH_MSGENULL 		"Arguments contained an unexpected null pointer"
#define PULSARSIGNALH_MSGENONULL	"Output pointer is not NULL"
#define PULSARSIGNALH_MSGEMEM		"Out of memory"
#define PULSARSIGNALH_MSGESAMPLING	"Waveform sampling interval too large."
#define PULSARSIGNALH_MSGESSBCONVERT	"SSB->GPS iterative conversion failed"
#define PULSARSIGNALH_MSGESYS		"System error, probably while File I/O"

/*************************************************** </lalErrTable> */

  
/********************************************************** <lalLaTeX>
\vfill{\footnotesize\input{GeneratePulsarSignalHV}}
\newpage\input{GeneratePulsarSignalC}
******************************************************* </lalLaTeX> */

/* New structures and types */

typedef struct {
  LIGOTimeGPS TRefSSB;	/* reference time for pulsar parameters (in SSB time!) if not given, startTimeGPS is used */
  REAL8 Alpha;		/* source location in equatorial coordinates (in radians) */
  REAL8 Delta;
  REAL4 psi;            /* polarization angle (radians) at TRef */
  REAL4 aPlus, aCross;    /* polarization amplitudes at TRef */
  REAL8 phi0;             /* initial phase (radians) at TRef */
  REAL8 f0;               /* initial frequency (Hz) at TRef */
  REAL8Vector *f;         /* f0-normalized Taylor parameters at TRef */
} PulsarSourceParams;

typedef struct {
  LIGOTimeGPS orbitEpoch; /* time of a periapsis passage */
  REAL8 omega;            /* argument of periapsis (radians) */
  REAL8 rPeriNorm;        /* projected, normalized periapsis (s) */
  REAL8 oneMinusEcc;      /* 1 - orbital eccentricity */
  REAL8 angularSpeed;     /* angular speed at periapsis (Hz) */
} BinaryOrbitParams;

typedef struct {
  /* defining the actual pulsar-source */
  PulsarSourceParams pulsar;
  /* and its binary orbit if applicable (NULL if not) */
  BinaryOrbitParams *orbit;
  /* characterize the detector */
  COMPLEX8FrequencySeries *transferFunction;    /* frequency transfer function */
  LALDetector *site;        		   	/* detector location and orientation */  
  EphemerisData *ephemerides;  			/* Earth and Sun ephemerides */
  
  /* characterize the output time-series */
  LIGOTimeGPS startTimeGPS;     /* start time of output time series */
  UINT4 duration;           	/* length of time series in s*/
  REAL8 samplingRate;		/* sampling rate of time-series (=2 * fmax) */
  REAL8 fHeterodyne;		/* heterodyning frequency for output time-series */
} PulsarSignalParams;


/* Function prototypes */
void LALGeneratePulsarSignal (LALStatus *stat, REAL4TimeSeries *signal, PulsarSignalParams *params);
void LALPrintR4TimeSeries (LALStatus *stat, REAL4TimeSeries *series, const CHAR *fname);
void PrintGWSignal (LALStatus *stat, CoherentGW *signal, const CHAR *fname);

/********************************************************** <lalLaTeX>
\newpage\input{LALSampleTestC}
******************************************************* </lalLaTeX> */

/* #ifdef  __cplusplus */
/* } */
/* #endif   */  
/* C++ protection. */

#endif  /* Double-include protection. */
