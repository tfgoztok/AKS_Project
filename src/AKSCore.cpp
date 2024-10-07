#include "AKSCore.h"
#include "Logger.h"
#include "GlobalConstants.h"
#include <QTimer>

// Constructor for AKSCore
AKSCore::AKSCore(QObject *parent)
    : QObject(parent), communicationManager(nullptr), launchSequenceTimer(nullptr),
      launchingMissileIndex(-1), ansConnected(false), launcherConnected(false),
      communicationCheckTimer(new QTimer(this)), currentlyPoweredMissile(-1)
{
    missileStates.resize(4, Unhealthy); // Initialize missile states to Unhealthy for 4 missiles
}

// Destructor for AKSCore
AKSCore::~AKSCore()
{
    stop(); // Ensure all operations are stopped during destruction
}

// Start the core operations
void AKSCore::start()
{
    communicationManager = new CommunicationManager(this); // Create a new communication manager
    // Connect signals from the communication manager to the corresponding slots in AKSCore
    connect(communicationManager, &CommunicationManager::ansHeartbeatReceived, this, &AKSCore::onANSHeartbeatReceived);
    connect(communicationManager, &CommunicationManager::launcherHeartbeatReceived, this, &AKSCore::onLauncherHeartbeatReceived);
    connect(communicationManager, &CommunicationManager::platformInfoReceived, this, &AKSCore::onPlatformInfoReceived);
    connect(communicationManager, &CommunicationManager::ansCommunicationLost, this, &AKSCore::onANSCommunicationLost);
    connect(communicationManager, &CommunicationManager::launcherCommunicationLost, this, &AKSCore::onLauncherCommunicationLost);

    // Connect the communication check timer to the checkCommunicationStatus slot
    connect(communicationCheckTimer, &QTimer::timeout, this, &AKSCore::checkCommunicationStatus);
    communicationCheckTimer->start(GlobalConstants::COMMUNICATION_CHECK_INTERVAL_MS); // Start the timer to check communication every 5 seconds

    communicationManager->start(); // Start the communication manager
    LOG_INFO("AKS Core started");  // Log that the AKS Core has started
}

// Stop the core operations
void AKSCore::stop()
{
    // Stop timers, close connections, etc.
    communicationManager->stop(); // Stop the communication manager
    LOG_INFO("AKS Core stopped"); // Log that the AKS Core has stopped
}

// Update target information with latitude, longitude, altitude, speed, and distance
void AKSCore::updateTargetInfo(double latitude, double longitude, double altitude, double speed, double distance)
{
    // Validate input for latitude
    if (latitude < -90 || latitude > 90)
    {
        emitError("Invalid latitude value"); // Emit error for invalid latitude
        return;                              // Exit if invalid
    }
    // Validate input for longitude
    if (longitude < -180 || longitude > 180)
    {
        emitError("Invalid longitude value"); // Emit error for invalid longitude
        return;                               // Exit if invalid
    }
    // Validate input for altitude
    if (altitude < 0)
    {
        emitError("Invalid altitude value"); // Emit error for invalid altitude
        return;                              // Exit if invalid
    }
    // Validate input for speed
    if (speed < 0)
    {
        emitError("Invalid speed value"); // Emit error for invalid speed
        return;                           // Exit if invalid
    }
    // Validate input for distance
    if (distance < 0)
    {
        emitError("Invalid distance value"); // Emit error for invalid distance
        return;                              // Exit if invalid
    }

    // Log the updated target information
    LOG_INFO(QString("Target info updated: Lat %1, Lon %2, Alt %3, Speed %4, Distance %5")
                 .arg(latitude)
                 .arg(longitude)
                 .arg(altitude)
                 .arg(speed)
                 .arg(distance));
}

// Toggle the power state of a missile based on its index
void AKSCore::toggleMissilePower(int missileIndex)
{
    if (missileIndex < 0 || missileIndex >= missileStates.size())
    {
        emitError(QString("Invalid missile index: %1").arg(missileIndex));
        return;
    }
    else if (launchingMissileIndex != -1)
    {
        emitError("Cannot toggle power during launch sequence");
        return;
    }

    MissileState currentState = missileStates[missileIndex];
    LOG_INFO(QString("Attempting to toggle missile %1, current state: %2").arg(missileIndex).arg(currentState));

    switch (currentState)
    {
    case Fired:
        emitError(QString("Cannot toggle power for missile %1: Missile has been fired").arg(missileIndex));
        return;
    case Healthy:
        if (currentlyPoweredMissile != -1 && currentlyPoweredMissile != missileIndex)
        {
            // Power off the currently powered missile
            missileStates[currentlyPoweredMissile] = Healthy;
            updateMissileStatus(currentlyPoweredMissile);
            LOG_INFO(QString("Missile %1 powered off due to new selection").arg(currentlyPoweredMissile));
        }
        // Power on the newly selected missile
        missileStates[missileIndex] = Powered;
        currentlyPoweredMissile = missileIndex;
        LOG_INFO(QString("Missile %1 powered on").arg(missileIndex));
        break;
    case Powered:
        // Power off the current missile
        missileStates[missileIndex] = Healthy;
        currentlyPoweredMissile = -1;
        LOG_INFO(QString("Missile %1 powered off").arg(missileIndex));
        break;
    case Unhealthy:
        emitError(QString("Cannot toggle power for missile %1: Missile is unhealthy").arg(missileIndex));
        return;
    }

    updateMissileStatus(missileIndex);
}

// Launch a missile if conditions are met
void AKSCore::launchMissile()
{
    if (!launcherConnected) // Check if the launcher is connected
    {
        emitError("Launcher communication is down. Cannot launch any missile!");         // Emit error if not connected
        LOG_WARNING("Attempted to launch missile while launcher communication is down"); // Log warning
        return;                                                                          // Exit
    }

    // Check if there is a powered missile available for launch
    if (currentlyPoweredMissile != -1 && missileStates[currentlyPoweredMissile] == Powered)
    {
        startLaunchSequence(currentlyPoweredMissile); // Start the launch sequence for the powered missile
    }
    else
    {
        emitError("No powered missile available for launch"); // Emit error if no powered missile
    }
}

// Update missile configuration based on index, type, and health status
void AKSCore::updateMissileConfig(int index, const QString &type, bool healthy)
{
    // Validate the missile index
    if (index >= 0 && index < 4)
    {
        // Ensure the vector has enough capacity
        if (missileConfigs.size() <= index)
        {
            missileConfigs.resize(index + 1); // Resize the missileConfigs vector if necessary
        }
        missileConfigs[index] = {index, type, healthy};                                                        // Update the missile configuration
        missileStates[index] = healthy ? Healthy : Unhealthy;                                                  // Update the missile state based on health
        updateMissileStatus(index);                                                                            // Update the missile status
        LOG_INFO(QString("Missile %1 config updated: Type %2, Healthy %3").arg(index).arg(type).arg(healthy)); // Log the update
    }
    else
    {
        emitError(QString("Invalid missile index for config update: %1").arg(index)); // Emit error for invalid index
    }
}

// Slot for handling ANS heartbeat reception
void AKSCore::onANSHeartbeatReceived()
{
    bool prevStatus = ansConnected;                 // Store previous connection status
    ansConnected = true;                            // Update connection status to true
    communicationManager->updateANSLastHeartbeat(); // Update the last heartbeat time
    if (!prevStatus)                                // Check if the status has changed
    {
        emit communicationStatusChanged(ansConnected, launcherConnected); // Emit signal for status change
        LOG_INFO("ANS connection established");                           // Log the connection establishment
    }
}

// Slot for handling Launcher heartbeat reception
void AKSCore::onLauncherHeartbeatReceived()
{
    bool prevStatus = launcherConnected;                 // Store previous connection status
    launcherConnected = true;                            // Update connection status to true
    communicationManager->updateLauncherLastHeartbeat(); // Update the last heartbeat time
    if (!prevStatus)                                     // Check if the status has changed
    {
        emit communicationStatusChanged(ansConnected, launcherConnected); // Emit signal for status change
        LOG_INFO("Launcher connection established");                      // Log the connection establishment
    }
}

// Update launcher communication status
void AKSCore::updateLauncherCommunicationStatus(bool connected)
{
    launcherConnected = connected;                                                                                // Update the launcher connection status
    emit communicationStatusChanged(ansConnected, launcherConnected);                                             // Emit signal for status change
    LOG_INFO(QString("Launcher communication status updated: %1").arg(connected ? "Connected" : "Disconnected")); // Log the update
}

// Slot for handling ANS communication loss
void AKSCore::onANSCommunicationLost()
{
    if (ansConnected) // Check if currently connected
    {
        ansConnected = false;                                             // Update connection status to false
        emit communicationStatusChanged(ansConnected, launcherConnected); // Emit signal for status change
        LOG_WARNING("ANS communication lost");                            // Log the communication loss
    }
}

// Slot for handling Launcher communication loss
void AKSCore::onLauncherCommunicationLost()
{
    if (launcherConnected) // Check if currently connected
    {
        launcherConnected = false;                                        // Update connection status to false
        emit communicationStatusChanged(ansConnected, launcherConnected); // Emit signal for status change
        LOG_WARNING("Launcher communication lost");                       // Log the communication loss
    }
}

// Slot for handling platform info reception
void AKSCore::onPlatformInfoReceived(double latitude, double longitude, double altitude)
{
    emit platformInfoUpdated(latitude, longitude, altitude);                                                         // Emit signal with platform info
    LOG_DEBUG(QString("Platform info received: Lat %1, Lon %2, Alt %3").arg(latitude).arg(longitude).arg(altitude)); // Log the received info
}

// Check the communication status of ANS and Launcher
void AKSCore::checkCommunicationStatus()
{
    bool prevANSStatus = ansConnected;           // Store previous ANS connection status
    bool prevLauncherStatus = launcherConnected; // Store previous Launcher connection status

    // Check if heartbeats have been received recently
    if (!communicationManager->hasRecentANSHeartbeat())
    {
        ansConnected = false; // Update ANS connection status
    }
    if (!communicationManager->hasRecentLauncherHeartbeat())
    {
        launcherConnected = false; // Update Launcher connection status
    }

    // Emit signal only if status has changed
    if (prevANSStatus != ansConnected || prevLauncherStatus != launcherConnected)
    {
        emit communicationStatusChanged(ansConnected, launcherConnected); // Emit signal for status change
        LOG_INFO(QString("Communication status changed - ANS: %1, Launcher: %2")
                     .arg(ansConnected ? "Connected" : "Disconnected")
                     .arg(launcherConnected ? "Connected" : "Disconnected")); // Log the status change
    }
}

// Update the status of a specific missile
void AKSCore::updateMissileStatus(int index)
{
    if (index >= 0 && index < missileStates.size()) // Validate index
    {
        MissileState state = missileStates[index];                                             // Get the state of the missile
        bool healthy = (state != Unhealthy);                                                   // Determine if the missile is healthy
        bool powered = (state == Powered);                                                     // Determine if the missile is powered
        bool fired = (state == Fired);                                                         // Determine if the missile has been fired
        emit missileStatusChanged(index, missileConfigs[index].type, healthy, powered, fired); // Emit signal with missile status
        LOG_INFO(QString("Missile %1 status updated: State %2").arg(index).arg(state));        // Log the status update
    }
}

// Start the launch sequence for a missile
void AKSCore::startLaunchSequence(int missileIndex)
{
    // Validate missile index and state
    if (missileIndex >= 0 && missileIndex < 4 && missileStates[missileIndex] == Powered)
    {
        launchingMissileIndex = missileIndex;                                                   // Set the launching missile index
        launchSequenceTimer = new QTimer(this);                                                 // Create a new timer for the launch sequence
        connect(launchSequenceTimer, &QTimer::timeout, this, &AKSCore::continueLaunchSequence); // Connect timer to continueLaunchSequence
        launchSequenceTimer->start(2000);                                                       // Start the timer with a 2-second interval
        LOG_INFO(QString("Launch sequence started for missile %1").arg(missileIndex));          // Log the launch sequence start
    }
    else
    {
        emitError(QString("Cannot start launch sequence for missile %1").arg(missileIndex)); // Emit error if conditions are not met
    }
}

// Continue the launch sequence
void AKSCore::continueLaunchSequence()
{
    static int sequenceStep = 0; // Static variable to track the current step in the sequence

    switch (sequenceStep) // Handle each step of the launch sequence
    {
    case 0:
        LOG_INFO("Launch sequence: Initializing guidance system"); // Log step
        break;
    case 1:
        LOG_INFO("Launch sequence: Activating propulsion system"); // Log step
        break;
    case 2:
        LOG_INFO("Launch sequence: Conducting final system check"); // Log step
        break;
    case 3:
        LOG_INFO("Launch sequence: Initiating countdown"); // Log step
        break;
    case 4:
        LOG_INFO("Launch sequence: Missile launched!"); // Log launch
        sequenceStep = 0;                               // Reset sequence step
        finalizeLaunch();                               // Finalize the launch
        return;                                         // Exit
    }

    sequenceStep++; // Move to the next step
}

// Finalize the launch of a missile
void AKSCore::finalizeLaunch()
{
    missileStates[launchingMissileIndex] = Fired; // Set the missile state to Fired
    updateMissileStatus(launchingMissileIndex);   // Update the missile status
    launchSequenceTimer->stop();                  // Stop the launch sequence timer
    delete launchSequenceTimer;                   // Delete the timer
    launchSequenceTimer = nullptr;                // Reset the timer pointer
    launchingMissileIndex = -1;                   // Reset the launching missile index
}

// Emit an error message
void AKSCore::emitError(const QString &message)
{
    LOG_ERROR(message);          // Log the error message
    emit errorOccurred(message); // Emit signal with the error message
}
