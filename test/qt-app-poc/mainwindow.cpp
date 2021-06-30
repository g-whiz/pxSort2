#include <QFileDialog>
#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <iostream>
#include <QImageReader>
#include <QMessageBox>
#include <QColorSpace>
#include <QTimer>

#include "Image.h"
#include "segmentation/Grid.h"
#include "Segment.h"
#include "effect/PartialBubbleSort.h"
#include "effect/PartialHeapify.h"
#include "Comparator.h"
#include "Mixer.h"

using namespace pxsort;

/**
 * Framerate in frames-per-second.
 */
#define MAX_FRAMERATE 24.0

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      frameTimer(new QTimer(this))
{
    ui->setupUi(this);
    
    ui->imageLabel->setBackgroundRole(QPalette::Base);
    ui->imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    ui->imageLabel->setScaledContents(true);

    int interval = (1.0 / ((double) MAX_FRAMERATE)) * 1000.0;
    frameTimer->setInterval(interval);
    connect(frameTimer, SIGNAL(timeout()), this, SLOT(stepImageAndDraw()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::open() {
    fileName = QFileDialog::getOpenFileName(this);
    if (!fileName.isEmpty())
        loadFile(fileName);
}


bool MainWindow::loadFile(const QString& imageFileName)
{
    std::cout << imageFileName.toStdString() << std::endl;
    QImageReader reader(imageFileName);
    reader.setAutoTransform(true);
    const QImage newImage = reader.read();
    if (newImage.isNull()) {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                 tr("Cannot load %1: %2")
                                         .arg(QDir::toNativeSeparators(imageFileName), reader.errorString()));
        return false;
    }

    setImage(newImage);

    setWindowFilePath(imageFileName);

    const QString message = tr("Opened \"%1\", %2x%3")
            .arg(QDir::toNativeSeparators(imageFileName)).arg(image->width).arg(image->height);
    statusBar()->showMessage(message);
    return true;
}

void MainWindow::setImage(const QImage &newImage)
{
    int view_h = ui->imageLabel->height();
    int view_w = ui->imageLabel->width();

    QImage scaledImage = newImage.scaled(view_w, view_h, Qt::AspectRatioMode::KeepAspectRatio);
    int img_h = scaledImage.height();
    int img_w = scaledImage.width();

    Image::ColorSpace colorSpace = pxsort::Image::HSV;
    uint8_t * data = scaledImage.bits();
    image = std::make_shared<Image>(img_w, img_h, colorSpace, data);

    initSegmentation();

    drawImage();
    ui->imageLabel->setVisible(true);
    ui->imageLabel->adjustSize();
    frameTimer->start();
}

void MainWindow::drawImage() {
    frameData = image->toRGB32();
    QImage qImage(frameData.get(), image->width, image->height, QImage::Format_RGB32);
    ui->imageLabel->setPixmap(QPixmap::fromImage(qImage));
}

void MainWindow::initSegmentation() {
    if (image == nullptr)
        return;

    int rows = 30;
    int columns = 40;
    int x0 = 0;
    int y0 = 0;

    segmentation = std::make_unique<Grid>(image, rows, columns, x0, y0);

    initEffects();
}

void MainWindow::initEffects() {
    ChannelSkew skew(0, -4, 1,
                     0, 6, -1);
    SegmentTraversal traversal = FORWARD;
    PixelComparator cmp = comparator::channel(
            pxsort::G,
            pxsort::comparator::ASCENDING);
    PixelMixer mix = mixer::swapper(pxsort::mixer::RB);

    std::unique_ptr<Sorter> e =
            std::make_unique<PartialBubbleSort>(skew, traversal, cmp, mix);
    segmentation->addEffect(std::move(e));
}

void MainWindow::stepImageAndDraw() {
    segmentation->applyEffects();
    drawImage();
}
