/***************************************************************************
 *   Copyright (C) 2008-2009 by Amine Roukh       <amineroukh@gmail.com>   *
 *   Copyright (C) 2009 by Eduard Kalinowski      <e-h-s@arcor.de>         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <QtGui>
#include <QMainWindow>
#include <QProcess>

#ifndef STRUCT_RAPIDDOWN
#define STRUCT_RAPIDDOWN

struct workList
{
    QString url;
    QString nameToSave;
    QString message;
    short status; // init = 0, err = -1, downloaded = 255, another number - trying nr
};

class EditList;

struct Settings
{
    QString urlListFileName;// integrated
    QString rapidshareUser;// integrated
    QString rapidsharePass;// integrated
    QString homeSettings;// integrated
    QString datum;// integrated

    int speedLimit;// integrated
    int waitTimeErr;// integrated
    int waitTimeNorm;// integrated
    int nrErrors;
    int  rotations;// integrated
    float dayLimitGb;// integrated
    float todayDownloaded;// integrated

    bool autostartDownload; // integrated
    bool autoBeginAfterAdd; // integrated
    bool autoLoadList; // integrated
    bool autoStartMinimized;
    bool confirmExit; // integrated
    bool confirmHide; // integrated
    bool confirmDelete; // integrated
    bool showPopups; // integrated
    bool delCompletFiles; // integrated
    bool delCorruptFiles; // integrated
    bool delNotExists; // integrated
    bool reDonlSizeErr;
    //bool rAccount; // if not, then free download
};

#endif

#ifndef HEADER_RAPIDDOWN
#define HEADER_RAPIDDOWN

#define MAXDOWNLOADS 1

#include "editlist.h"
#include "ui_mainForm.h"

class TrayIcon;

class RapidDown : public QMainWindow, private Ui::MainForm
{
    Q_OBJECT

public :
    RapidDown( QWidget* parent = 0, Qt::WFlags fl = 0 );

protected:
    void closeEvent( QCloseEvent *event );

private slots :
    void about();
    void saveSettings();
    void updateName();
    void startDown();
    void abortDown();
    void inputsEnable( );
    void manualStart();
    void exitApp();
    void noMoreWait();
    void getFromFile();
    void getFromClipboardLast();
    void getFromClipboard(bool first);
    void getFromClipboardFirst();
    void loadAppList();
    void loadList( const QString& n );
    void saveAppList();
    void getTheList();
    void displayOutputMsg();
    void displayProgressMsg();
    void processFinished( int, QProcess::ExitStatus );

private :
    void processComplete();
    void createActions();
    void createMenus();
    bool handleListAtBeginDownload(QString &addr);
    void setDownloadRunning( bool runs );
    void createStatusBar();
    bool pasteIfNotInList( const QString url, bool pasteAtFirst );
    int posUrlInList( const QString url );
    void createUrlGroupBox();
    void createProgressGroupBox();
    void readSettings();
    void writeSettings();
    void createTrayIcon();
    void createTrayActions();

    QMenu *fileMenu;
    QMenu *prefMenu;
    QMenu *helpMenu;

    QAction *exitAct;
    QAction *aboutAct;
    QAction *aboutQtAct;

    long long actualSizeInBytes;
    float actualSize;
    int downloadedUrls;
    int waiting;
    bool sizeFound;
    bool downloadRuns;

    Settings pref;
    TrayIcon *m_trayIcon;

    QProcess *process[MAXDOWNLOADS];
    short runProcesses;
    EditList *eList;
    QString appDir;
    QString downFileName;

public:
    QClipboard *clipboard;
    QList <workList> urlWorkList;

    QAction *optionsAct;
    QAction *saveAct;
    QAction *loadAct;
    QAction *startDownAct;
    QAction *abortDownAct;
    QAction *editListAct;
    QAction *quitAct;
};

#endif
