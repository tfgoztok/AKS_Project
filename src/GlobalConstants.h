#ifndef GLOBALCONSTANTS_H
#define GLOBALCONSTANTS_H

namespace GlobalConstants
{
    const int HEARTBEAT_TIMEOUT_MS = 5000;            // Timeout for heartbeats (5 seconds)
    const int HEARTBEAT_INTERVAL_MS = 1000;           // Interval for sending heartbeats (1 second)
    const int COMMUNICATION_CHECK_INTERVAL_MS = 5000; // Interval for checking communication (5 second)
}

#endif