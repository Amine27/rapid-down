#ifndef TRAYICON_H
#define TRAYICON_H

#include <QtGui>

class RapidDown;

class TrayIcon : public QWidget
{
    Q_OBJECT

public:
    TrayIcon(RapidDown* parent);
    void setTrayIconStats(QString fileName, QString speed, QString eta, QString downloaded, QString progress);
    void finished(QString fileName, QString DownloadedSize);
    void Allfinished(int downloadedUrls);
    void dayLimitGbReached();

    QSystemTrayIcon *trayIcon;

private slots:
    void iconActivated( QSystemTrayIcon::ActivationReason reason );

private:
    void createTrayIconMenu();

    QMenu *trayIconMenu;
    RapidDown* m_rapidDown;
};

#endif // TRAYICON_H
