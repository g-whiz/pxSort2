#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QScrollArea>
#include <pxsort/common.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    bool loadFile(const QString& filename);

private slots:
    void open();
    void drawImage();

private:
    void setImage(const QImage &newImage);
    void initSegmentation();
    void initEffects();

    std::shared_ptr<ps::Image> image;
    std::unique_ptr<ps::Segmentation> segmentation;

    Ui::MainWindow *ui;
    QLabel *imageLabel;
    QScrollArea *scrollArea;
};
#endif // MAINWINDOW_H
