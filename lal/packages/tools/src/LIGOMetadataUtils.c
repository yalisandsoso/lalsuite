/*
*  Copyright (C) 2007 Drew Keppel, Duncan Brown, Jolien Creighton, Patrick Brady, Stephen Fairhurst, Thomas Cokelaer
*
*  This program is free software; you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation; either version 2 of the License, or
*  (at your option) any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with with program; see the file COPYING. If not, write to the
*  Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,
*  MA  02111-1307  USA
*/

/*----------------------------------------------------------------------- 
 * 
 * File Name: LIGOMetadataUtils.c
 *
 * Author: Brown, D. A.
 * 
 * Revision: $Id$
 * 
 *-----------------------------------------------------------------------
 */

#if 0
<lalVerbatim file="LIGOMetadataUtilsCV">
Author: Brown, D. A.
$Id$
</lalVerbatim> 
#endif

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <lal/LALStdlib.h>
#include <lal/LALStdio.h>
#include <lal/LIGOMetadataTables.h>
#include <lal/LIGOMetadataUtils.h>
#include <lal/Date.h>

NRCSID( LIGOMETADATAUTILSC, "$Id$" );

#if 0
<lalLaTeX>
\subsection{Module \texttt{LIGOMetadataUtils.c}}

\noindent General routines for manipulating LIGO metadatabase tables.

\subsubsection*{Prototypes}
\vspace{0.1in}
\input{LIGOMetadataUtilsCP}
\idx{LALPlaygroundInSearchSummary()}
\idx{LALCompareSearchSummaryByInTime()}
\idx{LALCompareSearchSummaryByOutTime ()}
\idx{LALTimeSortSearchSummary()}
\idx{LALIfoScanSearchSummary()}
\idx{LALIfoScanSummValue()}
\idx{LALCompareSummValueByTime()}
\idx{LALTimeSortSummValue()}
\idx{LALCheckOutTimeFromSearchSummary()}
\idx{LALDistanceScansummValueTable()}
  
\subsubsection*{Description}

The function \texttt{LALPlaygroundInSearchSummary()} determines the
ammount of time in the search summary table \texttt{ssTable} that overlaps
with playground data. The time between \texttt{in\_start\_time} and
\texttt{in\_end\_time} that overlaps with playground is returned in
\texttt{inPlayTime} and the time between \texttt{out\_start\_time} and
\texttt{out\_end\_time} that overlaps with playground is returned in
\texttt{outPlayTime}.

\texttt{LALCompareSearchSummaryByInTime()} is a function to compare the in
times in two search summary tables.  It returns 1 if the
\texttt{in\_start\_time} of the first table is after the
\texttt{in\_start\_time} of the second and -1 if it is before.  If the two
\texttt{in\_start\_time}s are identical, the test is repeated on the
\texttt{in\_end\_time}s.  If these are also equal, the comparison returns 0.
\texttt{LALCompareSearchSummaryByOutTime()} operates in a similar manner, but
uses the out, rather than in, times.

\texttt{LALTimeSortSearchSummary()} will time sort a linked list of search
summary tables.  You can sort on in our out start time depending which
\texttt{comparfunc} is specified.

\texttt{LALIfoScanSearchSummary()} steps through a linked list of search
summary tables and returns a pointer \texttt{output} to a linked list of those
tables whos \texttt{ifos} field matches the string \texttt{ifos}.


\texttt{LALIfoScanSummValue()}, \texttt{LALCompareSummValueByTime()} and
\texttt{LALTimeSortSummValue()} performs the same functions as described
above.  The only difference being that they act on summ value tables.

\texttt{LALCheckOutTimeFromSearchSummary()} verifies that all times
between the specified \texttt{startTime} and \texttt{endTime} have been
searched precisely once for the given \texttt{ifo}.

Finally, \texttt{LALDistanceScanSummValueTable()} scan a summ value table
 searching for a trigger belonging to a given ifo and englobing a give GPS
 time.

 
\subsubsection*{Algorithm}

\noindent None.

\subsubsection*{Uses}

\noindent LALGPStoINT8, LALCalloc, LALMalloc, LALFree.

\subsubsection*{Notes}
%% Any relevant notes.

\vfill{\footnotesize\input{LIGOMetadataUtilsCV}}

</lalLaTeX>
#endif

/* <lalVerbatim file="LIGOMetadataUtilsCP"> */
int XLALCountProcessTable(ProcessTable *head)
/* </lalVerbatim> */
{
	int length;

	/* count the number of events in the list */
	for(length = 0; head; head = head->next)
		length++;

	return(length);
}

/* <lalVerbatim file="LIGOMetadataUtilsCP"> */
int XLALCountProcessParamsTable(ProcessParamsTable *head)
/* </lalVerbatim> */
{
	int length;

	/* count the number of events in the list */
	for(length = 0; head; head = head->next)
		length++;

	return(length);
}

/* <lalVerbatim file="LIGOMetadataUtilsCP"> */
int XLALCountMultiInspiralTable(MultiInspiralTable *head)
/* </lalVerbatim> */
{
	int length;
	/* count the number of events in the list */
	for(length = 0; head; head = head->next)
		length++;

	return(length);
}


/* <lalVerbatim file="LIGOMetadataUtilsCP"> */
int 
XLALIFONumber( 
    const char *ifo 
    )
/* </lalVerbatim> */
{
  switch( ifo[0] )
  {
    case 'G':
      return LAL_IFO_G1;
      break;

    case 'H':
      if ( !strcmp( ifo, "H1" ) )
      {
        return LAL_IFO_H1;
      }
      else if (!strcmp( ifo, "H2" ) )
      {
        return LAL_IFO_H2;
      }
      else
      {
        /* Invalid Hanford Detector */
        return LAL_UNKNOWN_IFO ;
      } 
      break;

    case 'L':
      return LAL_IFO_L1;
      break;

    case 'T':
      return LAL_IFO_T1;
      break;

    case 'V':
      return LAL_IFO_V1;
      break;

    default:
      /* Invalid Detector Site */
      return LAL_UNKNOWN_IFO ;
  }
}

/* <lalVerbatim file="LIGOMetadataUtilsCP"> */
void 
XLALReturnIFO( 
    char                *ifo,
    InterferometerNumber IFONumber 
    )
/* </lalVerbatim> */
{
  switch( IFONumber )
  {
    case LAL_IFO_G1:
      LALSnprintf( ifo, LIGOMETA_IFO_MAX, "G1");
      break;

    case LAL_IFO_H1:
      LALSnprintf( ifo, LIGOMETA_IFO_MAX, "H1");
      break;

    case LAL_IFO_H2:
      LALSnprintf( ifo, LIGOMETA_IFO_MAX, "H2");
      break;

    case LAL_IFO_L1:
      LALSnprintf( ifo, LIGOMETA_IFO_MAX, "L1");
      break;

    case LAL_IFO_T1:
      LALSnprintf( ifo, LIGOMETA_IFO_MAX, "T1");
      break;

    case LAL_IFO_V1:
      LALSnprintf( ifo, LIGOMETA_IFO_MAX, "V1");
      break;

    default:
      /* Invalid Detector Site */
      LALSnprintf( ifo, LIGOMETA_IFO_MAX, "");
  }
}


/* <lalVerbatim file="LIGOMetadataUtilsCP"> */
void
XLALReturnDetector(
    LALDetector           *det,
    InterferometerNumber   IFONumber 
    )
/* </lalVerbatim> */
{
  switch( IFONumber )
  {
    case LAL_IFO_G1:
      *det = lalCachedDetectors[LALDetectorIndexGEO600DIFF];
      break;

    case LAL_IFO_H1:
      *det = lalCachedDetectors[LALDetectorIndexLHODIFF];
      break;

    case LAL_IFO_H2:
      *det = lalCachedDetectors[LALDetectorIndexLHODIFF];
      break;

    case LAL_IFO_L1:
      *det = lalCachedDetectors[LALDetectorIndexLLODIFF];
      break;

    case LAL_IFO_T1:
      *det = lalCachedDetectors[LALDetectorIndexTAMA300DIFF];
      break;

    case LAL_IFO_V1:
      *det = lalCachedDetectors[LALDetectorIndexVIRGODIFF];
      break;

    default:
      /* Invalid Detector Site */
      memset(det, 0, sizeof(LALDetector) );
  }
}



static INT8 PlaygroundOverlap( INT8 seg_end, INT8 seg_length )
{
  const INT8 play_length = LAL_INT8_C(600000000000);
  const INT8 S2_start = LAL_INT8_C(729273613000000000);
  const INT8 mod_play = LAL_INT8_C(6370000000000);
  INT8 end_mod_play;

  /* if the end precedes the start of S2, then there is no playground, since
   * we only define playground from S2 onwards */
  if ( seg_end < S2_start )
  {
    return LAL_INT8_C(0);
  }

  /* handle a segment that contains two or more playground */
  /* segments by recursively bisecting it                  */
  if ( seg_length >= (mod_play - play_length) )
  {
    INT8 low_len, high_len, low_play, high_play;
    low_len = high_len = seg_length / LAL_INT8_C(2);
    if ( seg_length % LAL_INT8_C(2) ) ++low_len;

    low_play = PlaygroundOverlap( seg_end - high_len, low_len );
    high_play = PlaygroundOverlap( seg_end, high_len );

    return low_play + high_play;
  }

  end_mod_play = ((seg_end - S2_start) % mod_play );

  /* if no overlap with playground, return zero */
  if ( end_mod_play >= play_length + seg_length )
  {
    return LAL_INT8_C(0);
  }
  else
  {
    if ( seg_length > play_length )
    {
      if ( (seg_length < end_mod_play) && 
          (end_mod_play < play_length + seg_length ) )
      {
        return play_length + seg_length - end_mod_play;
      }
      else if ( (play_length <= end_mod_play) && 
          (end_mod_play <= seg_length ) )
      {
        return play_length;
      }
      else if ( end_mod_play < play_length )
      {
        return end_mod_play;
      }
    }
    else if ( seg_length <= play_length )
    {
      if ( (play_length < end_mod_play) && 
          (end_mod_play < seg_length + play_length ) )
      {
        return play_length + seg_length - end_mod_play;
      }
      else if ( (seg_length <= end_mod_play) && 
          (end_mod_play <= play_length ) )
      {
        return seg_length;
      }
      else if ( end_mod_play < seg_length )
      {
        return end_mod_play;
      }
    }
    else
    {
      LALPrintError( "Error determining playground overlap\n" );
      return LAL_INT8_C(-1);
    }
  }
  /* JC: HOPEFULLY NEVER GET HERE */
  return LAL_INT8_C(-1);
}

/* <lalVerbatim file="LIGOMetadataUtilsCP"> */
void
LALPlaygroundInSearchSummary (
    LALStatus          *status,
    SearchSummaryTable *ssTable,
    LIGOTimeGPS        *inPlayTime,
    LIGOTimeGPS        *outPlayTime
    )
/* </lalVerbatim> */
{
  INT4 playCheck = 0;
  INITSTATUS( status, "LALPlaygroundInSearchSummary", LIGOMETADATAUTILSC );
  ATTATCHSTATUSPTR( status );

  playCheck = XLALPlaygroundInSearchSummary ( ssTable, inPlayTime, 
      outPlayTime );

  if ( playCheck < 0 )
  {
    ABORT( status, LIGOMETADATAUTILSH_ETIME, LIGOMETADATAUTILSH_MSGETIME );
  }

  DETATCHSTATUSPTR( status );
  RETURN( status );
}

/* <lalVerbatim file="LIGOMetadataUtilsCP"> */
int
XLALPlaygroundInSearchSummary (
    SearchSummaryTable *ssTable,
    LIGOTimeGPS        *inPlayTime,
    LIGOTimeGPS        *outPlayTime
    )
/* </lalVerbatim> */
{
  static const char *func = "PlaygroundInSearchSummary";
  INT8 startNS, endNS, lengthNS, playNS;

  startNS = XLALGPStoINT8( &(ssTable->in_start_time) );
  endNS = XLALGPStoINT8( &(ssTable->in_end_time) );
  lengthNS = endNS - startNS;
  playNS = PlaygroundOverlap( endNS, lengthNS );
  if ( playNS < 0 )
  {
    XLAL_ERROR(func,XLAL_EIO);
  }
  inPlayTime = XLALINT8toGPS( inPlayTime, playNS );

  startNS = XLALGPStoINT8( &(ssTable->out_start_time) );
  endNS = XLALGPStoINT8( &(ssTable->out_end_time) );
  lengthNS = endNS - startNS;

  playNS = PlaygroundOverlap( endNS, lengthNS );
  if ( playNS < 0 )
  {
    XLAL_ERROR(func,XLAL_EIO);
  }
  outPlayTime = XLALINT8toGPS( outPlayTime, playNS );

  return( 0 );
}


/* <lalVerbatim file="LIGOMetadataUtilsCP"> */
int
LALCompareSearchSummaryByInTime (
    const void *a,
    const void *b
    )
/* </lalVerbatim> */
{
  LALStatus     status;

  const SearchSummaryTable *aPtr = *((const SearchSummaryTable * const *)a);
  const SearchSummaryTable *bPtr = *((const SearchSummaryTable * const *)b);

  INT8 ta = 0;
  INT8 tb = 0;

  /* determine the in start times */
  memset( &status, 0, sizeof(LALStatus) );
  LALGPStoINT8( &status, &ta, &(aPtr->in_start_time) );
  LALGPStoINT8( &status, &tb, &(bPtr->in_start_time) );

  if ( ta > tb )
  {
    return 1;
  }
  else if ( ta < tb )
  {
    return -1;
  }
  else
  {
    /* determine the in end times */ 
    memset( &status, 0, sizeof(LALStatus) );
    LALGPStoINT8( &status, &ta, &( aPtr->in_end_time) );
    LALGPStoINT8( &status, &tb, &( bPtr->in_end_time) );

    if ( ta > tb )
    {
      return 1;
    }
    else if ( ta < tb )
    {
      return -1;
    }
    else
    {
      return 0;
    }
  }
}


/* <lalVerbatim file="LIGOMetadataUtilsCP"> */
int
LALCompareSearchSummaryByOutTime (
    const void *a,
    const void *b
    )
/* </lalVerbatim> */
{
  LALStatus     status;

  const SearchSummaryTable *aPtr = *((const SearchSummaryTable * const *)a);
  const SearchSummaryTable *bPtr = *((const SearchSummaryTable * const *)b);

  INT8 ta = 0;
  INT8 tb = 0;

  /* determine the out start times */
  memset( &status, 0, sizeof(LALStatus) );
  LALGPStoINT8( &status, &ta, &(aPtr->out_start_time) );
  LALGPStoINT8( &status, &tb, &(bPtr->out_start_time) );

  if ( ta > tb )
  {
    return 1;
  }
  else if ( ta < tb )
  {
    return -1;
  }
  else
  {
    /* determine the out end times */
    memset( &status, 0, sizeof(LALStatus) );
    LALGPStoINT8( &status, &ta, &(aPtr->out_end_time) );
    LALGPStoINT8( &status, &tb, &(bPtr->out_end_time) );

    if ( ta > tb )
    {
      return 1;
    }
    else if ( ta < tb )
    {
      return -1;
    }
    else
    {
      return 0;
    }
  }
}

/* <lalVerbatim file="LIGOMetadataUtilsCP"> */
int
XLALTimeSortSearchSummary(
    SearchSummaryTable  **summHead,
    int(*comparfunc)    (const void *, const void *)
    )
/* </lalVerbatim> */
{
  static const char *func = "TimeSortSearchSummary";
  INT4                  i;
  INT4                  numSumms = 0;
  SearchSummaryTable    *thisSearchSumm = NULL;
  SearchSummaryTable   **summHandle = NULL;

  if ( !summHead )
  {
    XLAL_ERROR(func,XLAL_EIO);
  }


  /* count the number of summs in the linked list */
  for ( thisSearchSumm = *summHead; thisSearchSumm; 
      thisSearchSumm = thisSearchSumm->next )
  {
    ++numSumms;
  }
  if ( ! numSumms )
  {
    return( 0 );
  }

  /* allocate memory for an array of ptrs to sort and populate array */
  summHandle = (SearchSummaryTable **) 
    LALCalloc( numSumms, sizeof(SearchSummaryTable *) );
  for ( i = 0, thisSearchSumm = *summHead; i < numSumms; 
      ++i, thisSearchSumm = thisSearchSumm->next )
  {
    summHandle[i] = thisSearchSumm;
  }

  /* qsort the array using the specified function */
  qsort( summHandle, numSumms, sizeof(summHandle[0]), comparfunc );

  /* re-link the linked list in the right order */
  thisSearchSumm = *summHead = summHandle[0];
  for ( i = 1; i < numSumms; ++i, thisSearchSumm = thisSearchSumm->next )
  {
    thisSearchSumm->next = summHandle[i];
  }
  thisSearchSumm->next = NULL;

  /* free the internal memory */
  LALFree( summHandle );

  return( 0 );
}



/* <lalVerbatim file="LIGOMetadataUtilsCP"> */
void
LALTimeSortSearchSummary (
    LALStatus            *status,
    SearchSummaryTable  **summHead,
    int(*comparfunc)    (const void *, const void *)
    )
/* </lalVerbatim> */
{
  INITSTATUS( status, "LALTimeSortSearchSummary", LIGOMETADATAUTILSC );
  ATTATCHSTATUSPTR( status );

  ASSERT( summHead, status, 
      LIGOMETADATAUTILSH_ENULL, LIGOMETADATAUTILSH_MSGENULL );

  XLALTimeSortSearchSummary( summHead, comparfunc );

  DETATCHSTATUSPTR (status);
  RETURN( status );
}


/* <lalVerbatim file="LIGOMetadataUtilsCP"> */
SearchSummaryTable *
XLALIfoScanSearchSummary(
    SearchSummaryTable         *input,
    CHAR                       *ifos
    )
/* </lalVerbatim> */
{
  static const char *func = "IfoScanSearchSummary";
  SearchSummaryTable    *output = NULL;
  SearchSummaryTable    *thisSearchSumm = NULL;
  SearchSummaryTable    *keptSumm = NULL;


  if ( !input )
  {
    XLAL_ERROR_NULL(func,XLAL_EIO);
  }
    
  /* Scan through a linked list of search_summary tables and return a
     pointer to the head of a linked list of tables for a specific IFO */

  for( thisSearchSumm = input; thisSearchSumm; 
      thisSearchSumm = thisSearchSumm->next )
  {

    if ( !strcmp(thisSearchSumm->ifos, ifos) ) 
    {
      /* IFOs match so write this entry to the output table */
      if ( ! output  )
      {
        output = keptSumm = (SearchSummaryTable *) 
          LALMalloc( sizeof(SearchSummaryTable) );
      }
      else
      {
        keptSumm = keptSumm->next = (SearchSummaryTable *) 
          LALMalloc( sizeof(SearchSummaryTable) );
      }
      if ( !keptSumm )
      {  
        while ( output )
        {
          thisSearchSumm = output;
          output = (output)->next;
          LALFree( thisSearchSumm );
        }
        XLAL_ERROR_NULL(func,XLAL_ENOMEM);
      }
      memcpy(keptSumm, thisSearchSumm, sizeof(SearchSummaryTable));
      keptSumm->next = NULL;
    }
  }
  return( output);
}  



/* <lalVerbatim file="LIGOMetadataUtilsCP"> */
void
LALIfoScanSearchSummary(
    LALStatus                  *status,
    SearchSummaryTable        **output,
    SearchSummaryTable         *input,
    CHAR                       *ifos
    )
/* </lalVerbatim> */
{
  INITSTATUS( status, "LALIfoScanSearchSummary", LIGOMETADATAUTILSC );
  ATTATCHSTATUSPTR( status );

  *output = XLALIfoScanSearchSummary( input, ifos );

  DETATCHSTATUSPTR (status);
  RETURN (status);

}  

/* <lalVerbatim file="LIGOMetadataUtilsCP"> */
void
LALDistanceScanSummValueTable (
    LALStatus            *status,
    SummValueTable       *summValueList,
    LIGOTimeGPS          gps,
    CHAR                 *ifo,
    REAL4                *distance)
/* </lalVerbatim> */
{
  SummValueTable    *thisSummValue = NULL;
  INT4 test=0; 
  INT8 ta=0, tb=0, tc=0;

  INITSTATUS( status, "LALDistanceScanSummValueTable", LIGOMETADATAUTILSC );
  ATTATCHSTATUSPTR( status );

  /* initialize diatnce to zero */
  *distance = 0;

  /* convert the input GPS time into INT8 */
  LALGPStoINT8( status->statusPtr, &ta, &(gps) );

  /* scan the summ value table */
  for( thisSummValue = summValueList; thisSummValue; 
      thisSummValue = thisSummValue->next )
  {
    /* if this is the requested ifo */
    if ( !strcmp(thisSummValue->ifo, ifo) ) 
    {
      /* IFOs match so now let us check if this entry coincides 
	with the requested GPS time */

      LALGPStoINT8( status->statusPtr, &tb, &(thisSummValue->start_time) );
      LALGPStoINT8( status->statusPtr, &tc, &(thisSummValue->end_time) );
      if ( ta >= tb && ta<=tc )
      {
        *distance = thisSummValue->value; 
        break;
      } 
    }
  }

  if ( *distance == 0 )
    {
      ABORT ( status, LIGOMETADATAUTILSH_EDIST, LIGOMETADATAUTILSH_MSGEDIST );
    }
  
  DETATCHSTATUSPTR (status);
  RETURN (status);
}

/* <lalVerbatim file="LIGOMetadataUtilsCP"> */
void
LALCheckOutTimeFromSearchSummary (
    LALStatus            *status,
    SearchSummaryTable   *summList,
    CHAR                 *ifo,
    LIGOTimeGPS          *startTime,
    LIGOTimeGPS          *endTime
    )
/* </lalVerbatim> */
{
  SearchSummaryTable   *thisIFOSummList = NULL;
  SearchSummaryTable   *thisSearchSumm = NULL;
  INT8  startTimeNS = 0;
  INT8  endTimeNS = 0;
  INT8  unsearchedStartNS = 0;
  INT8  outStartNS = 0;
  INT8  outEndNS = 0;


  INITSTATUS( status, "LALCheckOutTimeSearchSummary", LIGOMETADATAUTILSC );
  ATTATCHSTATUSPTR( status );

  /* check that the data has been searched once 
     and only once for the given IFO */

  /* first, create a list of search summary tables applicable to this IFO */ 
  LALIfoScanSearchSummary( status->statusPtr,  &thisIFOSummList, summList,
      ifo );
  CHECKSTATUSPTR( status );

  /* now, time sort the output list */
  LALTimeSortSearchSummary ( status->statusPtr,  &thisIFOSummList, 
      *LALCompareSearchSummaryByOutTime );
  CHECKSTATUSPTR( status );

  /* calculate requested start and end time in NS */
  LALGPStoINT8( status->statusPtr, &startTimeNS, startTime );
  CHECKSTATUSPTR( status );
  LALGPStoINT8( status->statusPtr, &endTimeNS, endTime );
  CHECKSTATUSPTR( status );

  unsearchedStartNS = startTimeNS;

  /* check that all times are searched */
  for ( thisSearchSumm = thisIFOSummList; thisSearchSumm; 
      thisSearchSumm = thisSearchSumm->next )
  {
    LALGPStoINT8( status->statusPtr, &outStartNS, 
        &(thisSearchSumm->out_start_time) );
    CHECKSTATUSPTR( status );

    if ( outStartNS < startTimeNS )    
    {
      /* file starts before requested start time */
      LALGPStoINT8( status->statusPtr, &outEndNS, 
          &(thisSearchSumm->out_end_time) );
      CHECKSTATUSPTR( status );

      if ( outEndNS > startTimeNS )
      {
        /* file is partially in requested times, update unsearchedStart */
        unsearchedStartNS = outEndNS;
      }
    }
    else if ( outStartNS == unsearchedStartNS )
    {
      /* this file starts at the beginning of the unsearched data */
      /* calculate the end time and set unsearched start to this */
      LALGPStoINT8( status->statusPtr, &outEndNS, 
          &(thisSearchSumm->out_end_time) );
      CHECKSTATUSPTR( status );

      unsearchedStartNS = outEndNS;
    }
    else if ( outStartNS > unsearchedStartNS )
    {
      /* there is a gap in the searched data between unsearchedStart
         and outStart */
      ABORT( status, LIGOMETADATAUTILSH_ESGAP, LIGOMETADATAUTILSH_MSGESGAP );
    }
    else if ( outStartNS < unsearchedStartNS )    
    {
      /* there is a region of data which was searched twice */
      ABORT( status, LIGOMETADATAUTILSH_ESDUB, LIGOMETADATAUTILSH_MSGESDUB );
    }
  }

  /* check that we got to the end of the requested time */
  if ( unsearchedStartNS < endTimeNS )
  {
    ABORT( status, LIGOMETADATAUTILSH_ESGAP, LIGOMETADATAUTILSH_MSGESGAP );
  }

  /* free memory allocated in LALIfoScanSearchSummary */
  while ( thisIFOSummList )
  {
    thisSearchSumm = thisIFOSummList;
    thisIFOSummList = thisIFOSummList->next;
    LALFree( thisSearchSumm );
  }

  DETATCHSTATUSPTR (status);
  RETURN (status);

}



/* <lalVerbatim file="LIGOMetadataUtilsCP"> */
void
LALIfoScanSummValue(
    LALStatus                  *status,
    SummValueTable            **output,
    SummValueTable             *input,
    CHAR                       *ifo
    )
/* </lalVerbatim> */
{
  SummValueTable    *thisSummValue = NULL;
  SummValueTable    *keptSumm = NULL;

  INITSTATUS( status, "LALIfoScanSummValue", LIGOMETADATAUTILSC );
  ATTATCHSTATUSPTR( status );

  /* check that output is null and input non-null */
  ASSERT( !(*output), status, 
      LIGOMETADATAUTILSH_ENNUL, LIGOMETADATAUTILSH_MSGENNUL );
  ASSERT( input, status, 
      LIGOMETADATAUTILSH_ENULL, LIGOMETADATAUTILSH_MSGENULL );

  /* Scan through a linked list of search_summary tables and return a
     pointer to the head of a linked list of tables for a specific IFO */

  for( thisSummValue = input; thisSummValue; 
      thisSummValue = thisSummValue->next )
  {

    if ( !strcmp(thisSummValue->ifo, ifo) ) 
    {
      /* IFOs match so write this entry to the output table */
      if ( ! *output  )
      {
        *output = keptSumm = (SummValueTable *) 
          LALMalloc( sizeof(SummValueTable) );
      }
      else
      {
        keptSumm = keptSumm->next = (SummValueTable *) 
          LALMalloc( sizeof(SummValueTable) );
      }
      memcpy(keptSumm, thisSummValue, sizeof(SummValueTable));
      keptSumm->next = NULL;
    }
  }

  DETATCHSTATUSPTR (status);
  RETURN (status);

}  



/* <lalVerbatim file="LIGOMetadataUtilsCP"> */
int
LALCompareSummValueByTime (
    const void *a,
    const void *b
    )
/* </lalVerbatim> */
{
  LALStatus     status;

  const SummValueTable *aPtr = *((const SummValueTable * const *)a);
  const SummValueTable *bPtr = *((const SummValueTable * const *)b);

  INT8 ta = 0;
  INT8 tb = 0;

  /* determine the out start times */
  LALGPStoINT8( &status, &ta, &(aPtr->start_time) );
  LALGPStoINT8( &status, &tb, &(bPtr->start_time) );

  if ( ta > tb )
  {
    return 1;
  }
  else if ( ta < tb )
  {
    return -1;
  }
  else
  {
    /* determine the out end times */ 
    LALGPStoINT8( &status, &ta, &(aPtr->end_time) );
    LALGPStoINT8( &status, &tb, &(bPtr->end_time) );

    if ( ta > tb )
    {
      return 1;
    }
    else if ( ta < tb )
    {
      return -1;
    }
    else
    {
      return 0;
    }
  }
}

/* <lalVerbatim file="LIGOMetadataUtilsCP"> */
int
XLALTimeSortSummValue(
    SummValueTable      **summHead,
    int(*comparfunc)    (const void *, const void *)
    )
/* </lalVerbatim> */
{
  static const char *func = "TimeSortSummValue";
  INT4                  i;
  INT4                  numSumms = 0;
  SummValueTable    *thisSummValue = NULL;
  SummValueTable   **summHandle = NULL;

  if ( !summHead )
  {
    XLAL_ERROR(func,XLAL_EIO);
  }

  /* count the number of summs in the linked list */
  for ( thisSummValue = *summHead; thisSummValue; 
      thisSummValue = thisSummValue->next )
  {
    ++numSumms;
  }
  if ( ! numSumms )
  {
    return(0);
  }

  /* allocate memory for an array of ptrs to sort and populate array */
  summHandle = (SummValueTable **) 
    LALCalloc( numSumms, sizeof(SummValueTable *) );
  if ( !summHandle )
  {
    XLAL_ERROR(func,XLAL_ENOMEM);
  }

  for ( i = 0, thisSummValue = *summHead; i < numSumms; 
      ++i, thisSummValue = thisSummValue->next )
  {
    summHandle[i] = thisSummValue;
  }

  /* qsort the array using the specified function */
  qsort( summHandle, numSumms, sizeof(summHandle[0]), comparfunc );

  /* re-link the linked list in the right order */
  thisSummValue = *summHead = summHandle[0];
  for ( i = 1; i < numSumms; ++i )
  {
    thisSummValue = thisSummValue->next = summHandle[i];
  }
  thisSummValue->next = NULL;

  /* free the internal memory */
  LALFree( summHandle );

  return(0);
}


/* <lalVerbatim file="LIGOMetadataUtilsCP"> */
void
LALTimeSortSummValue (
    LALStatus            *status,
    SummValueTable      **summHead,
    int(*comparfunc)    (const void *, const void *)
    )
/* </lalVerbatim> */
{
  INITSTATUS( status, "LALTimeSortSummValue", LIGOMETADATAUTILSC );
  ATTATCHSTATUSPTR( status );

  ASSERT( summHead, status, 
       LIGOMETADATAUTILSH_ENULL, LIGOMETADATAUTILSH_MSGENULL );
  
  XLALTimeSortSummValue( summHead, comparfunc );

  DETATCHSTATUSPTR (status);
  RETURN( status );
}



/**
 * Create a ProcessTable structure.
 */


ProcessTable *XLALCreateProcessTableRow(long id)
{
	static const char func[] = "XLALCreateProcessTableRow";
	ProcessTable *new = XLALMalloc(sizeof(*new));

	if(!new)
		XLAL_ERROR_NULL(func, XLAL_EFUNC);

	new->next = NULL;
	memset(new->program, 0, sizeof(new->program));
	memset(new->version, 0, sizeof(new->version));
	memset(new->cvs_repository, 0, sizeof(new->cvs_repository));
	XLALGPSSet(&new->cvs_entry_time, 0, 0);
	memset(new->comment, 0, sizeof(new->comment));
	new->is_online = 0;
	memset(new->node, 0, sizeof(new->node));
	memset(new->username, 0, sizeof(new->username));
	XLALGPSSet(&new->start_time, 0, 0);
	XLALGPSSet(&new->end_time, 0, 0);
	new->jobid = 0;
	memset(new->domain, 0, sizeof(new->domain));
	new->unix_procid = 0;
	memset(new->ifos, 0, sizeof(new->ifos));
	new->process_id = id;

	return new;
}


/**
 * Destroy a ProcessTable structure.
 */


void XLALDestroyProcessTableRow(ProcessTable *process)
{
	XLALFree(process);
}


/**
 * Destroy a ProcessTable linked list.
 */


void XLALDestroyProcessTable(ProcessTable *head)
{
	while(head) {
		ProcessTable *next = head->next;
		XLALDestroyProcessTableRow(head);
		head = next;
	}
}


/**
 * Create a ProcessParamsTable structure.
 */


ProcessParamsTable *XLALCreateProcessParamsTableRow(const ProcessTable *process)
{
	static const char func[] = "XLALCreateProcessParamsTableRow";
	ProcessParamsTable *new = XLALMalloc(sizeof(*new));

	if(!new)
		XLAL_ERROR_NULL(func, XLAL_EFUNC);

	new->next = NULL;
	memset(new->program, 0, sizeof(new->program));
	if(process)
		new->process_id = process->process_id;
	else
		new->process_id = -1;
	memset(new->param, 0, sizeof(new->param));
	memset(new->type, 0, sizeof(new->type));
	memset(new->value, 0, sizeof(new->value));

	return new;
}


/**
 * Destroy a ProcessParamsTable structure.
 */


void XLALDestroyProcessParamsTableRow(ProcessParamsTable *process_params)
{
	XLALFree(process_params);
}


/**
 * Destroy a ProcessParamsTable linked list.
 */


void XLALDestroyProcessParamsTable(ProcessParamsTable *head)
{
	while(head) {
		ProcessParamsTable *next = head->next;
		XLALDestroyProcessParamsTableRow(head);
		head = next;
	}
}
