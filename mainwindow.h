#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>
#include <QSqlRecord>

#include "custommodel.h"

#define CONNECTIONNAME "con1"
#define DATABASEFILE "data.db"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    CustomModel *model;
    QSqlDatabase db;


private slots:
    void AddNewItemToTable();
    void DelNewItemToTable();

};
#endif // MAINWINDOW_H


