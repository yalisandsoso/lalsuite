/**** <lalVerbatim file="FrameCalibrationCV">
 * Author: Brown, D. A.
 * $Id$
 **** </lalVerbatim> */

/**** <lalLaTeX>
 *
 * \subsection{Module \texttt{FrameCalibration.c}}
 * \label{ss:FrameCalibration.c}
 *
 * This module contains code used to extract calibration information contained
 * in frame files, and to construct a response (or transfer) function.  This
 * is supposed to provide a high-level interface for search authors to obtain a
 * response function in the desired form.
 *
 * \subsection*{Prototypes}
 * \input{FrameCalibrationCP}
 *
 * The routine \texttt{LALFrameExtractResponse()} extracts the necessary
 * calibration information from the frames. The frames used to construct
 * the calibration are located using the specified catalog. The function
 * constructs a response function (as a frequency series) from this
 * information.  If the fourth argument is non-\texttt{NULL} then this
 * string specifies the detector (H1, H2, L1, etc.) for which the calibration
 * is required.  Certain fields of the output should be set before this
 * routine is called.  In particular: 
 * \begin{enumerate}
 * \item The epoch field of the frequency series should be set to the correct
 * epoch so that the routine can generate a response function tailored to that
 * time (accounting for calibration drifts). 
 *
 * \item The units of the response function should be set to be either
 * strain-per-count (for a response function) or count-per-strain (for a
 * transfer function); the routine will then return either the response
 * function or its inverse depending on the specified units.  Furthermore, the
 * power-of-ten field of the units is examined to scale the response function
 * accordingly. 
 *
 * \item The data vector should be allocated to the required length and the
 * frequency step size should be set to the required value so that the routine
 * can interpolate the response function to the required frequencies.
 * \end{enumerate}
 * The format of the frame catalog must be as follows.
 * \begin{enumerate}
 * \item It must contain an entry for one frame of type \verb+CAL_REF+ which 
 * contains the response and cavity gain frequency series that will be up
 * dated to the specified point in time. For example it must contain an entry
 * such as
 * \begin{verbatim}
 * L CAL_REF 715388533 64 file://localhost/path/to/L-CAL_REF-715388533-64.gwf
 * \end{verbatim}
 * where the frame file contains the channels \verb+L1:CAL-RESPONSE+ and
 * \verb+L1:CAL-CAV_GAIN+.
 *
 * \item It must also contain entries for the frames needed to update the
 * point calibration to the current time. These must contain the
 * \verb+L1:CAL-OLOOP_FAC+ and \verb+L1:CAL-CAV_FAC+ channels. The update
 * factor frames may either be SenseMon type frames, containing the factor
 * channels as \verb+real_8+ trend data or frames generated by the lalapps
 * program \verb+lalapps_mkcalfac+ which creates channels of type
 * \verb+complex_8+. The entries in the cahce file must be of the format
 * \begin{verbatim}
 * L CAL_FAC 714240000 1369980 file://localhost/path/to/L-CAL_FAC-714240000-1369980.gwf
 * \end{verbatim}
 * for \verb+lalapps_mkcalfac+ type frames or
 * \begin{verbatim}
 * L SenseMonitor_L1_M 729925200 3600 file://localhost/path/to/L-SenseMonitor_L1_M-729925200-3600.gwf
 * \end{verbatim}
 * for SenseMon type frames.  If both types of frame are present in the cache,
 * SenseMon frames are used in preference.
 * \end{enumerate}
 *
 * \vfill{\footnotesize\input{FrameCalibrationCV}}
 *
 **** </lalLaTeX> */
#include <string.h>
#include <lal/LALStdlib.h>
#include <lal/AVFactories.h>
#include <lal/Calibration.h>
#include <lal/FrameStream.h>
#include <lal/FrameCalibration.h>

NRCSID( FRAMECALIBRATIONC, "$Id$" );

#define RESPONSE_CHAN "CAL-RESPONSE"
#define CAV_GAIN_CHAN "CAL-CAV_GAIN"
#define OLOOP_FAC_CHAN "CAL-OLOOP_FAC"
#define CAV_FAC_CHAN "CAL-CAV_FAC"

#define REF_TYPE "CAL_REF"
#define FAC_TYPE "CAL_FAC"
#define SENSEMON_FAC_TYPE "SenseMonitor_%s_M"

#define RETURN_POINT_CAL \
  calfuncs.responseFunction->sampleUnits = strainPerCount; \
  TRY( LALResponseConvert( status->statusPtr, \
        output, calfuncs.responseFunction ), status ); \
  if ( R0.data ) { \
    TRY( LALCDestroyVector( status->statusPtr, &R0.data ), status ); \
  } \
  if ( C0.data ) { \
    TRY( LALCDestroyVector( status->statusPtr, &C0.data ), status ); \
  } \
  TRY( LALDestroyFrCache( status->statusPtr, &calCache ), status );

/* <lalVerbatim file="FrameCalibrationCP"> */
void
LALExtractFrameResponse(
    LALStatus               *status,
    COMPLEX8FrequencySeries *output,
    const CHAR              *catalog,
    const CHAR              *ifo
    )
{ /* </lalVerbatim> */
  UINT4 k;
  const LALUnit strainPerCount = {0,{0,0,0,0,0,1,-1},{0,0,0,0,0,0,0}};

  FrCache      *calCache  = NULL;
  FrCache      *refCache  = NULL;
  FrCache      *facCache  = NULL;
  FrStream     *refStream = NULL;
  FrStream     *facStream = NULL;
  FrCacheSieve  sieve;
  FrChanIn      frameChan;
  FrPos         facPos;
  
  CHAR          facDsc[LALNameLength];
  CHAR          channelName[LALNameLength];

  COMPLEX8FrequencySeries       R0;
  COMPLEX8FrequencySeries       C0;
  COMPLEX8TimeSeries            ab;
  COMPLEX8TimeSeries            a;
  COMPLEX8Vector                abVec;
  COMPLEX8Vector                aVec;
  COMPLEX8                      abData;
  COMPLEX8                      aData;
  CalibrationFunctions          calfuncs;
  CalibrationUpdateParams       calfacts;

  INITSTATUS( status, "LALFrameExtractResponse", FRAMECALIBRATIONC );
  ATTATCHSTATUSPTR( status );

  ASSERT( output, status, 
      FRAMECALIBRATIONH_ENULL, FRAMECALIBRATIONH_MSGENULL );
  ASSERT( output->data, status, 
      FRAMECALIBRATIONH_ENULL, FRAMECALIBRATIONH_MSGENULL );
  ASSERT( output->data->data, status, 
      FRAMECALIBRATIONH_ENULL, FRAMECALIBRATIONH_MSGENULL );
  ASSERT( catalog, status, 
      FRAMECALIBRATIONH_ENULL, FRAMECALIBRATIONH_MSGENULL );
  ASSERT( ifo, status, 
      FRAMECALIBRATIONH_ENULL, FRAMECALIBRATIONH_MSGENULL );

  /* set up and clear the structures to hold the input data */
  memset( &R0, 0, sizeof(COMPLEX8FrequencySeries) );
  memset( &C0, 0, sizeof(COMPLEX8FrequencySeries) );
  memset( &ab, 0, sizeof(COMPLEX8TimeSeries) );
  memset( &a,  0, sizeof(COMPLEX8TimeSeries) );
  ab.data = &abVec;
  a.data  = &aVec;
  abVec.length = 1;
  aVec.length  = 1;
  abVec.data = &abData;
  aVec.data  = &aData;
  calfuncs.responseFunction = &R0;
  calfuncs.sensingFunction  = &C0;
  calfacts.openLoopFactor   = &ab;
  calfacts.sensingFactor    = &a;
  calfacts.epoch = output->epoch;
  frameChan.name = channelName;

  /* open the cache and sieve to get the reference cal and update factors */
  LALFrCacheImport( status->statusPtr, &calCache, catalog );
  if ( status->statusPtr->statusCode )
  {
    ABORT( status, FRAMECALIBRATIONH_EMCHE, FRAMECALIBRATIONH_MSGEMCHE );
  }
  memset( &sieve, 0, sizeof(FrCacheSieve) );
  sieve.dscRegEx = REF_TYPE;
  LALFrCacheSieve( status->statusPtr, &refCache, calCache, &sieve );
  CHECKSTATUSPTR( status );
  if ( ! refCache->numFrameFiles )
  {
    /* if we don't have a reference calibration, we can't do anything */
    LALDestroyFrCache( status->statusPtr, &refCache );
    CHECKSTATUSPTR( status );
    ABORT( status, FRAMECALIBRATIONH_ECREF, FRAMECALIBRATIONH_MSGECREF );
  }

  /* read in the reference calibration */
  LALFrCacheOpen( status->statusPtr, &refStream, refCache );
  CHECKSTATUSPTR( status );
  
  LALSnprintf( channelName, LALNameLength * sizeof(CHAR), 
      "%s:" RESPONSE_CHAN,  ifo );
  LALFrGetCOMPLEX8FrequencySeries( status->statusPtr, 
      &R0, &frameChan, refStream );
  CHECKSTATUSPTR( status );
  
  LALSnprintf( channelName, LALNameLength * sizeof(CHAR), 
      "%s:" CAV_GAIN_CHAN,  ifo );
  LALFrGetCOMPLEX8FrequencySeries( status->statusPtr, 
      &C0, &frameChan, refStream );
  if ( status->statusPtr->statusCode )
  {
    /* no cavity gain response to update point cal */
    LALDestroyFrCache( status->statusPtr, &refCache );
    CHECKSTATUSPTR( status );
    LALFrClose( status->statusPtr, &refStream );
    CHECKSTATUSPTR( status );
    RETURN_POINT_CAL
    ABORT( status, FRAMECALIBRATIONH_EGAIN, FRAMECALIBRATIONH_MSGEGAIN );
  }
  
  LALDestroyFrCache( status->statusPtr, &refCache );
  CHECKSTATUSPTR( status );
  LALFrClose( status->statusPtr, &refStream );
  CHECKSTATUSPTR( status );

  /* try and get some update factors. first we try to get a cache  */
  /* containing sensemon frames. if that fails, try the S1 type    */
  /* calibration data, otherwise just return the point calibration */
  sieve.dscRegEx = facDsc;
  do
  {
    /* try and get sensemon frames */
    LALSnprintf( facDsc, LALNameLength * sizeof(CHAR), 
        SENSEMON_FAC_TYPE, ifo );
    LALFrCacheSieve( status->statusPtr, &facCache, calCache, &sieve );
    CHECKSTATUSPTR( status );
    if ( facCache->numFrameFiles )
    { 
      /* sensemon stores fac times series as real_8 adc trend data */
      REAL8TimeSeries     sensemonTS;
      REAL8Vector         sensemonTSVec;
      REAL8               sensemonTSData;
      sensemonTS.data    = &sensemonTSVec;
      sensemonTSVec.data = &sensemonTSData;
      sensemonTSVec.length = 1;
      sensemonTS.epoch = output->epoch;
      
      LALFrCacheOpen( status->statusPtr, &facStream, facCache );
      CHECKSTATUSPTR( status );
      LALFrSeek( status->statusPtr, &(output->epoch), facStream );
      CHECKSTATUSPTR( status );
      LALFrGetPos( status->statusPtr, &facPos, facStream );
      CHECKSTATUSPTR( status );

      LALSnprintf( channelName, LALNameLength * sizeof(CHAR), 
          "%s:" CAV_FAC_CHAN ".mean" ,  ifo );
      LALFrGetREAL8TimeSeries( status->statusPtr, 
          &sensemonTS, &frameChan, facStream );
      BEGINFAIL( status )
      {
        TRY( LALDestroyFrCache( status->statusPtr, &facCache ), status );
        TRY( LALFrClose( status->statusPtr, &facStream ), status );
        RETURN_POINT_CAL;
      }
      ENDFAIL( status );

      a.data->data[0].re = (REAL4) sensemonTSData;
      a.data->data[0].im = 0;
      a.epoch  = sensemonTS.epoch;
      a.deltaT = sensemonTS.deltaT;
      strncpy( a.name, sensemonTS.name, LALNameLength );

      LALFrSetPos( status->statusPtr, &facPos, facStream );
      CHECKSTATUSPTR( status );

      LALSnprintf( channelName, LALNameLength * sizeof(CHAR), 
          "%s:" OLOOP_FAC_CHAN ".mean",  ifo );
      LALFrGetREAL8TimeSeries( status->statusPtr, 
          &sensemonTS, &frameChan, facStream );
      BEGINFAIL( status )
      {
        TRY( LALDestroyFrCache( status->statusPtr, &facCache ), status );
        TRY( LALFrClose( status->statusPtr, &facStream ), status );
        RETURN_POINT_CAL;
      }
      ENDFAIL( status );

      ab.data->data[0].re = (REAL4) sensemonTSData;
      ab.data->data[0].im = 0;
      ab.epoch  = sensemonTS.epoch;
      ab.deltaT = sensemonTS.deltaT;
      strncpy( ab.name, sensemonTS.name, LALNameLength );

      break; 
    }
    else
    {
      /* destroy the empty frame cache and try again */
      LALDestroyFrCache( status->statusPtr, &facCache );
      CHECKSTATUSPTR( status );
    }

    /* try and get the old type of frames */
    sieve.dscRegEx = FAC_TYPE;
    LALFrCacheSieve( status->statusPtr, &facCache, calCache, &sieve );
    CHECKSTATUSPTR( status );
    if ( facCache->numFrameFiles )
    {
      /* the old frames are complex_8 proc data */
      LALFrCacheOpen( status->statusPtr, &facStream, facCache );
      CHECKSTATUSPTR( status );
      LALFrSeek( status->statusPtr, &(output->epoch), facStream );
      CHECKSTATUSPTR( status );
      LALFrGetPos( status->statusPtr, &facPos, facStream );
      CHECKSTATUSPTR( status );

      LALSnprintf( channelName, LALNameLength * sizeof(CHAR), 
          "%s:" CAV_FAC_CHAN,  ifo );
      LALFrGetCOMPLEX8TimeSeries( status->statusPtr, 
          &a, &frameChan, facStream );
      BEGINFAIL( status )
      {
        TRY( LALDestroyFrCache( status->statusPtr, &facCache ), status );
        TRY( LALFrClose( status->statusPtr, &facStream ), status );
        RETURN_POINT_CAL;
      }
      ENDFAIL( status );

      LALFrSetPos( status->statusPtr, &facPos, facStream );
      CHECKSTATUSPTR( status );

      LALSnprintf( channelName, LALNameLength * sizeof(CHAR), 
          "%s:" OLOOP_FAC_CHAN,  ifo );
      LALFrGetCOMPLEX8TimeSeries( status->statusPtr, 
          &ab, &frameChan, facStream );
      BEGINFAIL( status )
      {
        TRY( LALDestroyFrCache( status->statusPtr, &facCache ), status );
        TRY( LALFrClose( status->statusPtr, &facStream ), status );
        RETURN_POINT_CAL;
      }
      ENDFAIL( status );

      break;
    }
    else
    {
      /* destroy the empty frame cache and give up */
      LALDestroyFrCache( status->statusPtr, &facCache );
      CHECKSTATUSPTR( status );
    }

    /* no update factors available, so just return the point cal */
    RETURN_POINT_CAL;
    ABORT( status, FRAMECALIBRATIONH_ECFAC, FRAMECALIBRATIONH_MSGECFAC );

  } while ( 0 );

  /* close the update factor stream */
  LALDestroyFrCache( status->statusPtr, &facCache );
  CHECKSTATUSPTR( status );
  LALFrClose( status->statusPtr, &facStream );
  CHECKSTATUSPTR( status );

  /* should be able to update into the same functions... */
  calfuncs.responseFunction->sampleUnits = strainPerCount;
  LALUpdateCalibration( status->statusPtr, &calfuncs, &calfuncs, &calfacts );
  BEGINFAIL( status )
  {
    RETURN_POINT_CAL;
  }
  ENDFAIL( status );

  /* now convert response to get output, hardwire units */
  LALResponseConvert( status->statusPtr, output, calfuncs.responseFunction );
  CHECKSTATUSPTR( status );

  /* free the allocated memory */
  LALDestroyFrCache( status->statusPtr, &calCache );
  CHECKSTATUSPTR( status );
  LALCDestroyVector( status->statusPtr, &R0.data );
  CHECKSTATUSPTR( status );
  LALCDestroyVector( status->statusPtr, &C0.data );
  CHECKSTATUSPTR( status );

  DETATCHSTATUSPTR( status );
  RETURN( status );
}
