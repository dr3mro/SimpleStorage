#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QResizeEvent>

#include "custommodel.h"

#define CONNECTIONNAME "con1"
#define DATABASEFILE "data.db"




QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

    typedef struct{
        QString MonthName;
        QString MonthNum;
    }Month;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QSqlDatabase db;
    QSqlQuery *query;
    CustomModel *model;
    QString getFilterString()const;


    Month months[12] = {{"January","01"},
                        {"February","02"},
                        {"March","03"},
                        {"April","04"},
                        {"May","05"},
                        {"June","06"},
                        {"July","07"},
                        {"August","08"},
                        {"September","09"},
                        {"October","10"},
                        {"November","11"},
                        {"December","12"}};


    void connectSignals();
    void setupDatabase();
    void updateDaysOfMonth();
    void setMonths();
    void setYears();
    void setupModel();
    void calculate();
    void resizeTableColumn();

private slots:
    void AddNewItemToTable();
    void DelNewItemToTable();
    void Refresh();
    void Filter();
    void ResetDate();
    void ToggleDelButton(const QModelIndex &index);

protected:
    void resizeEvent(QResizeEvent * event) override;
    void showEvent(QShowEvent *event) override;

};
#endif // MAINWINDOW_H


