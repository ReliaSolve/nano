
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
	justStartedLogging = false;
	stopLoggingNextMainloop = false;
	connectionNotConnectedAtStartLogging = false;
	thread = NULL;
	keepRunning = false;
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
	testAndCreateDirectory( );
	EnterCriticalSection( &cslogging );
	if( logfileName != NULL )
		delete logfileName;
	char newname[512];
	time_t longtime;
	time( &longtime );
	struct tm* mytime = localtime( &longtime );
	sprintf( newname, "D:\\Data\\sem_video\\%04d-%02d-%02d--%02d-%02d-%02d.nms.sem",
			mytime->tm_year + 1900, mytime->tm_mon, mytime->tm_mday,
			mytime->tm_hour, mytime->tm_min, mytime->tm_sec );
	logfileName = new char[ strlen( newname ) + 1 ];
	strcpy( logfileName, newname );
	LeaveCriticalSection( &cslogging );
}


void Logging::
testAndCreateDirectory( )
{
	// see if D:\Data\sem_video exists
	HANDLE dirH = CreateFile( "D:\\Data\\sem_video\\", 0, FILE_SHARE_READ | FILE_SHARE_WRITE,
								NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL );
	if( dirH == INVALID_HANDLE_VALUE ) 
	{
		// try to create the directory
		int retval = CreateDirectory( "D:\\Data\\sem_video\\", NULL );
		if( retval == 0 /*failure*/ )
		{
			LPVOID lpMsgBuf;
			FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
							NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
							(LPTSTR) &lpMsgBuf, 0, NULL );
			fprintf( stderr, "Couldn't create data directory D:\\Data\\sem_video "
					"or C:\\Data\\sem_video\n" );
			fprintf( stderr, (LPCTSTR)lpMsgBuf );
			fprintf( stderr, "\n" );
			exit( -1 );
		}
	}
}


const char* Logging::
startLogging ( )
{
	EnterCriticalSection( &cslogging );
	if( !isLogging )
	{
		makeNewLogfileName();
		conn = vrpn_get_connection_by_name( connName, logfileName );
		//conn->mainloop( );
		if( !conn->connected( ) )  connectionNotConnectedAtStartLogging = true;
		sem = new nmm_Microscope_SEM_Remote( connName, conn );
		sem->registerChangeHandler( NULL, (nmm_Microscope_SEM_Remote_ChangeHandler_t) SEM_handler );
		sem->requestScan( 1 );
		isLogging = true;
		fprintf( stdout, "logging to %s\n", logfileName );
		justStartedLogging = true;
	    LPDWORD lpThreadID = NULL;
		keepRunning = true;
		thread = CreateThread( NULL, 0, Logging_threadFunc, NULL, 0, lpThreadID );
	}
	LeaveCriticalSection( &cslogging );
	return logfileName;
}


const char* Logging::
stopLogging( )
{
	EnterCriticalSection( &cslogging );
	char* oldName = logfileName;
	if( isLogging && !justStartedLogging )
	{
		keepRunning = false;
		printf( "keepRunning is false; waiting for logging thread to stop...\n" );
		// wait for the logging thread to stop
		LeaveCriticalSection( &cslogging );
		DWORD lpExitCode = 0;
		GetExitCodeThread( thread, &lpExitCode );
		while( lpExitCode == STILL_ACTIVE )
		{
			vrpn_SleepMsecs( 5 );
			GetExitCodeThread( thread, &lpExitCode );
		}
		EnterCriticalSection( &cslogging );
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
	if( justStartedLogging )
	{
		stopLoggingNextMainloop = true;
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
		if( connectionNotConnectedAtStartLogging )
		{
			if( conn->connected( ) )
			{
				sem->requestScan( 1 );
				connectionNotConnectedAtStartLogging = false;
			}
		}
		if( justStartedLogging ) justStartedLogging = false;
		if( stopLoggingNextMainloop ) 
		{
			stopLogging( );
			stopLoggingNextMainloop = false;
		}
		LeaveCriticalSection( &cslogging );
	}
}


//static 
void Logging::
SEM_handler( void*, const nmm_Microscope_SEM_ChangeHandlerData& d )
{  
	static int completedImages = 0;
	if( d.sem->lastScanMessageCompletesImage( ) )
	{
		//printf( "%d completed images\n", ++completedImages );
		d.sem->requestScan( 1 );
	}
	/*
	if( d.msg_type != nmm_Microscope_SEM::SCANLINE_DATA )
		printf( "message from SEM.  type:  %d.\n", d.msg_type );
		*/
}


DWORD WINAPI Logging::
Logging_threadFunc( LPVOID lpParameter )
{
	while( instance && instance->keepRunning )
	{
		EnterCriticalSection( &cslogging );
		if( instance ) instance->mainloop( );
		LeaveCriticalSection( &cslogging );
		vrpn_SleepMsecs( 5 );
	}

	return 0;
}
