// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "switch.h"

Switch::Switch(QWidget *parent) : QAbstractButton(parent),
_height(16),
_opacity(0.000),
_switch(false),
_margin(3)//,
//_thumb("steelblue"),
//_anim(new QPropertyAnimation(this, "offset", this))
{
    setOffset(_height / 2);
    _y = _height / 2;
    setBrush(QColor("steelblue"));
    setCheckable(true);
    setFocusPolicy(Qt::StrongFocus);
    setCursor(Qt::PointingHandCursor);
//    QString parentObjName = parent->window()->objectName();
//    if ( parentObjName == "printDrugsGeneralTab" )
//        connect(this,SIGNAL(clicked()),parent->window(),SLOT(refreshView()),Qt::QueuedConnection);
}

Switch::Switch(const QBrush &brush, QWidget *parent) : QAbstractButton(parent),
_height(16),
_opacity(0.000),
_switch(false),
_margin(3)//,
//_thumb("#d5d5d5"),
//_anim(new QPropertyAnimation(this, "offset", this))
{
    setOffset(_height / 2);
    _y = _height / 2;
    setBrush(brush);
    setCheckable(true);
    setFocusPolicy(Qt::StrongFocus);
    setCursor(Qt::PointingHandCursor);
//    QString parentObjName = parent->window()->objectName();
//    if ( parentObjName == "printDrugsGeneralTab" )
//        connect(this,SIGNAL(clicked()),parent->window(),SLOT(refreshView()),Qt::QueuedConnection);

}

void Switch::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.setPen(Qt::NoPen);
    int startPoint = _switch ? (width() - _height):offset();

    if (isEnabled()) {
        p.setBrush(_switch ? brush() : Qt::black);
        p.setOpacity(_switch ? 0.5 : 0.38);
        p.setRenderHint(QPainter::Antialiasing, true);
        p.drawRoundedRect(QRect(_margin, _margin, width() - 2 * _margin, height() - 2 * _margin), 8.0, 8.0);
        p.setBrush(_switch? _brush:QBrush(QColor("#d5d5d5")));
        p.setOpacity(1.0);
        p.drawEllipse(QRectF(startPoint - (_height / 2), _y - (_height / 2), height(), height()));
    } else {
        p.setBrush(Qt::black);
        p.setOpacity(0.12);
        p.drawRoundedRect(QRect(_margin, _margin, width() - 2 * _margin, height() - 2 * _margin), 8.0, 8.0);
        p.setOpacity(1.0);
        p.setBrush(QColor("#BDBDBD"));
        p.drawEllipse(QRectF(startPoint - (_height / 2), _y - (_height / 2), height(), height()));
    }
}

void Switch::mouseReleaseEvent(QMouseEvent *e)
{
    if (e->button() & Qt::LeftButton)
        _switch = !_switch;
    QAbstractButton::mouseReleaseEvent(e);
}

//    if (e->button() & Qt::LeftButton)
//    {
//        _switch = !_switch;
//        int start = _height/2;
//        int end = width() - _height;
//        if (!_switch) std::swap(start, end);
//        _thumb = _switch ? _brush : QBrush("#d5d5d5");
//        _anim->setStartValue(start);
//        _anim->setEndValue(end);
//        _anim->setDuration(120);
//        _anim->start();
//    }
//    QAbstractButton::mouseReleaseEvent(e);
//}

void Switch::mouseMoveEvent(QMouseEvent *e) {
    setCursor(Qt::PointingHandCursor);
    QAbstractButton::mouseMoveEvent(e);
}

QSize Switch::sizeHint() const {
    return QSize(2 * (_height + _margin), _height + 2 * _margin);
}

void Switch::setChecked(bool b)
{
    _switch = b;
    QAbstractButton::setChecked(_switch);
}

bool Switch::isChecked()
{
    return _switch;
}
