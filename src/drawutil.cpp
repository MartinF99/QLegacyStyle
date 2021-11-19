#include <QBitmap>
#include <QPainter>
#include <QPalette>
#include <QBrush>
#include "drawutil.h"

void drawBeButton(QPainter *p,int left, int top, int width, int height,
                  const QPalette &g, bool sunken,
                  const QBrush *fill)
{
    QPen oldPen = p->pen();
    int x2 = left+width-1;
    int y2 = top+height-1;
    p->setPen(g.dark().color());
    p->drawLine(left+1, top, x2-1, top);
    p->drawLine(left, top+1, left, y2-1);
    p->drawLine(left+1, y2, x2-1, y2);
    p->drawLine(x2, top+1, x2, y2-1);


    if(!sunken){
        p->setPen(g.light().color());
        p->drawLine(left+2, top+2, x2-1, top+2);
        p->drawLine(left+2, top+3, x2-2, top+3);
        p->drawLine(left+2, top+4, left+2, y2-1);
        p->drawLine(left+3, top+4, left+3, y2-2);
    }
    else{
        p->setPen(g.mid().color());
        p->drawLine(left+2, top+2, x2-1, top+2);
        p->drawLine(left+2, top+3, x2-2, top+3);
        p->drawLine(left+2, top+4, left+2, y2-1);
        p->drawLine(left+3, top+4, left+3, y2-2);
    }


    p->setPen(sunken? g.light().color() : g.mid().color());
    p->drawLine(x2-1, top+2, x2-1, y2-1);
    p->drawLine(left+2, y2-1, x2-1, y2-1);

    p->setPen(g.mid().color());
    p->drawLine(left+1, top+1, x2-1, top+1);
    p->drawLine(left+1, top+2, left+1, y2-1);
    p->drawLine(x2-2, top+3, x2-2, y2-2);

    if(fill)
        p->fillRect(left+4, top+4, width-6, height-6, *fill);

    p->setPen(oldPen);
}

void colorBitmaps(QPainter *p, const QPalette &pal, int left, int top,
                   QBitmap *lightColor, QBitmap *midColor,
                   QBitmap *midlightColor, QBitmap *darkColor,
                   QBitmap *blackColor, QBitmap *whiteColor )
{
    QBitmap *bitmaps[]={lightColor, midColor, midlightColor, darkColor,
            blackColor, whiteColor};

        QColor colors[]={pal.light().color(), pal.mid().color(), pal.midlight().color(), pal.dark().color(),
            Qt::black, Qt::white};

        int i;
        for(i=0; i < 6; ++i){
            if(bitmaps[i]){
                if(!bitmaps[i]->mask())
                    bitmaps[i]->setMask(*bitmaps[i]);
                p->setPen(colors[i]);
                p->drawPixmap(left, top, *bitmaps[i]);
            }
        }
}

void colorBitmaps(QPainter *p, const QPalette pal, int left, int top, int width,
                   int height, bool isXBitmaps, const uchar *lightColor,
                   const uchar *midColor, const uchar *midlightColor,
                   const uchar *darkColor, const uchar *blackColor,
                   const uchar *whiteColor)
{
    const uchar *data[]={lightColor, midColor, midlightColor, darkColor,
        blackColor, whiteColor};

    QColor colors[]={pal.light().color(), pal.mid().color(), pal.midlight().color(), pal.dark().color(),
        Qt::black, Qt::white};

    int i;
    QBitmap b;
    QImage::Format f = (isXBitmaps)?QImage::Format::Format_MonoLSB:QImage::Format::Format_Mono;
    for(i=0; i < 6; ++i){
        if(data[i]){
            b = QBitmap::fromData(QSize(width, height), data[i], f );
            b.setMask(b);
            p->setPen(colors[i]);
            p->drawPixmap(left, top, b);
        }
    }
}
