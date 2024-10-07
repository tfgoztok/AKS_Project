#include "CommunicationManager.h"
#include "Logger.h"
#include "GlobalConstants.h"
#include <QNetworkDatagram>

// Constructor for CommunicationManager
CommunicationManager::CommunicationManager(QObject *parent)
    : QObject(parent), socket(new QUdpSocket(this)) // Initialize the UDP socket
      ,
      heartbeatTimer(new QTimer(this)) // Initialize the heartbeat timer
      ,
      communicationCheckTimer(new QTimer(this)) // Initialize the communication check timer
      ,
      ansMissedHeartbeats(0) // Initialize missed heartbeats counter for ANS
      ,
      launcherMissedHeartbeats(0) // Initialize missed heartbeats counter for Launcher
      ,
      lastANSHeartbeat(QDateTime::currentDateTime()) // Record the last ANS heartbeat time
      ,
      lastLauncherHeartbeat(QDateTime::currentDateTime()) // Record the last Launcher heartbeat time
{
    // Connect socket's readyRead signal to the readPendingDatagrams slot
    connect(socket, &QUdpSocket::readyRead, this, &CommunicationManager::readPendingDatagrams);
    // Connect heartbeat timer's timeout signal to the sendHeartbeat slot
    connect(heartbeatTimer, &QTimer::timeout, this, &CommunicationManager::sendHeartbeat);
    // Connect communication check timer's timeout signal to the checkCommunicationStatus slot
    connect(communicationCheckTimer, &QTimer::timeout, this, &CommunicationManager::checkCommunicationStatus);
}

// Destructor for CommunicationManager
CommunicationManager::~CommunicationManager()
{
    stop(); // Stop all operations
}

// Start the communication manager
void CommunicationManager::start()
{
    // Attempt to bind the socket to the specified port
    if (socket->bind(QHostAddress::LocalHost, AKS_PORT))
    {
        LOG_INFO(QString("CommunicationManager: Bound to port %1").arg(AKS_PORT));        // Log successful binding
        heartbeatTimer->start(GlobalConstants::HEARTBEAT_INTERVAL_MS);                    // Start the heartbeat timer with a 1-second interval
        communicationCheckTimer->start(GlobalConstants::COMMUNICATION_CHECK_INTERVAL_MS); // Start the communication check timer with a 1-second interval
    }
    else
    {
        LOG_ERROR(QString("CommunicationManager: Failed to bind to port %1").arg(AKS_PORT)); // Log binding failure
    }
}

// Stop the communication manager
void CommunicationManager::stop()
{
    heartbeatTimer->stop();          // Stop the heartbeat timer
    communicationCheckTimer->stop(); // Stop the communication check timer
    socket->close();                 // Close the UDP socket
}

// Send a heartbeat message
void CommunicationManager::sendHeartbeat()
{
    QByteArray heartbeat = "AKS_HEARTBEAT"; // Define the heartbeat message
    // Send the heartbeat message to both ANS and Launcher ports
    socket->writeDatagram(heartbeat, QHostAddress::LocalHost, ANS_PORT);
    socket->writeDatagram(heartbeat, QHostAddress::LocalHost, LAUNCHER_PORT);
}

// Read pending datagrams from the socket
void CommunicationManager::readPendingDatagrams()
{
    // Process all pending datagrams
    while (socket->hasPendingDatagrams())
    {
        QNetworkDatagram datagram = socket->receiveDatagram(); // Receive the datagram
        QByteArray data = datagram.data();                     // Extract the data from the datagram
        quint16 senderPort = datagram.senderPort();            // Get the sender's port

        // Check if the sender is ANS
        if (senderPort == ANS_PORT)
        {
            if (data == "ANS_HEARTBEAT")
            {
                processANSHeartbeat(); // Process ANS heartbeat
            }
            else
            {
                processPlatformInfo(data); // Process platform info
            }
        }
        // Check if the sender is Launcher
        else if (senderPort == LAUNCHER_PORT)
        {
            if (data == "LAUNCHER_HEARTBEAT")
            {
                processLauncherHeartbeat(); // Process Launcher heartbeat
            }
        }
    }
}

// Process ANS heartbeat
void CommunicationManager::processANSHeartbeat()
{
    lastANSHeartbeat = QDateTime::currentDateTime(); // Update the last ANS heartbeat time
    ansMissedHeartbeats = 0;                         // Reset missed heartbeats counter for ANS
    emit ansHeartbeatReceived();                     // Emit signal that ANS heartbeat was received
}

// Process Launcher heartbeat
void CommunicationManager::processLauncherHeartbeat()
{
    lastLauncherHeartbeat = QDateTime::currentDateTime(); // Update the last Launcher heartbeat time
    launcherMissedHeartbeats = 0;                         // Reset missed heartbeats counter for Launcher
    emit launcherHeartbeatReceived();                     // Emit signal that Launcher heartbeat was received
}

// Update the last ANS heartbeat time
void CommunicationManager::updateANSLastHeartbeat()
{
    lastANSHeartbeat = QDateTime::currentDateTime(); // Update the last ANS heartbeat time
}

// Update the last Launcher heartbeat time
void CommunicationManager::updateLauncherLastHeartbeat()
{
    lastLauncherHeartbeat = QDateTime::currentDateTime(); // Update the last Launcher heartbeat time
}

// Check if there has been a recent ANS heartbeat
bool CommunicationManager::hasRecentANSHeartbeat() const
{
    return lastANSHeartbeat.msecsTo(QDateTime::currentDateTime()) < GlobalConstants::HEARTBEAT_TIMEOUT_MS; // Return true if recent
}

// Check if there has been a recent Launcher heartbeat
bool CommunicationManager::hasRecentLauncherHeartbeat() const
{
    return lastLauncherHeartbeat.msecsTo(QDateTime::currentDateTime()) < GlobalConstants::HEARTBEAT_TIMEOUT_MS; // Return true if recent
}

// Process platform information received from datagrams
void CommunicationManager::processPlatformInfo(const QByteArray &data)
{
    QList<QByteArray> parts = data.split(','); // Split the data into parts
    // Check if the data format is correct
    if (parts.size() == 4 && parts[0] == "PLATFORM_INFO")
    {
        double latitude = parts[1].toDouble();                    // Extract latitude
        double longitude = parts[2].toDouble();                   // Extract longitude
        double altitude = parts[3].toDouble();                    // Extract altitude
        emit platformInfoReceived(latitude, longitude, altitude); // Emit signal with platform info
    }
}

// Check the communication status with ANS and Launcher
void CommunicationManager::checkCommunicationStatus()
{
    QDateTime currentTime = QDateTime::currentDateTime(); // Get the current time

    // Check if the last ANS heartbeat is recent
    if (lastANSHeartbeat.msecsTo(currentTime) > GlobalConstants::HEARTBEAT_INTERVAL_MS)
    {
        ansMissedHeartbeats++; // Increment missed heartbeats counter for ANS
        if (ansMissedHeartbeats >= 5)
        {
            emit ansCommunicationLost(); // Emit signal if communication is lost
        }
    }

    // Check if the last Launcher heartbeat is recent
    if (lastLauncherHeartbeat.msecsTo(currentTime) > GlobalConstants::HEARTBEAT_INTERVAL_MS)
    {
        launcherMissedHeartbeats++; // Increment missed heartbeats counter for Launcher
        if (launcherMissedHeartbeats >= 5)
        {
            emit launcherCommunicationLost(); // Emit signal if communication is lost
        }
    }
}
