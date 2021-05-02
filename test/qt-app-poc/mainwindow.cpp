#include <QFileDialog>
#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <iostream>
#include <QImageReader>
#include <QMessageBox>
#include <QColorSpace>

#include <pxsort/Image.h>
#include <pxsort/segmentation/Grid.h>
#include <pxsort/Segment.h>
#include <pxsort/effect/BubbleSort.h>
#include <pxsort/Comparator.h>
#include <pxsort/Mixer.h>

using namespace ps;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
    scrollArea(new QScrollArea)
{
    ui->setupUi(this);
    imageLabel = ui->imageLabel;

    imageLabel->setBackgroundRole(QPalette::Base);
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabel->setScaledContents(true);

    scrollArea->setBackgroundRole(QPalette::Dark);
    scrollArea->setWidget(imageLabel);
    scrollArea->setVisible(false);
    setCentralWidget(scrollArea);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::open() {
    QString fileName = QFileDialog::getOpenFileName(this);
    if (!fileName.isEmpty())
        loadFile(fileName);
}


bool MainWindow::loadFile(const QString& fileName)
{
    std::cout << fileName.toStdString() << std::endl;
    QImageReader reader(fileName);
    reader.setAutoTransform(true);
    const QImage newImage = reader.read();
    if (newImage.isNull()) {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                 tr("Cannot load %1: %2")
                                         .arg(QDir::toNativeSeparators(fileName), reader.errorString()));
        return false;
    }

    setImage(newImage);

    setWindowFilePath(fileName);

    const QString message = tr("Opened \"%1\", %2x%3")
            .arg(QDir::toNativeSeparators(fileName)).arg(image->width).arg(image->height);
    statusBar()->showMessage(message);
    return true;
}

void MainWindow::setImage(const QImage &newImage)
{
    int view_h = imageLabel->height();
    int view_w = imageLabel->width();

    QImage scaledImage = newImage.scaled(view_w, view_h, Qt::AspectRatioMode::KeepAspectRatio);
    int img_h = scaledImage.height();
    int img_w = scaledImage.width();

    Image::ColorSpace colorSpace = ps::Image::RGB;
    std::unique_ptr<uint8_t[]> data(scaledImage.bits());
    image = std::make_shared<Image>(img_w, img_h, colorSpace, std::move(data));

    initSegmentation();

    drawImage();
    scrollArea->setVisible(true);
    imageLabel->adjustSize();
}

void MainWindow::drawImage() {
    auto data = image->to_rgb32();
    QImage qImage(data.get(), image->width, image->height, QImage::Format_RGB32);
    imageLabel->setPixmap(QPixmap::fromImage(qImage));
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
    ChannelSkew skew(0, 0, 0,
                     0, 0, 0);
    SegmentTraversal traversal = FORWARD;
    PixelComparator cmp = comparator::compareChannel(
            ps::comparator::RED,
            ps::comparator::ASCENDING);
    PixelMixer mix = mixer::swapper(ps::mixer::RGB);

    std::unique_ptr<Effect> e =
            std::make_unique<BubbleSort>(skew, traversal, cmp, mix);
    segmentation->addEffect(std::move(e));
}