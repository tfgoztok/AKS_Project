#include "ClickableMissileWidget.h"
#include <QVBoxLayout>

// Constructor for ClickableMissileWidget
ClickableMissileWidget::ClickableMissileWidget(QWidget *parent)
    : QWidget(parent), m_healthy(false), m_powered(false), m_fired(false)
{
    setFixedSize(60, 60); // Set the fixed size of the widget

    m_label = new QLabel(this);             // Create a label to display missile type
    m_label->setAlignment(Qt::AlignCenter); // Center the label text

    QVBoxLayout *layout = new QVBoxLayout(this); // Create a vertical layout
    layout->addWidget(m_label);                  // Add the label to the layout

    updateStyle(); // Update the style of the widget
}

// Set missile information
void ClickableMissileWidget::setMissileInfo(const QString &type, bool healthy, bool powered, bool fired)
{
    m_type = type; // Store the missile type

    // Ensure that once a missile is fired, it remains in that state.
    if (!m_fired)
    {
        m_healthy = healthy; // Update health status if not fired
        m_powered = powered; // Update power status if not fired
    }

    m_fired = fired;                  // Update fired status
    m_label->setText(type.trimmed()); // Set the label text to the missile type
    updateStyle();                    // Update the style based on the new state
    update();                         // Request a repaint of the widget
}

// Update the style of the widget based on its state
void ClickableMissileWidget::updateStyle()
{
    // Define the default style for the label
    QString styleSheet = "QLabel { color: black; background-color: transparent; }";
    m_label->setStyleSheet(styleSheet); // Apply the style to the label
}

// Handle mouse press events
void ClickableMissileWidget::mousePressEvent(QMouseEvent *event)
{
    // Check if the left mouse button was pressed and the missile has not been fired
    if (event->button() == Qt::LeftButton && !m_fired)
    {
        emit clicked();                  // Emit the clicked signal
        QWidget::mousePressEvent(event); // Call the base class implementation
    }
}

// Handle paint events to draw the missile widget
void ClickableMissileWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);                               // Ignore the event parameter
    QPainter painter(this);                        // Create a painter for the widget
    painter.setRenderHint(QPainter::Antialiasing); // Enable antialiasing for smoother edges

    QColor fillColor;               // Variable to hold the fill color
    QColor borderColor = Qt::black; // Default border color

    // Determine the fill color based on the missile's state
    if (m_type.trimmed().isEmpty())
    {
        fillColor = Qt::transparent; // No type means transparent
    }
    else if (m_fired)
    {
        fillColor = Qt::gray; // Fired missiles are gray
    }
    else if (!m_healthy)
    {
        fillColor = Qt::red; // Unhealthy missiles are red
    }
    else if (m_powered)
    {
        fillColor = Qt::green;    // Powered missiles are green
        borderColor = Qt::yellow; // Border for powered missiles is yellow
    }
    else
    {
        fillColor = Qt::green; // Default color for healthy missiles
    }

    painter.setPen(QPen(borderColor, 3));               // Set the pen for the border
    painter.setBrush(fillColor);                        // Set the brush for the fill color
    painter.drawEllipse(rect().adjusted(2, 2, -2, -2)); // Draw the missile as an ellipse
}
