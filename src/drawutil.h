#ifndef DRAWUTIL_H
#define DRAWUTIL_H
#include <QPainter>
#include <QBrush>
#include <QPalette>

void drawBeButton(QPainter *p,int left, int top, int width, int height,
                  const QPalette &g, bool sunken,
                  const QBrush *fill );

void colorBitmaps(QPainter *p, const QPalette &pal, int left, int top,
                   QBitmap *lightColor=nullptr, QBitmap *midColor=nullptr,
                   QBitmap *midlightColor=nullptr, QBitmap *darkColor=nullptr,
                   QBitmap *blackColor=nullptr, QBitmap *whiteColor=nullptr);

void colorBitmaps(QPainter *p, const QPalette pal, int left, int top, int width,
                   int height, bool isXBitmaps=true, const uchar *lightColor = nullptr,
                   const uchar *midColor=nullptr, const uchar *midlightColor=nullptr,
                   const uchar *darkColor=nullptr, const uchar *blackColor=nullptr,
                   const uchar *whiteColor=nullptr);

#endif // DRAWUTIL_H
