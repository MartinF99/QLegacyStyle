#include "q5legacystyle.h"
#include "ui_q5legacystyle.h"

Q5LegacyStyle::Q5LegacyStyle(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::Q5LegacyStyle)
{
    m_ui->setupUi(this);
}

Q5LegacyStyle::~Q5LegacyStyle() = default;
