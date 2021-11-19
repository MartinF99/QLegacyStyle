#include <QCommonStyle>
#include <QGradient>
#include "qlegacystyle.h"
#include "bitmaps.h"
#include "drawutil.h"
#include <QStyleOption>
#include <QStyleOptionButton>
#include <QStyleOptionFrame>
#include <QPainter>
#include <QHash>
#include <QPainterPath>
#include <QDebug>


static const int itemFrame       = 1;
static const int itemHMargin     = 3;
static const int itemVMargin     = 0;
static const int arrowHMargin    = 6;
static const int rightBorder     = 12;

GradientSet::GradientSet(){
    c = Qt::gray;
    for(int i=0; i < gradientCount; i++)
        gradients[i] = nullptr;
}
GradientSet::GradientSet(const QColor& baseColor)
{
    c = baseColor;
    for(int i=0; i < gradientCount; i++)
        gradients[i] = nullptr;
}
GradientSet::~GradientSet()
{
    for(int i=0; i < gradientCount; i++)
        if(gradients[i])
            delete gradients[i];
}

int GradientSet::getGradientSize(GradientType t)
{
    int val ;
    switch(t)
    {
    case VSmall:
        val = vSmallHeight; break;
    case VMed:
        val = vMedHeight; break;
    case VLarge:
        val = vLargeHeight; break;
    case HMed:
        val =  hMedWidth; break;
    case HLarge:
        val = hLargeWidth; break;
    }
    return val;
}

QLinearGradient* GradientSet::gradient(GradientType type)
{
    
    if (gradients[type])
        return gradients[type];

    switch(type)
    {
        case VSmall: {
            QLinearGradient * grad = new QLinearGradient(0,0,vWidth, vSmallHeight);
            gradients[VSmall] = grad;
            break;
        }

        case VMed: {
            gradients[VMed] = new QLinearGradient(0,0,vWidth,vMedHeight);
            gradients[VMed]->setFinalStop(0,1);
            gradients[VMed]->setColorAt(0,c.lighter(110));
            gradients[VMed]->setColorAt(1,c.darker(110));
            break;
        }

        case VLarge: {
            gradients[VLarge] = new QLinearGradient(0,0,vWidth,vLargeHeight);
            gradients[VLarge]->setFinalStop(0, 1);
            gradients[VLarge]->setColorAt(0,c.lighter(110));
            gradients[VLarge]->setColorAt(1,c.darker(110));
            break;
        }

        case HMed: {
            gradients[HMed] = new QLinearGradient(0,0,hMedWidth,hHeight);
            gradients[HMed]->setFinalStop(1,0);
            gradients[HMed]->setColorAt(0,c.lighter(110));
            gradients[HMed]->setColorAt(1,c.darker(110));
            break;
        }

        case HLarge: {
            gradients[HMed] = new QLinearGradient(0,0,hLargeWidth,hHeight);
            gradients[HMed]->setFinalStop(1,0);
            gradients[HMed]->setColorAt(0,c.lighter(110));
            gradients[HMed]->setColorAt(1,c.darker(110));
            break;
        }

        default:
            break;
    }
    return(gradients[type]);
}

QLegacyStyle::QLegacyStyle(StyleType t) : QCommonStyle()
{
    type = t;

    highcolor = (type == StyleType::HighColor && QPixmap::defaultDepth());
    gDict = new QHash<int, GradientSet *> ();
    xBmp = QBitmap(QBitmap::fromData(QSize(7,7), x_bits));
    lightBmp  = new QBitmap(QBitmap::fromData(QSize(13, 13), radiooff_light_bits));
    grayBmp   = new QBitmap(QBitmap::fromData(QSize(13, 13), radiooff_gray_bits));
    dgrayBmp  = new QBitmap(QBitmap::fromData(QSize(13, 13), radiooff_dgray_bits));
    centerBmp = new QBitmap(QBitmap::fromData(QSize(13, 13), radiooff_center_bits));
    centerBmp->setMask( *centerBmp );
    xBmp.setMask(xBmp);
}


QLegacyStyle::~QLegacyStyle()
{
    
}

void QLegacyStyle::drawControl(QStyle::ControlElement element, const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    switch(element)
    {
        // Button for real now
        case QStyle::CE_PushButton:
        {
            const QStyleOptionButton *buttonOpt = qstyleoption_cast<const QStyleOptionButton *>(opt);
            if(type != HighColor)
            {
                QRect br = buttonOpt -> rect;
                bool btnDefault = buttonOpt->features & QStyleOptionButton::DefaultButton;
                bool btnAutoDefault = buttonOpt->features & QStyleOptionButton::DefaultButton;
                
                if(btnDefault || btnAutoDefault)
                {
                    static int di = pixelMetric(PM_DefaultFrameWidth, opt, w);
                    br.adjust(di, di, -di, -di);
                }
                if(btnDefault)
                {
                    drawPrimitive(PE_FrameDefaultButton, opt, p, w);
                }
                
                QStyleOptionButton newOpt = *buttonOpt;
                newOpt.rect = br;
                
                drawPrimitive(PE_PanelButtonCommand, &newOpt, p, w);
                drawControl(CE_PushButtonLabel, &newOpt, p, w);
            }
            else{
                drawPrimitive(PE_PanelButtonCommand, opt, p, w);
                drawControl(CE_PushButtonLabel, opt, p, w);
            }
            
            break;
        }
        case CE_PushButtonLabel: {
            const QStyleOptionButton *buttonOpt = qstyleoption_cast<const QStyleOptionButton* >(opt);
			bool active = ((buttonOpt->state & State_On) || (buttonOpt->state & State_Sunken));
			int x, y, width, h;
			buttonOpt->rect.getRect( &x, &y, &width, &h );

			// Shift button contents if pushed.
			if ( active ) {
				x += pixelMetric(PM_ButtonShiftHorizontal, opt, w); 
				y += pixelMetric(PM_ButtonShiftVertical, opt, w);
				//buttonOpt->state |= State_Sunken;
			}

			// Does the button have a popup menu?
			if ( buttonOpt->features & QStyleOptionButton::HasMenu ) {
				int dx = pixelMetric( PM_MenuButtonIndicator, opt, w );
                QStyleOption newOpt = *opt;
                newOpt.rect = QRect(x + width -dx -2, y +2, dx, h-4);
				drawPrimitive( PE_IndicatorArrowDown, &newOpt, newOpt, w );
				w -= dx;
			}

			// Draw the icon if there is one
			if ( !buttonOpt->icon.isNull() ) {
				QIcon::Mode  mode  = QIcon::Disabled;
				QIcon::State state = QIcon::Off;

				if (buttonOpt->state & State_Enabled)
					mode = (buttonOpt->state & State_HasFocus) ? QIcon::Active : QIcon::Normal;
				if ((buttonOpt->state & State_On))
					state = QIcon::On;

				QPixmap pixmap = buttonOpt->icon.pixmap( buttonOpt->iconSize, mode, state );

				// Center the iconset if there's no text or pixmap
				if (buttonOpt->text.isEmpty() ) // foregroundPixmap ? does something like that still exist
					p->drawPixmap( x + (width - pixmap.width())  / 2, 
								   y + (h - pixmap.height()) / 2, pixmap );
				else
					p->drawPixmap( x + 4, y + (h - pixmap.height()) / 2, pixmap );

				int  pw = pixmap.width();
				x += pw + 4;
				w -= pw + 4;
			}

			// Make the label indicate if the button is a default button or not
			if ( active || (buttonOpt->features & QStyleOptionButton::DefaultButton) ) {
				// Draw "fake" bold text  - this enables the font metrics to remain
				// the same as computed in TQPushButton::sizeHint(), but gives
				// a reasonable bold effect.
				int i;

				// Text shadow
				if (buttonOpt->state & State_Enabled) // Don't draw double-shadow when disabled
					for(i=0; i<2; i++)
                        drawItemText(p, QRect(x+i+1, y+i+1, width, height), ;
						//drawItem( p, TQRect(x+i+1, y+1, w, h), AlignCenter | ShowPrefix, 
						//		ceData.colorGroup, (elementFlags & CEF_IsEnabled), NULL,
						//		ceData.text, -1,	
						//		(active ? &ceData.colorGroup.dark() : &ceData.colorGroup.mid()) );

				// Normal Text
				for(i=0; i<2; i++)
					drawItem( p, TQRect(x+i, y, w, h), AlignCenter | ShowPrefix, 
							ceData.colorGroup, (elementFlags & CEF_IsEnabled), ((i == 0) ? (ceData.fgPixmap.isNull())?NULL:&ceData.fgPixmap : NULL),
							ceData.text, -1,
							(active ? &ceData.colorGroup.light() : &ceData.colorGroup.buttonText()) );
			} else {
				drawItem( p, TQRect(x, y, w, h), AlignCenter | ShowPrefix, ceData.colorGroup,
						(elementFlags & CEF_IsEnabled), (ceData.fgPixmap.isNull())?NULL:&ceData.fgPixmap, ceData.text, -1,
						(active ? &ceData.colorGroup.light() : &ceData.colorGroup.buttonText()) );
			}

			// Draw a focus rect if the button has focus
			if ( flags & Style_HasFocus )
				drawPrimitive( PE_FocusRect, p, ceData, elementFlags,
						TQStyle::visualRect(subRect(SR_PushButtonFocusRect, ceData, elementFlags, widget), ceData, elementFlags),
						cg, flags );
			break;
		}
        // scrollbar
        case CE_ScrollBarSlider:
        {
            QStyle::State flags = opt->state;
            QRect r = opt->rect;
            QPalette cg = opt->palette;
            flags ^= State_Horizontal;

            drawPrimitive(PE_PanelButtonBevel,  opt, p, w);

            // Draw a scrollbar riffle (note direction after above changes)
            if ( type != B3 ) {

                // HighColor & Default scrollbar
                if (flags & State_Horizontal) {
                    if (r.height() >= 15) {
                        int x = r.x()+3;
                        int y = r.y() + (r.height()-7)/2;
                        int x2 = r.right()-3;
                        p->setPen(cg.light().color());
                        p->drawLine(x, y, x2, y);
                        p->drawLine(x, y+3, x2, y+3);
                        p->drawLine(x, y+6, x2, y+6);

                        p->setPen(cg.mid().color());
                        p->drawLine(x, y+1, x2, y+1);
                        p->drawLine(x, y+4, x2, y+4);
                        p->drawLine(x, y+7, x2, y+7);
                    }
                } else {
                    if (r.width() >= 15) {
                        int y = r.y()+3;
                        int x = r.x() + (r.width()-7)/2;
                        int y2 = r.bottom()-3;
                        p->setPen(cg.light().color());
                        p->drawLine(x, y, x, y2);
                        p->drawLine(x+3, y, x+3, y2);
                        p->drawLine(x+6, y, x+6, y2);

                        p->setPen(cg.mid().color());
                        p->drawLine(x+1, y, x+1, y2);
                        p->drawLine(x+4, y, x+4, y2);
                        p->drawLine(x+7, y, x+7, y2);
                    }
                }
            } else {

                // B3 scrollbar
                if (flags & State_Horizontal) {
                    int buttons = 0;

                    if (r.height() >= 36) buttons = 3;
                    else if (r.height() >=24) buttons = 2;
                    else if (r.height() >=16) buttons = 1;

                    int x = r.x() + (r.width()-7) / 2;
                    int y = r.y() + (r.height() - (buttons * 5) -
                                     (buttons-1)) / 2;
                    int x2 = x + 7;

                    for ( int i=0; i<buttons; i++, y+=6 )
                    {
                        p->setPen( cg.mid().color() );
                        p->drawLine( x+1, y, x2-1, y );
                        p->drawLine( x, y+1, x, y+3 );
                        p->setPen( cg.light().color() );
                        p->drawLine( x+1, y+1, x2-1, y+1 );
                        p->drawLine( x+1, y+1, x+1, y+3 );
                        p->setPen( cg.dark().color() );
                        p->drawLine( x+1, y+4, x2-1, y+4 );
                        p->drawLine( x2, y+1, x2, y+3 );
                    }
                } else {
                    int buttons = 0;

                    if (r.width() >= 36) buttons = 3;
                    else if (r.width() >=24) buttons = 2;
                    else if (r.width() >=16) buttons = 1;

                    int x = r.x() + (r.width() - (buttons * 5) -
                                     (buttons-1)) / 2;
                    int y = r.y() + (r.height()-7) / 2;
                    int y2 = y + 7;

                    for ( int i=0; i<buttons; i++, x+=6 )
                    {
                        p->setPen( cg.mid().color() );
                        p->drawLine( x+1, y, x+3, y );
                        p->drawLine( x, y+1, x, y2-1 );
                        p->setPen( cg.light().color() );
                        p->drawLine( x+1, y+1, x+3, y+1 );
                        p->drawLine( x+1, y+1, x+1, y2-1 );
                        p->setPen( cg.dark().color() );
                        p->drawLine( x+1, y2, x+3, y2 );
                        p->drawLine( x+4, y+1, x+4, y2-1 );
                    }
                }
            }
            break;
        }


        case CE_ScrollBarAddPage:
        case CE_ScrollBarSubPage: {
            // get the values from opt and name them according to their qt3 names, TODO: rename to better names
            QRect r = opt->rect;
            QStyle::State flags = opt->state;
            QPalette cg = opt->palette;
            int x, y, w, h;
            r.getRect(&x, &y, &w, &h);
            int x2 = x+w-1;
            int y2 = y+h-1;

            if ( type != B3 ) {
                // HighColor & Default scrollbar

                p->setPen(cg.shadow().color());

                if (flags & State_Horizontal) {
                    p->drawLine(x, y, x2, y);
                    p->drawLine(x, y2, x2, y2);
                    renderGradient(p, QRect(x, y+1, w, h-2),
                                   cg.mid().color(), false);
                } else {
                    p->drawLine(x, y, x, y2);
                    p->drawLine(x2, y, x2, y2);
                    renderGradient(p, QRect(x+1, y, w-2, h),
                                   cg.mid().color(), true);
                }
            } else {
                // B3 scrollbar

                p->setPen( cg.mid().color() );

                if (flags & State_Horizontal) {
                    p->drawLine(x, y, x2, y);
                    p->drawLine(x, y2, x2, y2);
                    p->fillRect( QRect(x, y+1, w, h-2),
                                 flags & State_Sunken ? cg.button().color() : cg.midlight().color() );
                } else {
                    p->drawLine(x, y, x, y2);
                    p->drawLine(x2, y, x2, y2);
                    p->fillRect( QRect(x+1, y, w-2, h),
                                 flags & State_Sunken ? cg.button().color() : cg.midlight().color() );
                }
            }
            break;
        }


        case CE_ScrollBarAddLine: {
            QStyleOption bevelOpts = QStyleOption(*opt);
            bevelOpts.state ^= (opt->state & State_Sunken) ? State_Sunken : State_Raised;

            drawPrimitive( PE_PanelButtonBevel, &bevelOpts, p, w );

            drawPrimitive( ((opt->state & State_Horizontal) ? PE_IndicatorArrowRight : PE_IndicatorArrowDown),
                           opt, p, w );
            break;
        }


        case CE_ScrollBarSubLine: {
            QStyleOption bevelOpts = QStyleOption(*opt);
            bevelOpts.state ^= (opt->state & State_Sunken) ? State_Sunken : State_Raised;

            drawPrimitive( PE_PanelButtonBevel, &bevelOpts, p, w );

            drawPrimitive( ((opt->state & State_Horizontal) ? PE_IndicatorArrowLeft : PE_IndicatorArrowUp),
                           opt, p, w );
            break;
        }

        default: QCommonStyle::drawControl(element, opt, p, w); break;
    }
    
}

void QLegacyStyle::drawComplexControl(QStyle::ComplexControl cc, const QStyleOptionComplex* opt, QPainter* p, const QWidget* widget) const
{
    switch(cc)
    {
        default:  QCommonStyle::drawComplexControl(cc, opt, p, widget); break;
    }
    
}

void QLegacyStyle::drawPrimitive(QStyle::PrimitiveElement pe, const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    bool down = opt->state & QStyle::State_Sunken;
    bool on = opt->state & QStyle::State_On;
    bool active = opt->state & QStyle::State_Active;
    
    switch(pe){
    case QStyle::PE_FrameDefaultButton:
    {
        //const QStyleOptionFrame *frameOpt = qstyleoption_cast<const QStyleOptionFrame *>(opt);
        QRect rect = opt->rect;
        if(type != HighColor)
        {
        int left = rect.left(), right = rect.right(), top = rect.top(), bottom = rect.bottom();
        p->setPen(opt->palette.shadow().color());
        p->drawLine( left+1, top, right-1, top );
        p->drawLine( left, top+1, left, bottom-1 );
        p->drawLine( left+1, bottom, right-1, bottom );
        p->drawLine( right, top+1, right, bottom-1 );
        }
    }
    case QStyle::PE_IndicatorButtonDropDown:
    case QStyle::PE_FrameButtonTool:
    {
        bool sunken = on || down;
        int left = opt->rect.left();
        int top = opt->rect.top();
        int width = opt->rect.width();
        int height = opt->rect.height();
        int x2 = left+width-1;
        int y2 = top+height-1;
        QPen oldPen = p->pen();
        p->setPen(opt->palette.shadow().color());

        p->drawLine(left+1,top,x2-1,top);
        p->drawLine(left,top+1,left,y2-1);
        p->drawLine(left+1,y2,x2-1,y2);
        p->drawLine(x2,top+1,x2,y2-1);

        p->setPen(sunken ? opt->palette.mid().color() : opt->palette.light().color());
        p->drawLine(left+1, top+1, x2-1, top+1);
        p->drawLine(left+1, top+1, left+1, y2-1);
        p->setPen(sunken ? opt->palette.light().color() : opt->palette.mid().color());
        p->drawLine(left+2, y2-1, x2-1, y2-1);
        p->drawLine(x2-1, top+2, x2-1, y2-1);

        p->fillRect(left+2, top+2, width-4, height-4, opt->palette.button().color());

        p->setPen( oldPen );
        break;
    }
    case QStyle::PE_PanelButtonCommand:
    {
        const QStyleOptionButton *buttonOpt = qstyleoption_cast<const QStyleOptionButton *>(opt);
        if(!buttonOpt)
        {
            break;
        }
        bool mouse_over = buttonOpt->state & State_MouseOver && buttonOpt->state & State_KeyboardFocusChange && buttonOpt->state & State_HasFocus;
        bool sunken = on || down;
        bool flat = !(buttonOpt->state & (State_Sunken | State_Raised));
        int  left, top, width, height;
        buttonOpt->rect.getRect(&left, &top, &width, &height);
        int x2 = left+width-1;
        int y2 = top+height-1;

        if ( sunken ){
            drawBeButton( p, left, top, width, height, buttonOpt->palette, true,
                          &buttonOpt->palette.brush(QPalette::Mid) );
        }
        else if ( mouse_over && !flat ) {
            QBrush brush(buttonOpt->palette.button().color().lighter(110));
            drawBeButton( p, left, top, width, height, buttonOpt->palette, false, &brush );
        }

        // "Flat" button
        else if ( flat ) {
            if ( mouse_over )
                p->fillRect(buttonOpt->rect, buttonOpt->palette.button().color().lighter(110));
            else
                renderGradient(p, QRect(left, top, width-1, height-1),
                               buttonOpt->palette.button().color(), false);

            p->setPen(buttonOpt->palette.button().color().lighter(75));
            p->drawLine(left, top, x2, top);
            p->drawLine(left, top, left, y2);
            p->drawLine(left, y2, x2, y2);
            p->drawLine(x2, top, x2, y2);
        }

        else if( highcolor )
        {
            int x2 = left+width-1;
            int y2 = top+height-1;
            p->setPen(buttonOpt->palette.shadow().color());
            p->drawLine(left+1, top, x2-1, top);
            p->drawLine(left+1, y2, x2-1, y2);
            p->drawLine(left, top+1, left, y2-1);
            p->drawLine(x2, top+1, x2, y2-1);

            p->setPen(buttonOpt->palette.light().color());
            p->drawLine(left+2, top+2, x2-1, top+2);
            p->drawLine(left+2, top+3, x2-2, top+3);
            p->drawLine(left+2, top+4, left+2, y2-1);
            p->drawLine(left+3, top+4, left+3, y2-2);

            p->setPen(buttonOpt->palette.mid().color());
            p->drawLine(x2-1, top+2, x2-1, y2-1);
            p->drawLine(left+2, y2-1, x2-1, y2-1);

            p->drawLine(left+1, left+1, x2-1, left+1);
            p->drawLine(left+1, left+2, left+1, y2-1);
            p->drawLine(x2-2, left+3, x2-2, y2-2);

            renderGradient(p, QRect(left+4, left+4, width-6, height-6),
                           buttonOpt->palette.button().color(), false);
        } else
            drawBeButton(p, left, left, width, height, buttonOpt->palette, false,
                         &buttonOpt->palette.brush(QPalette::Button));
        break;
    }
    case QStyle::PE_PanelButtonBevel:
    {
        int left,top,width, height;
        opt->rect.getRect(&left, &top, &width, &height);

        bool sunken = on|| down;
        int x2 = left+width-1;
        int y2 = top+height-1;

        p->setPen(opt->palette.shadow().color());
        p->drawRect(opt->rect);

        // Bevel
        p->setPen(sunken ? opt->palette.mid().color() : opt->palette.light().color());
        p->drawLine(left+1, top+1, x2-1, top+1);
        p->drawLine(left+1, top+1, left+1, y2-1);
        p->setPen(sunken ? opt->palette.light().color() : opt->palette.mid().color());
        p->drawLine(left+2, y2-1, x2-1, y2-1);
        p->drawLine(x2-1, top+2, x2-1, y2-1);
        if (width > 4 && height > 4) {
            if (sunken)
                p->fillRect(left+2, top+2, width-4, height-4, opt->palette.button().color());
            else
                renderGradient( p, QRect(left+2, top+2, width-4, height-4),
                                opt->palette.button().color(), opt->state & State_Horizontal );
        }
    }
    case QStyle::PE_FrameFocusRect:
    {
        if (const QStyleOptionFocusRect *fropt = qstyleoption_cast<const QStyleOptionFocusRect *>(opt)) {
            //### check for d->alt_down
            if (!(fropt->state & State_KeyboardFocusChange) && !proxy()->styleHint(SH_UnderlineShortcut, opt))
                return;
            QRect r = opt->rect;
            p->save();
            p->setBackgroundMode(Qt::TransparentMode);
            QColor bg_col = fropt->backgroundColor;
            if (!bg_col.isValid())
                bg_col = p->background().color();
            // Create an "XOR" color.
            QColor patternCol((bg_col.red() ^ 0xff) & 0xff,
                              (bg_col.green() ^ 0xff) & 0xff,
                              (bg_col.blue() ^ 0xff) & 0xff);
            p->setBrush(QBrush(patternCol, Qt::Dense4Pattern));
            p->setBrushOrigin(r.topLeft());
            p->setPen(Qt::NoPen);
            p->drawRect(r.left(), r.top(), r.width(), 1);    // Top
            p->drawRect(r.left(), r.bottom(), r.width(), 1); // Bottom
            p->drawRect(r.left(), r.top(), 1, r.height());   // Left
            p->drawRect(r.right(), r.top(), 1, r.height());  // Right
            p->restore();

        }
        break;

    }
    case QStyle::PE_IndicatorCheckBox:
    {
        const QStyleOptionButton *checkOpts = qstyleoption_cast<const QStyleOptionButton *>(opt);
        bool enabled = checkOpts->state & State_Enabled;
        bool nochange = checkOpts->state & State_NoChange;
        int left, top, width, height;
        checkOpts->rect.getRect(&left, &top, &width, &height);
        int x2 = left + width - 1;
        int y2 = top  + height - 1;
        
        p->setPen(checkOpts->palette.mid().color());
        p->drawLine(left, top, x2, top);
        p->drawLine(left, top, left, y2);
        
        p->setPen(checkOpts->palette.light().color());
        p->drawLine(x2, top+1, x2, y2);
        p->drawLine(left+1, y2, x2, y2);
        
        p->setPen(checkOpts->palette.shadow().color());
        p->drawLine(left+1, top+1, x2-1, top+1);
        p->drawLine(left+1, top+1, left+1, y2-1);
        
        p->setPen(checkOpts->palette.midlight().color());
        p->drawLine(x2-1, top+2, x2-1, y2-1);
        p->drawLine(left+2, y2-1, x2-1, y2-1);
        
        if ( enabled )
            p->fillRect(left+2, top+2, width-4, height-4,
                        down ? checkOpts->palette.button().color(): checkOpts->palette.base().color());
        else
            p->fillRect(left+2, top+2, width-4, height-4, checkOpts->palette.window().color());

        if (!(checkOpts->state & State_Off)) {
            if (on || nochange) {
                p->setPen((nochange)?
                              checkOpts->palette.dark().color():
                              checkOpts->palette.text().color());
                p->drawPixmap(left+3, top+3, xBmp);
            }
            else {
                p->setPen(checkOpts->palette.shadow().color());
                p->drawRect(left+2, top+2, width-4, height-4);
                p->setPen(nochange ? checkOpts->palette.text().color() : checkOpts->palette.dark().color());
                p->drawLine(left+3, (top+height)/2-2, left+width-4, (top+height)/2-2);
                p->drawLine(left+3, (top+height)/2, left+width-4, (top+height)/2);
                p->drawLine(left+3, (top+height)/2+2, left+width-4, (top+height)/2+2);
            }
        }
        break;
    }
    case QStyle::PE_IndicatorRadioButton:
    {
        const QStyleOptionButton *radioOpt = qstyleoption_cast<const QStyleOptionButton *>(opt);

        QPalette pal = radioOpt->palette;
        QRect optRect = radioOpt->rect;
        // Bevel
        colorBitmaps(p, pal, optRect.left(), optRect.top(), this->lightBmp , this->grayBmp,
                     nullptr, this->grayBmp);

        // The center fill of the indicator (grayed out when disabled)
        if ( radioOpt->state & State_Enabled )
            p->setPen( down ? pal.button().color() : pal.base().color() );
        else
            p->setPen( pal.window().color() );
        p->drawPixmap( optRect.left(), optRect.top(), *centerBmp );

        // Indicator "dot"
        if ( on ) {
            QColor color = radioOpt->state & State_NoChange ?
                        pal.dark().color() : pal.text().color();

            p->setPen(color);
            QPoint centerPoint = optRect.center() + QPoint(1,1);
            p->drawLine(centerPoint.x()-1, centerPoint.y()-2, centerPoint.x()+1, centerPoint.y()+2);
            p->drawLine(centerPoint.x()-2, centerPoint.y()-1, centerPoint.x()-2, centerPoint.y()+1);
            p->drawLine(centerPoint.x()-1, centerPoint.y()+2, centerPoint.x()+1, centerPoint.y()+2);
            p->drawLine(centerPoint.x()+2, centerPoint.y()-1, centerPoint.x()+2, centerPoint.y()+1);
            p->fillRect(centerPoint.x()-1, centerPoint.y()-1, 3,3, color);



                    // Qpoint(cx-1,cy-2) l01;     // QPoint(cx+1, cy+2) l02
 // QPoint(cx-2,cy-1) l11;                                                        // QPoint(cx+2, cy-1) l31;
                                            //centerPoint
 // QPoint(cx-2,cy+1) l12;                                                        // QPoint(cx+2,cy+1) l32;
                    // QPoint(cx-1,cy+2) l21;                 // QPoint(cx+1,cy+2) l22;





        }

        break;
    }
    case QStyle::PE_IndicatorDockWidgetResizeHandle:
    {
        int left,top,width,height;
        opt->rect.getRect(&left, &top, &width, &height);
        int x2 = left+width-1;
        int y2 = top+height-1;

        p->setPen(opt->palette.dark().color());
        p->drawRect(left, top, width, height);
        p->setPen(opt->palette.window().color());
        p->drawPoint(left, top);
        p->drawPoint(x2, top);
        p->drawPoint(left, y2);
        p->drawPoint(x2, y2);
        p->setPen(opt->palette.light().color());
        p->drawLine(left+1, top+1, left+1, y2-1);
        p->drawLine(left+1, top+1, x2-1, top+1);
        p->setPen(opt->palette.midlight().color());
        p->drawLine(left+2, top+2, left+2, y2-2);
        p->drawLine(left+2, top+2, x2-2, top+2);
        p->setPen(opt->palette.mid().color());
        p->drawLine(x2-1, top+1, x2-1, y2-1);
        p->drawLine(left+1, y2-1, x2-1, y2-1);
        p->fillRect(left+3, top+3, width-5, height-5, opt->palette.window());
        break;
    }
    case QStyle::PE_FrameGroupBox:
    case QStyle::PE_Frame:
    case QStyle::PE_FrameMenu:
    case QStyle::PE_FrameWindow:
    case QStyle::PE_FrameLineEdit:
    {
        const QStyleOptionFrame *frameOpts = qstyleoption_cast<const QStyleOptionFrame *>(opt);
        bool sunken = frameOpts->state & State_Sunken;
        int lineWidth;
        if(frameOpts->lineWidth == 0){
            lineWidth = pixelMetric(PM_DefaultFrameWidth, opt, w);
        }
        else {
            lineWidth = frameOpts->lineWidth;
        }
        if(lineWidth == 2)
        {
        QPen oldPen = p->pen();
        int left,top,width,height;
        frameOpts->rect.getRect(&left, &top, &width, &height);
        int x2 = left+width-1;
        int y2 = top+height-1;
        p->setPen(sunken ? frameOpts->palette.light().color() : frameOpts->palette.dark().color());
        p->drawLine(left, y2, x2, y2);
        p->drawLine(x2, top, x2, y2);
        p->setPen(sunken ? frameOpts->palette.mid().color() : frameOpts->palette.light().color());
        p->drawLine(left, top, x2, top);
        p->drawLine(left, top, left, y2);
        p->setPen(sunken ? frameOpts->palette.midlight().color() : frameOpts->palette.mid().color());
        p->drawLine(left+1, y2-1, x2-1, y2-1);
        p->drawLine(x2-1, top+1, x2-1, y2-1);
        p->setPen(sunken ? frameOpts->palette.dark().color() : frameOpts->palette.midlight().color());
        p->drawLine(left+1, top+1, x2-1, top+1);
        p->drawLine(left+1, top+1, left+1, y2-1);
        p->setPen(oldPen);
        }
        else{
            QCommonStyle::drawPrimitive(pe, opt, p, w);
        }

        break;
    }
    case PE_PanelMenuBar:
    {
        int x2 = opt->rect.left()+opt->rect.width()-1;
        int y2 = opt->rect.top()+opt->rect.height()-1;
        int lw = pixelMetric(PM_DefaultFrameWidth, opt, w);
        QRect r = opt->rect;
        if (lw)
        {
            p->setPen(opt->palette.light().color());
            p->drawLine(r.x(), r.y(), x2-1,  r.y());
            p->drawLine(r.x(), r.y(), r.x(), y2-1);
            p->setPen(opt->palette.dark().color());
            p->drawLine(r.x(), y2, x2, y2);
            p->drawLine(x2, r.y(), x2, y2);


            renderGradient( p, QRect(r.x()+1, r.y()+1, r.width()-2, r.height()-2),
                            opt->palette.button().color(),
                            opt->state & State_Horizontal );
        }
        else
            renderGradient( p, r, opt->palette.button().color(),
                            opt->state & State_Horizontal );

        break;
    }
    case PE_FrameDockWidget:
    {
        const QStyleOptionFrame *frameOpts = qstyleoption_cast<const QStyleOptionFrame *>(opt);
        QRect r = frameOpts->rect;
        int x2 = r.left()+ r.width()-1;
        int y2 = r.top()+r.height()-1;
        int lw = (frameOpts->lineWidth == 0) ? pixelMetric(PM_DefaultFrameWidth, opt, w)
                                 : frameOpts->lineWidth;

        if (lw)
        {
            p->setPen(frameOpts->palette.light().color());
            p->drawLine(r.left(), r.top(), x2-1,  r.top());
            p->drawLine(r.left(), r.top(), r.left(), y2-1);
            p->setPen(frameOpts->palette.dark().color());
            p->drawLine(r.left(), y2, x2, y2);
            p->drawLine(x2, r.top(), x2, y2);

            renderGradient( p, QRect(r.left()+1, r.top()+1, r.width()-2, r.height()-2),
                            frameOpts->palette.button().color(),
                            opt->state & State_Horizontal );
        }
        else
            renderGradient( p, r, frameOpts->palette.button().color(),
                            opt->state & State_Horizontal );

        break;
    }
        
   case PE_IndicatorMenuCheckMark:
   {
       int left, top, width, height;
       opt->rect.getRect( &left, &top, &width, &height );
       bool rtl = opt->direction == Qt::RightToLeft;
       int checkCol = std::max((qstyleoption_cast<const QStyleOptionMenuItem *>(opt))->maxIconWidth, 20);
       //to get the maximum icon width
       int cx = rtl? left+width - checkCol : left;

       QStyleOption newOpts = QStyleOption();
       newOpts.direction = opt->direction;
       newOpts.rect = QRect(cx + itemFrame, top + itemFrame, checkCol - itemFrame*2, height - itemFrame*2  );
       newOpts.state = (opt->state & active)? State_Enabled : State_On;
       drawPrimitive( PE_IndicatorCheckBox, &newOpts, p, w );
       break;
   }
   case QStyle::PE_IndicatorArrowRight:
   case QStyle::PE_IndicatorArrowUp:
   case QStyle::PE_IndicatorArrowLeft:
   case QStyle::PE_IndicatorArrowDown:
   { 
        drawArrow(pe, p, opt, w);
        break;
   }
   default: QCommonStyle::drawPrimitive(pe, opt, p, w); break;
        
        
    }
}

void QLegacyStyle::drawArrow ( QStyle::PrimitiveElement pe, QPainter *p, const QStyleOption *opt, const QWidget *w) const
{
    QPolygon polygon;
    
    if(type != B3)
    {
    switch(pe){
        case QStyle::PE_IndicatorArrowDown:
        {
            polygon.setPoints(sizeof(d_arrow) / (2*sizeof(int)), d_arrow );
            
            break;
        }
        case QStyle::PE_IndicatorArrowUp:
        {
            polygon.setPoints(sizeof(u_arrow) / (2*sizeof(int)), u_arrow );
            break;
        }
        case QStyle::PE_IndicatorArrowLeft:
        {
            polygon.setPoints(sizeof(l_arrow) / (2*sizeof(int)), l_arrow );
            break;
    }
        case QStyle::PE_IndicatorArrowRight:
        {
            polygon.setPoints(sizeof(r_arrow) / (2*sizeof(int)), r_arrow );
            break;
    }

        default: return;
    }
    }
    else
    {
        switch(pe){
        case QStyle::PE_IndicatorArrowDown:
        {
           polygon.setPoints(sizeof(B3::d_arrow) / (2*sizeof(int)), B3::d_arrow ); break;
        }
        case QStyle::PE_IndicatorArrowUp:
        {
            polygon.setPoints(sizeof(B3::u_arrow) / (2*sizeof(int)), B3::u_arrow ); break;
        }
        case QStyle::PE_IndicatorArrowLeft:
        {
            polygon.setPoints(sizeof(B3::l_arrow) / (2*sizeof(int)), B3::l_arrow ); break;
        }
        case QStyle::PE_IndicatorArrowRight:
        {
            polygon.setPoints(sizeof(B3::r_arrow) / (2*sizeof(int)), B3::r_arrow ); break;
        }
        default: return;
    }
    }
       
    p->save();
    if ( opt->state & State_Sunken ){
        p->translate( pixelMetric( PM_ButtonShiftHorizontal, opt, w ),
                      pixelMetric( PM_ButtonShiftVertical, opt, w ) );
    }
    if ( opt->state & State_Enabled ) {
        polygon.translate( opt->rect.left() + opt->rect.width() / 2, opt->rect.top() + opt->rect.height() / 2 );
        p->setPen( opt->palette.buttonText().color() );
        p->drawPolygon(polygon);
    } else {
        polygon.translate( opt->rect.left() + opt->rect.width() / 2 + 1, opt->rect.top() + opt->rect.height() / 2 + 1 );
        p->setPen( opt->palette.light().color() );
        p->drawPolygon( polygon );
        polygon.translate( -1, -1 );
        p->setPen( opt->palette.mid().color() );
        p->drawPolygon( polygon );
    }
    p->restore();
}


QPixmap QLegacyStyle::generatedIconPixmap(QIcon::Mode iconMode, const QPixmap& pixmap, const QStyleOption* opt) const
{
    switch(iconMode)
    {
    default: return QCommonStyle::generatedIconPixmap(iconMode, pixmap, opt);
    }
    
}

int QLegacyStyle::layoutSpacing(QSizePolicy::ControlType control1, QSizePolicy::ControlType control2, Qt::Orientation orientation, const QStyleOption* option, const QWidget* widget) const
{
    return QCommonStyle::layoutSpacing(control1, control2, orientation, option, widget);
}

QStyle::SubControl QLegacyStyle::hitTestComplexControl(QStyle::ComplexControl cc, const QStyleOptionComplex* opt, const QPoint& pt, const QWidget* widget) const
{
    switch(cc)
    {
    default :  return QCommonStyle::hitTestComplexControl(cc, opt, pt, widget);
    }

}

int QLegacyStyle::pixelMetric(QStyle::PixelMetric metric, const QStyleOption* option, const QWidget* widget) const
{
    return QCommonStyle::pixelMetric(metric, option, widget);
}

void QLegacyStyle::polish(QApplication* application)
{
    return QCommonStyle::polish(application);
}

void QLegacyStyle::polish(QPalette& palette)
{
    return QCommonStyle::polish(palette);
}

void QLegacyStyle::polish(QWidget* widget)
{
    return QCommonStyle::polish(widget);
}
QSize QLegacyStyle::sizeFromContents(QStyle::ContentsType ct, const QStyleOption* opt, const QSize& contentsSize, const QWidget* w) const
{
    switch(ct)
    {
        default : return QCommonStyle::sizeFromContents(ct, opt, contentsSize, w);
    }
    
}

QIcon QLegacyStyle::standardIcon(QStyle::StandardPixmap standardIcon, const QStyleOption* option, const QWidget* widget) const
{
    return QCommonStyle::standardIcon(standardIcon, option, widget);
}

int QLegacyStyle::styleHint(QStyle::StyleHint stylehint, const QStyleOption* opt, const QWidget* widget, QStyleHintReturn* returnData) const
{
    switch(stylehint){
        default: return QCommonStyle::styleHint(stylehint, opt, widget, returnData);
    }
    
}

QRect QLegacyStyle::subControlRect(QStyle::ComplexControl cc, const QStyleOptionComplex* opt, QStyle::SubControl sc, const QWidget* widget) const
{
    switch(cc){
        default : return QCommonStyle::subControlRect(cc, opt, sc, widget);
    }
    
}

QRect QLegacyStyle::subElementRect(QStyle::SubElement subElement, const QStyleOption* option, const QWidget* widget) const
{
    switch(subElement)
    {
        default: return QCommonStyle::subElementRect(subElement, option, widget);
    }
   
}

void QLegacyStyle::unpolish(QApplication* application)
{
    QCommonStyle::unpolish(application);
}

void QLegacyStyle::unpolish(QWidget* widget)
{
    QCommonStyle::unpolish(widget);
}

void QLegacyStyle::renderGradient(QPainter *p, const QRect& r, QColor clr, bool horizontal,
                                  int pleft, int ptop, int pwidth,int pheight ) const
{
    // Make 8 bit displays happy is this even necessary still?
    //QPixmap::DefaultDepth might crash the app after all?
        if (!highcolor) {
            p->fillRect(r, clr);
            return;
        }

        //Simplified code
        //TODO: remove all references to GradientSet
        // We can just create a new gradient without ever needing the grSet
        // should also remove Bugs
        // probably need to store somewhere though (Hash?)
        int width = (pwidth != -1) ? pwidth : r.width();
        int height = (pheight != -1) ? pheight: r.height();
        QLinearGradient gradient = QLinearGradient(0, 0, width, height);
        if (horizontal) {
            gradient.setFinalStop(0,1);
        }
        else {
            gradient.setFinalStop(1,0);
        }
        gradient.setColorAt(0, clr.lighter(110));
        gradient.setColorAt(0.5, clr);
        gradient.setColorAt(1, clr.darker(110) );
        p->setPen(Qt::transparent);
        p->fillRect(r.left(),r.top(), width, height, gradient);
}





