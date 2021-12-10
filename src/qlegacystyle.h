#ifndef Q5LEGACYSTYLE_H
#define Q5LEGACYSTYLE_H

#include <QCommonStyle>
#include <QBitmap>
#include <QPixmap>

    
    
    class QLegacyStyle : public QCommonStyle
    {
        Q_OBJECT
        
    public:
        enum StyleType { HighColor = 0, Default, B3 };
        explicit QLegacyStyle(StyleType);
        
        virtual ~QLegacyStyle() override;
        //public function overrides from QCommonStyle
        
        void drawPrimitive(QStyle::PrimitiveElement pe, const QStyleOption * opt, QPainter * p, const QWidget * w) const override;
        
        void drawControl(QStyle::ControlElement element, const QStyleOption * opt, QPainter * p, const QWidget * w) const override;
        
        void drawComplexControl(QStyle::ComplexControl cc, const QStyleOptionComplex * opt, QPainter * p, const QWidget * widget) const override;
        
        QStyle::SubControl hitTestComplexControl(QStyle::ComplexControl cc, const QStyleOptionComplex * opt, const QPoint & pt, const QWidget * widget) const override;
        
        QSize sizeFromContents(QStyle::ContentsType ct, const QStyleOption * opt, const QSize & contentsSize, const QWidget * w) const override;
        
        QPixmap generatedIconPixmap(QIcon::Mode iconMode, const QPixmap & pixmap, const QStyleOption * opt) const override;
        
        QIcon standardIcon(QStyle::StandardPixmap standardIcon, const QStyleOption * option, const QWidget * widget) const override;
        
        QRect subElementRect(QStyle::SubElement subElement, const QStyleOption * option, const QWidget * widget) const override;
        
        QRect subControlRect(QStyle::ComplexControl cc, const QStyleOptionComplex * opt, QStyle::SubControl sc, const QWidget * widget) const override;
        
        int layoutSpacing(QSizePolicy::ControlType control1, QSizePolicy::ControlType control2, Qt::Orientation orientation, const QStyleOption * option, const QWidget * widget) const override;
        
        int pixelMetric(QStyle::PixelMetric metric, const QStyleOption * option, const QWidget * widget) const override;
        
        void polish(QApplication * application) override;
        
        void polish(QPalette & palette) override;
        
        void polish(QWidget * widget) override;
        
        void unpolish(QApplication * application) override;
        
        void unpolish(QWidget * widget) override;
        
        int styleHint(QStyle::StyleHint stylehint, const QStyleOption * opt, const QWidget * widget, QStyleHintReturn * returnData) const override;
    protected:
        void drawArrow(PrimitiveElement pe, QPainter* p, const QStyleOption* opt, const QWidget* w) const;
        void renderGradient( QPainter* p,
                             const QRect& r,
                             QColor clr,
                             bool horizontal,
                             int px=0,
                             int py=0,
                             int pwidth=-1,
                             int pheight=-1 )
        const;
        QBrush mapBrushToRect(const QBrush &brush, const QRectF &rect) const;
        QLinearGradient mapGradientToRect(const QLinearGradient &gradient, const QRectF &rect) const;
        QRadialGradient mapGradientToRect(const QRadialGradient &gradient, const QRectF &rect) const;
        QConicalGradient mapGradientToRect(const QConicalGradient &gradient, const QRectF &rect) const;
        //disabled for now
        //void childEvent(QChildEvent * event) override;
        void drawTabShape(const QStyleOption *option, QPainter *painter, const QWidget *widget=nullptr) const;
        StyleType type;
        bool highcolor;
        mutable bool selectionBackground;
        void drawComboBox(const QStyleOption *option, QPainter *painter, const QWidget *widget = nullptr);
        void drawVerticalProgressBar(const QStyleOption *opt, QPainter *p, const QWidget *widget = nullptr) const;
        void drawProgressBarLabel(const QStyleOption *opt, QPainter *p, const QWidget *widget = nullptr) const;
        void drawHorizontalProgressBar(const QStyleOption *opt, QPainter *p, const QWidget *widget = nullptr) const;
        void drawSliderGroove(const QStyleOptionComplex *opt, QPainter *p, const QWidget *widget = nullptr) const;
        void drawSliderHandle(const QStyleOptionComplex *opt, QPainter *p, const QWidget *widget = nullptr) const;
     private:
        QBitmap xBmp;
        // formerly Statics (I don't like them there)
        QBitmap *lightBmp;
        QBitmap *grayBmp;
        QBitmap *dgrayBmp;
        QBitmap *centerBmp;
        QBitmap *maskBmp;
    };

#endif // Q5LEGACYSTYLE_H
