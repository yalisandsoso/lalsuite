/*----------------------------------------------------------------------- 
 * 
 * File Name: FindChirpBCVTemplate.c
 *
 * Author: Brown D. A., Messaritaki, E., and Woods, D.
 * 
 * Revision: $Id$
 * 
 *-----------------------------------------------------------------------
 */

#if 0 
<lalVerbatim file="FindChirpBCVTemplateCV">
Author: Brown, D. A., Messaritaki, E., and Woods, D.
$Id$
</lalVerbatim> 

<lalLaTeX>
\subsection{Module \texttt{FindChirpBCVTemplate.c}}
\label{ss:FindChirpBCVTemplate.c}

Provides functions to create BCV detection templates in a form that can be
used by the \texttt{FindChirpBCVFilter()} function.

\subsubsection*{Prototypes}
\vspace{0.1in}
\input{FindChirpBCVTemplateCP}
\idx{LALFindChirpBCVTemplate()}

The function \texttt{LALFindChirpBCVTemplate()} creates the BCV
template as described by the algorithm below.

\subsubsection*{Algorithm}

Blah.

\subsubsection*{Uses}
\begin{verbatim}
LALCalloc()
LALFree()
LALCreateVector()
LALDestroyVector()
\end{verbatim}

\subsubsection*{Notes}

\vfill{\footnotesize\input{FindChirpBCVTemplateCV}}
</lalLaTeX> 
#endif

#include <lal/LALStdlib.h>
#include <lal/AVFactories.h>
#include <lal/DataBuffer.h>
#include <lal/LALInspiral.h>
#include <lal/FindChirp.h>
#include <lal/FindChirpBCV.h>


NRCSID (FINDCHIRPBCVTEMPLATEC, "$Id$");


/* <lalVerbatim file="FindChirpBCVTemplateCP"> */
void
LALFindChirpBCVTemplate (
    LALStatus                  *status,
    FindChirpTemplate          *fcTmplt,
    InspiralTemplate           *tmplt,
    FindChirpTmpltParams       *params
    )
/* </lalVerbatim> */
{
  UINT4        numPoints  = 0;
  REAL4        deltaF     = 0.0;
  REAL4        m          = 0.0;  
  /* REAL4        chirpMass  = 0.0; */
  REAL4        eta        = 0.0; 
  REAL4        mu         = 0.0;  /* now only used in normalisation */
  COMPLEX8    *expPsi     = NULL;
  REAL4       *xfac       = NULL;
  REAL4        x1         = 0.0;  
  REAL4        psi0       = 0.0;
  REAL4        psi00      = 0.0;
  REAL4        psi05      = 0.0;
  REAL4        psi10      = 0.0;
  REAL4        psi15      = 0.0;
  REAL4        psi20      = 0.0;        
  REAL4        fHi        = 0.0;  
  INT4         k          = 0;    
  INT4         kmin       = 0;     
  INT4         kmax       = 0;    
  REAL4        distNorm;
  const REAL4  cannonDist = 1.0; /* Mpc */
  
  INITSTATUS( status, "LALFindChirpBCVTemplate", FINDCHIRPBCVTEMPLATEC );
  ATTATCHSTATUSPTR( status );


  /*
   *
   * check that the arguments are reasonable
   *
   */
  

  /* check that the output structures exist */
  ASSERT( fcTmplt, status, 
      FINDCHIRPBCVH_ENULL, FINDCHIRPBCVH_MSGENULL );
  ASSERT( fcTmplt->data, status, 
      FINDCHIRPBCVH_ENULL, FINDCHIRPBCVH_MSGENULL );
  ASSERT( fcTmplt->data->data, status,
      FINDCHIRPBCVH_ENULL, FINDCHIRPBCVH_MSGENULL );

  /* check that the parameter structure exists */         
  ASSERT( params, status, FINDCHIRPBCVH_ENULL, FINDCHIRPBCVH_MSGENULL );

  /* check that the parameter structure is set */
  /* to the correct waveform approximant       */
  ASSERT( params->approximant == BCV, status, 
      FINDCHIRPBCVH_EMAPX, FINDCHIRPBCVH_MSGEMAPX );

  /* check that the timestep is positive */
  ASSERT( params->deltaT > 0, status,                        
      FINDCHIRPBCVH_EDELT, FINDCHIRPBCVH_MSGEDELT );

  /* check that the input exists */
  ASSERT( tmplt, status, FINDCHIRPBCVH_ENULL, FINDCHIRPBCVH_MSGENULL );


  /*
   *
   * compute the BCV template
   *
   */


  /* set up pointers */
  expPsi    = fcTmplt->data->data;
  xfac      = params->xfacVec->data;
  numPoints = 2 * (fcTmplt->data->length - 1);

  /* store the waveform approximant */
  tmplt->approximant = BCV;

  /* zero output */
  memset( expPsi, 0, numPoints * sizeof(COMPLEX8) );

  /* psi coefficients; BCV only uses psi0, psi15: */
  psi00 = tmplt->psi0;  
  psi05 = 0.0; /*tmplt->psi1;*/
  psi10 = 0.0; /*tmplt->psi2;*/ 
  psi15 = tmplt->psi3;      
  psi20 = 0.0; /*tmplt->psi4;*/

  /* parameters */
  deltaF = 1.0 / ( (REAL4) params->deltaT * (REAL4) numPoints ); 
  /* m and mu MUST BE in units of Msun for the calculation of tmpltNorm */
  m    = - psi15 / ( 16 * LAL_PI * LAL_PI * psi00 ) /  LAL_MTSUN_SI;
  eta  = 3 / ( 128 * psi00 * pow( LAL_PI * m, 5.0/3.0 ) ) /LAL_MTSUN_SI;       
  mu   = eta * m;

  /* removed definition of chirp mass; not necessary in this function */
  /* chirpMass = pow( 1.0 / LAL_PI, 5.0/3.0) * ( 3 / (128 * psi00));  */

  /* template dependent normalisation */
  distNorm = 2.0 * LAL_MRSUN_SI / (cannonDist * 1.0e6 * LAL_PC_SI);
  distNorm *= params->dynRange;

  fcTmplt->tmpltNorm = sqrt( (5.0*mu) / 96.0 ) *
    pow( m / (LAL_PI*LAL_PI) , 1.0/3.0 ) *
    pow( LAL_MTSUN_SI / (REAL4) params->deltaT, -1.0/6.0 );
  
  fcTmplt->tmpltNorm *= fcTmplt->tmpltNorm;
  
  fcTmplt->tmpltNorm *= distNorm * distNorm;

  x1 = pow( deltaF, -1.0/3.0 );

  /* frequency cutoffs */
  fHi  = tmplt->fFinal;
  kmin = params->fLow / deltaF > 1 ? params->fLow / deltaF : 1;
  kmax = fHi / deltaF < numPoints/2 ? fHi / deltaF : numPoints/2;
  
  /* compute psi0: used in range reduction */
  {
    REAL4 x    = x1 * xfac[kmin];
    REAL4 psi  = 
      psi20 + (x * x) * ( psi15 + x * ( psi10 + x * ( psi05 + x * psi00 )));
    psi0 = -2 * LAL_PI * ( floor ( 0.5 * psi / LAL_PI ) );
  }


  /*
   *
   * calculate the stationary phase chirp
   *
   */


  for ( k = kmin; k < kmax ; ++k )
    {
      REAL4 x    = x1 * xfac[k];
      REAL4 psi  = 
        psi20 + (x * x) * ( psi15 + x * ( psi10 + x * ( psi05 + x * psi00 )));
      REAL4 psi1 = psi + psi0;
      REAL4 psi2;  

      /* range reduction of psi1 */
      while ( psi1 < -LAL_PI )
      {
         psi1 += 2 * LAL_PI;
         psi0 += 2 * LAL_PI;
      }
      while ( psi1 > LAL_PI )
      {
         psi1 -= 2 * LAL_PI;
         psi0 -= 2 * LAL_PI;
      }

      /* compute sine and cosine of psi1 */
      expPsi[k].im =   -sin(psi1);
      expPsi[k].re =   cos(psi1);
      /* very expensive computation method */
    }

  /* copy the template parameters to the finchirp template structure */
  memcpy( &(fcTmplt->tmplt), tmplt, sizeof(InspiralTemplate) );

  /* normal exit */
  DETATCHSTATUSPTR( status );
  RETURN( status );
}
