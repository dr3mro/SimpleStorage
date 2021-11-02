#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QResizeEvent>
#include <QCloseEvent>
#include <QSettings>
#include <QTranslator>
#include "custommodel.h"

#define CONNECTIONNAME "con1"
#define DATABASEFILE "data.db"

#define  WINDOW_WIDTH "Window_Width"
#define  WINDOW_HEIGHT "Window_Height"
#define  SEARCH_MONTH "Search_Month"
#define  CURRENT_LOCALE "Current_Locale"
#define  ISWINDOWMAXIMIZED "is_Window_Maximized"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

    typedef struct{
        QString Lang;
        QString Locale;
    }iLocale;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:

    const QString regPath = QStringLiteral("HKEY_CURRENT_USER\\SOFTWARE\\AccountantApp\\");

    Ui::MainWindow *ui;
    QSqlDatabase db;
    QSqlQuery *query;
    CustomModel *model;
    QSettings *reg;
    QSettings settings;
    QTranslator translator;

    int initialWindowWidth=800;
    int initialWindowHeight=600;

    bool searchMonth=false;
    int currentLocale=0;
    bool isWindowMaximized=false;

    QString getFilterString()const;

    iLocale ilocales[2] = {
        {"عربي","ar_EG"},
        {"English","en_US"}
    };

    void connectSignals();
    void setupDatabase();
    void setupModel();
    void setlocales();
    void calculate();
    void resizeTableColumn();
    void readRegistery();
    inline void tweakTableView();

private slots:
    void AddNewItemToTable();
    void DelNewItemToTable();
    void Refresh();
    void Filter();
    void ToggleDelButton(const QModelIndex &index);
    void TranslateApp(int index);

protected:
    void resizeEvent(QResizeEvent * event) override;
    void showEvent(QShowEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

};
#endif // MAINWINDOW_H


