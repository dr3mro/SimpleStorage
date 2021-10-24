// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#ifndef SERVICEMODEL_H
#define SERVICEMODEL_H

#include <QObject>
#include <QEvent>
#include <QtSql/QSqlTableModel>


class CustomModel : public QSqlTableModel
{
public:
    explicit CustomModel(QObject *parent = Q_NULLPTR, QSqlDatabase db = QSqlDatabase());
    virtual Qt::ItemFlags flags(const QModelIndex &index) const override;

protected:

};

#endif // SERVICEMODEL_H
