#include "AKSGUI.h"
#include "ClickableMissileWidget.h"
#include "ui_mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QDoubleValidator>

// Constructor for AKSGUI
AKSGUI::AKSGUI(QWidget *parent) : QWidget(parent), ui(new Ui::MainWindow), launcherConnected(false)
{
    setupUi();             // Initialize the user interface
    setupMissileWidgets(); // Setup missile widgets
    connectSignals();      // Connect signals and slots
}

// Destructor for AKSGUI
AKSGUI::~AKSGUI()
{
    delete ui; // Clean up the UI object
}

// Setup the user interface components
void AKSGUI::setupUi()
{
    // Setup platform info group
    QGroupBox *platformGroup = new QGroupBox("Platform Information", this);
    QGridLayout *platformLayout = new QGridLayout(platformGroup);
    QString platformLabels[] = {"Latitude:", "Longitude:", "Altitude:"}; // Labels for platform info
    for (int i = 0; i < 3; ++i)
    {
        platformInfoLabels[i] = new QLabel(platformLabels[i], platformGroup); // Create label
        platformInfoValues[i] = new QLineEdit(platformGroup);                 // Create read-only line edit
        platformInfoValues[i]->setReadOnly(true);                             // Set line edit to read-only
        platformLayout->addWidget(platformInfoLabels[i], i, 0);               // Add label to layout
        platformLayout->addWidget(platformInfoValues[i], i, 1);               // Add line edit to layout
    }

    // Setup target info group
    QGroupBox *targetGroup = new QGroupBox("Target Information", this);
    QGridLayout *targetLayout = new QGridLayout(targetGroup);
    QString targetLabels[] = {"Latitude:", "Longitude:", "Altitude:", "Speed:", "Distance:"}; // Labels for target info
    for (int i = 0; i < 5; ++i)
    {
        targetInfoLabels[i] = new QLabel(targetLabels[i], targetGroup);       // Create label
        targetInfoValues[i] = new QLineEdit(targetGroup);                     // Create line edit for target info
        targetInfoValues[i]->setValidator(new QDoubleValidator(targetGroup)); // Set validator for double values
        targetLayout->addWidget(targetInfoLabels[i], i, 0);                   // Add label to layout
        targetLayout->addWidget(targetInfoValues[i], i, 1);                   // Add line edit to layout
    }
    updateTargetButton = new QPushButton("Update Target", targetGroup); // Create button to update target
    targetLayout->addWidget(updateTargetButton, 5, 0, 1, 2);            // Add button to layout

    // Setup communication status group
    QGroupBox *commGroup = new QGroupBox("Communication Status", this);
    QHBoxLayout *commLayout = new QHBoxLayout(commGroup);
    QString commLabels[] = {"ANS:", "Launcher:"}; // Labels for communication status
    for (int i = 0; i < 2; ++i)
    {
        QLabel *label = new QLabel(commLabels[i], commGroup);                                 // Create label for communication status
        communicationStatus[i] = new QWidget(commGroup);                                      // Create widget to show status
        communicationStatus[i]->setFixedSize(20, 20);                                         // Set fixed size for status indicator
        communicationStatus[i]->setStyleSheet("background-color: red; border-radius: 10px;"); // Set initial style
        commLayout->addWidget(label);                                                         // Add label to layout
        commLayout->addWidget(communicationStatus[i]);                                        // Add status indicator to layout
        commLayout->addSpacing(10);                                                           // Add spacing between items
    }

    // Setup missile status group
    missileGroup = new QGroupBox("LAUNCHER XYZ", this); // Create missile group box
    missileLayout = new QHBoxLayout(missileGroup);      // Create horizontal layout for missiles

    // Setup control buttons
    launchMissileButton = new QPushButton("Launch Missile", this); // Create launch button
    launchMissileButton->setEnabled(false);                        // Initially disable the launch button

    // Main layout for the GUI
    QVBoxLayout *mainLayout = new QVBoxLayout(this); // Create main vertical layout
    mainLayout->addWidget(platformGroup);            // Add platform info group
    mainLayout->addWidget(targetGroup);              // Add target info group
    mainLayout->addWidget(commGroup);                // Add communication status group
    mainLayout->addWidget(missileGroup);             // Add missile status group
    mainLayout->addWidget(launchMissileButton);      // Add launch button

    setLayout(mainLayout); // Set the main layout for the widget
}

// Setup missile widgets
void AKSGUI::setupMissileWidgets()
{
    for (int i = 0; i < 4; ++i)
    {                                                                             // Loop to create missile widgets
        ClickableMissileWidget *missileWidget = new ClickableMissileWidget(this); // Create missile widget
        missileWidgets.append(missileWidget);                                     // Add widget to the list

        // Connect the clicked signal to the handler
        connect(missileWidget, &ClickableMissileWidget::clicked, [this, i]()
                {
                    this->onMissileWidgetClicked(i); // Handle missile widget click
                });

        // Add the widget to the missile layout
        missileLayout->addWidget(missileWidget); // Add missile widget to layout
    }
}

// Connect signals to their respective slots
void AKSGUI::connectSignals()
{
    connect(updateTargetButton, &QPushButton::clicked, this, &AKSGUI::onUpdateTargetClicked);  // Connect update button
    connect(launchMissileButton, &QPushButton::clicked, this, &AKSGUI::onLaunchButtonClicked); // Connect launch button
}

// Handle missile widget click events
void AKSGUI::onMissileWidgetClicked(int index)
{
    if (index >= 0 && index < missileWidgets.size())
    { // Check if index is valid
        if (!missileWidgets[index]->isFired())
        {                                                                                            // Check if missile is not fired
            qDebug() << QString("GUI: Missile %1 clicked, emitting powerToggled signal").arg(index); // Log click event
            emit powerToggled(index);                                                                // Emit signal to toggle power
        }
        else
        {
            qDebug() << QString("GUI: Clicked missile %1 is fired, ignoring").arg(index); // Log that missile is fired
        }
    }
}

// Update platform information display
void AKSGUI::updatePlatformInfo(double latitude, double longitude, double altitude)
{
    platformInfoValues[0]->setText(QString::number(latitude, 'f', 6));  // Update latitude display
    platformInfoValues[1]->setText(QString::number(longitude, 'f', 6)); // Update longitude display
    platformInfoValues[2]->setText(QString::number(altitude, 'f', 2));  // Update altitude display
}

// Update communication status indicators
void AKSGUI::updateCommunicationStatus(bool ansStatus, bool launcherStatus)
{
    // Update ANS status indicator
    communicationStatus[0]->setStyleSheet(ansStatus ? "background-color: green; border-radius: 10px;" : "background-color: red; border-radius: 10px;");
    // Update Launcher status indicator
    communicationStatus[1]->setStyleSheet(launcherStatus ? "background-color: green; border-radius: 10px;" : "background-color: red; border-radius: 10px;");

    launcherConnected = launcherStatus; // Update launcher connection status
    updateLaunchButtonState();          // Update launch button state based on connection
}

// Update the state of the launch button
void AKSGUI::updateLaunchButtonState()
{
    bool canLaunch = launcherConnected;         // Determine if launch is possible
    launchMissileButton->setEnabled(canLaunch); // Enable or disable the launch button

    // Set tooltip for the launch button based on connection status
    if (!launcherConnected)
    {
        launchMissileButton->setToolTip("Launcher communication is down. Cannot launch missiles.");
    }
    else
    {
        launchMissileButton->setToolTip("Launch the powered missile.");
    }
}

// Update missile status based on received information
void AKSGUI::updateMissileStatus(int index, const QString &type, bool healthy, bool powered, bool fired)
{
    if (index < 0 || index >= missileWidgets.size())
    {
        emit errorOccurred(QString("Invalid missile index: %1").arg(index));
        return;
    }

    ClickableMissileWidget *widget = missileWidgets[index];

    // If the missile was previously fired, it should not change state
    if (widget->isFired() && !fired)
    {
        return;
    }

    widget->setMissileInfo(type, healthy, powered, fired);

    QString stateStr = fired ? "Fired" : (powered ? "Powered" : (healthy ? "Healthy" : "Unhealthy"));

    // Update other widgets if this missile is now powered
    if (powered)
    {
        for (int i = 0; i < missileWidgets.size(); ++i)
        {
            if (i != index && missileWidgets[i]->isPowered())
            {
                // missileWidgets[i]->setMissileInfo(missileWidgets[i]->getType(), true, false, false);
            }
        }
    }

    // Determine if a missile can be launched
    bool canLaunch = !type.trimmed().isEmpty() && healthy && powered && !fired;
    launchMissileButton->setEnabled(canLaunch);

    updateLaunchButtonState();
}

// Handle target update button click
void AKSGUI::onUpdateTargetClicked()
{
    double latitude = targetInfoValues[0]->text().toDouble();  // Get latitude from input
    double longitude = targetInfoValues[1]->text().toDouble(); // Get longitude from input
    double altitude = targetInfoValues[2]->text().toDouble();  // Get altitude from input
    double speed = targetInfoValues[3]->text().toDouble();     // Get speed from input
    double distance = targetInfoValues[4]->text().toDouble();  // Get distance from input

    emit targetInfoUpdated(latitude, longitude, altitude, speed, distance); // Emit signal with target info
}

// Handle power toggle button click
void AKSGUI::onPowerToggleClicked()
{
    for (int i = 0; i < missileWidgets.size(); ++i)
    { // Loop through missile widgets
        if (missileWidgets[i]->property("powered").toBool())
        {                         // Check if missile is powered
            emit powerToggled(i); // Emit signal to toggle power
            return;               // Exit after toggling
        }
    }
    emit errorOccurred("No powered missile available to toggle."); // Emit error if no powered missile
}

// Handle launch button click
void AKSGUI::onLaunchButtonClicked()
{
    QMessageBox::StandardButton reply; // Variable to hold the user's response
    reply = QMessageBox::question(this, "Confirm Launch", "Are you sure you want to launch the missile?",
                                  QMessageBox::Yes | QMessageBox::No); // Show confirmation dialog
    if (reply == QMessageBox::Yes)
    {                           // Check if user confirmed
        emit missileLaunched(); // Emit signal to indicate missile launch
    }
}
