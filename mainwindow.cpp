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
}

MainWindow::~MainWindow()
{
    delete model;
    delete query;
    delete reg;
    delete ui;
}

void MainWindow::AddNewItemToTable()
{
    int rows = model->rowCount();
    QSqlRecord record = model->record(rows);
    record.setValue("Item","");
    record.setValue("SellPrice",double(0));
    record.setValue("Timestamp",QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss" ));

    model->insertRecord(rows-1,record);

    if(ui->addToDate->isChecked() && ui->calendarWidget->selectedDate() != QDate::currentDate()){
        int id = -1;
        query->clear();
        bool x = query->exec("SELECT MAX(id) FROM Sellings");
        while (query->next()) {

            id = query->value(0).toInt();
        }
        query->finish();
        if (id > 0 && x){
            query->clear();
            QString timestamp = QString("%1 %2").arg(ui->calendarWidget->selectedDate().toString("yyyy-MM-dd"),QTime::currentTime().toString("hh:mm:ss"));
            QString qText = QString("UPDATE Sellings SET Timestamp='%1' WHERE id='%2';").arg(timestamp,QString::number(id));
            query->exec(qText);
            query->finish();
        }else{
            QMessageBox::information(this,tr("Error"),tr("Error 7"));
        }
    }
    model->select();
    ui->tableView->setFocus(Qt::FocusReason::OtherFocusReason);
    ui->tableView->scrollToBottom();

    ui->tableView->setCurrentIndex(model->index(rows, 1));
    ui->new_button->setDisabled(!model->isDirty());




}

void MainWindow::DelNewItemToTable()
{
    QMessageBox messageBox(QMessageBox::Question,
                           tr("Caution!"),
                           tr("Are you sure that you want to delete this record!"),
                           QMessageBox::Yes | QMessageBox::No,
                           this);
    messageBox.setButtonText(QMessageBox::Yes, tr("Yes"));
    messageBox.setButtonText(QMessageBox::No, tr("No"));


    if(messageBox.exec() == QMessageBox::Yes){
        int row = ui->tableView->currentIndex().row();
        model->removeRow(row);
        model->select();
        ui->del_button->setEnabled(false);
    }

}

void MainWindow::Refresh()
{
    model->select();
}

void MainWindow::Filter()
{
    model->setFilter(getFilterString());
    model->select();
    calculate();
    ui->new_button->setEnabled(
                ui->addToDate->isChecked()
             || ui->calendarWidget->selectedDate() == QDate::currentDate());
}

void MainWindow::ToggleDelButton(const QModelIndex &index)
{
    ui->del_button->setEnabled(index.isValid());
}

void MainWindow::TranslateApp(int index)
{
    const QString baseName = "SamarSimple_" + ilocales[index].Locale;
    if (translator.load(":/i18n/" + baseName)) {
        qApp->installTranslator(&translator);
        ui->retranslateUi(this);

        tweakTableView();

        if(ilocales[index].Lang != "عربي"){
            setLayoutDirection(Qt::LayoutDirection::LeftToRight);
            ui->calendarWidget->setLocale(QLocale(QLocale::English,QLocale::UnitedStates));
        }else{
            setLayoutDirection(Qt::LayoutDirection::RightToLeft);
            ui->calendarWidget->setLocale(QLocale(QLocale::Arabic,QLocale::Egypt));
        }
    }
    calculate();
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    resizeTableColumn();
    QMainWindow::resizeEvent(event);
}

void MainWindow::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);
    readRegistery();
    if(isWindowMaximized)
        setWindowState(Qt::WindowMaximized);
    else
        resize(initialWindowWidth,initialWindowHeight);
    ui->new_button->setFocus(Qt::FocusReason::OtherFocusReason);
    ui->calendarWidget->setSelectedDate(QDate::currentDate());

    qApp->processEvents();
    setupDatabase();
    connectSignals();
    setlocales();
    setupModel();
    Filter();
    resizeTableColumn();
    calculate();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(model->isDirty())
        model->submitAll();

    if(!this->isMaximized()){
        reg->setValue(WINDOW_WIDTH,this->width());
        reg->setValue(WINDOW_HEIGHT,this->height());
    }
    reg->setValue(SEARCH_MONTH,ui->searchByMonth->isChecked());
    reg->setValue(CURRENT_LOCALE,ui->setLang->currentIndex());
    reg->setValue(ISWINDOWMAXIMIZED,this->isMaximized());


    QMainWindow::closeEvent(event);
}

QString MainWindow::getFilterString() const
{
    QString filterName = ui->filter->text().simplified();
    int year  = ui->calendarWidget->selectedDate().year();
    int month = ui->calendarWidget->selectedDate().month();
    int day = ui->calendarWidget->selectedDate().day();

    QString nameFilterString = filterName.isEmpty()? "":QString("Name Like '%%1%' AND ").arg(filterName);

    if(ui->searchByMonth->isChecked()){
        return QString("%1 Timestamp BETWEEN '%2-%4-01 00:00:00' AND '%3-%5-01 00:00:00'")
                .arg(nameFilterString,
                     QString::number(year),
                     QString::number(month < 12 ? year:year+1),
                     QString("%1").arg(ui->calendarWidget->selectedDate().month(),2,10,QChar('0')),
                     QString("%1").arg(ui->calendarWidget->selectedDate().addMonths(1).month(),2,10,QChar('0')));
    }else{
        return QString("%1 Timestamp BETWEEN '%2-%3-%4 00:00:00' AND '%2-%3-%4 23:59:59'")
                .arg(nameFilterString,
                     QString::number(year),
                     QString("%1").arg(month,2,10,QChar('0')),
                     QString("%1").arg(day,2,10,QChar('0')));
    }
}

void MainWindow::connectSignals()
{
    connect(ui->new_button,&QPushButton::clicked,this,&::MainWindow::AddNewItemToTable);
    connect(ui->del_button,&QPushButton::clicked,this,&::MainWindow::DelNewItemToTable);
    connect(ui->refresh_button,&QPushButton::clicked,this,&::MainWindow::Refresh);
    connect(ui->filter,&QLineEdit::textChanged,this,&::MainWindow::Filter);
    connect(ui->searchByMonth,&Switch::clicked,this,&::MainWindow::Filter);
    connect(ui->addToDate,&Switch::clicked,this,&::MainWindow::Filter);
    connect(model,&QSqlTableModel::dataChanged,this,&::MainWindow::calculate);
    connect(model,&QSqlTableModel::primeInsert,this,&::MainWindow::calculate);
    connect(model,&QSqlTableModel::dataChanged,this,&::MainWindow::resizeTableColumn);
    connect(model,&QSqlTableModel::primeInsert,this,&::MainWindow::resizeTableColumn);
    connect(ui->resetDate_button,&QPushButton::clicked,this,[=](){
        ui->calendarWidget->setSelectedDate(QDate::currentDate());
    });
    connect(ui->tableView,&QTableView::pressed,this,&::MainWindow::ToggleDelButton);
    connect(ui->setLang,static_cast<void(QComboBox::*)(int)> (&QComboBox::currentIndexChanged),this,&::MainWindow::TranslateApp);
    connect(ui->info_button,&QToolButton::clicked,this,[=](){
        QMessageBox::information(this,tr("About"),tr("This app is being developed by Dr. Amr Osman"));
    });
    connect(ui->tableView->itemDelegate(),&QAbstractItemDelegate::closeEditor,this,[=](){
        ui->new_button->setEnabled(!model->isDirty() || ui->tableView->selectionModel()->currentIndex().column()==3);
        model->submit();
        ui->new_button->setFocus(Qt::FocusReason::OtherFocusReason);
    });

    connect(ui->calendarWidget,&QCalendarWidget::selectionChanged,this,&::MainWindow::Filter);
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
        QMessageBox::information(this,tr("Fatal Error"),tr("Failed to connect to database"));

        QMessageBox messageBox(QMessageBox::Information,
                               tr("Fatal Error"),
                               tr("Failed to connect to database"),
                               QMessageBox::Ok , this);
        messageBox.setButtonText(QMessageBox::Ok, tr("Ok"));

        messageBox.exec();

        exit(1);
    }
}

void MainWindow::setupModel()
{
    model->setTable("Sellings");
    model->select();
    tweakTableView();
}

void MainWindow::setlocales()
{

    for(uint i=0;i< sizeof(ilocales) / sizeof(ilocales[0]) ;i++){
        ui->setLang->insertItem(i,ilocales[i].Lang);
    }
    ui->setLang->setCurrentIndex(currentLocale);
}

void MainWindow::calculate()
{
    double sum = 0;
    int rows = model->rowCount();
    for(int i = 0 ;i < rows; i++ ){
        sum += model->index(i,2).data().toDouble();
    }
    ui->sum->setText(QString::number(sum));
}

void MainWindow::resizeTableColumn()
{
    ui->tableView->setColumnWidth(1,ui->tableView->viewport()->width()*0.66);
    ui->tableView->setColumnWidth(2,ui->tableView->viewport()->width()*0.10);
    ui->tableView->setColumnWidth(3,ui->tableView->viewport()->width()*0.24);
}

void MainWindow::readRegistery()
{
    reg = new QSettings(regPath, QSettings::NativeFormat);
    initialWindowWidth = reg->value(WINDOW_WIDTH).toInt();
    initialWindowHeight = reg->value(WINDOW_HEIGHT).toInt();
    searchMonth = reg->value(SEARCH_MONTH).toBool();
    currentLocale = reg->value(CURRENT_LOCALE).toInt();
    isWindowMaximized = reg->value(ISWINDOWMAXIMIZED).toBool();

    ui->searchByMonth->setChecked(searchMonth);
}

void MainWindow::tweakTableView()
{
    model->setHeaderData(0, Qt::Orientation::Horizontal, tr("ID"));
    model->setHeaderData(1, Qt::Orientation::Horizontal, tr("Item"));
    model->setHeaderData(2, Qt::Orientation::Horizontal, tr("Price"));
    model->setHeaderData(3, Qt::Orientation::Horizontal, tr("Date Time"));
    model->setEditStrategy(QSqlTableModel::OnRowChange);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->setSelectionBehavior(QTableView::SelectionBehavior::SelectRows);
    ui->tableView->setAlternatingRowColors(true);
    ui->tableView->setModel(model);
    ui->tableView->selectRow(0);
    ui->tableView->hideColumn(0);
}
