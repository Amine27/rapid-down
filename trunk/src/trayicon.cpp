#include "trayicon.h"
#include "RapidDown.h"

TrayIcon::TrayIcon(RapidDown* parent) : QWidget()
{
    m_rapidDown = parent;

    createTrayIconMenu();

    trayIcon = new QSystemTrayIcon();
    trayIcon->setContextMenu( trayIconMenu );

    connect( trayIcon, SIGNAL( activated( QSystemTrayIcon::ActivationReason ) ), this, SLOT( iconActivated( QSystemTrayIcon::ActivationReason ) ) );

    trayIcon->setIcon( QIcon( ":/data/rapid-down.png" ) );
    setTrayIconStats("", "0", "00:00", "0MB", "0");
    trayIcon->show();
}

void TrayIcon::createTrayIconMenu()
{
    trayIconMenu = new QMenu(this);

    trayIconMenu->addAction( m_rapidDown->startDownAct );
    trayIconMenu->addAction( m_rapidDown->abortDownAct );
    trayIconMenu->addSeparator();
    trayIconMenu->addAction( m_rapidDown->loadAct );
    trayIconMenu->addAction( m_rapidDown->saveAct );
    trayIconMenu->addAction( m_rapidDown->editListAct );
    trayIconMenu->addAction( m_rapidDown->optionsAct );
    trayIconMenu->addSeparator();
    trayIconMenu->addAction( m_rapidDown->quitAct );
}

void TrayIcon::iconActivated( QSystemTrayIcon::ActivationReason reason )
{
    switch ( reason )
    {
        case QSystemTrayIcon::DoubleClick:
        case QSystemTrayIcon::Trigger:
            if ( !m_rapidDown->isVisible() ) m_rapidDown->showNormal();
            else m_rapidDown->hide();
            break;
        case QSystemTrayIcon::Context:
        case QSystemTrayIcon::MiddleClick:
        case QSystemTrayIcon::Unknown:
            break;
    }
}

void TrayIcon::setTrayIconStats(QString fileName, QString speed, QString eta, QString downloaded, QString progress)
{
    QString tip = QString("<table cellpadding='2' cellspacing='2' align='center'><tr><td colspan='2'>Current: <font color='#1c9a1c'>%1</font></td><td></td></tr><tr><td>Speed: <font color='#1c9a1c'>%2</font></td><td>Finshing in: <font color='#1c9a1c'>%3</font></td></tr><tr><td>Download: <font color='#1c9a1c'>%4</font></td><td>Progress: <font color='#1c9a1c'>%5</font></td></tr></table>").arg(
            fileName,
            speed+"b/s",
            eta,
            downloaded,
            progress+"%");

    trayIcon->setToolTip( tip );
}

void TrayIcon::finished(QString fileName, QString DownloadedSize)
{
    QString msg = QString("%1 has completed downloading."
                    "\n%2").arg(
                    fileName,
                    DownloadedSize);

    trayIcon->showMessage("Finished", msg);
}

void TrayIcon::Allfinished(int downloadedUrls)
{
    QString msg = QString("All downloads has been finished (%1 files).").arg(
            downloadedUrls);

    trayIcon->showMessage("Finished", msg);
}

void TrayIcon::dayLimitGbReached()
{
    QString msg = "Day download limit was reached.";

    trayIcon->showMessage("Limit reached", msg, QSystemTrayIcon::Warning);
}
