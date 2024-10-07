#include "AKSApp.h"
#include "Logger.h"
#include <QMessageBox>
#include <QApplication>
#include <QDir>

AKSApp::AKSApp(QWidget *parent)
    : QMainWindow(parent), gui(new AKSGUI(this)), core(new AKSCore(this)), configReader(new ConfigReader(this))
{
    // Log the start of the application
    LOG_INFO("AKS Application starting");
    setCentralWidget(gui); // Set the central widget to the GUI
    setupConnections();    // Setup signal-slot connections
    loadMissileConfig();   // Load missile configuration

    core->start(); // Start the core functionality
    LOG_INFO("AKS Application started successfully");
}

AKSApp::~AKSApp()
{
    // Log the shutdown of the application
    LOG_INFO("AKS Application shutting down");
    core->stop(); // Stop the core functionality
}

void AKSApp::setupConnections()
{
    // Connect GUI signals to Core slots
    connect(gui, &AKSGUI::targetInfoUpdated, core, &AKSCore::updateTargetInfo, Qt::UniqueConnection);
    connect(gui, &AKSGUI::missileLaunched, core, &AKSCore::launchMissile, Qt::UniqueConnection);
    connect(gui, &AKSGUI::powerToggled, core, &AKSCore::toggleMissilePower, Qt::UniqueConnection);

    // Connect Core signals to GUI slots
    connect(core, &AKSCore::platformInfoUpdated, gui, &AKSGUI::updatePlatformInfo, Qt::UniqueConnection);
    connect(core, &AKSCore::communicationStatusChanged, gui, &AKSGUI::updateCommunicationStatus, Qt::UniqueConnection);
    connect(core, &AKSCore::missileStatusChanged, gui, &AKSGUI::updateMissileStatus, Qt::UniqueConnection);

    // Connect Core signals to AKSApp slots
    connect(core, &AKSCore::platformInfoUpdated, this, &AKSApp::onPlatformInfoUpdated, Qt::UniqueConnection);
    connect(core, &AKSCore::communicationStatusChanged, this, &AKSApp::onCommunicationStatusChanged, Qt::UniqueConnection);
    connect(core, &AKSCore::missileStatusChanged, this, &AKSApp::onMissileStatusChanged, Qt::UniqueConnection);

    // Connect GUI signals to AKSApp slots
    connect(gui, &AKSGUI::targetInfoUpdated, this, &AKSApp::onTargetInfoUpdated, Qt::UniqueConnection);
    connect(gui, &AKSGUI::powerToggled, this, &AKSApp::onPowerToggled, Qt::UniqueConnection);
    connect(gui, &AKSGUI::missileLaunched, this, &AKSApp::onMissileLaunched, Qt::UniqueConnection);

    // Connect CommunicationManager signals to GUI slots
    connect(core, &AKSCore::communicationStatusChanged, gui, &AKSGUI::updateCommunicationStatus, Qt::UniqueConnection);

    // Connect error handling slots
    connect(core, &AKSCore::errorOccurred, this, &AKSApp::handleError, Qt::UniqueConnection);
    connect(gui, &AKSGUI::errorOccurred, this, &AKSApp::handleError, Qt::UniqueConnection);
}

void AKSApp::loadMissileConfig()
{
    // Construct the path to the missile configuration file
    QString configPath = QApplication::applicationDirPath() + QDir::separator() + "config" + QDir::separator() + "missiles.conf";
    if (configReader->loadMissileConfig(configPath)) // Load the missile configuration
    {
        QVector<MissileConfig> configs = configReader->getMissileConfigs(); // Get the loaded missile configurations
        for (const auto &config : configs)                                  // Iterate through each missile configuration
        {
            gui->updateMissileStatus(config.index, config.type, config.healthy, false, false); // Update GUI with missile status
            qDebug() << "Missile index: " << config.index << "Missile type: " << config.type << "Missile healthy: " << config.healthy;
            core->updateMissileConfig(config.index, config.type, config.healthy); // Update core with missile configuration
        }
        LOG_INFO("Missile configuration loaded successfully"); // Log successful loading
    }
    else
    {
        // Log and display an error message if loading fails
        QString errorMsg = "Failed to load missile configuration: " + configReader->getLastError();
        LOG_ERROR(errorMsg);
        QMessageBox::critical(this, "Configuration Error", errorMsg);
    }
}

// Method to handle updates to target information
void AKSApp::onTargetInfoUpdated(double latitude, double longitude, double altitude, double speed, double distance)
{
    // LOG_INFO(QString("Target info updated: Lat %1, Lon %2, Alt %3, Speed %4, Distance %5")
    //     .arg(latitude).arg(longitude).arg(altitude).arg(speed).arg(distance));
}

// Method to handle toggling missile power
void AKSApp::onPowerToggled(int missileIndex)
{
    // LOG_INFO(QString("Missile power toggled for index: %1").arg(missileIndex));
}

// Method to handle missile launch events
void AKSApp::onMissileLaunched()
{
    LOG_INFO("Missile launch process started.");
}

// Method to handle updates to platform information
void AKSApp::onPlatformInfoUpdated(double latitude, double longitude, double altitude)
{
    // LOG_INFO(QString("Platform info updated: Lat %1, Lon %2, Alt %3")
    //     .arg(latitude).arg(longitude).arg(altitude));
}

// Method to handle changes in communication status
void AKSApp::onCommunicationStatusChanged(bool ansStatus, bool launcherStatus)
{
    LOG_INFO(QString("Communication status changed - ANS: %1, Launcher: %2")
                 .arg(ansStatus ? "Connected" : "Disconnected")
                 .arg(launcherStatus ? "Connected" : "Disconnected"));
}

// Method to handle changes in missile status
void AKSApp::onMissileStatusChanged(int index, const QString &type, bool healthy, bool powered, bool fired)
{
    LOG_INFO(QString("Missile status changed - Index: %1, Type: %2, Healthy: %3, Powered: %4, Fired: %5")
                 .arg(index)
                 .arg(type)
                 .arg(healthy)
                 .arg(powered)
                 .arg(fired));
}

// Add a new method to handle application errors
void AKSApp::handleError(const QString &message)
{
    LOG_ERROR(message);                            // Log the error message
    QMessageBox::critical(this, "Error", message); // Display an error message box
}
