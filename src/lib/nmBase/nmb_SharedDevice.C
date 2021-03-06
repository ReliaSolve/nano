/* The nanoManipulator and its source code have been released under the
 * Boost software license when nanoManipulator, Inc. ceased operations on
 * January 1, 2014.  At this point, the message below from 3rdTech (who
 * sublicensed from nanoManipulator, Inc.) was superceded.
 * Since that time, the code can be used according to the following
 * license.  Support for this system is now through the NIH/NIBIB
 * National Research Resource at cismm.org.

Boost Software License - Version 1.0 - August 17th, 2003

Permission is hereby granted, free of charge, to any person or organization
obtaining a copy of the software and accompanying documentation covered by
this license (the "Software") to use, reproduce, display, distribute,
execute, and transmit the Software, and to prepare derivative works of the
Software, and to permit third-parties to whom the Software is furnished to
do so, all subject to the following:

The copyright notices in the Software and this entire statement, including
the above license grant, this restriction and the following disclaimer,
must be included in all copies of the Software, in whole or in part, and
all derivative works of the Software, unless such copies or derivative
works are solely in the form of machine-executable object code generated by
a source language processor.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
*/

#include "nmb_SharedDevice.h"




nmb_SharedDevice_Server::nmb_SharedDevice_Server (const char * name,
                             vrpn_Connection * c) :
    nmb_Device_Server (name, c),
    d_mutex (name, c) {

}

// virtual
nmb_SharedDevice_Server::~nmb_SharedDevice_Server (void) {

}







nmb_SharedDevice_Remote::nmb_SharedDevice_Remote (const char * name,
                      vrpn_Connection * connectionToDeviceServer) :
    nmb_Device_Client (name, connectionToDeviceServer),
    d_mutex (name, connectionToDeviceServer),
    d_gotMutexCallbacks (NULL),
    d_deniedMutexCallbacks (NULL),
    d_mutexTakenCallbacks (NULL),
    d_mutexReleasedCallbacks (NULL)
{
  int i;

  for (i = 0; i < vrpn_CONNECTION_MAX_TYPES; i++) {
    d_typeSafe[i] = VRPN_FALSE;
  }

  d_mutex.addRequestGrantedCallback(this, handle_gotMutex);
  d_mutex.addRequestDeniedCallback(this, handle_deniedMutex);
  d_mutex.addTakeCallback(this, handle_mutexTaken);
  d_mutex.addReleaseCallback(this, handle_mutexReleased);

//fprintf(stderr, "nmb_SharedDevice_Remote::nmb_SharedDevice_Remote:  requesting the lock on %s.\n", d_myName);

  // If we're currently running standalone grab the mutex.
  d_mutex.request();

  // XXX HACK
  // Call mainloop now to make sure we have the mutex before we return;
  // some of the code we're driving tries to send initialization commands/
  // queries before it ever calls mainloop, and we need to not quash those
  // when running standalone - they're being changed from commands to
  // queries by Aron/Alexandra so that they don't hurt when we get a
  // late joiner while running multiuser.  We could instead attach those
  // commands to a gotMutex callback...
  //d_mutex.mainloop();

}




// virtual
nmb_SharedDevice_Remote::~nmb_SharedDevice_Remote (void) {


}




vrpn_bool nmb_SharedDevice_Remote::haveMutex (void) const {
  return d_mutex.isHeldLocally();
}

vrpn_bool nmb_SharedDevice_Remote::typeIsSafe (vrpn_int32 type) const {
  if ((type < 0) || (type >= vrpn_CONNECTION_MAX_TYPES)) {
    return VRPN_FALSE;
  }
  return d_typeSafe[type];
}






int nmb_SharedDevice_Remote::mainloop (void) {
  d_mutex.mainloop();
  return 0;
}




void nmb_SharedDevice_Remote::requestMutex (void) {

//fprintf(stderr, "nmb_SharedDevice_Remote::requestMutex:  requesting the lock on %s.\n",d_myName);

  d_mutex.request();

}

void nmb_SharedDevice_Remote::releaseMutex (void) {

//fprintf(stderr, "nmb_SharedDevice_Remote:  releasing the lock on %s.\n", d_myName);

  d_mutex.release();

}


// virtual
long nmb_SharedDevice_Remote::sendBuffer(void) {
  long retval;
  vrpn_bool use_buffer_save = getBufferEnable();

  setBufferEnable(vrpn_FALSE);

  retval = nmb_Device_Client::sendBuffer();

  setBufferEnable(use_buffer_save);

  return retval;
}


// virtual
long nmb_SharedDevice_Remote::dispatchMessage (long len, const char * buf,
                                        vrpn_int32 type) {

  vrpn_bool bufferingOn = getBufferEnable();
  if (d_mutex.isHeldLocally() || typeIsSafe(type) || bufferingOn) {
    return nmb_Device_Client::dispatchMessage(len, buf, type);
  }

//fprintf(stderr, "nmb_SharedDevice_Remote:  throwing out a message\n"
//"  (type %d, to %s) since we don't have the lock.\n", type, d_myName);

  return 0;
}


// virtual
long nmb_SharedDevice_Remote::dispatchRedundantMessage
                                       (long len, const char * buf,
                                        vrpn_int32 type) {

  vrpn_bool bufferingOn = getBufferEnable();
  if (d_mutex.isHeldLocally() || typeIsSafe(type) || bufferingOn) {
    return nmb_Device_Client::dispatchRedundantMessage(len, buf, type);
  }

//fprintf(stderr, "nmb_SharedDevice_Remote:  throwing out a message\n"
//"  (type %d, to %s) since we don't have the lock.\n", type, d_myName);

  return 0;
}





void nmb_SharedDevice_Remote::markTypeAsSafe (vrpn_int32 type) {
  if ((type < 0) || (type >= vrpn_CONNECTION_MAX_TYPES)) {
    fprintf(stderr, "nmb_SharedDevice_Remote::markTypeAsSafe:  "
            "illegal type %d.\n", type);
    return;
  }
  d_typeSafe[type] = VRPN_TRUE;
}




void nmb_SharedDevice_Remote::registerGotMutexCallback
            (void * userdata,
             void (VRPN_CALLBACK * f) (void *, nmb_SharedDevice_Remote *)) {
  sharedDeviceCallback * cb;

  cb = new sharedDeviceCallback;
  if (!cb) {
    fprintf(stderr, "nmb_SharedDevice_Remote::registerGotMutexCallback:  "
                    "Out of memory.\n");
    return;
  }

  cb->f = f;
  cb->userdata = userdata;
  cb->next = d_gotMutexCallbacks;
  d_gotMutexCallbacks = cb;

}

void nmb_SharedDevice_Remote::unregisterGotMutexCallback
	 (void * userdata,
          void (VRPN_CALLBACK * f) (void *, nmb_SharedDevice_Remote *))
{
  sharedDeviceCallback * victim, ** snitch;

  snitch = &d_gotMutexCallbacks;
  victim = *snitch;
  while (victim &&
         (victim->f != f) &&
         (victim->userdata != userdata)) {
    snitch = &((*snitch)->next);
    victim = *snitch;
  }

  if (!victim) {
    fprintf(stderr, "nmb_SharedDevice_Remote::unregisterGotMutexCallback:  "
                    "No such handler.\n");
    return;
  }

  *snitch = victim->next;
  delete victim;
}


// static
void nmb_SharedDevice_Remote::registerDeniedMutexCallback
            (void * userdata,
             void (VRPN_CALLBACK * f) (void *, nmb_SharedDevice_Remote *)) {
  sharedDeviceCallback * cb;

  cb = new sharedDeviceCallback;
  if (!cb) {
    fprintf(stderr, "nmb_SharedDevice_Remote::registerDeniedMutexCallback:  "
                    "Out of memory.\n");
    return;
  }

  cb->f = f;
  cb->userdata = userdata;
  cb->next = d_deniedMutexCallbacks;
  d_deniedMutexCallbacks = cb;

}

void nmb_SharedDevice_Remote::unregisterDeniedMutexCallback
         (void * userdata,
          void (VRPN_CALLBACK * f) (void *, nmb_SharedDevice_Remote *))
{
  sharedDeviceCallback * victim, ** snitch;

  snitch = &d_deniedMutexCallbacks;
  victim = *snitch;
  while (victim &&
         (victim->f != f) &&
         (victim->userdata != userdata)) {
    snitch = &((*snitch)->next);
    victim = *snitch;
  }

  if (!victim) {
    fprintf(stderr, "nmb_SharedDevice_Remote::unregisterDeniedMutexCallback:  "
                    "No such handler.\n");
    return;
  }

  *snitch = victim->next;
  delete victim;
}



// static
void nmb_SharedDevice_Remote::registerMutexTakenCallback
            (void * userdata,
             void (VRPN_CALLBACK * f) (void *, nmb_SharedDevice_Remote *)) {
  sharedDeviceCallback * cb;

  cb = new sharedDeviceCallback;
  if (!cb) {
    fprintf(stderr, "nmb_SharedDevice_Remote::registerMutexTakenCallback:  "
                    "Out of memory.\n");
    return;
  }

  cb->f = f;
  cb->userdata = userdata;
  cb->next = d_mutexTakenCallbacks;
  d_mutexTakenCallbacks = cb;

}

void nmb_SharedDevice_Remote::unregisterMutexTakenCallback
         (void * userdata,
          void (VRPN_CALLBACK * f) (void *, nmb_SharedDevice_Remote *))
{
  sharedDeviceCallback * victim, ** snitch;

  snitch = &d_mutexTakenCallbacks;
  victim = *snitch;
  while (victim &&
         (victim->f != f) &&
         (victim->userdata != userdata)) {
    snitch = &((*snitch)->next);
    victim = *snitch;
  }

  if (!victim) {
    fprintf(stderr, "nmb_SharedDevice_Remote::unregisterMutexTakenCallback:  "
                    "No such handler.\n");
    return;
  }

  *snitch = victim->next;
  delete victim;
}


// static
void nmb_SharedDevice_Remote::registerMutexReleasedCallback
            (void * userdata,
             void (VRPN_CALLBACK * f) (void *, nmb_SharedDevice_Remote *)) {
  sharedDeviceCallback * cb;

  cb = new sharedDeviceCallback;
  if (!cb) {
    fprintf(stderr, "nmb_SharedDevice_Remote::registerMutexReleasedCallback:  "
                    "Out of memory.\n");
    return;
  }

  cb->f = f;
  cb->userdata = userdata;
  cb->next = d_mutexReleasedCallbacks;
  d_mutexReleasedCallbacks = cb;

}

void nmb_SharedDevice_Remote::unregisterMutexReleasedCallback
         (void * userdata,
          void (VRPN_CALLBACK * f) (void *, nmb_SharedDevice_Remote *))
{
  sharedDeviceCallback * victim, ** snitch;

  snitch = &d_mutexReleasedCallbacks;
  victim = *snitch;
  while (victim &&
         (victim->f != f) &&
         (victim->userdata != userdata)) {
    snitch = &((*snitch)->next);
    victim = *snitch;
  }

  if (!victim) {
    fprintf(stderr,"nmb_SharedDevice_Remote::unregisterMutexReleasedCallback:"
                    "  No such handler.\n");
    return;
  }

  *snitch = victim->next;
  delete victim;
}


// static
int nmb_SharedDevice_Remote::handle_gotMutex (void * userdata) {
  nmb_SharedDevice_Remote * me = (nmb_SharedDevice_Remote *) userdata;
  sharedDeviceCallback * cb;

//fprintf(stderr, "nmb_SharedDevice_Remote:  got the lock on %s.\n",me->d_myName);

  for (cb = me->d_gotMutexCallbacks;  cb;  cb = cb->next) {
    (cb->f)(cb->userdata, me);
  }

  return 0;
}


// static
int nmb_SharedDevice_Remote::handle_deniedMutex (void * userdata) {
  nmb_SharedDevice_Remote * me = (nmb_SharedDevice_Remote *) userdata;
  sharedDeviceCallback * cb;

//fprintf(stderr, "nmb_SharedDevice_Remote:  denied the lock on %s.\n",me->d_myName);

  for (cb = me->d_deniedMutexCallbacks;  cb;  cb = cb->next) {
    (cb->f)(cb->userdata, me);
  }

  return 0;
}


// static
int nmb_SharedDevice_Remote::handle_mutexTaken (void * userdata) {
  nmb_SharedDevice_Remote * me = (nmb_SharedDevice_Remote *) userdata;
  sharedDeviceCallback * cb;

//fprintf(stderr, "nmb_SharedDevice_Remote:  somebody got %s.\n", me->d_myName);

  for (cb = me->d_mutexTakenCallbacks;  cb;  cb = cb->next) {
    (cb->f)(cb->userdata, me);
  }

  return 0;
}


// static
int nmb_SharedDevice_Remote::handle_mutexReleased (void * userdata) {
  nmb_SharedDevice_Remote * me = (nmb_SharedDevice_Remote *) userdata;
  sharedDeviceCallback * cb;

//fprintf(stderr, "nmb_SharedDevice_Remote:  somebody released %s.\n",me->d_myName);

  for (cb = me->d_mutexReleasedCallbacks;  cb;  cb = cb->next) {
    (cb->f)(cb->userdata, me);
  }

  return 0;
}


