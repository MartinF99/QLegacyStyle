#ifndef QLEGACYSTYLEPLUGIN_H
#define QLEGACYSTYLEPLUGIN_H

#include <QStylePlugin>

class QLegacyStylePlugin : public QStylePlugin
{
  Q_OBJECT
      Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QStyleFactoryInterface" FILE "qlegacystyle.json")
  public:
      QLegacyStylePlugin(QObject *parent = 0);

      QStyle *create(const QString &key) override;
};

#endif // QLEGACYSTYLEPLUGIN_H
