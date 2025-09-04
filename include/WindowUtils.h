#ifndef WINDOWUTILS_H
#define WINDOWUTILS_H

#include <QtWidgets/QWidget>
#include <QtCore/QSettings>
#include <QtGui/QScreen>
#include <QtWidgets/QApplication>

namespace WindowUtils {

    // Centers child window over parent (or screen if no parent)
    inline void centerOnParent(QWidget* child, QWidget* parent, QSize preferredSize = QSize(400, 300)) {
        if (!child) return;

        if (preferredSize.isValid() && !child->isVisible()) {
            child->resize(preferredSize);
        }

        QRect targetGeometry;
        if (parent) {
            targetGeometry = parent->geometry();
        }
        else {
            targetGeometry = QApplication::primaryScreen()->availableGeometry();
        }

        QRect geom = child->geometry();
        geom.moveCenter(targetGeometry.center());
        child->setGeometry(geom);
    }

    // Saves window geometry into QSettings
    inline void saveWindowGeometry(QWidget* w, const QString& key) {
        if (!w) return;
        QSettings settings;
        settings.beginGroup("Windows");
        settings.setValue(key + "/geometry", w->saveGeometry());
        settings.endGroup();
    }

    // Restores window geometry, falls back to centering if none stored
    inline void restoreWindowGeometry(QWidget* w, const QString& key, QSize defaultSize = QSize(400, 300)) {
        if (!w) return;
        QSettings settings;
        settings.beginGroup("Windows");
        QByteArray g = settings.value(key + "/geometry").toByteArray();
        settings.endGroup();

        if (!g.isEmpty()) {
            w->restoreGeometry(g);
        }
        else {
            centerOnParent(w, w->parentWidget(), defaultSize);
        }
    }

} // namespace WindowUtils

#endif