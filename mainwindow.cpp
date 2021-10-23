#include "mainwindow.h"
#include "ui_mainwindow.h"


#include <QtDebug>
#include <QDateTime>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    connect(ui->new_button,&QPushButton::clicked,this,&::MainWindow::AddNewItemToTable);
    connect(ui->del_button,&QPushButton::clicked,this,&::MainWindow::DelNewItemToTable);

    if ( QSqlDatabase::contains(CONNECTIONNAME))
    {
        db = QSqlDatabase::database(CONNECTIONNAME);
    }
    else
    {
        db = QSqlDatabase::addDatabase("QSQLITE",CONNECTIONNAME);
    }
    db.setHostName("127.0.0.1");
    db.setDatabaseName(DATABASEFILE);

    if(!db.open()){
        exit(1);
    }


    model = new CustomModel(this,db);

    model->setTable("Sellings");

    model->setHeaderData(0, Qt::Orientation::Horizontal, tr("Name"));
    model->setHeaderData(1, Qt::Orientation::Horizontal, tr("Price"));
    model->setHeaderData(2, Qt::Orientation::Horizontal, tr("Date Time"));

    model->setEditStrategy(QSqlTableModel::OnRowChange);

    model->select();

    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui->tableView->setModel(model);

}

MainWindow::~MainWindow()
{
    delete model;
    delete ui;
}

void MainWindow::AddNewItemToTable()
{
    int rowsCount =model->rowCount();

    QSqlRecord record = model->record(rowsCount);
    record.setValue("Name","");
    record.setValue("SellPrice",0);
    record.setValue("Timestamp",QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss" ));

    model->insertRecord(rowsCount,record);
    model->select();
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
