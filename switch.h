// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#ifndef SWITCH_H
#define SWITCH_H

//#include <QPropertyAnimation>
#include <QBrush>
#include <QAbstractButton>
#include <QPainter>
#include <QMouseEvent>
#include <QtDebug>



class Switch : public QAbstractButton {
    Q_OBJECT
    //Q_PROPERTY(int offset READ offset WRITE setOffset)

public:
    explicit Switch(QWidget *parent = nullptr);
    explicit Switch(const QBrush &brush, QWidget *parent = nullptr);

    QSize sizeHint() const override;

    QBrush brush() const {
        return _brush;
    }
    void setBrush(const QBrush &brsh) {
        _brush = brsh;
    }

    int offset() const {
        return _x;
    }
    void setOffset(int o) {
        _x = o;
        update();
    }

public slots:
    void setChecked(bool b);
    bool isChecked();

protected:
    void paintEvent(QPaintEvent *) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;

private:
    int _height;
    qreal _opacity;
    bool _switch;
    int _margin;
    int _x;
    int _y;
    QBrush _brush;
    //QBrush _thumb, _track, _brush;
    //QPropertyAnimation *_anim = nullptr;
};
#endif // SWITCH_H
