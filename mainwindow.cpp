#include "mainwindow.h"
#include "ui_mainwindow.h"


#include <QtDebug>
#include <QDateTime>
#include <QMessageBox>
#include <QSqlError>
#include <QTableView>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    if ( QSqlDatabase::contains(CONNECTIONNAME)){
        db = QSqlDatabase::database(CONNECTIONNAME);
    }else{
        db = QSqlDatabase::addDatabase("QSQLITE",CONNECTIONNAME);
    }

    db.setHostName("127.0.0.1");
    db.setDatabaseName(DATABASEFILE);

    model = new CustomModel(this,db);
    query = new QSqlQuery(db);

    if(db.open()){
        bool x = query->exec("CREATE TABLE IF NOT EXISTS Sellings("
                    "Name TEXT,"
                    "SellPrice REAL,"
                    "Timestamp DATETIME DEFAULT CURRENT_TIMESTAMP);");
        if(!x){
            qDebug() << query->executedQuery();
            qDebug() << query->lastError();
        }
        query->finish();
    }else{
        QMessageBox::information(this,"Fatal Error","Failed to connect to database");
        exit(1);
    }



    ///////////////////////////////////////////////////////////////////////////////////

    int startYear = QDate::currentDate().year();
    for(int i = startYear-10,j=0; i < startYear + 10 ; i++,j++){
        ui->year->insertItem(j,QString::number(i));
    }
    ui->year->setCurrentText(QString::number(QDate::currentDate().year()));


    ////////////////////////////////////////////////////////////////////////////////////
    connect(ui->new_button,&QPushButton::clicked,this,&::MainWindow::AddNewItemToTable);
    connect(ui->del_button,&QPushButton::clicked,this,&::MainWindow::DelNewItemToTable);
    connect(ui->refresh_button,&QPushButton::clicked,this,&::MainWindow::Refresh);
    connect(ui->filter,&QLineEdit::textChanged,this,&::MainWindow::Filter);
    connect(ui->Date,&QComboBox::currentTextChanged,this,&::MainWindow::Filter);
    connect(ui->year,&QComboBox::currentTextChanged,this,&::MainWindow::Filter);
    /////////////////////////////////////////////////////////////////////////////////////



    //////////////////////////////////////////////////////////////////////



    model->setTable("Sellings");
    model->setHeaderData(0, Qt::Orientation::Horizontal, tr("Name"));
    model->setHeaderData(1, Qt::Orientation::Horizontal, tr("Price"));
    model->setHeaderData(2, Qt::Orientation::Horizontal, tr("Date Time"));
    model->setEditStrategy(QSqlTableModel::OnRowChange);


    model->select();

    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->setSelectionBehavior(QTableView::SelectionBehavior::SelectRows);
    ui->tableView->setAlternatingRowColors(true);
    ui->tableView->setModel(model);
    ui->tableView->selectRow(0);
}

MainWindow::~MainWindow()
{
    delete model;
    delete query;
    delete ui;
}

void MainWindow::AddNewItemToTable()
{
    int rowsCount = model->rowCount();

    QSqlRecord record = model->record(rowsCount);
    record.setValue("Item","");
    record.setValue("SellPrice",0);
    record.setValue("Timestamp",QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss" ));

    model->insertRecord(rowsCount,record);
    model->select();

    ui->tableView->setFocus(Qt::FocusReason::OtherFocusReason);

    ui->tableView->scrollToBottom();
    ui->tableView->selectRow(rowsCount);



}

void MainWindow::DelNewItemToTable()
{
    int reply = QMessageBox::question(this,"Caution!","Are you sure that you want to delete this record!",
                                      QMessageBox::Yes,
                                      QMessageBox::No);

    if(reply == QMessageBox::Yes){
        int row = ui->tableView->currentIndex().row();
        model->removeRow(row);
        model->select();
    }
}

void MainWindow::Refresh()
{
    model->select();
}

void MainWindow::Filter()
{
    model->setFilter(getFilterString());
}

QString MainWindow::getFilterString() const
{
    QString filterName = ui->filter->text().simplified();
    QString year = ui->year->currentText();
    int date = ui->Date->currentIndex();
    QString nameFilterString = QString("Name Like '\%%1\%'").arg(filterName);

    if(ui->Date->currentIndex() ==0 ){
        return nameFilterString;
    }

    return QString("%1 AND Timestamp BETWEEN '%2-%3-1 00:00:00' AND '%2-%4-1 23:59:59'")
            .arg(nameFilterString,year,QString::number(date),QString::number(date+1));
}
