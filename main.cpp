/* 
 *  VieSched++ Very Long Baseline Interferometry (VLBI) Scheduling Software
 *  Copyright (C) 2018  Matthias Schartner
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "version.h"
#ifndef VIESCHEDPP_VERSION
#define VIESCHEDPP_VERSION "unknown"
#endif
#ifndef VIESCHEDPP_GUI_VERSION
#define VIESCHEDPP_GUI_VERSION "unknown"
#endif

#include "mainwindow.h"
#include <QApplication>
#include <QString>
#include <QProxyStyle>
#include <QPainter>
#include <QStyleOption>

#include <QProxyStyle>
#include <QPainter>
#include <QStyleOption>

#include <QProxyStyle>
#include <QPainter>
#include <QStyleOption>

class UnifiedCheckStyle : public QProxyStyle {
public:
    using QProxyStyle::QProxyStyle;

    int pixelMetric(PixelMetric metric, const QStyleOption *option,
                    const QWidget *widget) const override
    {
        // Checkboxes and radios sizes
        if (metric == PM_IndicatorWidth || metric == PM_IndicatorHeight)
            return 20;  // checkbox
        if (metric == PM_ExclusiveIndicatorWidth || metric == PM_ExclusiveIndicatorHeight)
            return 24;  // radio
        return QProxyStyle::pixelMetric(metric, option, widget);
    }

    void drawPrimitive(PrimitiveElement element, const QStyleOption *option,
                       QPainter *painter, const QWidget *widget = nullptr) const override
    {
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing, true);

        switch (element) {
        case PE_IndicatorCheckBox:
            drawCheckBox(option, painter);
            break;
        case PE_IndicatorRadioButton:
            drawRadioButton(option, painter);
            break;
        default:
            painter->restore();
            QProxyStyle::drawPrimitive(element, option, painter, widget);
            return;
        }

        painter->restore();
    }

private:
    void drawCheckBox(const QStyleOption *option, QPainter *painter) const
    {
        QRect r = option->rect.adjusted(2, 2, -2, -2);

        // Determine base colors and pen width
        bool enabled = option->state & State_Enabled;
        QColor borderColor = enabled ? Qt::black : QColor(160, 160, 160);
        QColor fillColor = enabled ? Qt::white : QColor(245, 245, 245);
        qreal borderWidth = enabled ? 1.4 : 1.0;

        // --- Interaction feedback ---
        if (enabled) {
            if (option->state & State_Sunken) {
                fillColor = QColor(220, 220, 220);
                borderColor = QColor(60, 60, 60);
            } else if (option->state & State_MouseOver) {
                fillColor = QColor(240, 240, 240);
            }
        }

        // Outline
        painter->setPen(QPen(borderColor, borderWidth));
        painter->setBrush(fillColor);
        painter->drawRoundedRect(r, 4, 4);

        // --- Checked ---
        if (option->state & State_On) {
            QColor accent(enabled ? "#0078d7" : "#a0a0a0");
            painter->setBrush(accent);
            painter->setPen(QPen(borderColor, borderWidth));
            painter->drawRoundedRect(r, 4, 4);

            // Checkmark ✓
            QColor tick = enabled ? Qt::white : QColor(230, 230, 230);
            painter->setPen(QPen(tick, 2.6, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            QPainterPath path;
            path.moveTo(r.left() + r.width() * 0.23, r.center().y());
            path.lineTo(r.center().x() - 1, r.bottom() - r.height() * 0.25);
            path.lineTo(r.right() - r.width() * 0.22, r.top() + r.height() * 0.25);
            painter->drawPath(path);
        }

        // --- Partially checked ---
        if (option->state & State_NoChange) {
            painter->setBrush(QColor(enabled ? "#0078d7" : "#a0a0a0"));
            painter->setPen(Qt::NoPen);
            painter->drawRoundedRect(r, 4, 4);
            painter->setPen(QPen(Qt::white, 2));
            painter->drawLine(r.left() + 3, r.center().y(),
                              r.right() - 3, r.center().y());
        }
    }

    void drawRadioButton(const QStyleOption *option, QPainter *painter) const
    {
        QRect r = option->rect.adjusted(2, 2, -2, -2);

        bool enabled = option->state & State_Enabled;
        QColor borderColor = enabled ? Qt::black : QColor(160, 160, 160);
        QColor fillColor = enabled ? Qt::white : QColor(245, 245, 245);
        qreal borderWidth = enabled ? 1.4 : 1.0;

        // --- Interaction feedback ---
        if (enabled) {
            if (option->state & State_Sunken) {
                fillColor = QColor(220, 220, 220);
                borderColor = QColor(60, 60, 60);
            } else if (option->state & State_MouseOver) {
                fillColor = QColor(240, 240, 240);
            }
        }

        // Outer circle
        painter->setPen(QPen(borderColor, borderWidth));
        painter->setBrush(fillColor);
        painter->drawEllipse(r);

        // --- Checked inner dot ---
        if (option->state & State_On) {
            QColor accent(enabled ? "#0078d7" : "#a0a0a0");
            QRect inner = r.adjusted(r.width() * 0.28, r.height() * 0.28,
                                     -r.width() * 0.28, -r.height() * 0.28);
            painter->setBrush(accent);
            painter->setPen(Qt::NoPen);
            painter->drawEllipse(inner);
        }
    }
};

void setAlwaysLightPalette(QApplication &app)
{
    QPalette lightPalette;

    // Base colors
    lightPalette.setColor(QPalette::Window, QColor(255, 255, 255));
    lightPalette.setColor(QPalette::WindowText, QColor(0, 0, 0));
    lightPalette.setColor(QPalette::Base, QColor(255, 255, 255));
    lightPalette.setColor(QPalette::AlternateBase, QColor(245, 245, 245)); // used for alternating rows
    lightPalette.setColor(QPalette::ToolTipBase, QColor(255, 255, 220));
    lightPalette.setColor(QPalette::ToolTipText, QColor(0, 0, 0));
    lightPalette.setColor(QPalette::Text, QColor(0, 0, 0));
    lightPalette.setColor(QPalette::Button, QColor(240, 240, 240));
    lightPalette.setColor(QPalette::ButtonText, QColor(0, 0, 0));
    lightPalette.setColor(QPalette::BrightText, QColor(255, 0, 0));

    // Highlight (selection)
    lightPalette.setColor(QPalette::Highlight, QColor(76, 163, 224)); // nice blue
    lightPalette.setColor(QPalette::HighlightedText, QColor(255, 255, 255));

    // Disabled state
    lightPalette.setColor(QPalette::Disabled, QPalette::Text, QColor(120, 120, 120));
    lightPalette.setColor(QPalette::Disabled, QPalette::WindowText, QColor(120, 120, 120));
    lightPalette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(120, 120, 120));
    lightPalette.setColor(QPalette::Disabled, QPalette::Highlight, QColor(200, 200, 200));
    lightPalette.setColor(QPalette::Disabled, QPalette::HighlightedText, QColor(100, 100, 100));

    // Placeholder text (Qt 6 only)
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    lightPalette.setColor(QPalette::PlaceholderText, QColor(127, 127, 127));
#endif

    app.setPalette(lightPalette);
}


int main(int argc, char *argv[])
{
    int result = 0;

    do{
        QApplication app(argc, argv);
        QApplication::setStyle(new UnifiedCheckStyle("Basic"));
        setAlwaysLightPalette(app);

        MainWindow w;
        w.show();
        result = app.exec();
    }while(result == 1000);

    return result;
}
