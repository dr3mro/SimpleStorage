#include "mainwindow.h"
#include "ui_mainwindow.h"


#include <QtDebug>
#include <QDateTime>
#include <QMessageBox>
#include <QSqlError>
#include <QTableView>
#include <QCalendar>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    resize(800,600);
    setupDatabase();
    setYears();
    setMonths();
    updateDaysOfMonth();
    ResetDate();
    setupModel();
    connectSignals();
    calculate();
    resizeTableColumn();
}

MainWindow::~MainWindow()
{
    if(model->isDirty())
        model->submitAll();

    delete model;
    delete query;
    delete ui;
}

void MainWindow::AddNewItemToTable()
{

    int rowsCount = model->rowCount();

    QSqlRecord record = model->record(-1);
    record.setValue("Item","");
    record.setValue("SellPrice",double(0));
    record.setValue("Timestamp",QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss" ));
    model->insertRecord(-1,record);
    model->select();
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
    //qDebug() << getFilterString();
    model->setFilter(getFilterString());
    calculate();
}

void MainWindow::ResetDate()
{
    ui->day->setCurrentIndex(QDate::currentDate().day()-1);
    ui->month->setCurrentIndex(QDate::currentDate().month()-1);
    ui->year->setCurrentText(QString::number(QDate::currentDate().year()));
}

void MainWindow::ToggleDelButton(const QModelIndex &index)
{
    ui->del_button->setEnabled(index.isValid());
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    resizeTableColumn();
    QWidget::resizeEvent(event);
}

void MainWindow::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    resizeTableColumn();
}

QString MainWindow::getFilterString() const
{
    QString filterName = ui->filter->text().simplified();
    QString year = ui->year->currentText();
    int month = ui->month->currentIndex();
    int day = ui->day->currentIndex()+1;
    QString nameFilterString = filterName.isEmpty()? "":QString("Name Like '%%1%' AND ").arg(filterName);

    Q_ASSERT(!year.isEmpty());
    Q_ASSERT(day > 0);
    Q_ASSERT(month > 0 && month <=12);

    if(ui->searchByMonth->isChecked()){
        return QString("%1 Timestamp BETWEEN '%2-%3-1 00:00:00' AND '%2-%4-1 23:59:59'")
                .arg(nameFilterString,year,months[month].MonthNum,months[month+1].MonthNum);
    }else{
        return QString("%1 Timestamp BETWEEN '%2-%3-%4 00:00:00' AND '%2-%3-%4 23:59:59'")
                .arg(nameFilterString,
                     year,
                     months[month].MonthNum,
                     QString::number(day));
    }
}

void MainWindow::connectSignals()
{
    connect(ui->new_button,&QPushButton::clicked,this,&::MainWindow::AddNewItemToTable);
    connect(ui->del_button,&QPushButton::clicked,this,&::MainWindow::DelNewItemToTable);
    connect(ui->refresh_button,&QPushButton::clicked,this,&::MainWindow::Refresh);
    connect(ui->filter,&QLineEdit::textChanged,this,&::MainWindow::Filter);
    connect(ui->day,&QComboBox::currentTextChanged,this,&::MainWindow::Filter);
    connect(ui->month,&QComboBox::currentTextChanged,this,&::MainWindow::Filter);
    connect(ui->year,&QComboBox::currentTextChanged,this,&::MainWindow::Filter);
    connect(ui->searchByMonth,&Switch::clicked,this,&::MainWindow::Filter);
    connect(model,&QSqlTableModel::dataChanged,this,&::MainWindow::calculate);
    connect(model,&QSqlTableModel::primeInsert,this,&::MainWindow::calculate);
    connect(model,&QSqlTableModel::dataChanged,this,&::MainWindow::resizeTableColumn);
    connect(model,&QSqlTableModel::primeInsert,this,&::MainWindow::resizeTableColumn);
    connect(ui->resetDate_button,&QPushButton::clicked,this,&::MainWindow::ResetDate);
    connect(ui->tableView,&QTableView::pressed,this,&::MainWindow::ToggleDelButton);
}

void MainWindow::setupDatabase()
{
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
                             "id INTEGER NOT NULL PRIMARY KEY,"
                             "Name TEXT,"
                             "SellPrice REAL,"
                             "Timestamp DATETIME DEFAULT (datetime('now','localtime')));");
        if(!x){
            qDebug() << query->executedQuery();
            qDebug() << query->lastError();
        }
        query->finish();
    }else{
        QMessageBox::information(this,"Fatal Error","Failed to connect to database");
        exit(1);
    }
}

void MainWindow::updateDaysOfMonth()
{
    ui->day->clear();
    QCalendar cal;
    int daysOfMonth = cal.daysInMonth(ui->month->currentIndex() + 1,ui->year->currentText().toInt());

    for(int i=1;i<=daysOfMonth;i++){
        ui->day->addItem(QString::number(i));
    }
}

void MainWindow::setMonths()
{
    for(int i=0;i<12;i++){
        ui->month->insertItem(i,months[i].MonthName);
    }
}

void MainWindow::setYears()
{
    int startYear = QDate::currentDate().year();

    for(int i = startYear-6,j=0; i <= startYear ; i++,j++){
        ui->year->insertItem(j,QString::number(i));
    }
}

void MainWindow::setupModel()
{
    model->setTable("Sellings");
    model->setHeaderData(0, Qt::Orientation::Horizontal, tr("ID"));
    model->setHeaderData(1, Qt::Orientation::Horizontal, tr("Name"));
    model->setHeaderData(2, Qt::Orientation::Horizontal, tr("Price"));
    model->setHeaderData(3, Qt::Orientation::Horizontal, tr("Date Time"));
    model->setEditStrategy(QSqlTableModel::OnRowChange);

    model->select();

    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->setSelectionBehavior(QTableView::SelectionBehavior::SelectRows);
    ui->tableView->setAlternatingRowColors(true);
    ui->tableView->setModel(model);
    ui->tableView->selectRow(0);
    ui->tableView->hideColumn(0);
}

void MainWindow::calculate()
{
    double sum = 0;
    int rows = model->rowCount();
    for(int i = 0 ;i < rows; i++ ){
        sum += model->index(i,2).data().toDouble();
    }
    ui->sum->setText(QString("%1 LE").arg(sum));
}

void MainWindow::resizeTableColumn()
{
    ui->tableView->setColumnWidth(1,ui->tableView->viewport()->width()-160);
    ui->tableView->setColumnWidth(2,40);
    ui->tableView->setColumnWidth(3,110);
}