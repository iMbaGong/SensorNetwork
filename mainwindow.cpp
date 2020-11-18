#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    graph = new bool*[8];
    for(int i=0;i<8;i++){
        graph[i] = new bool[12];
    }
    QObject* object;
    QString name;
    foreach(object,ui->sensorNet->children()){
        QAbstractButton* btn = dynamic_cast<QAbstractButton*>(object);
        name = object->objectName();
        if(name.startsWith("radioButton")){
            QObject::connect(btn,SIGNAL(clicked(bool)),this,SLOT(sensorClicked(bool)));
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
    delete[] graph;
}

void MainWindow::sensorClicked(bool checked)
{
    QString name = sender()->objectName();
    QString indexS = name.split("_")[1];
    int index = indexS.toInt()-1;
    int rol = index/12;
    int col = index%12;
    graph[rol][col] = checked;
}




