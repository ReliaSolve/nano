#ifndef NMB_DEVICESEQUENCER_H
#define NMB_DEVICESEQUENCER_H

#include "nmb_Device.h"

#define NMB_DEVICESEQUENCER_MAX_SENDING_DEVICES (16)
#define NMB_DEVICESEQUENCER_MAX_DEVICES (16)
#define NMB_DEVICESEQUENCER_MAX_DATA_MESSAGES (16)

class SequenceEntry {
  public:
    SequenceEntry();
    void print();

    int numSenders;
    // index into d_devices
    int senderIndex[NMB_DEVICESEQUENCER_MAX_SENDING_DEVICES];

    int numDevices;
    // index into d_devices
    int deviceIndex[NMB_DEVICESEQUENCER_MAX_DEVICES];

    int numMessages;
    // index into d_messageTypes
    int messageIndex[NMB_DEVICESEQUENCER_MAX_DATA_MESSAGES];
    int numRepeat[NMB_DEVICESEQUENCER_MAX_DATA_MESSAGES];
    SequenceEntry *next;
};

/**

The sequence is specified as a series of synchronization checkpoints with
various stuff that can happen in between them. Any stuff before a
checkpoint is guaranteed to happen before anything that comes after it.
The "stuff" that can be inserted between checkpoints is any number of the
following:
a) send: let a given client send all of the messages it tried to
        send since the last time it was allowed to send anything
        (to enforce this I must intercept all messages sent by the client
         and buffer them until it is okay to send them)

b) receive: make sure a client receives a given message from a given
        device a given number of times

related to specification of (a) above:
An important thing that needs to be recognized by the user is that in order
to regulate the sending of commands that may change the state of devices,
it is necessary that all commands which affect the device state be sent
using the nmb_Device_Client::dispatchMessage() function. If you fail to do this
then the code won't work as intended.

example:

 nmb_DeviceSequencer s;
 s.begin();
 s.addDeviceMessageSend(device_A);
 s.addCheckPoint();
 s.addDataMessage(device_B, data_message_type, 1);
 s.end();
 s.takeSendControl();

This example will alternate between sending commands to device A and
receiving a single data message from device B. 
Messages sent to device B will not
be regulated in any way and all messages received from device A will be
ignored by the sequence. The cycle of sending commands and receiving data will
be repeated indefinitely. When the operation is complete, the user should
call s.releaseControl(). Then the devices should behave as if the sequence
was never created.

Algorithm summary:
for each checkpoint interval, the sequence-executing process does the following:

phase 1. sends out all commands to devices for which addDeviceMessageSend() was 
         called for that interval
phase 2. sends out synchronization requests to all devices for which either
         addDeviceMessageSend() or addDataMessage() was called for that interval
phase 3. - as synchronization responses come back for each device, data message
           counters are reset for that device
         - as synchronization responses come back for each sending device, those
           devices are flagged as having completed their commands
         - as each data message arrives, the counter for that message is
           incremented
         - as soon as the data message counters have reached the specified
           number of messages and all synchronization responses have arrived
           the sequence calls iteration callbacks, increments to the next
           checkpoint interval, and repeats starting in phase 1
   
*/


class nmb_DeviceSequencer {

  public:
    nmb_DeviceSequencer(int verbosity = 0);
    ~nmb_DeviceSequencer();
    // ************************************ 
    // functions for specifying the sequence:
    // ************************************
    /// initialization for specifying a new sequence
    int begin();
    /// cleanup for when you are done specifying a sequence
    int end();
    /// adds a new synchronization check point
    int addCheckPoint();
    /// let this device send messages between the last and next checkpoints
    int addDeviceMessageSend(nmb_Device_Client *device); 
                                // let this device send messages 
                                // between last checkpoint and the next one

    /// make sure we receive this message between the last and next checkpoints
    int addDataMessage(nmb_Device_Client *device, 
                       vrpn_int32 message_type, 
                       int n_repeat);

    /**
      These functions enable/disable any influence that the sequence has
      on the devices that have been passed to it as parameters of
      the sequence specification functions.
    */
    int takeControl();
    int releaseControl();

    /**
       callback registration - when your callback gets called it means
       that all devices have been synchronized and the sequence is about to start
       running on the first sequence entry - 
       any commands you give to sending devices in this callback which are
       involved in the sequence will be executed before the next time the 
       callback is called by this sequence.
       You have the option at this point to disable the sequence by calling
       releaseControl().
    */
    int registerIterationHandler (int (* handler) (void *), void *userdata);
    int unregisterIterationHandler (int (* handler) (void *), void *userdata);

  protected:

    // helper functions:
    int unregisterSynchHandlers();
    int registerSynchHandlers();
    int unregisterMessageHandlers();
    int registerMessageHandlers();
    int addEntry();
    int addDevice(int deviceIndex);
    int addDevice(nmb_Device_Client *device);
    int addSender(nmb_Device_Client *device);
    int addDeviceMessageSend(int senderIndex);
    int addDataMessage(int messageIndex, int n_repeat);
    vrpn_bool synchCompleted();
    vrpn_bool dataCompleted();
    void increment();
    void requestSynch();
    void enableCommandBuffers();
    void disableCommandBuffers();
    void sendCommands();
    void resetDataCounters(int deviceIndex);
    void doIterationCallbacks();

    static int dataMessageHandler(void *ud, vrpn_HANDLERPARAM p);
    static int deviceSynchHandler(void *ud, const nmb_SynchMessage *sm);

    struct iterationHandlerEntry {
        int (* handler) (void *);
        void *userdata;
        iterationHandlerEntry * next;
    };
    iterationHandlerEntry * d_iterationHandlers;

    struct dataMessageSpec {
      nmb_Device_Client *device;
      vrpn_int32 message_type;
    };

    // this is a list of all devices involved in the sequence including all
    // those in the following two lists
    int d_numDevices;
    nmb_Device_Client *d_devices[NMB_DEVICESEQUENCER_MAX_DEVICES];

    // this is a subset of d_devices containing devices for which we need
    // to regulate the sending of commands
    int d_numSenders;
    nmb_Device_Client *d_senders[NMB_DEVICESEQUENCER_MAX_DEVICES];

    // this is a list of all devices/message types for messages specified
    // to be received in the sequence 
    int d_numMessageTypes;
    dataMessageSpec d_messageTypes[NMB_DEVICESEQUENCER_MAX_DATA_MESSAGES];

    struct dataHandlerInfo {
      nmb_DeviceSequencer *sequencer;
      int messageTypeIndex;
    };
    dataHandlerInfo d_dataHandlerInfo[NMB_DEVICESEQUENCER_MAX_DATA_MESSAGES];

    struct synchHandlerInfo {
      nmb_DeviceSequencer *sequencer;
      int deviceIndex;
    };
    synchHandlerInfo d_synchHandlerInfo[NMB_DEVICESEQUENCER_MAX_DEVICES];

    SequenceEntry *d_sequenceHead;
    SequenceEntry *d_sequenceTail;

    int d_verbosity;
    vrpn_bool d_needNewSequenceEntry;

    vrpn_bool d_running;
    vrpn_bool d_specified;

    // state that changes while running the sequence:

    // which part of the sequence are we in at the moment?:
    SequenceEntry *d_currentSequenceEntry;

    // how many of each type have we received in the current interval?:
    int d_numMessagesReceived[NMB_DEVICESEQUENCER_MAX_DATA_MESSAGES];

    // has a device echoed back the last synchronization request indicating
    // that all subsequent data will be acquired and subsequent commands 
    // will be executed after our request?:
    vrpn_bool d_deviceSynch[NMB_DEVICESEQUENCER_MAX_DEVICES];
    int d_numSynchExpected;
    int d_numSynchReceived;
    vrpn_bool d_synchInProgress;
};

#endif