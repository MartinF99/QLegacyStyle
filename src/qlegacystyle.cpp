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
#include <QVector2D>
#include <QDebug>
#include <qdrawutil.h>
#include <QToolBar>
#include <QGuiApplication>
#include <QScreen>
#include <QPushButton>
#include <QMainWindow>
#include <QDockWidget>

QLinearGradient QLegacyStyle::mapGradientToRect(const QLinearGradient &gradient, const QRectF &rect) const
{
  QLinearGradient tmpGrad(rect.center().x(), rect.top(),
                          rect.center().x(), rect.bottom());
  tmpGrad.setStops(gradient.stops());
  return tmpGrad;
}

QConicalGradient QLegacyStyle::mapGradientToRect(const QConicalGradient &gradient, const QRectF &rect) const
{
  // set the center of a conical gradient to the rectangles center
  QConicalGradient tmpGrad(gradient);
  tmpGrad.setCenter(rect.center());
  return tmpGrad;

}

QRadialGradient QLegacyStyle::mapGradientToRect(const QRadialGradient &gradient, const QRectF &rect) const
{
  QRadialGradient tmpGradient(gradient);
  tmpGradient.setCenter(rect.center());
  // The Radius needs to be half the diagonal of the Rectangle, otherwise the Gradient will create Edges that are not filled correctly
  QVector2D bottomLeft(rect.bottomLeft());
  QVector2D topRight(rect.topRight());
  float diagonal = bottomLeft.distanceToPoint(topRight);
  tmpGradient.setCenterRadius(diagonal / 2);
  return tmpGradient;
}
QBrush QLegacyStyle::mapBrushToRect(const QBrush &brush, const QRectF &rect) const
{
  QBrush tmp;
  if (!brush.gradient())
    tmp = brush;
  else if (brush.gradient()->type()==QGradient::LinearGradient)
    {
      tmp = QBrush(mapGradientToRect(*static_cast<const QLinearGradient *>(brush.gradient()), rect));
    }
  else if(brush.gradient()->type() == QGradient::RadialGradient)
    {
      tmp = QBrush(mapGradientToRect(*static_cast<const QRadialGradient *>(brush.gradient()), rect));
    }
  else if(brush.gradient()->type() == QGradient::ConicalGradient)
    {
      tmp = QBrush(mapGradientToRect(*static_cast<const QConicalGradient *>(brush.gradient()), rect));
    }
  else tmp = brush;

  return tmp;
}

static const int itemFrame       = 1;
static const int itemHMargin     = 3;
static const int itemVMargin     = 0;
static const int arrowHMargin    = 6;
static const int rightBorder     = 12;



QLegacyStyle::QLegacyStyle(QLegacyStyle::StyleType t) : QCommonStyle()
{
  type = t;

  highcolor = (type == QLegacyStyle::StyleType::HighColor);
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
    case QStyle::CE_RubberBand:
      {
        const QStyleOptionRubberBand *rubOpt = qstyleoption_cast<const QStyleOptionRubberBand *>(opt);
        if(!rubOpt) return;
        QColor color= QColor(opt->palette.highlight().color());
        if(!rubOpt->opaque ){
            // make semi transparent
            color.setAlpha(127);
          }
        QRect r = rubOpt->rect;
        if(rubOpt->shape == QRubberBand::Rectangle)
          {

            p->fillRect(r, color);
          }
        p->save();
        // do this if it's a line too'
        p->setPen(color.darker(150));
        p->setBrush(Qt::NoBrush);
        p->drawRect(r);
        p->restore();





      }
    case QStyle::CE_Splitter:
      // we're drawing a PE_IndicatorDockWidgetResizeHandle because they're the same in HighcolorStyle
      drawPrimitive(PE_IndicatorDockWidgetResizeHandle, opt, p, w);
      break;
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
                     drawPrimitive(QStyle::PE_FrameButtonBevel, &newOpt, p, w);
                     drawPrimitive(QStyle::PE_PanelButtonBevel, &newOpt, p, w);
                     drawControl(CE_PushButtonLabel, &newOpt, p, w);
                 }
                 else{
                     drawPrimitive(PE_PanelButtonCommand, opt, p, w);
                     drawPrimitive(QStyle::PE_PanelButtonBevel, opt, p, w);
                     drawControl(CE_PushButtonLabel, opt, p, w);
                 }

                 break;
             }
    case QStyle::CE_TabBarTabShape:
      {
        const QStyleOptionTab *tabOpts = qstyleoption_cast<const QStyleOptionTab *>(opt);
        if(tabOpts==nullptr) return;
        drawTabShape(opt, p, w);
        break;
      }
    case QStyle::CE_ToolBoxTab:
      {

        const QStyleOptionToolBox *toolOpt = qstyleoption_cast<const QStyleOptionToolBox *>(opt);
        if(toolOpt == nullptr) return;
        bool selected = toolOpt->state & QStyle::State_Selected;
        bool pressed = toolOpt->state & QStyle::State_Sunken;
        int left, right, top, bottom;
        //get the rectangles coordinates
        toolOpt->rect.getCoords(&left, &right, &top, &bottom);

        p->setPen(pressed? toolOpt->palette.shadow().color() : toolOpt->palette.light().color());
        // draw top horizontal line
        p->drawLine(left, top, right - 1, top);
        // draw left vertical line
        p->drawLine(left, top, left, bottom-1);

        // when the toolBoxTab is pressed, then use light color otherwise use shadow color (reversed from top and left lines)
        p->setPen(pressed? toolOpt->palette.light().color() : toolOpt->palette.shadow().color());
        // draw bottom horizontal line
        p->drawLine(left, bottom, right - 1, bottom);
        // draw right vertical line
        p->drawLine(right, top, right, bottom-1);

        QColor fillColor = selected ? toolOpt->palette.highlight().color() : toolOpt->palette.button().color();
        selectionBackground = selected;

        if ( pressed )
          p->fillRect( QRect(left+1, top+1, toolOpt->rect.width()-2, toolOpt->rect.height()-2), fillColor );
        else
          renderGradient(p, QRect(left+1, top+1, toolOpt->rect.width()-2, toolOpt->rect.height()-2),
                         fillColor, false);

        drawControl(QStyle::CE_ToolBoxTabShape, opt, p, w);
        drawControl(QStyle::CE_ToolBoxTabLabel, opt, p, w);
        break;
      }

    case QStyle::CE_MenuBarEmptyArea:
      {
        renderGradient(p, opt->rect, opt->palette.button().color(), false);
        break;
      }

    case QStyle::CE_MenuBarItem:
      {
        // the styleoption;
        const QStyleOptionMenuItem *menuItemOption = qstyleoption_cast<const QStyleOptionMenuItem *>(opt);
        if(menuItemOption == nullptr){
            return;
          }
        bool selected, focused;
        selected = menuItemOption->state & State_Selected;
        focused = menuItemOption->state & State_HasFocus;
        if(selected && focused)
          {
            qDrawShadePanel(p, menuItemOption->rect, menuItemOption->palette, true, 1, &menuItemOption->palette.midlight());

          }
        else
          {
            renderGradient(p, menuItemOption->rect, menuItemOption->palette.button().color(), false, menuItemOption->rect.left(), menuItemOption->rect.top()-1, menuItemOption->menuRect.width() -2 , menuItemOption->rect.height() - 2);
          }
        drawItemText(p, menuItemOption->rect, Qt::AlignCenter | Qt::AlignVCenter | Qt::TextSingleLine | Qt::TextDontClip, menuItemOption->palette, menuItemOption->state & State_Enabled, menuItemOption->text);
      }
    case QStyle::CE_MenuItem:
      {
        const QStyleOptionMenuItem *itemOpt = qstyleoption_cast<const QStyleOptionMenuItem *>(opt);
        if(!itemOpt) return;

        bool checkable, exclusive, enabled;
        checkable = itemOpt->checkType != QStyleOptionMenuItem::NotCheckable;
        enabled = itemOpt->state & State_Enabled;
        int tab = itemOpt->tabWidth;
        int checkCol = itemOpt->maxIconWidth;
        bool active = itemOpt->state & State_Selected;
        QStyleHintReturn styleHintRet = QStyleHintReturn();
        bool etchtext   = styleHint( SH_EtchDisabledText, opt, w, &styleHintRet);
        bool reverse = itemOpt->direction == Qt::RightToLeft;
        QRect itemRect = itemOpt->rect;
        QRect menuRect = itemOpt->menuRect;
        if(checkable)
          {
            checkCol = qMax(checkCol, 20);
          }

        if(itemOpt->menuItemType == QStyleOptionMenuItem::Separator)
          {
            p->setPen(itemOpt->palette.dark().color());
            p->drawLine(itemRect.topLeft(), itemRect.topRight());
            p->setPen(itemOpt->palette.light().color());
            p->drawLine(itemRect.left(), itemRect.top()+1, itemRect.right(), itemRect.top()+1);
            break;
          }

        if(active)
          {
            qDrawShadePanel(p, itemRect, itemOpt->palette, true, 1, &itemOpt->palette.brush(QPalette::Midlight));
          }
        else // draw a solid background original Code used transparancy pixmap (we don't have that, so no transparancy for now);
          p->fillRect( itemRect, itemOpt->palette.button().color() );
        if(!itemOpt->icon.isNull())
          {
            QIcon::Mode mode;
            QRect iconRect = visualRect(itemOpt->direction, QRect(itemRect.left(), itemRect.top(), checkCol, itemRect.height()), itemRect);
            // set mode to disabled if the item is disabled
            // if item is enabled and selected set iconMode to active
            // otherwise set the normal Icon mode
            mode = enabled? (active? QIcon::Active : QIcon::Normal) : QIcon::Disabled;
            if ( checkable && !active && itemOpt->checked )
              {
                // draw A nice frame around the icon if the menuItem is checkable and has an icon;
                int cx = reverse? iconRect.left(): iconRect.right();
                qDrawShadePanel( p, cx, itemRect.top(), checkCol, itemRect.height(), itemOpt->palette, true, 1,
                                 &itemOpt->palette.brush(QPalette::Midlight));
              }
            // Draw the icon (get Small Icon size (16 ) )
            int iconExtent = pixelMetric(QStyle::PM_SmallIconSize, opt, w);
            QPixmap pixmap = itemOpt->icon.pixmap( QSize(iconExtent, iconExtent), mode );
            QRect pixmapRect( 0, 0, pixmap.width(), pixmap.height() );
            pixmapRect.moveCenter( iconRect.center() );
            p->drawPixmap( pixmapRect.topLeft(), pixmap );
          }
        else if(checkable && itemOpt->checked) // Radiobuttons drawn like normal checkboxes
          {
            QRect checkRect = visualRect(itemOpt->direction, QRect(itemRect.left(), itemRect.top(), itemOpt->maxIconWidth, itemRect.height()), itemRect);
            if(!active)
              {
                qDrawShadePanel( p, checkRect.left(), checkRect.top(), checkRect.width(), checkRect.height(), itemOpt->palette, true, 1, &itemOpt->palette.brush(QPalette::Midlight));
                QStyleOption newOpt =*opt;
                newOpt.rect = checkRect;
                drawPrimitive(QStyle::PE_IndicatorMenuCheckMark, &newOpt, p, w);
              }

          }
        // Time to draw the menu item label...
        int xm = itemFrame + checkCol + itemHMargin; // X position margin

        int xp = reverse ? // X position
                           itemRect.left() + tab + rightBorder + itemHMargin + itemFrame - 1 :
                           itemRect.left() + xm;

        int offset = reverse ? -1 : 1;	// Shadow offset for etched text

        // Label width (minus the width of the accelerator portion)
        int tw = itemRect.width()- xm - tab - arrowHMargin - itemHMargin * 3 - itemFrame + 1;


        p->setPen( enabled ? itemOpt->palette.buttonText().color() : itemOpt->palette.mid().color() );

        QColor discol = itemOpt->palette.mid().color();

        QString s = itemOpt->text;
        if(!s.isEmpty())
          {
            int t = s.indexOf(QLatin1Char('\t'));
            int m = itemVMargin;
            int text_flags = Qt::AlignVCenter  | Qt::TextDontClip | Qt::TextShowMnemonic | Qt::TextSingleLine;
            text_flags |= reverse ? Qt::AlignRight : Qt::AlignLeft;

            if ( t >= 0 ) {
                int tabx = reverse ? itemRect.x() + rightBorder + itemHMargin + itemFrame :
                                     itemRect.x() + itemRect.width() - tab - rightBorder - itemHMargin - itemFrame;

                // Draw the right part of the label (accelerator text)
                if ( etchtext && !enabled && !active ) {
                    // Draw etched text if we're inactive and the menu item is disabled
                    p->setPen( itemOpt->palette.light().color() );
                    p->drawText( QRect(tabx+offset, itemRect.top()+m+1, tab, itemRect.height()-2*m), text_flags, s.mid( t+1 ) );
                    p->setPen( discol );
                  }
                p->drawText( tabx, itemRect.top()+m, tab, itemRect.height()-2*m, text_flags, s.mid( t+1 ) );
                s = s.left( t );
              }

            // Draw the left part of the label (or the whole label
            // if there's no accelerator)
            if ( etchtext && !enabled && !active ) {
                // Etched text again for inactive disabled menu items...
                p->setPen( itemOpt->palette.light().color() );
                p->drawText( QRect(xp+offset, itemRect.top()+m+1, tw, itemRect.height()-2*m), text_flags, s);
                p->setPen( discol );
              }

            p->drawText( xp, itemRect.y()+m, tw, itemRect.height()-2*m, text_flags, s.mid(0, t-1) );




          }

        break;
      }
    case QStyle::CE_MenuScroller:
      {
        p->fillRect(opt->rect, opt->palette.window().color());
        drawPrimitive(PE_PanelButtonTool, opt, p, w);
        drawPrimitive((opt->state & State_UpArrow)?PE_IndicatorArrowUp:PE_IndicatorArrowDown, opt, p, w);
      }
    case QStyle::CE_PushButtonLabel: {
        const QStyleOptionButton *buttonOpt = qstyleoption_cast<const QStyleOptionButton* >(opt);
        bool active = ((buttonOpt->state & State_On) || (buttonOpt->state & State_Sunken));
        int x, y, width, height;
        buttonOpt->rect.getRect( &x, &y, &width, &height );

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
            newOpt.rect = QRect(x + width -dx -2, y +2, dx, height-4);
            drawPrimitive( PE_IndicatorArrowDown, &newOpt, p, w );
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
                             y + (height - pixmap.height()) / 2, pixmap );
            else
              p->drawPixmap( x + 4, y + (height - pixmap.height()) / 2, pixmap );

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
                drawItemText(p, QRect(x+i+1, y+i+1, width, height), Qt::AlignCenter, buttonOpt->palette, true, buttonOpt->text, (active?QPalette::Dark : QPalette::Mid)) ;

            // Normal Text
            for(i=0; i<2; i++)
              drawItemText( p, QRect(x+i, y, width, height), Qt::AlignCenter,
                            buttonOpt->palette, buttonOpt->state & State_Enabled, buttonOpt->text,
                            (active ? QPalette::Light : QPalette::ButtonText) );
          } else {
            drawItemText( p, QRect(x, y, width, height), Qt::AlignCenter , buttonOpt->palette,
                          (buttonOpt->state & State_Enabled), buttonOpt->text,
                          (active ? QPalette::Light : QPalette::ButtonText) );
          }

        // Draw a focus rect if the button has focus
        if ( buttonOpt->state & State_HasFocus ){
            QStyleOption newOpt = *opt;
            newOpt.rect = visualRect(opt->direction, subElementRect(SE_PushButtonFocusRect, opt, w), opt->rect);
            drawPrimitive( PE_FrameFocusRect, &newOpt, p, w);
          }
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
        int x, y, w, height;
        r.getRect(&x, &y, &w, &height);
        int x2 = x+w-1;
        int y2 = y+height-1;

        if ( type != B3 ) {
            // HighColor & Default scrollbar

            p->setPen(cg.shadow().color());

            if (flags & State_Horizontal) {
                p->drawLine(x, y, x2, y);
                p->drawLine(x, y2, x2, y2);
                renderGradient(p, QRect(x, y+1, w, height-2),
                               cg.mid().color(), false);
              } else {
                p->drawLine(x, y, x, y2);
                p->drawLine(x2, y, x2, y2);
                renderGradient(p, QRect(x+1, y, w-2, height),
                               cg.mid().color(), true);
              }
          } else {
            // B3 scrollbar

            p->setPen( cg.mid().color() );

            if (flags & State_Horizontal) {
                p->drawLine(x, y, x2, y);
                p->drawLine(x, y2, x2, y2);
                p->fillRect( QRect(x, y+1, w, height-2),
                             flags & State_Sunken ? cg.button().color() : cg.midlight().color() );
              } else {
                p->drawLine(x, y, x, y2);
                p->drawLine(x2, y, x2, y2);
                p->fillRect( QRect(x+1, y, w-2, height),
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

      //HEADEr
    case QStyle::CE_HeaderSection:
      {
        const QStyleOptionHeader *headerOpt = qstyleoption_cast<const QStyleOptionHeader *>(opt);
        if(headerOpt == nullptr)
          {
            return;
          }
        QPalette pal = headerOpt->palette;
        QRect rect = headerOpt->rect;
        bool sunken = headerOpt->state & State_Sunken;
        int left, top, width, height, right, bottom;
        rect.getRect(&left, &top, &width, &height);
        right = rect.right()-1;
        bottom = rect.bottom()-1;
        QPen oldPen = p->pen();

        p->setPen(sunken?pal.mid().color(): pal.light().color());
        p->drawLine(left, top, right-1, top);
        p->drawLine(left, top, left, bottom-1);
        p->setPen(sunken?pal.light().color():pal.mid().color());
        p->drawLine(left+1, bottom-1, right-1, bottom-1);
        p->drawLine(right-1, top+1, right-1, bottom-1);
        p->setPen(pal.shadow().color());
        p->drawLine(left, bottom, right, bottom);
        p->drawLine(right, top, right, bottom);

        if(sunken)
          {
            p->fillRect(left+1, top+1, width-3, height-3, pal.button());
          }
        else{
            renderGradient(p, QRect(left+1,top+1, width-3, height-3) , pal.button().color(), !headerOpt->state & State_Horizontal );
          }
        p->setPen(oldPen);
        break;
      }

    case QStyle::CE_ProgressBarGroove:
      {

        // taken from KSTyle 3
        QRect fr = subElementRect(SE_ProgressBarGroove, opt, w);
        QStyleOptionFrame fropt = QStyleOptionFrame();
        fropt.state=State_Sunken;
        fropt.rect=fr;
        drawPrimitive(PE_Frame, &fropt, p, w);
        break;

      }
    case QStyle::CE_ProgressBarContents:
      {
        const QStyleOptionProgressBar *progOpt = qstyleoption_cast<const QStyleOptionProgressBar *>(opt);
        if(progOpt == nullptr) return;

        if (progOpt->state & QStyle::State_Horizontal)
          {
            drawHorizontalProgressBar(progOpt, p, w);
          }
        else
          {
            drawVerticalProgressBar(progOpt, p, w);
          }
        break;
      }
    case QStyle::CE_ProgressBarLabel:
      {
        drawProgressBarLabel(opt, p, w);
        break;
      }
    default: QCommonStyle::drawControl(element, opt, p, w); break;
    }

}

void QLegacyStyle::drawComplexControl(QStyle::ComplexControl cc, const QStyleOptionComplex* opt, QPainter* p, const QWidget* widget) const
{

  switch(cc)
    {
    // 3 BUTTON SCROLLBAR
    // ------------------------------------------------------------------------
    case CC_ScrollBar: {
        const QStyleOptionSlider *scrollBar = qstyleoption_cast<const QStyleOptionSlider *>(opt);
        // Many thanks to Brad Hughes for contributing this code.
        bool useThreeButtonScrollBar = true;

        bool   maxedOut   = (scrollBar->minimum    == scrollBar->maximum);
        bool   horizontal = (scrollBar->orientation == Qt::Horizontal);
        QStyle::State sflags     = ((horizontal ? State_Horizontal : State_None) |
                                    (maxedOut   ? State_None : State_Enabled));

        QRect  addline, subline, subline2, addpage, subpage, slider, first, last;
        subline = subControlRect(cc, opt, SC_ScrollBarSubLine, widget);
        addline = subControlRect(cc, opt, SC_ScrollBarAddLine, widget);
        subpage = subControlRect(cc, opt, SC_ScrollBarSubPage, widget);
        addpage = subControlRect(cc, opt, SC_ScrollBarAddPage, widget);
        slider  = subControlRect(cc, opt, SC_ScrollBarSlider, widget);
        first   = subControlRect(cc, opt, SC_ScrollBarFirst, widget);
        last    = subControlRect(cc, opt, SC_ScrollBarLast, widget);
        subline2 = addline;

        if ( useThreeButtonScrollBar ) {
            if (horizontal) {
                subline2.translate(-addline.width(), 0);
              }
            else {
                subline2.translate(0, -addline.height());
              }
          }

        // Draw the up/left button set
        if ((scrollBar->subControls & SC_ScrollBarSubLine) && subline.isValid()) {
            QStyleOption newOpt = QStyleOption();
            newOpt.rect = subline;
            newOpt.palette = opt->palette;
            newOpt.state = opt->state | (scrollBar->activeSubControls & SC_ScrollBarSubLine ? State_Sunken : State_None);
            newOpt.fontMetrics = opt->fontMetrics;
            newOpt.direction = opt->direction;
            drawControl(CE_ScrollBarSubLine, &newOpt, p, widget);
            if (useThreeButtonScrollBar && subline2.isValid())
              {
                QStyleOption newOpt2 = QStyleOption();
                newOpt2.rect = subline2;
                newOpt2.palette = opt->palette;
                newOpt2.state = opt->state | (scrollBar->activeSubControls & SC_ScrollBarSubLine ? State_Sunken : State_None);
                newOpt2.fontMetrics = opt->fontMetrics;
                newOpt2.direction = opt->direction;
                drawControl(CE_ScrollBarSubLine, &newOpt2, p, widget);
              }
          }

        if ((scrollBar->subControls & SC_ScrollBarAddLine) && addline.isValid())
          {
            QStyleOption newOpt = QStyleOption();
            newOpt.rect = addline;
            newOpt.palette = opt->palette;
            newOpt.state = opt->state | (scrollBar->activeSubControls & SC_ScrollBarAddLine ? State_Sunken : State_None);
            newOpt.fontMetrics = opt->fontMetrics;
            newOpt.direction = opt->direction;
            drawControl(CE_ScrollBarAddLine, &newOpt, p, widget);
          }

        if ((scrollBar->subControls & SC_ScrollBarSubPage) && subpage.isValid())
          {
            QStyleOption newOpt = QStyleOption();
            newOpt.rect = subpage;
            newOpt.palette = opt->palette;
            newOpt.state = opt->state | (scrollBar->activeSubControls & SC_ScrollBarSubPage ? State_Sunken : State_None);
            newOpt.fontMetrics = opt->fontMetrics;
            newOpt.direction = opt->direction;
            drawControl(CE_ScrollBarSubPage, &newOpt, p, widget);
          }
        if ((scrollBar->subControls & SC_ScrollBarAddPage) && addpage.isValid())
          {
            QStyleOption newOpt = QStyleOption();
            newOpt.rect = addpage;
            newOpt.palette = opt->palette;
            newOpt.state = opt->state | (scrollBar->activeSubControls & SC_ScrollBarAddPage ? State_Sunken : State_None);
            newOpt.fontMetrics = opt->fontMetrics;
            newOpt.direction = opt->direction;
            drawControl(CE_ScrollBarAddPage, &newOpt, p, widget);
          }
        if ((scrollBar->subControls & SC_ScrollBarFirst) && first.isValid())
          {
            QStyleOption newOpt = QStyleOption();
            newOpt.rect = first;
            newOpt.palette = opt->palette;
            newOpt.state = opt->state | (scrollBar->activeSubControls & SC_ScrollBarFirst ? State_Sunken : State_None);
            newOpt.fontMetrics = opt->fontMetrics;
            newOpt.direction = opt->direction;
            drawControl(CE_ScrollBarFirst, &newOpt, p, widget);
          }
        if ((scrollBar->subControls & SC_ScrollBarLast) && last.isValid())
          {
            QStyleOption newOpt = QStyleOption();
            newOpt.rect = last;
            newOpt.palette = opt->palette;
            newOpt.state = opt->state | (scrollBar->activeSubControls & SC_ScrollBarLast ? State_Sunken : State_None);
            newOpt.fontMetrics = opt->fontMetrics;
            newOpt.direction = opt->direction;
            drawControl(CE_ScrollBarLast, &newOpt, p, widget);
          }
        if ((scrollBar->subControls & SC_ScrollBarSlider) && slider.isValid())
          {
            QStyleOption newOpt = QStyleOption();
            newOpt.rect = slider;
            newOpt.palette = opt->palette;
            newOpt.direction = opt->direction;
            newOpt.state = opt->state;
            drawControl(CE_ScrollBarSlider, &newOpt, p, widget);
            // Draw focus rect
            if (scrollBar->state & State_HasFocus) {
                QRect fr(slider.x() + 2, slider.y() + 2,
                         slider.width() - 5, slider.height() - 5);
                QStyleOption frame = QStyleOption();
                frame.rect = fr;
                frame.state = opt->state | State_None;
                frame.direction = scrollBar->direction;
                frame.palette = scrollBar->palette;
                frame.fontMetrics = scrollBar->fontMetrics;
                drawPrimitive(PE_FrameFocusRect, &frame, p, widget);
              }
          }
        break;
      }

    case CC_Slider:
      {
        const QStyleOptionSlider *slider = qstyleoption_cast<const QStyleOptionSlider *>(opt);
        if(slider == nullptr) return;
        bool horizontal = slider->orientation == Qt::Horizontal;
        const QPalette cg = slider->palette;
        if(slider->subControls & SC_SliderGroove)
          {
            const QRect groove = subControlRect(CC_Slider, opt, SC_SliderGroove, widget);

            const QRect r = groove; // for old code

            QRect gr;
            if (horizontal)
              gr = QRect(r.x(), r.center().y()-3, r.width(), 7); // put the line 3 pixels above the middle
            else
              gr = QRect(r.center().x()-3, r.y(), 7, r.height());

            int x,y,w,h;
            gr.getRect(&x, &y, &w, &h);
            int x2=x+w-1;
            int y2=y+h-1;

            // Draw the slider groove.
            p->setPen(cg.dark().color());
            p->drawLine(x+2, y, x2-2, y);
            p->drawLine(x, y+2, x, y2-2);
            p->fillRect(x+2,y+2,w-4, h-4,
                        (slider->state & State_Enabled) ? cg.dark().color() : cg.mid().color());
            p->setPen(cg.shadow().color());
            p->drawRect(x+1, y+1, w-2, h-2);
            p->setPen(cg.light().color());
            p->drawPoint(x+1,y2-1);
            p->drawPoint(x2-1,y2-1);
            p->drawLine(x2, y+2, x2, y2-2);
            p->drawLine(x+2, y2, x2-2, y2);
          }
        if(slider->subControls & SC_SliderHandle)
          {
            int x,y,w,h;
            const QRect handle = subControlRect(CC_Slider, opt, SC_SliderHandle, widget);
            const QRect r = handle;
            handle.getRect(&x, &y, &w, &h);
            // get the slider position

            int x2 = x+w-1;
            int y2 = y+h-1;

            p->setPen(cg.mid().color());
            p->drawLine(x+1, y, x2-1, y);
            p->drawLine(x, y+1, x, y2-1);
            p->setPen(cg.shadow().color());
            p->drawLine(x+1, y2, x2-1, y2);
            p->drawLine(x2, y+1, x2, y2-1);

            p->setPen(cg.light().color());
            p->drawLine(x+1, y+1, x2-1, y+1);
            p->drawLine(x+1, y+1, x+1,  y2-1);
            p->setPen(cg.dark().color());
            p->drawLine(x+2, y2-1, x2-1, y2-1);
            p->drawLine(x2-1, y+2, x2-1, y2-1);
            p->setPen(cg.midlight().color());
            p->drawLine(x+2, y+2, x2-2, y+2);
            p->drawLine(x+2, y+2, x+2, y2-2);
            p->setPen(cg.mid().color());
            p->drawLine(x+3, y2-2, x2-2, y2-2);
            p->drawLine(x2-2, y+3, x2-2, y2-2);
            renderGradient(p, QRect(x+3, y+3, w-6, h-6),
                           cg.button().color(), !horizontal);

            //                 Paint riffles
            if (horizontal) {
                p->setPen(cg.light().color());
                p->drawLine(x+5, y+4, x+5, y2-4);
                p->drawLine(x+8, y+4, x+8, y2-4);
                p->drawLine(x+11,y+4, x+11, y2-4);
                p->setPen((slider->state & State_Enabled) ? cg.shadow().color(): cg.mid().color());
                p->drawLine(x+6, y+4, x+6, y2-4);
                p->drawLine(x+9, y+4, x+9, y2-4);
                p->drawLine(x+12,y+4, x+12, y2-4);
              } else {
                p->setPen(cg.light().color());
                p->drawLine(x+4, y+5, x2-4, y+5);
                p->drawLine(x+4, y+8, x2-4, y+8);
                p->drawLine(x+4, y+11, x2-4, y+11);
                p->setPen((slider->state & State_Enabled) ? cg.shadow().color() : cg.mid().color());
                p->drawLine(x+4, y+6, x2-4, y+6);
                p->drawLine(x+4, y+9, x2-4, y+9);
                p->drawLine(x+4, y+12, x2-4, y+12);
              }
          }
        if(slider->subControls & SC_SliderTickmarks)
          {
            QStyleOptionSlider slider2 = *slider;
            slider2.subControls = SC_SliderTickmarks;
            QCommonStyle::drawComplexControl(CC_Slider, &slider2, p, widget);
          }
        break;
      }

    case CC_ComboBox:
      {
        const QStyleOptionComboBox *comboOpt = qstyleoption_cast<const QStyleOptionComboBox *>(opt);
        if(comboOpt == nullptr) return;
        bool enabled = comboOpt->state & State_Enabled;
        bool hasFocus = comboOpt->state & State_HasFocus;
        bool sunken = (comboOpt->activeSubControls & SC_ComboBoxArrow) || (comboOpt->activeSubControls & SC_ComboBoxEditField);
        if ( comboOpt->subControls & QStyle::SC_ComboBoxArrow)
          {

            // Draw the combo
            int x,y,w,h;
            comboOpt->rect.getRect(&x, &y, &w, &h);
            int x2 = x+w-1;
            int y2 = y+h-1;

            p->setPen(comboOpt->palette.shadow().color());
            p->drawLine(x+1, y, x2-1, y);
            p->drawLine(x+1, y2, x2-1, y2);
            p->drawLine(x, y+1, x, y2-1);
            p->drawLine(x2, y+1, x2, y2-1);

            // Ensure the edge notches are properly colored
            p->setPen(comboOpt->palette.button().color());
            p->drawPoint(x,y);
            p->drawPoint(x,y2);
            p->drawPoint(x2,y);
            p->drawPoint(x2,y2);

            renderGradient( p, QRect(x+2, y+2, w-4, h-4),
                            comboOpt->palette.button().color(), false);

            p->setPen(sunken ? comboOpt->palette.light().color() : comboOpt->palette.mid().color());
            p->drawLine(x2-1, y+2, x2-1, y2-1);
            p->drawLine(x+1, y2-1, x2-1, y2-1);

            p->setPen(sunken ? comboOpt->palette.mid().color() : comboOpt->palette.light().color());
            p->drawLine(x+1, y+1, x2-1, y+1);
            p->drawLine(x+1, y+2, x+1, y2-2);


            if(widget == nullptr) return;
            QRect ar = visualRect(comboOpt->direction, comboOpt->rect ,subControlRect(CC_ComboBox, opt, SC_ComboBoxArrow, widget));
            QStyleOption arrowOpt = QStyleOption();
            arrowOpt.rect = ar;
            arrowOpt.palette = comboOpt->palette;
            // Are we enabled?
            if ( enabled )
              arrowOpt.state |= State_Enabled;
            drawPrimitive(PE_IndicatorArrowDown,  &arrowOpt, p, widget );

          }
        if ( comboOpt->subControls & QStyle::SC_ComboBoxEditField)
          {
            QRect re = visualRect(comboOpt->direction, comboOpt->rect, subControlRect(CC_ComboBox, opt, SC_ComboBoxEditField, widget));


            if(comboOpt->editable){
                p->setPen(comboOpt->palette.dark().color());
                p->drawLine( re.x(), re.y()-1, re.x()+re.width(), re.y()-1 );
                p->drawLine( re.x()-1, re.y(), re.x()-1, re.y()+re.height() );
              }
            if(!comboOpt->editable)
              {
                if(hasFocus && highcolor)
                  {
                    renderGradient(p, re, comboOpt->palette.highlight().color(), true);
                  }
                else if (hasFocus)
                  {
                    p->fillRect(re, comboOpt->palette.highlight().color());
                  }
                else if(highcolor)
                  {
                    // else render the same way as a button (if !editable)
                    renderGradient(p, re, comboOpt->palette.button().color(), true);
                  }
              }
            else
              {
                if(hasFocus)
                  {
                    // draw Flat highlight color
                    p->fillRect(re, comboOpt->palette.highlight().color());
                  }
                else p->fillRect(re, comboOpt->palette.base().color());
              }
            // the check for sunken is necessary because otherwise the disabled color is used
            p->setPen(hasFocus?comboOpt->palette.highlightedText().color():(sunken?comboOpt->palette.buttonText().color(): comboOpt->palette.buttonText().color()));



            if(hasFocus && comboOpt->editable)
              {
                p->fillRect( re.x(), re.y(), re.width(), re.height(), comboOpt->palette.highlight());
                QStyleOption newOpt = QStyleOption();
                newOpt.direction = comboOpt->direction;
                newOpt.state = comboOpt->state;
                newOpt.rect = re;
                newOpt.palette = comboOpt->palette;
                drawPrimitive(PE_FrameFocusRect, &newOpt, p, widget); // find a better way of drawing comboboxes;
              }
              //drawPrimitive(PE_PanelLineEdit, opt, p, widget);
              //drawControl(CE_ComboBoxLabel, opt, p, widget); // apparently the label isn't drawn by QCommonStyle
          }
        else  // everything else drawn by QCommonStyle
          {QCommonStyle::drawComplexControl(cc, opt, p, widget);}
        break;


      }
    case CC_ToolButton:
      {
        const QStyleOptionToolButton *toolOpt = qstyleoption_cast<const QStyleOptionToolButton *>(opt);
        if(toolOpt == nullptr) return;

        //get all toolOpt members for shorter access
        QPalette pal = toolOpt->palette;
        QRect toolButtonRect = toolOpt->rect;
        int features = toolOpt->features;
        int state = toolOpt->state;
        QPoint pos = toolOpt->pos;
        Qt::ToolButtonStyle toolBtnStyle = toolOpt->toolButtonStyle;
        Qt::ArrowType arrowType = toolOpt->arrowType;

        QRect buttonRect = subControlRect(CC_ToolButton, opt, SC_ToolButton, widget);
        QRect menuArea = subControlRect(CC_ToolButton, opt, SC_ToolButtonMenu, widget);
        bool toolButtonActive = toolOpt->activeSubControls & SC_ToolButton;
        bool toolMenuActive = toolOpt->activeSubControls & SC_ToolButtonMenu;
        const bool inToolBar( widget && qobject_cast<const QToolBar*>( widget->parentWidget() ) );
        if(toolOpt->subControls & SC_ToolButton)
          {
            if(toolOpt->state & (State_On | State_Raised))
              {
                QStyleOptionButton butOpt = QStyleOptionButton();
                butOpt.direction = toolOpt->direction;
                butOpt.state |= toolOpt->state;
                butOpt.rect = buttonRect;
                butOpt.icon = toolOpt->icon;
                drawPrimitive(PE_PanelButtonTool, &butOpt, p, widget );
              }
            else if(toolButtonActive)
              {
                QStyleOptionButton butOpt = QStyleOptionButton();
                butOpt.direction = toolOpt->direction;
                butOpt.state |= toolOpt->state;
                butOpt.state |= State_Sunken;
                butOpt.rect = buttonRect;
                butOpt.icon = toolOpt->icon;
                drawPrimitive(PE_PanelButtonTool, &butOpt, p, widget );
              }
            else if(inToolBar)
              {
                QToolBar *toolBar = qobject_cast<QToolBar *>(widget->parentWidget());
                QRect toolbarRect = toolBar->geometry();
                if(toolBar->orientation() == Qt::Horizontal)
                  {
                    renderGradient( p, toolButtonRect, pal.button().color(), false, toolButtonRect.left(), toolButtonRect.top(),
                                    toolButtonRect.width(), toolbarRect.height() );
                  } else {
                    renderGradient( p, toolButtonRect, pal.button().color(), true, toolButtonRect.left(), toolButtonRect.top(),
                                    toolbarRect.width(), toolButtonRect.height() );
                  }
              }



          }
        if(toolOpt->subControls & SC_ToolButtonMenu)
          {
            QStyleOption newOpt= QStyleOption();
            newOpt.direction = toolOpt->direction;
            newOpt.state = toolOpt->state;
            if(toolOpt->activeSubControls & SC_ToolButtonMenu)
              {
                newOpt.state |= State_Sunken;
              }
            newOpt.rect = menuArea;
            newOpt.palette = toolOpt->palette;
            newOpt.styleObject = toolOpt->styleObject;
            newOpt.fontMetrics = toolOpt->fontMetrics;
            if(newOpt.state & (State_Raised | State_Sunken | State_On))
              {
                drawPrimitive(PE_IndicatorButtonDropDown, &newOpt, p, widget);
              }
            drawPrimitive(PE_IndicatorArrowDown, &newOpt, p, widget);
          }
        if ((toolOpt->state & State_HasFocus)) {
            QStyleOptionFocusRect fropt = QStyleOptionFocusRect();
            fropt.palette = toolOpt->palette;
            fropt.rect = toolOpt->rect;
            fropt.rect.adjust(3, 3, -3, -3);
            fropt.backgroundColor = pal.window().color();
            fropt.direction = toolOpt->direction;
            fropt.state = toolOpt->state;

            drawPrimitive(PE_FrameFocusRect, &fropt, p, widget);
          }
        break;
      }

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
        break;
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

        //button
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
        bool mouse_over = buttonOpt->state & State_MouseOver; //
        //bool focus = buttonOpt->state & State_KeyboardFocusChange || buttonOpt->state & State_HasFocus;
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
            QColor HoverColor = (buttonOpt-> palette.button().color().lighter(110));
            QBrush brush(HoverColor);
            printf("Hello \n");
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
    case PE_PanelLineEdit:
      {
        if (const QStyleOptionFrame *lineEdit = qstyleoption_cast<const QStyleOptionFrame *>(opt)) {
            // Panel of a line edit inside combo box or spin box is drawn in CC_ComboBox and CC_SpinBox
            if (w) {
                // Spinbox doesn't need a separate palette for the lineedit
                if (qobject_cast<const QAbstractSpinBox *>(w->parentWidget()))
                  break;
              }

            p->save();

            // Fill the line edit insides
            QRect filledRect = lineEdit->rect.adjusted(1, 1, -1, -1);
            QBrush baseBrush = mapBrushToRect(lineEdit->palette.base(), filledRect);
            p->setBrushOrigin(filledRect.topLeft());
            p->fillRect(filledRect.adjusted(1, 1, -1, -1), baseBrush);

            p->setPen(QPen(baseBrush, 0));
            const QLine lines[4] = {
              QLine(filledRect.left(), filledRect.top() + 1, filledRect.left(), filledRect.bottom() - 1), QLine(filledRect.right(), filledRect.top() + 1, filledRect.right(), filledRect.bottom() - 1),
              QLine(filledRect.left() + 1, filledRect.top(), filledRect.right() - 1, filledRect.top()),
              QLine(filledRect.left() + 1, filledRect.bottom(), filledRect.right() - 1, filledRect.bottom())
            };
            p->drawLines(lines, 4);

            if (lineEdit->lineWidth != 0)
              drawPrimitive(PE_FrameLineEdit, opt, p, w);

            p->restore();
            break;
          }
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
      //taken from KStyle 3 was originally DockWindowHandle
    case QStyle::PE_IndicatorToolBarHandle:
      {
        // original code checked whether the paint device was a widget,
        //is that necessary, considering we got a widget along with the function call;
        // I assume DockWindowHandle is not a thing anymore
        // Because OG KStyle Drew KPE_ToolBarHandle, we get the code from Highcolor's drawTDEStylePrimitve function
        //copy the rectangle;
        QRect r = QRect(opt->rect);


        int x = r.left(); int y = r.top();
        int x2 = r.left() + r.width()-1;
        int y2 = r.top() + r.height()-1;

        if(opt->state & State_Horizontal)
          {
            renderGradient(p, r, opt->palette.button().color(), false);
            p->setPen(opt->palette.light().color());
            p->drawLine(x+1, y+4, x+1, y2-4);
            p->drawLine(x+3, y+4, x+3, y2-4);
            p->drawLine(x+5, y+4, x+5, y2-4);

            p->setPen(opt->palette.mid().color());
            p->drawLine(x+2, y+4, x+2, y2-4);
            p->drawLine(x+4, y+4, x+4, y2-4);
            p->drawLine(x+6, y+4, x+6, y2-4);
          }
        else {
            renderGradient(p, r, opt->palette.button().color(), true);
            p->setPen(opt->palette.light().color());
            p->drawLine(x+4, y+1, x2-4, y+1);
            p->drawLine(x+4, y+3, x2-4, y+3);
            p->drawLine(x+4, y+5, x2-4, y+5);

            p->setPen(opt->palette.mid().color());
            p->drawLine(x+4, y+2, x2-4, y+2);
            p->drawLine(x+4, y+4, x2-4, y+4);
            p->drawLine(x+4, y+6, x2-4, y+6);

          }
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

void QLegacyStyle::drawArrow(PrimitiveElement pe, QPainter* p, const QStyleOption* opt, const  QWidget* w) const
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
  switch(metric)
    {
    // BUTTONS
    // ------------------------------------------------------------------------
    // BUTTONS
    // -------------------------------------------------------------------
    case PM_ButtonMargin:				// Space btw. frame and label
      return 4;

    case PM_ButtonDefaultIndicator: {
        if ( type == HighColor )
          return 0;					// No indicator when highcolor
        else
          return 3;
      }

    case PM_MenuButtonIndicator: {		// Arrow width
        if ( type != B3 )
          return 8;
        else
          return 7;
      }

      // CHECKBOXES / RADIO BUTTONS
      // -------------------------------------------------------------------
    case PM_ExclusiveIndicatorWidth:	// Radiobutton size
    case PM_ExclusiveIndicatorHeight:
    case PM_IndicatorWidth:				// Checkbox size
    case PM_IndicatorHeight: {
        return 13;						// 13x13
      }


    case PM_ButtonShiftHorizontal:		// Offset by 1
    case PM_ButtonShiftVertical:		// ### Make configurable
      return 1;

    case PM_DockWidgetHandleExtent:
      {
        const QStyleOptionDockWidget *dockWidget = qstyleoption_cast<const QStyleOptionDockWidget *>(option);
        // Check that we are not a normal toolbar or a hidden dockwidget,
        // in which case we need to adjust the height for font size
        if(dockWidget == nullptr) return 0;
        if(widget == nullptr) return 0;
        const QDockWidget *dock = qobject_cast<const QDockWidget *>(widget);
        if(dock && dock->isMinimized()) return widget->fontMetrics().lineSpacing();
        if (widget
            && !(qobject_cast<const QToolBar *>(widget->parentWidget()))
            && !(qobject_cast<const QMainWindow *>(widget->parentWidget()))
            )
          return option->fontMetrics.lineSpacing();

        else
          return QCommonStyle::pixelMetric(metric, option, widget);
      }

      // TABS
      // ------------------------------------------------------------------------
    case PM_TabBarTabHSpace:
      return 24;

    case PM_TabBarTabVSpace: {
        const QStyleOptionTab *tabBar = qstyleoption_cast<const QStyleOptionTab *>(option);
        if(!tabBar) return 4;

        if ( tabBar->shape == QTabBar::RoundedNorth ||
             tabBar->shape == QTabBar::RoundedSouth )
          return 10;
        else
          return 4;
      }

    case PM_TabBarTabOverlap: {
        const QStyleOptionTab *tabBar = qstyleoption_cast<const QStyleOptionTab *>(option);
        if(!tabBar) return 0;
        QTabBar::Shape tbs = tabBar->shape;

        if ( (tbs == QTabBar::RoundedNorth) ||
             (tbs == QTabBar::RoundedSouth) )
          return 0;
        else
          return 2;
      }
    case PM_SliderLength:
      return 18;

    case PM_SliderThickness:
      return 18;

      // Determines how much space to leave for the actual non-tickmark
      // portion of the slider.
    case PM_SliderControlThickness: {
        const QStyleOptionSlider *slider = qstyleoption_cast<const QStyleOptionSlider *>(option);
        if(slider == nullptr) return QCommonStyle::pixelMetric(PM_SliderControlThickness, option, widget);
        QSlider::TickPosition ts = slider->tickPosition;
        int thickness = (slider->orientation == Qt::Horizontal) ?
              slider->rect.height() : slider->rect.width();
        switch (ts) {
          case QSlider::NoTicks:				// Use total area.
            break;
          case QSlider::TicksBothSides :
            thickness = (thickness/2) + 3;	// Use approx. 1/2 of area.
            break;
          default:							// Use approx. 2/3 of area
            thickness = ((thickness*2)/3) + 3;
            break;
          };
        return thickness;
      }
      // SPLITTER
      // ------------------------------------------------------------------------
    case PM_SplitterWidth:
      return 6; //original was 8 if dockwidget

      // FRAMES
      // ------------------------------------------------------------------------
    case PM_MenuBarPanelWidth:
      return 1;

    case PM_DockWidgetFrameWidth:
      return 1;

      // GENERAL
      // ------------------------------------------------------------------------
    case PM_MaximumDragDistance:
      return -1;

    case PM_MenuBarItemSpacing:
      return 5;

    case PM_ToolBarItemSpacing:
      return 0;

    default:
      return QCommonStyle::pixelMetric(metric, option, widget);
    }
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
  if(qobject_cast<QPushButton *>(widget))
    {
      // allow mouse mouse_over
      widget->setAttribute(Qt::WA_Hover, true);
    }
  // Make the combobox menu also hoverable (maybe?)
  QCommonStyle::polish(widget);
}
QSize QLegacyStyle::sizeFromContents(QStyle::ContentsType ct, const QStyleOption* opt, const QSize& contentsSize, const QWidget* widget) const
{
  switch(ct)
    {
    case QStyle::CT_PushButton:
      {
        const QStyleOptionButton *button = qstyleoption_cast<const QStyleOptionButton *>(opt);
        if(!button) return QCommonStyle::sizeFromContents(ct, opt, contentsSize, widget); // let QCommonStyle figure out weird
        int w  = contentsSize.width();
        int h  = contentsSize.height();
        int bm = pixelMetric( PM_ButtonMargin, opt, widget );
        int fw = pixelMetric( PM_DefaultFrameWidth, opt, widget ) * 2;

        w += bm + fw + 6;	// ### Add 6 to make way for bold font.
        h += bm + fw;

        // Ensure we stick to standard width and heights.
        if ((button->features & QStyleOptionButton::DefaultButton) ||
            (button->features & QStyleOptionButton::AutoDefaultButton)) {
            if ( w < 80 && !button->text.isEmpty() )
              w = 80;

            if ( type != HighColor ) {
                // Compensate for default indicator
                int di = pixelMetric( PM_ButtonDefaultIndicator, button, widget );
                w += di * 2;
                h += di * 2;
              }
          }

        if ( h < 22 )
          h = 22;

        return QSize( w, h );
      }
    case QStyle::CT_MenuItem:
      {
        const QStyleOptionMenuItem *menuItem = qstyleoption_cast<const QStyleOptionMenuItem *>(opt);
        if(!menuItem) return QCommonStyle::sizeFromContents(ct, opt, contentsSize, widget);
        QStyle::State state = menuItem->state;
        bool checkable = menuItem->checkType != QStyleOptionMenuItem::NotCheckable;

        int maxIconWidth = menuItem->maxIconWidth;
        int width, height;
        width = contentsSize.width();
        height = contentsSize.height();

        bool itemHasIcon = !menuItem->icon.isNull();
        bool isSeperator = menuItem->menuItemType == QStyleOptionMenuItem::Separator;
        bool isSubMenu = menuItem->menuItemType == QStyleOptionMenuItem::SubMenu;

        if(isSeperator)
          {
            width = 10; // seperator width is probably unimportant
            height = 2;
          }

        if(itemHasIcon)
          {
            int extent = pixelMetric(PM_SmallIconSize, opt, widget);
            height = qMax(height, menuItem->icon.pixmap(QSize(extent, extent), QIcon::Normal).height() + itemFrame * 2);
          }
        else{ // text-only menuItem
            height = qMax(height, 16 + 2*itemVMargin +2 *itemFrame); // > MinimumSize
            height = qMax(height, menuItem->fontMetrics.height() + 2*itemVMargin + 2*itemFrame);
          }

        if(!menuItem->text.isNull() && menuItem->text.indexOf('\t') >= 0)
          {
            width+=12;
          }
        if(isSubMenu)
          {
            width+= 2* arrowHMargin;
          }

        if ( maxIconWidth )
          width += maxIconWidth + 6;
        if ( checkable && maxIconWidth < 20 )
          width += 20 - maxIconWidth;
        if ( checkable || maxIconWidth > 0 )
          width += 12;

        width += rightBorder;

        return QSize( width, height );

        return contentsSize;
      }
    default : return QCommonStyle::sizeFromContents(ct, opt, contentsSize, widget);
    }

}

QIcon QLegacyStyle::standardIcon(QStyle::StandardPixmap standardIcon, const QStyleOption* option, const QWidget* widget) const
{
  switch(standardIcon)
    {
    case SP_TitleBarMinButton:
      return QPixmap((const char **)hc_minimize_xpm);
    case SP_TitleBarCloseButton:
      return QPixmap((const char **)hc_close_xpm);
    case SP_TitleBarShadeButton:
      return QPixmap(const_cast<const char**>(hc_shade_xpm));
    case SP_TitleBarUnshadeButton:
      return QPixmap(const_cast<const char**>(hc_unshade_xpm));
    case SP_TitleBarNormalButton:
      return QPixmap(const_cast<const char**>(hc_normalizeup_xpm));
    case SP_TitleBarMaxButton:
      return QPixmap(const_cast<const char**>(hc_maximize_xpm));
    case SP_DockWidgetCloseButton:
      return QPixmap(const_cast<const char**>(dock_widget_close_xpm ));
    case SP_MessageBoxInformation:
      return QPixmap(const_cast<const char**>(information_xpm));
    case SP_MessageBoxWarning:
      return QPixmap(const_cast<const char**>(warning_xpm));
    case SP_MessageBoxCritical:
      return QPixmap(const_cast<const char**>(critical_xpm));
    default:
      return QCommonStyle::standardIcon(standardIcon, option, widget);
    }

}

int QLegacyStyle::styleHint(QStyle::StyleHint stylehint, const QStyleOption* opt, const QWidget* widget, QStyleHintReturn* returnData) const
{
  int ret;

  switch (stylehint) {
    case SH_EtchDisabledText:
      return 1;

    case SH_Menu_Scrollable:
      return 1;

    case SH_MenuBar_AltKeyNavigation:
      return 1;

    case SH_Menu_SubMenuPopupDelay:
      if ( styleHint( SH_Menu_SloppySubMenus, opt, widget ) )
        return 100;
      else
        return 250; // KDE Default value from KDE 3

    case SH_Menu_SloppySubMenus:
      return false;

    case SH_ItemView_ChangeHighlightOnFocus:
    case SH_Slider_SloppyKeyEvents:
    case SH_MainWindow_SpaceBelowMenuBar:
    case SH_Menu_AllowActiveAndDisabled:
    case SH_TabBar_PreferNoArrows:
      return 0;

    case SH_Slider_SnapToValue:
    case SH_PrintDialog_RightAlignButtons:
    case SH_FontDialog_SelectAssociatedText:
    case SH_MenuBar_MouseTracking:
    case SH_Menu_MouseTracking:
    case SH_ComboBox_ListMouseTracking:
    case SH_ScrollBar_MiddleClickAbsolutePosition:
      return 1;
    case SH_LineEdit_PasswordCharacter:
      {
        const QFontMetrics &fm = opt->fontMetrics;
        if (fm.inFont(QChar(0x25CF))) {
            return 0x25CF;
          } else if (fm.inFont(QChar(0x2022))) {
            return 0x2022;
          }
        return '*';
      }

    default:
      return QCommonStyle::styleHint(stylehint, opt, widget, returnData);
    }
}

QRect QLegacyStyle::subControlRect(QStyle::ComplexControl cc, const QStyleOptionComplex* opt, QStyle::SubControl sc, const QWidget* widget) const
{
  QRect ret;
  switch(cc){
    case CC_ScrollBar:
      {
        const QStyleOptionSlider * scrollBar = qstyleoption_cast<const QStyleOptionSlider *>(opt);
        if(!scrollBar) return QCommonStyle::subControlRect(cc, opt, sc, widget);
        bool threeButtonScrollBar = true;

        bool horizontal = scrollBar->orientation == Qt::Horizontal;
        int sliderstart = scrollBar->minimum;
        int sbextent    = pixelMetric(PM_ScrollBarExtent, opt, widget);
        int maxlen      = (horizontal ? opt->rect.width() : opt->rect.height())
            - (sbextent * (threeButtonScrollBar ? 3 : 2));
        int sliderlen;

        // calculate slider length
        if (scrollBar->maximum != scrollBar->minimum)
          {
            uint range = scrollBar->maximum - scrollBar->minimum;
            sliderlen = (scrollBar->pageStep * maxlen) /	(range + scrollBar->pageStep);

            int slidermin = pixelMetric( PM_ScrollBarSliderMin, opt , widget );
            if ( sliderlen < slidermin || range > INT_MAX / 2 )
              sliderlen = slidermin;
            if ( sliderlen > maxlen )
              sliderlen = maxlen;
          } else
          sliderlen = maxlen;


        // Subcontrols
        switch (sc)
          {
          case SC_ScrollBarSubLine: {

              break;
            }

          case SC_ScrollBarAddLine: {
              if (horizontal)
                ret.setRect(scrollBar->rect.width() - sbextent, 0, sbextent, sbextent);
              else
                ret.setRect(0, scrollBar->rect.height() - sbextent, sbextent, sbextent);
              break;
            }

          case SC_ScrollBarSubPage: {
              // between top/left button and slider
              if (horizontal)
                ret.setRect(sbextent, 0, sliderstart - sbextent, sbextent);
              else
                ret.setRect(0, sbextent, sbextent, sliderstart - sbextent);
              break;
            }

          case SC_ScrollBarAddPage: {
              // between bottom/right button and slider
              int fudge;

              fudge = sbextent;

              if (horizontal)
                ret.setRect(sliderstart + sliderlen, 0,
                            maxlen - sliderstart - sliderlen + fudge, sbextent);
              else
                ret.setRect(0, sliderstart + sliderlen, sbextent,
                            maxlen - sliderstart - sliderlen + fudge);
              break;
            }

          case SC_ScrollBarGroove: {
              int multi = threeButtonScrollBar ? 3 : 2;
              int fudge;
              fudge = sbextent;

              if (horizontal)
                ret.setRect(fudge, 0, scrollBar->rect.width() - sbextent * multi, scrollBar->rect.height());
              else
                ret.setRect(0, fudge, scrollBar->rect.width(), scrollBar->rect.height() - sbextent * multi);
              break;
            }

          case SC_ScrollBarSlider: {
              if (horizontal)
                ret.setRect(sliderstart, 0, sliderlen, sbextent);
              else
                ret.setRect(0, sliderstart, sbextent, sliderlen);
              break;
            }
          default:
            return QCommonStyle::subControlRect(cc, opt ,sc, widget);

          }
      }
        default : return QCommonStyle::subControlRect(cc, opt, sc, widget);
      }
      return ret;
    }

  QRect QLegacyStyle::subElementRect(QStyle::SubElement subElement, const QStyleOption* option, const QWidget* widget) const
  {
    switch(subElement)
      {
      case SE_ProgressBarLabel:
      case SE_ProgressBarContents:
      case SE_ProgressBarGroove:
        return option->rect;
      case QStyle::SE_PushButtonFocusRect:
        {
          const QStyleOptionButton *butOpt = qstyleoption_cast<const QStyleOptionButton *>(option);
          if(!butOpt) return option->rect;
          QRect wrect(butOpt->rect);

          int dbw1 = 0, dbw2 = 0;
          if(butOpt->features & (butOpt->DefaultButton | butOpt->AutoDefaultButton))
            {
              dbw1 = pixelMetric(PM_ButtonDefaultIndicator, option, widget);
              dbw2 = dbw1 * 2;
            }
          int dfw1 = pixelMetric(PM_DefaultFrameWidth, option, widget);
          int dfw2 = dfw1 * 2;
          return QRect(wrect.x()      + dfw1 + dbw1 + 1,
                       wrect.y()      + dfw1 + dbw1 + 1,
                       wrect.width()  - dfw2 - dbw2 - 1,
                       wrect.height() - dfw2 - dbw2 - 1);
        }
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

  void QLegacyStyle::renderGradient(QPainter *p, const QRect& r, QColor clr, bool horizontal, int pleft, int ptop, int pwidth,int pheight ) const
  {
    // Make 8 bit displays happy is this even necessary still?
    //QPixmap::DefaultDepth might crash the app after all?
    if (!highcolor) {
        p->fillRect(r, clr);
        return;
      }
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




  // TODO Triangular Tabs; unless I don't want them;
  void QLegacyStyle::drawTabShape(const QStyleOption* option, QPainter* painter, const QWidget* widget) const
  {
    if(const QStyleOptionTab *tab = qstyleoption_cast<const QStyleOptionTab *>(option))
      {
        QRect tabRect = tab->rect;
        bool selected = tab->state & State_Selected;
        QRect rect = tab->rect;
        int left, right, top, bottom;
        rect.getCoords(&left, &top, &right, &bottom);
        QPalette cg = tab->palette;

        switch(tab->shape){
          case QTabBar::TriangularNorth:
          case QTabBar::RoundedNorth:
            {
              if (!selected)
                {
                  painter->translate(0,1);
                }
              painter->setPen(selected ? cg.light().color() : cg.shadow().color());
              painter->drawLine(left, top+4, left, bottom);
              // corner
              painter->drawLine(left, top+4, left+4, top);
              painter->drawLine(left+4, top, right-1, top);
              if (selected)
                painter->setPen(cg.shadow().color());
              painter->drawLine(right, top+1, right, bottom);

              painter->setPen(cg.midlight().color());
              //
              painter->drawLine(left+1, top+4, left+1, bottom);

              painter->drawLine(left+1, top+4, left+4, top+1);
              painter->drawLine(left+5, top+1, right-2, top+1);

              if (selected) {
                  painter->setPen(cg.mid().color());
                  painter->drawLine(right-1, top+1, right-1, bottom);
                } else {
                  painter->setPen(cg.mid().color());
                  painter->drawPoint(right-1, top+1);
                  painter->drawLine(left+4, top+2, right-1, top+2);
                  painter->drawLine(left+3, top+3, right-1, top+3);
                  painter->fillRect(left+2, top+4, rect.width()-3, rect.height()-6, cg.mid().color());

                  painter->setPen(cg.light().color());
                  painter->drawLine(left, bottom-1, right, bottom-1);
                  painter->translate(0,-1);

                }
              break;
            }
          case QTabBar::TriangularSouth:
          case QTabBar::RoundedSouth:
            {
              if (!selected)
                painter->translate(0,-1);
              painter->setPen(selected ? cg.light().color() : cg.shadow().color());
              painter->drawLine(left, bottom-4, left, top);
              if (selected)
                painter->setPen(cg.mid().color());
              painter->drawLine(left, bottom-4, left+4, bottom);
              if (selected)
                painter->setPen(cg.shadow().color());
              painter->drawLine(left+4, bottom, right-1, bottom);
              painter->drawLine(right, bottom-1, right, top);

              painter->setPen(cg.midlight().color());
              painter->drawLine(left+1, bottom-4, left+1, top);
              painter->drawLine(left+1, bottom-4, left+4, bottom-1);
              painter->drawLine(left+5, bottom-1, right-2, bottom-1);

              if (selected) {
                  painter->setPen(cg.mid().color());
                  painter->drawLine(right-1, top, right-1, bottom-1);
                } else {
                  painter->setPen(cg.mid().color());
                  painter->drawPoint(right-1, bottom-1);
                  painter->drawLine(left+4, bottom-2, right-1, bottom-2);
                  painter->drawLine(left+3, bottom-3, right-1, bottom-3);
                  painter->fillRect(left+2, top+2, rect.width()-3, rect.height()-6, cg.mid().color());
                  painter->translate(0,1);
                  painter->setPen(cg.dark().color());
                  painter->drawLine(left, top, right, top);
                }
              break;
            }

          case QTabBar::TriangularWest:
          case QTabBar::RoundedWest:
            {
              if (!selected)
                painter->translate(1,0);
              painter->setPen(selected ? cg.light().color() : cg.shadow().color());
              // this is now the top line;
              painter->drawLine(left+4, top, right, top);
              // corner stays the same as it would just drawn from right to left (we don't need that);
              painter->drawLine(left, top+4, left+4, top);
              // this is the left from top - bottom line now
              painter->drawLine(left, top+4, left, bottom-1);
              if (selected)
                painter->setPen(cg.shadow().color());
              // this line is not the bottom line
              painter->drawLine(left+1, bottom, right, bottom);

              painter->setPen(cg.midlight().color());
              //this line is now on the top from left to right
              painter->drawLine(left+4, top+1, right, top+1);
              // corner line, stays the same
              painter->drawLine(left+1, top+4, left+4, top+1);
              // from top to bottom on the left
              painter->drawLine(left+1, top+5, left+1, bottom -2);

              if (selected) {
                  painter->setPen(cg.mid().color());
                  //painter->drawLine(right-1, top+1, right-1, bottom);
                  painter->drawLine(left+1, bottom -1, right, bottom -1);
                } else {
                  painter->setPen(cg.mid().color());
                  //painter->drawPoint(right-1, top+1);
                  painter->drawPoint(left+1, bottom -1);
                  //painter->drawLine(left+4, top+2, right-1, top+2);
                  painter->drawLine(left+2, top +4, left+2, bottom -1);
                  //painter->drawLine(left+3, top+3, right-1, top+3);
                  painter->drawLine(left+3, top +3, left+3, bottom -1);
                  painter->fillRect(left+4, top+2, rect.width()-6, rect.height()-3, cg.mid().color());

                  painter->setPen(cg.light().color());
                  //painter->drawLine(left, bottom-1, right, bottom-1);
                  painter->drawLine(right, top, right, bottom -1);
                  painter->translate(-1,0);
                }
              break;
            }
          case QTabBar::TriangularEast:
          case QTabBar::RoundedEast:
            {
              // We need to point mirror the RoundedNorth code, so that we draw the correct lines in relation to each other
              //I am unsure about the fillRect part, we need to fill from top left to bottom right, but without
              // filling until there;
              if (!selected)
                painter->translate(-1,0);
              painter->setPen(selected ? cg.light().color() : cg.shadow().color());
              // this is now the top line;
              painter->drawLine(left, top, right-4, top);
              // corner is now on the right;
              painter->drawLine(right, top+4, right-4, top);
              // this is the right from top - bottom line now
              painter->drawLine(right, top+4, right, bottom-1);
              if (selected)
                painter->setPen(cg.shadow().color());
              // this line is now the bottom line
              painter->drawLine(left, bottom, right-1, bottom);

              painter->setPen(cg.midlight().color());
              //this line is now on the top from left to right
              painter->drawLine(left, top+1, right-4, top+1);
              // corner line, stays the same
              painter->drawLine(right-1, top+4, right-4, top+1);
              // from top to bottom on the left
              painter->drawLine(right-1, top+5, right-1, bottom -2);

              if (selected) {
                  painter->setPen(cg.mid().color());
                  //painter->drawLine(right-1, top+1, right-1, bottom);
                  painter->drawLine(right-1, bottom -1, left, bottom -1);
                } else {
                  painter->setPen(cg.mid().color());
                  //painter->drawPoint(right-1, top+1);
                  painter->drawPoint(right-1, bottom -1);
                  //painter->drawLine(left+4, top+2, right-1, top+2);
                  painter->drawLine(right-2, top +4, right-2, bottom -1);
                  //painter->drawLine(left+3, top+3, right-1, top+3);
                  painter->drawLine(right-3, top +3, right-3, bottom -1);
                  painter->fillRect(left+2, top+2, rect.width()-6, rect.height()-3, cg.mid().color());

                  painter->setPen(cg.light().color());
                  //painter->drawLine(left, bottom-1, right, bottom-1);
                  painter->drawLine(left, top, left, bottom -1);
                  painter->translate(1,0);
                }
              break;
            }

          }
      }
  }




  void QLegacyStyle::drawHorizontalProgressBar(const QStyleOption *opt, QPainter* p, const QWidget* widget) const
  {
    const QStyleOptionProgressBar *progOpt = qstyleoption_cast<const QStyleOptionProgressBar *>(opt);
    if(progOpt==nullptr) return;
    QRect contentRect = subElementRect(QStyle::SE_ProgressBarContents, opt, widget);
    int max, prog, min;
    max = progOpt->maximum;
    min = progOpt->minimum;
    prog = progOpt->progress;
    double currentStep = prog -min;
    int totalSteps = max-min;

    bool inverted = ((progOpt->invertedAppearance && !(progOpt->direction == Qt::RightToLeft))
                     || (!progOpt->invertedAppearance && progOpt->direction == Qt::RightToLeft));

    // the layout is inverted if we are not RTL but invertedAppearance is true or if we're RTL and invertedAppearance is false
    if(!contentRect.isValid()) return;

    if(currentStep > 0 || totalSteps == 0)
      {
        double pg = currentStep/totalSteps;
        int width = qMin((double)contentRect.width(), pg*contentRect.width());

        if(totalSteps == 0)
          {
            //busy indicator;

            // Width < 1 useless for a busy indicator
            if(width < 1) width = 1;

            //never disappear completely
            int remainingWidth = contentRect.width() - width;
            // remainingWidth < 1 can cause crashes;
            if(remainingWidth <= 0) remainingWidth = 1;
            int pstep = (int)currentStep %(2*remainingWidth);
            if(pstep > remainingWidth)
              {
                pstep = - (pstep - 2 * remainingWidth );
              }
            if(inverted)
              {
                p->fillRect(contentRect.right() - width - pstep, contentRect.top(), width, contentRect.height(),
                            QPalette::Highlight);
              }
            else
              p->fillRect(contentRect.left() + pstep, contentRect.top(), width, contentRect.height(),
                          QBrush(progOpt->palette.highlight().color()));

            return;
          }
        if(QGuiApplication::primaryScreen()->depth() > 8)
          {
            QColor gradColour = progOpt->palette.highlight().color();
            QLinearGradient grad = QLinearGradient(contentRect.left(), contentRect.top(), contentRect.right(), contentRect.bottom());
            if(inverted)
              {
                grad.setStart(contentRect.right(), contentRect.top());
                grad.setFinalStop(contentRect.left(), contentRect.top());
              }
            else{
                grad.setStart(contentRect.left(), contentRect.top());
                grad.setFinalStop(contentRect.right(), contentRect.top());
              }
            grad.setColorAt(1, gradColour.lighter(150));
            grad.setColorAt(0, gradColour.darker(150));

            if(inverted)
              {
                p->fillRect(contentRect.right() -width, contentRect.top(),
                            width, contentRect.height(), QBrush(grad));
              }
            else{
                p->fillRect(contentRect.left(), contentRect.top(), width, contentRect.height(), QBrush(grad));
              }
          }
        else{
            if(inverted)
              {
                p->fillRect(contentRect.right() - width, contentRect.top(), width, contentRect.height(), QBrush(progOpt->palette.highlight().color()));
              }
            else
              {
                p->fillRect(contentRect.left(), contentRect.top(), width, contentRect.height(), QBrush(progOpt->palette.highlight().color()));
              }
          }

      }
  }
  void QLegacyStyle::drawVerticalProgressBar(const QStyleOption* opt, QPainter* p, const QWidget* widget) const
  {
    const QStyleOptionProgressBar *progOpt = qstyleoption_cast<const QStyleOptionProgressBar *>(opt);
    int max, prog, min;
    max = progOpt->maximum;
    min = progOpt->minimum;
    prog = progOpt->progress;
    double currentStep = prog -min;
    int totalSteps = max-min;
    QRect contentRect = subElementRect(QStyle::SE_ProgressBarContents, opt, widget);
    bool inverted = (progOpt->invertedAppearance && !(progOpt->direction == Qt::RightToLeft)) || (!progOpt->invertedAppearance && progOpt->direction == Qt::RightToLeft);
    // the layout is inverted if we are not RTL but invertedAppearance is true or if we're RTL and invertedAppearance is false
    if(!contentRect.isValid()) return;

    if(currentStep > 0 || totalSteps == 0)
      {
        double pg = currentStep/totalSteps;
        int height = qMin((double)contentRect.height(), pg*contentRect.height());

        if(totalSteps == 0)
          {
            //busy indicator;

            // Width < 1 useless for a busy indicator
            if(height < 1) height = 1;

            //never disappear completely
            int remainingHeight = contentRect.height() - height;
            // remainingWidth < 1 can cause crashes;
            if(remainingHeight <= 0) remainingHeight = 1;
            int pstep = (int)currentStep %(2*remainingHeight);
            if(pstep > remainingHeight)
              {
                pstep = - (pstep - 2 * remainingHeight );
              }
            if(inverted)
              {
                p->fillRect(contentRect.left(), contentRect.top()+pstep, contentRect.width(), height,
                            QBrush(progOpt->palette.highlight().color()));
              }
            else
              p->fillRect(contentRect.left(), contentRect.bottom() - height - pstep, contentRect.width(), height,
                          QBrush(progOpt->palette.highlight().color()));

            return;
          }
        if(QGuiApplication::primaryScreen()->depth() > 8)
          {
            QColor gradColour = progOpt->palette.highlight().color();
            QLinearGradient grad = QLinearGradient(contentRect.left(), contentRect.top(), contentRect.right(), contentRect.bottom());
            if(inverted)
              {
                grad.setStart(contentRect.left(), contentRect.top());
                grad.setFinalStop(contentRect.left(), contentRect.bottom());
              }
            else{
                grad.setStart(contentRect.left(), contentRect.bottom());
                grad.setFinalStop(contentRect.left(), contentRect.top());
              }
            grad.setColorAt(1, gradColour.lighter(150));
            grad.setColorAt(0, gradColour.darker(150));

            if(inverted)
              {
                p->fillRect(contentRect.left(), contentRect.top(),
                            contentRect.width(), height, QBrush(grad));
              }
            else{
                p->fillRect(contentRect.left(), contentRect.bottom() - height , contentRect.width(), height, QBrush(grad));
              }
          }
        else{
            if(inverted)
              {
                p->fillRect(contentRect.left(), contentRect.top(), contentRect.width(), height, QBrush(progOpt->palette.highlight().color()));
              }
            else
              {
                p->fillRect(contentRect.left(), contentRect.bottom() - height, contentRect.width(), height, QBrush(progOpt->palette.highlight().color()));
              }
          }

      }
  }



  void QLegacyStyle::drawProgressBarLabel(const QStyleOption* opt, QPainter* p, const QWidget* widget) const
  {
    const QStyleOptionProgressBar* progOpt = qstyleoption_cast<const QStyleOptionProgressBar *>(opt);
    if(!progOpt) return;
    if (progOpt->minimum == 0 && progOpt->maximum == 0)
      return;

    p->save();

    QRect rect = progOpt->rect;
    QRect leftRect;
    int steps = progOpt->maximum - progOpt->minimum;
    int currentStep = progOpt->progress - progOpt->minimum;
    QFont font;
    font.setBold(true);
    p->setFont(font);
    p->setPen(progOpt->palette.text().color());

    bool vertical = (progOpt->orientation == Qt::Vertical);
    bool inverted = progOpt->invertedAppearance;
    bool bottomToTop = progOpt->bottomToTop;

    if (vertical) {
        rect = QRect(rect.left(), rect.top(), rect.height(), rect.width()); // flip width and height
        QTransform m;
        if (bottomToTop) {
            m.translate(0.0, rect.width());
            m.rotate(270);
          } else {
            m.translate(rect.height(), 0.0);
            m.rotate(90);
          }
        p->setTransform(m, true);
      }
    double pg = (steps == 0) ? 1.0 : currentStep / steps;
    int progressIndicatorPos = pg * rect.width();

    bool flip = (!vertical && (((progOpt->direction == Qt::RightToLeft) && !inverted)
                               || ((progOpt->direction == Qt::LeftToRight) && inverted))) || (vertical && ((!inverted && !bottomToTop) || (inverted && bottomToTop)));
    if (flip) {
        int indicatorPos = rect.width() - progressIndicatorPos;
        if (indicatorPos >= 0 && indicatorPos <= rect.width()) {
            p->setPen(progOpt->palette.base().color());
            leftRect = QRect(rect.left(), rect.top(), indicatorPos, rect.height());
          } else if (indicatorPos > rect.width()) {
            p->setPen(progOpt->palette.text().color());
          } else {
            p->setPen(progOpt->palette.base().color());
          }
      } else {
        if (progressIndicatorPos >= 0 && progressIndicatorPos <= rect.width()) {
            leftRect = QRect(rect.left(), rect.top(), progressIndicatorPos, rect.height());
          } else if (progressIndicatorPos > rect.width()) {
            p->setPen(progOpt->palette.base().color());
          } else {
            p->setPen(progOpt->palette.text().color());
          }
      }

    QRegion rightRect = rect;
    rightRect = rightRect.subtracted(leftRect);
    p->setClipRegion(rightRect);
    p->drawText(rect, progOpt->text, QTextOption(Qt::AlignAbsolute | Qt::AlignHCenter | Qt::AlignVCenter));
    if (!leftRect.isNull()) {
        p->setPen(flip ? progOpt->palette.text().color() : progOpt->palette.base().color());
        p->setClipRect(leftRect);
        p->drawText(rect, progOpt->text, QTextOption(Qt::AlignAbsolute | Qt::AlignHCenter | Qt::AlignVCenter));
      }

    p->restore();
  }

