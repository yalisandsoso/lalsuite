/********************************* <lalVerbatim file="BCVTemplatesCV">
Author: B.S. Sathyaprakash
$Id$
**************************************************** </lalVerbatim> */

/********************************************************** <lalLaTeX>

\subsection{Program \texttt{BCVTemplates.c}}
\label{ss:BCVTemplates.c}

Creates a template mesh for BCV (or, alternatively, for SPA but
assuing a constant metric) using the mismatch metric.

\subsubsection*{Usage}

\subsubsection*{Description}

\subsubsection*{Exit codes}
****************************************** </lalLaTeX><lalErrTable> */
/******************************************** </lalErrTable><lalLaTeX>

\subsubsection*{Algorithm}

\subsubsection*{Uses}
\begin{verbatim}
lalDebugLevel
\end{verbatim}

\subsubsection*{Notes}

\vfill{\footnotesize\input{BCVTemplatesCV}}

******************************************************* </lalLaTeX> */

#include <math.h>
#include <stdlib.h>
#include <lal/LALInspiralBank.h>
#include <lal/LALStdio.h>
#include <lal/FileIO.h>
#include <lal/LALStdlib.h>
#include <lal/AVFactories.h>

NRCSID(FLATMESHTESTC,"$Id$");

/* Default parameter settings. */
int lalDebugLevel = 0;

static void
GetInspiralMoments (
		LALStatus            *status,
		InspiralMomentsEtc   *moments,
		REAL8FrequencySeries *psd,
		InspiralTemplate     *params );

int
main(int argc, char **argv)
{
  INT4 arg;
  static LALStatus status;     /* top-level status structure */
  REAL4 mismatch = 0.05; /* maximum mismatch level */
  REAL8 minimalmatch = 1.-mismatch; /* minimatch */
  REAL4Vector *mesh = NULL;      /* mesh of parameter values */

  static InspiralMetric metric;
  static InspiralTemplate params;
  UINT4   nlist, numPSDpts=262144;
  REAL8FrequencySeries shf;
  REAL8 samplingRate;
  void *noisemodel = LALLIGOIPsd;
  InspiralMomentsEtc moments;
  InspiralBankParams   bankParams; 
  InspiralCoarseBankIn coarseIn;
  InspiralTemplateList *list=NULL;
  REAL4 x0, x1, x0Min, x0Max, x1Min, x1Max;

/* Number of templates is nlist */

  nlist = 0;

  params.OmegaS = 0.;
  params.Theta = 0.;
  params.ieta=1; 
  params.mass1=1.; 
  params.mass2=1.; 
  params.startTime=0.0; 
  params.startPhase=0.0;
  params.fLower=40.0; 
  params.fCutoff=2000.00;
  params.tSampling=4096.0;
  params.order=4;
  params.approximant=TaylorT3;
  params.signalAmplitude=1.0;
  params.nStartPad=0;
  params.nEndPad=1000;
  params.massChoice=m1Andm2;
  params.distance = 1.e8 * LAL_PC_SI/LAL_C_SI;
  LALInspiralParameterCalc(&status, &params);
    
  coarseIn.fLower = params.fLower;
  coarseIn.fUpper = params.fCutoff;
  coarseIn.tSampling = params.tSampling;
  coarseIn.order = params.order;
  coarseIn.space = Tau0Tau3;
  coarseIn.approximant = params.approximant;
  coarseIn.mmCoarse = 0.90;
  coarseIn.mmFine = 0.97;
  coarseIn.iflso = 0.0L;
  coarseIn.mMin = 1.0;
  coarseIn.mMax = 20.0;
  coarseIn.MMax = coarseIn.mMax * 2.;
  coarseIn.massRange = MinMaxComponentMass; 
  /* coarseIn.massRange = MinComponentMassMaxTotalMass;*/
  /* minimum value of eta */
  coarseIn.etamin = coarseIn.mMin * ( coarseIn.MMax - coarseIn.mMin) / pow(coarseIn.MMax,2.);

  params.psi0 = 132250.;
  params.psi3 = -1314.2;
  /*
  params.alpha = 0.528;
  */
  params.alpha = 0.L;
  params.fendBCV = 868.7;
  metric.space = Tau0Tau3;

  samplingRate = params.tSampling;
  memset( &(shf), 0, sizeof(REAL8FrequencySeries) );
  shf.f0 = 0;
  LALDCreateVector( &status, &(shf.data), numPSDpts );
  shf.deltaF = samplingRate / (2.*(REAL8) shf.data->length + 1.L);
  LALNoiseSpectralDensity (&status, shf.data, noisemodel, shf.deltaF );

  /* compute the metric at this point, update bankPars and add the params to the list */
	  
  /*
  GetInspiralMoments (&status, &moments, &shf, &params);
  LALInspiralComputeMetric(&status, &metric, &params, &moments);
  */
  LALInspiralComputeMetricBCV(&status, &metric, &shf, &params);

  fprintf(stderr, "%e %e %e\n", metric.G00, metric.G01, metric.G11);
  fprintf(stderr, "%e %e %e\n", metric.g00, metric.g11, metric.theta);
  fprintf(stderr, "dp0=%e dp1=%e\n", sqrt (mismatch/metric.G00), sqrt (mismatch/metric.G11));
  fprintf(stderr, "dP0=%e dP1=%e\n", sqrt (mismatch/metric.g00), sqrt (mismatch/metric.g11));

  minimalmatch = 1. - mismatch;
  LALInspiralUpdateParams(&status, &bankParams, metric, minimalmatch);

  /*
  x0Min = 1.00;
  x1Min = 0.10;
  x0Max = 3.00; 
  x1Max = 0.30;
  */

  x0Min = 1.e5;
  x1Min = -1.e3;
  x0Max = 2.0e5;
  x1Max = 1.e3;

  for (x0 = x0Min; x0 <= x0Max; x0 += bankParams.dx0)
  {
	  for (x1 = x1Min; x1 <= x1Max; x1 += bankParams.dx1)
	  {
		  if (!(list = (InspiralTemplateList*) LALRealloc(list, sizeof(InspiralTemplateList)*(nlist+1)))) 
		  {
			  exit(1);
		  }
	  
  
		  list[nlist].ID = nlist; 
		  list[nlist].params.t0 = x0;
		  list[nlist].params.t3 = x1;
		  list[nlist].metric = metric; 
		  ++(nlist); 
        
		  fprintf(stdout, "%10.3e %10.3e\n", x0, x1);
	  }
  }

		  
  fprintf(stdout, "&\n");

  /* Prepare to print result. */
  {
    UINT4 i;
    UINT4 valid,k=0;

    params.massChoice=t03;
    /* Print out the template parameters */
    i = nlist;
    while ( i--) 
    {
	/*
	Retain only those templates that have meaningful masses:
	*/
	bankParams.x0 = (REAL8) list[k].params.t0;
	bankParams.x1 = (REAL8) list[k].params.t3;
	++k;
	LALInspiralValidParams(&status, &valid, bankParams, coarseIn);
        if (valid) fprintf(stdout, "%10.3e %10.3e\n", bankParams.x0, bankParams.x1);
    }
  }

  {

  int j;
  static RectangleIn RectIn;
  static RectangleOut RectOut;
     
  RectIn.dx = sqrt( (1. - coarseIn.mmCoarse)/metric.g00 );
  RectIn.dy = sqrt( (1. - coarseIn.mmCoarse)/metric.g11 );
  RectIn.theta = metric.theta;

  for (j=0; j<nlist; j++) 
  {
     RectIn.x0 = list[j].params.t0;
     RectIn.y0 = list[j].params.t3;
     LALRectangleVertices(&status, &RectOut, &RectIn);
     printf("%e %e\n%e %e\n%e %e\n%e %e\n%e %e\n", 
        RectOut.x1, RectOut.y1, 
        RectOut.x2, RectOut.y2, 
        RectOut.x3, RectOut.y3, 
        RectOut.x4, RectOut.y4, 
        RectOut.x5, RectOut.y5);
     printf("&\n");
     /*
     */
  }
  /* Free the mesh, and exit. */
  }
  LALFree(list);
  LALCheckMemoryLeaks();
}


static void
GetInspiralMoments (
		LALStatus            *status,
		InspiralMomentsEtc   *moments,
		REAL8FrequencySeries *psd,
		InspiralTemplate     *params )
{

   UINT4 k;
   InspiralMomentsIn in;

   INITSTATUS (status, "GetInspiralMoments", FLATMESHTESTC);
   ATTATCHSTATUSPTR(status);
  
   ASSERT (params, status, LALINSPIRALBANKH_ENULL, LALINSPIRALBANKH_MSGENULL);
   ASSERT (params->fLower>0, status, LALINSPIRALBANKH_ENULL, LALINSPIRALBANKH_MSGENULL);
   ASSERT (moments, status, LALINSPIRALBANKH_ENULL, LALINSPIRALBANKH_MSGENULL);
   ASSERT (psd, status, LALINSPIRALBANKH_ENULL, LALINSPIRALBANKH_MSGENULL);

   moments->a01 = 3.L/5.L;
   moments->a21 = 11.L * LAL_PI/12.L;
   moments->a22 = 743.L/2016.L * pow(25.L/(2.L*LAL_PI*LAL_PI), 1.L/3.L);
   moments->a31 = -3.L/2.L;
   moments->a41 = 617.L * LAL_PI * LAL_PI / 384.L;
   moments->a42 = 5429.L/5376.L * pow ( 25.L * LAL_PI/2.L, 1.L/3.L);
   moments->a43 = 1.5293365L/1.0838016L * pow(5.L/(4.L*pow(LAL_PI,4.L)), 1.L/3.L);
   
   /* setup the input structure needed in the computation of the moments */

   in.shf = psd;
   in.shf->f0 /= params->fLower;
   in.shf->deltaF /= params->fLower;
   in.xmin = params->fLower/params->fLower;
   in.xmax = params->fCutoff/params->fLower;
	   
   /* First compute the norm */

   in.norm = 1.L;
   in.ndx = 7.L/3.L; 
   LALInspiralMoments(status->statusPtr, &moments->j[7], in); 
   CHECKSTATUSPTR(status);
   in.norm = moments->j[7];

   if (lalDebugLevel & LALINFO)
   {
	   fprintf (stderr, "a01=%e a21=%e a22=%e a31=%e a41=%e a42=%e a43=%e \n", 
			   moments->a01, moments->a21, moments->a22, moments->a31, 
			   moments->a41, moments->a42, moments->a43);
   
	   fprintf(stderr, "j7=%e\n", moments->j[7]);
   }

   /* Normalised moments of the noise PSD from 1/3 to 17/3. */

   for (k=1; k<=17; k++)
   {
	   in.ndx = (REAL8) k /3.L; 
	   LALInspiralMoments(status->statusPtr,&moments->j[k],in);  
	   CHECKSTATUSPTR(status);
	   if (lalDebugLevel==1) fprintf(stderr, "j%1i=%e\n", k,moments->j[k]);
   }
   in.shf->deltaF *= params->fLower;
   in.shf->f0 *= params->fLower;
  
   DETATCHSTATUSPTR(status);
   RETURN (status);
}
