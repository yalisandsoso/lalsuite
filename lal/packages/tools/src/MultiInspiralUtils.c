/*-----------------------------------------------------------------------
 *
 * File Name: MultiInspiralUtils.c
 *
 * Author: Bose, S.
 *
 * Revision: $Id$
 *
 *-----------------------------------------------------------------------
 */

#if 0
<lalVerbatim file="MultiInspiralUtilsCV">
Author: Bose, S.
$Id$
</lalVerbatim>
#endif

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <lal/LALStdlib.h>
#include <lal/LALStdio.h>
#include <lal/LIGOMetadataTables.h>
#include <lal/LIGOMetadataUtils.h>
#include <lal/Date.h>
#include <lal/SkyCoordinates.h>
#include <lal/GeneratePPNInspiral.h>
#include <lal/DetectorSite.h>
#include <lal/DetResponse.h>
#include <lal/TimeDelay.h>

NRCSID( SNGLINSPIRALUTILSC, "$Id$");

#if 0
<lalLaTeX>
\subsection{Module \texttt{MultiInspiralUtils.c}}

Provides a set of utilities for manipulating \texttt{multiInspiralTable}s.

\subsubsection*{Prototypes}
\vspace{0.1in}
\input{MultiInspiralUtilsCP}
\idx{XLALFreeMultiInspiral()}
\idx{XLALSortMultiInspiral()}
\idx{XLALClusterMultiInspiralTable()}
\idx{XLALTimeCutSingleInspiral()}
\idx{XLALSNRCutSingleInspiral()}
\idx{XLALRsqCutSingleInspiral()}
\idx{XLALPlayTestSingleInspiral()}
\idx{XLALMaxMultiInspiralOverIntervals(()}
\idx{XLALCountMultiInspiral()}

\subsubsection*{Description}

\subsubsection*{Algorithm}

\noindent None.

\subsubsection*{Uses}

\noindent LALCalloc, LALFree, LALGPStoINT8, XLALINT8NanoSecIsPlayground.

\subsubsection*{Notes}
%% Any relevant notes.

\vfill{\footnotesize\input{MultiInspiralUtilsCV}}

</lalLaTeX>
#endif

  /* a few useful static functions */
static INT8 geocent_end_time(const SimInspiralTable *x)
{
  return(XLALGPSToINT8NS(&x->geocent_end_time));
}

/*
 * A few quickies for convenience.
 */

static INT8 end_time(const MultiInspiralTable *x)
{
	return(XLALGPSToINT8NS(&x->end_time));
}

#if 0
/* functions currently unused */
static INT4 end_time_sec(const MultiInspiralTable *x)
{
	return(x->end_time.gpsSeconds);
}

static INT4 end_time_nsec(const MultiInspiralTable *x)
{
	return(x->end_time.gpsNanoSeconds);
}
#endif

/* <lalVerbatim file="SnglInspiralUtilsCP"> */
void
LALFreeMultiInspiral (
    LALStatus          *status,
    MultiInspiralTable **eventHead
    )
/* </lalVerbatim> */
{
  INITSTATUS( status, "LALFreeMultiInspiral", SNGLINSPIRALUTILSC );
  XLALFreeMultiInspiral( eventHead );
  RETURN( status );
}

/* <lalVerbatim file="MultiInspiralUtilsCP"> */
int
XLALFreeMultiInspiral (
    MultiInspiralTable **eventHead
    )
/* </lalVerbatim> */
{
  EventIDColumn        *eventId;

  while ( (eventId = (*eventHead)->event_id) )
    {
      /* free any associated event_id's */
      (*eventHead)->event_id = (*eventHead)->event_id->next;
      LALFree( eventId );
    }
  LALFree( *eventHead );

  return (0);
}

/* <lalVerbatim file="MultiInspiralUtilsCP"> */
MultiInspiralTable *
XLALSortMultiInspiral (
    MultiInspiralTable *eventHead,
    int(*comparfunc)   (const void *, const void *)
    )
/* </lalVerbatim> */
{
  INT4                  i;
  INT4                  numEvents = 0;
  MultiInspiralTable    *thisEvent = NULL;
  MultiInspiralTable   **eventHandle = NULL;

  /* count the number of events in the linked list */
  for ( thisEvent = eventHead; thisEvent; thisEvent = thisEvent->next )
  {
    ++numEvents;
  }
  if ( ! numEvents )
  {
    XLALPrintInfo(
      "XLALSortMultiInspiral: Empty MultiInspiralTable passed as input\n" );
    return( eventHead );
  }

  /* allocate memory for an array of pts to sort and populate array */
  eventHandle = (MultiInspiralTable **)
    LALCalloc( numEvents, sizeof(MultiInspiralTable *) );
  for ( i = 0, thisEvent = eventHead; i < numEvents;
      ++i, thisEvent = thisEvent->next )
  {
    eventHandle[i] = thisEvent;
  }

  /* qsort the array using the specified function */
  qsort( eventHandle, numEvents, sizeof(eventHandle[0]), comparfunc );

  /* re-link the linked list in the right order */
  thisEvent = eventHead = eventHandle[0];
  for ( i = 1; i < numEvents; ++i )
  {
    thisEvent = thisEvent->next = eventHandle[i];
  }
  thisEvent->next = NULL;

  /* free the internal memory */
  LALFree( eventHandle );

  return( eventHead );
}

/* <lalVerbatim file="MultiInspiralUtilsCP"> */
REAL4
XLALMultiInspiralStat(
    MultiInspiralTable         *multiInspiral,
    SnglInspiralClusterChoice  multiStat
    )
{
  REAL4 statValue = 0;

  if ( multiStat == snr )
  {
    statValue = multiInspiral->snr;
  }
  else if ( multiStat == snrsq_over_chisq )
  {
    statValue = multiInspiral->snr * multiInspiral->snr / multiInspiral->chisq;
  }
  else
  {
    statValue = 0;
  }
  return( statValue );
}


/* <lalVerbatim file="MultiInspiralUtilsCP"> */
int
XLALClusterMultiInspiralTable (
    MultiInspiralTable         **inspiralList,
    INT8                         dtimeNS,
    SnglInspiralClusterChoice   clusterchoice
    )
/* </lalVerbatim> */
{
  static const char *func = "XLALClusterMultiInspiralTable";
  MultiInspiralTable     *thisEvent=NULL;
  MultiInspiralTable     *prevEvent=NULL;
  MultiInspiralTable     *nextEvent=NULL;
  int                    numMultiClust = 0;

  if ( !inspiralList )
  {
    XLAL_ERROR(func,XLAL_EIO);
  }

  if ( ! *inspiralList )
  {
    XLALPrintInfo(
      "XLALClusterMultiInspiralTable: Empty coincList passed as input\n" );
    return( 0 );
  }

  thisEvent = *inspiralList;
  nextEvent = (*inspiralList)->next;
  *inspiralList = NULL;

  while ( nextEvent )
  {
    INT8 thisTime = XLALGPSToINT8NS( &(thisEvent->end_time) );
    INT8 nextTime = XLALGPSToINT8NS( &(nextEvent->end_time) );;

    /* find events within the cluster window */
    if ( (nextTime - thisTime) < dtimeNS )
    {
      REAL4 thisStat = XLALMultiInspiralStat( thisEvent, clusterchoice );
      REAL4 nextStat = XLALMultiInspiralStat( nextEvent, clusterchoice );

      if ( nextStat > thisStat )
      {
        /* displace previous event in cluster */
        if( prevEvent )
        {
          prevEvent->next = nextEvent;
        }
        XLALFreeMultiInspiral( &thisEvent );
        thisEvent = nextEvent;
        nextEvent = thisEvent->next;
      }
      else
      {
        /* otherwise just dump next event from cluster */
        thisEvent->next = nextEvent->next;
        XLALFreeMultiInspiral ( &nextEvent );
        nextEvent = thisEvent->next;
      }
    }
    else
    {
      /* otherwise we keep this unique event trigger */
      if ( ! *inspiralList )
      {
        *inspiralList = thisEvent;
      }
      prevEvent = thisEvent;
      thisEvent = thisEvent->next;
      nextEvent = thisEvent->next;
      ++numMultiClust;
    }
  }

    /* store the last event */
  if ( ! (*inspiralList) )
  {
    *inspiralList = thisEvent;
  }
  ++numMultiClust;

  return(numMultiClust);
}

/* <lalVerbatim file="MultiInspiralUtilsCP"> */
MultiInspiralTable *
XLALTimeCutMultiInspiral(
    MultiInspiralTable          *eventHead,
    LIGOTimeGPS                *startTime,
    LIGOTimeGPS                *endTime
    )
/* </lalVerbatim> */
{
  MultiInspiralTable    *inspiralEventList = NULL;
  MultiInspiralTable    *thisEvent = NULL;
  MultiInspiralTable    *prevEvent = NULL;
  INT8                  startTimeNS = XLALGPSToINT8NS( startTime );
  INT8                  endTimeNS = XLALGPSToINT8NS( endTime );


  /* Remove all the triggers before and after the requested */
  /* gps start and end times */

  thisEvent = eventHead;

  while ( thisEvent )
  {
    MultiInspiralTable *tmpEvent = thisEvent;
    thisEvent = thisEvent->next;

    if ( end_time(tmpEvent) >= startTimeNS &&
        end_time(tmpEvent) < endTimeNS )
    {
      /* keep this template */
      if ( ! inspiralEventList  )
      {
        inspiralEventList = tmpEvent;
      }
      else
      {
        prevEvent->next = tmpEvent;
      }
      tmpEvent->next = NULL;
      prevEvent = tmpEvent;
    }
    else
    {
      /* discard this template */
      XLALFreeMultiInspiral ( &tmpEvent );
    }
  }
  eventHead = inspiralEventList;

  return (eventHead);
}


/* <lalVerbatim file="MultiInspiralUtilsCP"> */
MultiInspiralTable *
XLALSNRCutMultiInspiral (
    MultiInspiralTable          *eventHead,
    REAL4                       snrCut
    )
/* </lalVerbatim> */
{
  MultiInspiralTable    *thisEvent = NULL;
  MultiInspiralTable    *prevEvent = NULL;

  thisEvent = eventHead;
  eventHead = NULL;

  while ( thisEvent )
  {
    MultiInspiralTable *tmpEvent = thisEvent;
    thisEvent = thisEvent->next;

    if ( tmpEvent->snr >= snrCut )
    {
      /* keep this template */
      if ( ! eventHead  )
      {
        eventHead = tmpEvent;
      }
      else
      {
        prevEvent->next = tmpEvent;
      }
      tmpEvent->next = NULL;
      prevEvent = tmpEvent;
    }
    else
    {
      /* discard this template */
      XLALFreeMultiInspiral ( &tmpEvent );
    }
  }
  return( eventHead );
}


/* <lalVerbatim file="MultiInspiralUtilsCP"> */
MultiInspiralTable *
XLALPlayTestMultiInspiral(
    MultiInspiralTable          *eventHead,
    LALPlaygroundDataMask       *dataType
    )
/* </lalVerbatim> */
{
  MultiInspiralTable    *inspiralEventList = NULL;
  MultiInspiralTable    *thisEvent = NULL;
  MultiInspiralTable    *prevEvent = NULL;

  INT8 triggerTime = 0;
  INT4 isPlay = 0;
  INT4 numTriggers;

  /* Remove all the triggers which are not of the desired type */

  numTriggers = 0;
  thisEvent = eventHead;

  if ( (*dataType == playground_only) || (*dataType == exclude_play) )
  {
    while ( thisEvent )
    {
      MultiInspiralTable *tmpEvent = thisEvent;
      thisEvent = thisEvent->next;

      triggerTime = XLALGPSToINT8NS( &(tmpEvent->end_time) );
      isPlay = XLALINT8NanoSecIsPlayground( &triggerTime );

      if ( ( (*dataType == playground_only)  && isPlay ) ||
          ( (*dataType == exclude_play) && ! isPlay) )
      {
        /* keep this trigger */
        if ( ! inspiralEventList  )
        {
          inspiralEventList = tmpEvent;
        }
        else
        {
          prevEvent->next = tmpEvent;
        }
        tmpEvent->next = NULL;
        prevEvent = tmpEvent;
        ++numTriggers;
      }
      else
      {
        /* discard this template */
        XLALFreeMultiInspiral ( &tmpEvent );
      }
    }
    eventHead = inspiralEventList;
    if ( *dataType == playground_only )
    {
      XLALPrintInfo( "Kept %d playground triggers \n", numTriggers );
    }
    else if ( *dataType == exclude_play )
    {
      XLALPrintInfo( "Kept %d non-playground triggers \n", numTriggers );
    }
  }
  else if ( *dataType == all_data )
  {
    XLALPrintInfo( "Keeping all triggers since all_data specified\n" );
  }
  else
  {
    XLALPrintInfo( "Unknown data type, returning no triggers\n" );
    eventHead = NULL;
  }

  return(eventHead);
}

/*CHECK: This function is not needed since XLALCountMultiInspiralTable
  in LIGOMetadataUtils in support already does that job */
/* <lalVerbatim file="MultiInspiralUtilsCP"> */
INT4 XLALCountMultiInspiral( MultiInspiralTable *head )
/* </lalVerbatim> */
{
  INT4 length;
  MultiInspiralTable *event;

  if ( !head )
  {
    return( 0 );
  }

  /* count the number of events in the list */
  for(length = 0, event = head; event; event = event->next)
    length++;

  return length;
}

/* <lalVerbatim file="MultiInspiralUtilsCP"> */
int
LALCompareMultiInspiralByTime (
    const void *a,
    const void *b
    )
/* </lalVerbatim> */
{
  LALStatus     status;
  const MultiInspiralTable *aPtr = *((const MultiInspiralTable * const *)a);
  const MultiInspiralTable *bPtr = *((const MultiInspiralTable * const *)b);
  INT8 ta, tb;

  memset( &status, 0, sizeof(LALStatus) );
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

/* <lalVerbatim file="MultiInspiralUtilsCP"> */
int
XLALMultiSimInspiralTest (
    SimInspiralTable  **simHead,
    MultiInspiralTable **eventHead,
    SimInspiralTable  **missedSimHead,
    MultiInspiralTable **missedMultiHead,
    INT8                injectWindowNS
    )
/* </lalVerbatim> */
{

  /* Note: we are assuming that both the inspiral and */
  /* injection events are time sorted                 */
  SimInspiralTable *thisSimEvent = *simHead;
  SimInspiralTable *thisMissedSim= NULL;
  SimInspiralTable *prevSimEvent = NULL;
  MultiInspiralTable *thisEvent   = *eventHead;
  MultiInspiralTable *prevEvent   = NULL;
  MultiInspiralTable *thisMissed  = NULL;
  EventIDColumn     *thisId      = NULL;
  CHAR              *ifo = NULL;

  int numSimFound  = 0;
  int coincidence = 0;

  INT8 simGeocentTime, simSiteTime, inspiralTime;
  INT8 earthRadiusNS = (INT8) ( 1e9 * 2 * LAL_REARTH_SI / LAL_C_SI );

  *simHead     = NULL;
  *eventHead   = NULL;


  if ( ! thisEvent )
  {
    XLALPrintInfo( "No triggers in input data, all injections missed\n" );

    *missedSimHead = thisSimEvent;
    return(0);
  }
  else
  {

    /* begin loop over the sim_inspiral events */
    while ( thisSimEvent )
    {
      coincidence = 0;
      /* find the end time of the SimEvent */
      simGeocentTime = geocent_end_time( thisSimEvent );

      /* find the first inspiral event after the current sim event */
      while ( thisEvent )
      {
        /* compute the time in nanosec for thisEvent */
        inspiralTime = XLALGPSToINT8NS( &(thisEvent->end_time) );

        if( inspiralTime < (simGeocentTime - earthRadiusNS - injectWindowNS ) )
        {
          /* discard this event and move on to the next one */
          if ( ! *missedMultiHead )
          {
            *missedMultiHead = thisMissed = thisEvent;
          }
          else
          {
            thisMissed = thisMissed->next = thisEvent;
          }
          if ( prevEvent ) prevEvent->next = thisEvent->next;
          thisEvent = thisEvent->next;
          thisMissed->next = NULL;
          XLALPrintInfo( "-" );
        }
        else
        {
          /* we have reached the negative coincincidence window */
          break;
        }
      }

      while ( thisEvent )
      {
        /* compute the time in nanosec for thisEvent */
        inspiralTime = XLALGPSToINT8NS( &(thisEvent->end_time) );

        if( inspiralTime < (simGeocentTime + earthRadiusNS + injectWindowNS ) )
        {
	  /*CHECK: The following assumes that the first ifo
	    in the ifos string is the
	    reference ifo for checking time consistency. */
          /* This event may be in coincidence window, need to check site
           * end time
	   strncpy( ifos, thisEvent->ifos, 2);
	   simSiteTime = XLALReturnSimInspiralEndTime( thisSimEvent,
	                                                       ifos );
	  */

	  /* read in the first (single) ifo in the multiInspiral network (ifos) */
	  LALSnprintf( ifo, LIGOMETA_IFO_MAX * sizeof(CHAR),
		       "%s", thisEvent->ifos );

	  simSiteTime = XLALReturnSimInspiralEndTime( thisSimEvent, ifo );

	  if ( (inspiralTime > (simSiteTime - injectWindowNS)) &&
	       (inspiralTime < (simSiteTime + injectWindowNS)) ) {

	    /* this event is within the coincidence window  */

	    /* store the sim inspiral in the event_id's for this sngl */
	    thisId = thisEvent->event_id;
	    while ( thisId )
	      {
		thisId->simInspiralTable = thisSimEvent;
		thisId = thisId->next;
	      }

	    /* store this event and move on to the next one */
	    if ( ! *eventHead ) *eventHead = thisEvent;
	    prevEvent = thisEvent;
	    thisEvent = thisEvent->next;
	    coincidence = 1;
	    XLALPrintInfo( "+" );
	  }
	  else
	    {
	      /* discard this event and move on to the next one */
	      if ( ! *missedMultiHead )
		{
		  *missedMultiHead = thisMissed = thisEvent;
		}
	      else
		{
		  thisMissed = thisMissed->next = thisEvent;
		}

	      if ( prevEvent ) prevEvent->next = thisEvent->next;
	      thisEvent = thisEvent->next;
	      thisMissed->next = NULL;
	      XLALPrintInfo( "-" );
	    }
	}
	else
	  {
	    /* we have reached the end of the positive coincincidence window */
	    break;
	  }
      }

      if ( coincidence )
	{
	  /* keep this sim event in the list and move to the next sim event */
	  if ( ! *simHead ) *simHead = thisSimEvent;
	  prevSimEvent = thisSimEvent;
	  ++numSimFound;
	  thisSimEvent = thisSimEvent->next;
	  XLALPrintInfo( "F" );
	}
      else {
        /* save this sim event in the list of missed events... */
        if ( ! *missedSimHead )
        {
          *missedSimHead = thisMissedSim = thisSimEvent;
        }
        else
        {
          thisMissedSim = thisMissedSim->next = thisSimEvent;
        }

        /* ...and remove it from the list of found events */
        if ( prevSimEvent ) prevSimEvent->next = thisSimEvent->next;
        XLALPrintInfo( "M" );

        /* move to the next sim in the list */
        thisSimEvent = thisSimEvent->next;

        /* make sure the missed sim list is terminated */
        thisMissedSim->next = NULL;
      }

      if ( ! thisEvent )
      {
        /* these are no more events to process so all the rest of the */
        /* injections must be put in the missed injections list       */
        if ( ! *missedSimHead )
        {
          /* this and any subsequent events are in the missed sim list */
          if ( thisSimEvent ) thisMissedSim = *missedSimHead = thisSimEvent;
        }
        else
        {
          if ( thisSimEvent )
          {
            /* append the rest of the list to the list of missed injections */
            thisMissedSim = thisMissedSim->next = thisSimEvent;
          }
          else
          {
            /* there are no injections after this one */
            thisMissedSim = thisMissedSim->next = NULL;
          }
        }

        /* terminate the list of found injections correctly */
        if ( prevSimEvent ) prevSimEvent->next = NULL;

        while ( thisMissedSim )
        {
          /* count the number of injections just stuck in the missed list */
          XLALPrintInfo( "M" );
          thisMissedSim = thisMissedSim->next;
        }
        thisSimEvent = NULL;
        break;
      }
    }

    if ( thisEvent )
    {
      while( thisEvent )
      {
        /* discard this event and move on to the next one */
        if ( ! *missedMultiHead )
        {
          *missedMultiHead = thisMissed = thisEvent;
        }
        else
        {
          thisMissed = thisMissed->next = thisEvent;
        }
        if ( prevEvent ) prevEvent->next = thisEvent->next;
        thisEvent = thisEvent->next;
        thisMissed->next = NULL;
        XLALPrintInfo( "-" );
      }
    }
  }
  XLALPrintInfo( "\n" );
  return( numSimFound );
}
