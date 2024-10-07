#ifndef CLICKABLEMISSILEWIDGET_H
#define CLICKABLEMISSILEWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>

// Class representing a clickable missile widget
class ClickableMissileWidget : public QWidget
{
    Q_OBJECT

public:
    // Constructor for ClickableMissileWidget
    explicit ClickableMissileWidget(QWidget *parent = nullptr);

    // Set the missile information
    void setMissileInfo(const QString &type, bool healthy, bool powered, bool fired);

    // Check if the missile is powered
    bool isPowered() const { return m_powered; }

    // Check if the missile is fired
    bool isFired() const { return m_fired; }

signals:
    // Signal emitted when the widget is clicked
    void clicked();

protected:
    // Handle mouse press events
    void mousePressEvent(QMouseEvent *event) override;

    // Handle paint events
    void paintEvent(QPaintEvent *event) override;

private:
    QLabel *m_label; // Label to display missile information
    QString m_type;  // Type of the missile
    bool m_healthy;  // Health status of the missile
    bool m_powered;  // Power status of the missile
    bool m_fired;    // Fired status of the missile

    // Update the style of the widget
    void updateStyle();
};

#endif
