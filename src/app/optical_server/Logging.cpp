
#include <time.h>
#include "Logging.h"
#include "opticalServer.h"

Logging* Logging::instance;
CRITICAL_SECTION Logging::cslogging;
char* Logging::connName;
int Logging::staticInitializer = Logging::staticInitialize( );

// static
int Logging::
staticInitialize( )
{
	InitializeCriticalSection( &cslogging );
	instance = NULL;
	char* middle = "@localhost:";
	connName = new char[ strlen( opticalServerName ) + strlen( middle ) + (int) ceil( log10( opticalServerPort ) ) + 1 ];
	sprintf( connName, "%s%s%d", opticalServerName, middle, opticalServerPort );
	return 0;
}


Logging::Logging( )
{
	sem = NULL;
	conn = NULL;
	isLogging = false;
	logfileName = NULL;
}


Logging::~Logging( )
{
	EnterCriticalSection( &cslogging );
	if( sem ) sem->mainloop( );
	if( conn )
	{
		conn->mainloop( );
		conn->save_log_so_far( );
	}
	if( sem )
	{
		sem->unregisterChangeHandler( NULL, (nmm_Microscope_SEM_Remote_ChangeHandler_t) SEM_handler );
		delete sem;
		sem = NULL;
	}
	if( conn )
	{
		delete conn;
		conn = NULL;
	}
	LeaveCriticalSection( &cslogging );

}


// static
Logging* Logging::
getInstance( )
{
	if( instance == NULL )
	{
		EnterCriticalSection( &cslogging );
		if( instance == NULL )
			instance = new Logging ();
		LeaveCriticalSection( &cslogging );
	}
	return instance;
}


void Logging::
makeNewLogfileName()
{
	EnterCriticalSection( &cslogging );
	if( logfileName != NULL )
		delete logfileName;
	char newname[512];
	time_t longtime;
	time( &longtime );
	struct tm* mytime = localtime( &longtime );
	sprintf( newname, "%04d-%02d-%02d--%02d-%02d-%02d.nms.sem",
			mytime->tm_year + 1900, mytime->tm_mon, mytime->tm_mday,
			mytime->tm_hour, mytime->tm_min, mytime->tm_sec );
	logfileName = new char[ strlen( newname ) + 1 ];
	strcpy( logfileName, newname );
	LeaveCriticalSection( &cslogging );
}


const char* Logging::
startLogging ( )
{
	EnterCriticalSection( &cslogging );
	if( !isLogging )
	{
		makeNewLogfileName();
		conn = vrpn_get_connection_by_name( connName, logfileName );
		conn->mainloop( );
		sem = new nmm_Microscope_SEM_Remote( connName, conn );
		sem->registerChangeHandler( NULL, (nmm_Microscope_SEM_Remote_ChangeHandler_t) SEM_handler );
		sem->requestScan( 5 );
		isLogging = true;
		fprintf( stdout, "logging to %s\n", logfileName );
	}
	LeaveCriticalSection( &cslogging );
	return logfileName;
}


const char* Logging::
stopLogging( )
{
	EnterCriticalSection( &cslogging );
	char* oldName = logfileName;
	if( isLogging )
	{
		sem->mainloop( );
		conn->mainloop( );
		conn->save_log_so_far( );
		sem->unregisterChangeHandler( NULL, /*(nmm_Microscope_SEM_Remote_ChangeHandler_t)*/ SEM_handler );
		delete sem;
		sem = NULL;
		delete conn;
		conn = NULL;
		delete logfileName;
		logfileName = NULL;
		isLogging = false;
		fprintf( stdout, "logged to %s\n", logfileName );
	}
	LeaveCriticalSection( &cslogging );
	return oldName;
}


void Logging::
mainloop( )
{
	if( sem != NULL )
	{
		EnterCriticalSection( &cslogging );
		if( sem != NULL )  sem->mainloop( );
		if( conn != NULL )
		{
			conn->mainloop( );
			conn->save_log_so_far( );
		}
		LeaveCriticalSection( &cslogging );
	}
}


//static 
void Logging::
SEM_handler( void*, const nmm_Microscope_SEM_ChangeHandlerData& d )
{  
	if( d.sem->lastScanMessageCompletesImage( ) )
		d.sem->requestScan( 1 );
}
