#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSvgWidget>
class RenderArea;
namespace Ui {

class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


private slots:

    void on_lengthText_textChanged(const QString &arg1);

    void on_heightText_textChanged(const QString &arg1);

    void on_widthText_textChanged(const QString &arg1);

    void on_comboBox_2_currentIndexChanged(int index);

    void on_pushButton_clicked();

    void on_colorText_returnPressed();

    void on_fing_width_line_returnPressed();

    void on_thickness_line_returnPressed();

    void on_side_combo_activated(int index);

    void on_fing_slot_check_clicked(bool checked);

    void on_comboBox_currentIndexChanged(int index);

private:
    Ui::MainWindow *ui;
    RenderArea *renderArea;

    QSvgWidget svgWindow;
};

#endif // MAINWINDOW_H
