#include "mainwindow.h"
#include "renderarea.h"
#include "ui_mainwindow.h"
#include <QString>
#include <QtWidgets>
#include <QSvgGenerator>
#include <QSvgWidget>
#include <QTemporaryDir>
#include "qcairopaintdevice.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    renderArea = new RenderArea;
    ui->setupUi(this);
    ui->gridLayout_2->addWidget(renderArea, 0, 0, 1, 4);
    ui->depth_text->setVisible(false);
    ui->depth_lbl->setVisible(false);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_lengthText_textChanged(const QString &arg1)
{

    long double length = ui->lengthText->text().toDouble();
    renderArea->setLength(length);

}

void MainWindow::on_heightText_textChanged(const QString &arg1)
{
    long double height = ui->heightText->text().toDouble();
    renderArea->setHeight(height);
}

void MainWindow::on_widthText_textChanged(const QString &arg1)
{

    long double width = ui->widthText->text().toDouble();
    renderArea->setWidth(width);

}

void MainWindow::on_comboBox_2_currentIndexChanged(int index)
{

    RenderArea::Units units = RenderArea::Units(index);
    renderArea->setUnits(units);

}
//for the downloadSVG
void MainWindow::on_pushButton_clicked()
{

    QString path = QFileDialog::getSaveFileName(this, tr("Save SVG"), tr(".svg"));
    if (path != NULL)
    {
     renderArea->paintCairo(path.toStdString());
}
}

void MainWindow::on_colorText_returnPressed()
{
    /*
renderArea->setPenColour(ui->colorText->text());
*/
}

void MainWindow::on_fing_width_line_returnPressed()
{
    renderArea->setFingWidth(ui->fing_width_line->text().toDouble());
}

void MainWindow::on_thickness_line_returnPressed()
{
    renderArea->setMatThick(ui->thickness_line->text().toDouble());
}

void MainWindow::on_side_combo_activated(int index)
{
    renderArea->setSide(index);


}

void MainWindow::on_fing_slot_check_clicked(bool checked)
{
    renderArea->fingerSlotChecked(checked);
}

void MainWindow::on_comboBox_currentIndexChanged(int index)
{
    RenderArea::Shape shape = RenderArea::Shape(index);
    renderArea->setShape(shape);

    if (index == 0)
    {
        ui->depth_text->setVisible(false);
        ui->depth_lbl->setVisible(false);
    }
    else if (index == 1)    {
        ui->depth_text->setVisible(true);
        ui->depth_lbl->setVisible(true);
    }
}
