 /*----------------------------------------------------------------------- 
 * 
 * File Name: FindChirpBCVSpinFilter.c
 *
 * Author: Brown D. A., Spinning BCV-Modifications: Jones, G
 * 
 * Revision: $Id$
 * 
 *-----------------------------------------------------------------------
 */

#if 0 
<lalVerbatim file="FindChirpBCVSpinFilterCV">
Author: Brown, D. A., Spinning BCV-Modifications: Jones, G.
$Id$
</lalVerbatim> 

<lalLaTeX>
\subsection{Module \texttt{FindChirpBCVSpinFilter.c}}
\label{ss:FindChirpBCVSpinFilter.c}

\input{FindChirpBCVSpinFilterCDoc}

\vfill{\footnotesize\input{FindChirpBCVSpinFilterCV}}
</lalLaTeX> 
#endif

#include <math.h>
#include <lal/LALStdio.h>
#include <lal/LALStdlib.h>
#include <lal/LALConstants.h>
#include <lal/Date.h>
#include <lal/AVFactories.h>
#include <lal/FindChirp.h>
#include <lal/FindChirpSP.h>


NRCSID (FINDCHIRPBCVSPINFILTERC, "$Id$");

/*documenation later*/
void
LALFindChirpBCVSpinFilterSegment (
    LALStatus                  *status,
    SnglInspiralTable         **eventList,
    FindChirpFilterInput       *input,
    FindChirpFilterParams      *params,             
    FindChirpSPDataParams      *inputParams,
    FindChirpSegmentVector     *fcSegVec,
    DataSegmentVector          *dataSegVec
  )

{
  UINT4                 j, k;
  UINT4                 numPoints;
  UINT4                 deltaEventIndex;
  UINT4                 ignoreIndex;
  REAL4                 deltaT;
  REAL4                 norm;
  REAL4                 modqsqThresh;
  REAL4                 rhosqThresh;
  REAL4                 mismatch;
  REAL4                 chisqThreshFac;
  REAL4                 modChisqThresh;
  UINT4                 numChisqBins;
  UINT4                 eventStartIdx = 0;
  REAL4                 chirpTime     = 0;
  BOOLEAN               haveChisq     = 0;
  COMPLEX8             *qtilde        = NULL; 
  COMPLEX8             *qtildeBCV     = NULL; 
  COMPLEX8             *q             = NULL; 
  COMPLEX8             *qBCV          = NULL;
  COMPLEX8             *inputData     = NULL;
  COMPLEX8             *inputDataBCV  = NULL;
  COMPLEX8             *tmpltSignal   = NULL;
  SnglInspiralTable    *thisEvent     = NULL;
  LALMSTUnitsAndAcc     gmstUnits;
  FindChirpSegment      *fcSeg;
  DataSegment           *dataSeg; 
  REAL4                 templateNorm;
  REAL4                 modqsq;
  COMPLEX8              *wtilde;  /* need new pointer name? */
  REAL4                 *amp;
  REAL4                 *ampBCV;
  REAL4                 I = 0.0;
  REAL4                 J = 0.0;
  REAL4                 K = 0.0;
  REAL4                 L = 0.0;
  REAL4                 M = 0.0;
  REAL4                 Beta; /* Spin parameter, value from bank or external loop */  
  REAL4                 denominator;
  REAL4                 denominator1;
  REAL4                 a1;
  REAL4                 a2;                  
  REAL4                 a3;     
  COMPLEX8             *outputData1;
  COMPLEX8             *outputData2;
  COMPLEX8             *outputData3;
  FindChirpChisqInput  *chisqInput;
  FindChirpChisqInput  *chisqInputBCV;

  INITSTATUS( status, "LALFindChirpBCVSpinFilter", FINDCHIRPBCVSPINFILTERC );
  ATTATCHSTATUSPTR( status );

/*declaration*/

/*code*/

  amp        = inputParams->ampVec->data;
  ampBCV     = inputParams->ampVecBCV->data;
  wtilde     = inputParams->wtildeVec->data;

  for ( k = 1; k < fcSeg->data->data->length; ++k )
  {
    I += 4.0 * amp[k] * amp[k] * wtilde[k].re ;
    J += 4.0 * amp[k] * amp[k] * wtilde[k].re * 
      cos(Beta * amp[k] / ampBCV[k]);                
    K += 4.0 * amp[k] * amp[k] * wtilde[k].re * 
      sin(Beta * amp[k] / ampBCV[k]);
    L += 4.0 * amp[k] * amp[k] * wtilde[k].re * 
      sin(2 * Beta * amp[k] / ampBCV[k]);
    M += 4.0 * amp[k] * amp[k] * wtilde[k].re * 
      cos(2 * Beta * amp[k] / ampBCV[k]);
  }

 denominator = I*M  +  0.5*pow(I,2) - pow(J,2);
 denominator1 = sqrt ( 0.25 * pow(I,3) + M*(pow(J,2) - 
        pow(K,2)) - 0.5*(pow(J,2) + pow(K,2)) - I*(pow(L,2) + 
        pow(M,2)) + 2*J*K*L );

  /* 
   * the calculation of the orthonormalised 
   * amplitude vectors a1, a2, a3 
   *
   */   


  a1 = 1.0 * amp[k]/ sqrt(I) ;

  a2 = 1.0 * amp[k]/sqrt(denominator) * (I * cos(Beta * amp[k]) -  J);

  a3 = 1.0 * amp[k]/denominator1 * ( sin(Beta * amp[k]) - 
      (I*L - J*K)*cos(Beta * amp[k])/denominator + 
      (J*L - K*M + 0.5*I*K)/denominator );

  
 

  /*
   * initialising outputData vectors to
   * calibrated detector output as calc in LAL..Data()
   * note lack of exponential terms, these are
   * calc in LALFindChirpBCVSpinTemplate()
   */



  outputData1 = fcSeg->data->data->data;
  outputData2 = fcSeg->data->data->data;
  outputData3 = fcSeg->data->data->data;
  
  outputData1[k].re *= a1 * wtilde[k].re;
  outputData2[k].re *= a2 * wtilde[k].re;
  outputData3[k].re *= a3 * wtilde[k].re;
  

  /*
   * imaginary parts? square and add to find SNR
   *
   */

  DETATCHSTATUSPTR( status );
  RETURN( status );
}
