#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

QHash<QString, QVariantList> MainWindow::extractTable(QSqlQuery *query, int from, int length)
{
    QSqlRecord rec = query->record();
    QHash<QString,QVariantList> result;

    for( int i=0 ; i<rec.count() ; i++ )
    {
        QVariantList list;

        query->seek( from-1 );
        for( int j=0; (length != -1 && j<length && query->next()) || (length == -1 && query->next()) ; j++ )
            list << query->value(i);

        result.insert( rec.fieldName(i) , list );
    }

    return result;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_toolButton_clicked()
{
    QString filepath = QFileDialog::getOpenFileName(this,QString("Select Database"),ui->dbaddress->text());
    if (filepath.isEmpty())
        return;
    ui->dbaddress->setText(filepath);
}

void MainWindow::on_pushButton_clicked()
{
    db = QSqlDatabase::addDatabase("QSQLITE",QString("conn"));
    db.setDatabaseName(ui->dbaddress->text());
    db.open();
    if(!db.isOpen())
        QMessageBox::warning(this,"Error opening database","Error on opening "+ui->dbaddress->text());
    else
        QMessageBox::information(this,"Success","Database load successfully");
}

void MainWindow::on_pushButton_2_clicked()
{

}



void MainWindow::on_pushButton_3_clicked()
{
    QString queryt = ui->queryText->toPlainText();

    QSqlQuery query(db);
    if(!query.exec(queryt))
        QMessageBox::warning(this,"Error Exec",query.lastError().text());
    else{
        QHash<QString,QVariantList> results = extractTable(&query,0,-1);
        ui->queryResult->clear();
        if (results.isEmpty()){
            ui->queryResult->append("Empty Result");
        }else{
            QStringList keys = results.keys();
            int length = results[keys.at(0)].length();
            QString t;
            for(int i=0;i<keys.length();i++)
                t = t + QString("--%1--").arg(keys[i]);
            ui->queryResult->append(t);
            for(int i=0;i<length;i++){
                QString t;
                for(int j=0;j<keys.length();j++){
                    t = t + QString("--%1--").arg(results[keys[j]].at(i).toString());
                }
                ui->queryResult->append(t);
            }
        }
    }
}
