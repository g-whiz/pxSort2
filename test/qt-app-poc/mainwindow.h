#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QScrollArea>
#include <common.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    bool loadFile(const QString& imageFileName);

private slots:
    void open();
    void drawImage();
    void stepImageAndDraw();

private:
    void setImage(const QImage &newImage);
    void initSegmentation();
    void initEffects();

    QString fileName = "";

    std::unique_ptr<uint8_t[]> frameData;
    std::shared_ptr<pxsort::Image> image;
    std::unique_ptr<pxsort::Segmentation> segmentation;

    Ui::MainWindow *ui;
    QTimer *frameTimer;
};
#endif // MAINWINDOW_H
