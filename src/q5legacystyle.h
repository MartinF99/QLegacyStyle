#ifndef Q5LEGACYSTYLE_H
#define Q5LEGACYSTYLE_H

#include <QMainWindow>
#include <QScopedPointer>

namespace Ui {
class Q5LegacyStyle;
}

class Q5LegacyStyle : public QMainWindow
{
    Q_OBJECT

public:
    explicit Q5LegacyStyle(QWidget *parent = nullptr);
    ~Q5LegacyStyle() override;

private:
    QScopedPointer<Ui::Q5LegacyStyle> m_ui;
};

#endif // Q5LEGACYSTYLE_H
