#include <QApplication>
#include <QPalette>
#include <QColor>

void applyPinkPalette(QApplication &app) {
    QPalette palette;

    palette.setColor(QPalette::Window, QColor("#FFF0F5"));
    palette.setColor(QPalette::WindowText, QColor("#4A2E35"));
    palette.setColor(QPalette::Base, QColor("#FFFFFF"));
    palette.setColor(QPalette::AlternateBase, QColor("#FFE4E1"));

    palette.setColor(QPalette::Text, QColor("#4A2E35"));
    palette.setColor(QPalette::PlaceholderText, QColor("#4A2E35"));

    palette.setColor(QPalette::Button, QColor("#FFB6C1"));
    palette.setColor(QPalette::ButtonText, QColor("#4A2E35"));

    palette.setColor(QPalette::Highlight, QColor("#FF69B4"));
    palette.setColor(QPalette::HighlightedText, QColor("#FFFFFF"));

    palette.setColor(QPalette::Disabled, QPalette::WindowText, QColor("#4A2E35"));
    palette.setColor(QPalette::Disabled, QPalette::Button, QColor("#E0D0D3"));
    palette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor("#4A2E35"));

    app.setPalette(palette);    
}