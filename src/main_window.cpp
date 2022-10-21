#include "main_window.h"
#include "ui_main_window.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->actionOpenPointCloud, SIGNAL(triggered()), SLOT(selectFilePressed()));
    connect(ui->actionCalibration, SIGNAL(triggered()), SLOT(calibrationPressed()));
    // connect(ui->close, SIGNAL(triggered()), SLOT(closeAll()));

    cloud.reset(new pcl::PointCloud<PointT>);

    setAcceptDrops(true);
}

MainWindow::~MainWindow()
{

    if (calibration_window)
    {
        calibration_window.release();
    }
}

void MainWindow::dragEnterEvent(QDragEnterEvent *e)
{
    if (e->mimeData()->hasUrls())
    {
        e->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent *e)
{
    QString firstFileName = e->mimeData()->urls()[0].toLocalFile();
    if (firstFileName.endsWith(".pcd", Qt::CaseSensitive) || firstFileName.endsWith(".ply", Qt::CaseSensitive))
    {
        foreach (const QUrl &url, e->mimeData()->urls())
        {
            QString fileName = url.toLocalFile();
            qDebug() << "Dropped file:" << fileName;
            // TODO verify if is cloud or image
            std::cout << "loading...\n";
            utilities::loadPointCloudFile(fileName.toStdString(), *cloud);
            std::cout << "loaded " << cloud->size() << " points\n";
            ui->GLviewer->setCloud(cloud);
        }
    }

    else
        qDebug() << "format not supported!\n";
}

void MainWindow::setPointCloudFile(const std::string &filename)
{
    if (pcl::io::loadPCDFile(filename, *cloud) == 0)
        ui->GLviewer->setCloud(cloud);
}

void MainWindow::selectFilePressed()
{
    utilities::loadPointCloudDialog(this, *cloud);

    ui->GLviewer->setCloud(cloud);
}

void MainWindow::calibrationPressed()
{
    qDebug() << "Calibration\n";

    calibration_window.reset(new CalibrationWindow);

    calibration_window->show();
}

void MainWindow::closeEvent(QCloseEvent *)
{
    if(calibration_window)
        calibration_window->close();
}