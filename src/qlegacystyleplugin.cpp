#include "qlegacystyleplugin.h"
#include "qlegacystyle.h"

  QStyle *QLegacyStylePlugin::create(const QString &key)
  {
      QString lcKey = key.toLower();
      if (lcKey == "highcolor") {
          return new QLegacyStyle(QLegacyStyle::HighColor);
      } else if (lcKey == "b3") {
          return new QLegacyStyle(QLegacyStyle::B3);
      } else {
          return new QLegacyStyle(QLegacyStyle::Default);
        }
      return 0;
  }

  QLegacyStylePlugin::QLegacyStylePlugin(QObject *parent) : QStylePlugin(parent)
  {

  }


