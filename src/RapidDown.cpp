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


#include "RapidDown.h"
#include "optionsform.h"
#include "historyform.h"
#include "aboutform.h"
#include "trayicon.h"

#define PROGRAM_VERSION "Rapidshare Downloader v0.5.0"

RapidDown::RapidDown( QWidget* parent, Qt::WFlags fl )
        : QMainWindow( parent, fl ), Ui::MainForm()
{
    setupUi( this );

    createActions();
    createMenus();
    createStatusBar();
    createTrayActions();
    createTrayIcon();

    createUrlGroupBox();
    createProgressGroupBox();

    readSettings();
    for ( int i = 0; i <MAXDOWNLOADS; i++ )
        process[i] = NULL;
    runProcesses = 0;

    eList = NULL;
    appDir = QDir::currentPath();
    clipboard = QApplication::clipboard();

    downloadedUrls = 0;
    waiting = 0;
    downloadRuns = false;

    if ( pref.autostartDownload == true )
    {
        setDownloadRunning( true );
        startDown();
    }
}

void RapidDown::createActions()
{
    loadAct = new QAction( QIcon( ":/data/fileopen.png" ), tr( "&Load list" ), this );
    loadAct->setShortcut( tr( "Ctrl+L" ) );
    loadAct->setStatusTip( tr( "Load the list with URLs" ) );
    connect( loadAct, SIGNAL( triggered() ), this, SLOT( loadAppList() ) );

    saveAct = new QAction( QIcon( ":/data/filesave.png" ), tr( "&Save list" ), this );
    saveAct->setShortcut( tr( "Ctrl+S" ) );
    saveAct->setStatusTip( tr( "Save the list with URLs" ) );
    connect( saveAct, SIGNAL( triggered() ), this, SLOT( saveAppList() ) );

    exitAct = new QAction( QIcon( ":/data/quit.png" ), tr( "E&xit" ), this );
    exitAct->setShortcut( tr( "Ctrl+Q" ) );
    exitAct->setStatusTip( tr( "Exit the application" ) );
    connect( exitAct, SIGNAL( triggered() ), this, SLOT( close() ) );

    optionsAct = new QAction( QIcon( ":/data/settings.png" ), tr( "&Preferences" ), this );
    optionsAct->setShortcut( tr( "Ctrl+P" ) );
    optionsAct->setStatusTip( tr( "Preferences of this program" ) );
    connect( optionsAct, SIGNAL( triggered() ), this, SLOT( saveSettings() ) );

    historyAct = new QAction( QIcon( ":/data/history.png" ), tr( "&History" ), this );
    historyAct->setShortcut( tr( "Ctrl+H" ) );
    historyAct->setStatusTip( tr( "Downloads History" ) );
    connect( historyAct, SIGNAL( triggered() ), this, SLOT( showHisory() ) );

    aboutQtAct = new QAction( QIcon( ":/data/qt.png" ), tr( "About &Qt" ), this );
    aboutQtAct->setStatusTip( tr( "About Qt library" ) );
    connect( aboutQtAct, SIGNAL( triggered() ), qApp, SLOT( aboutQt() ) );

    aboutAct = new QAction( QIcon( ":/data/rapid-down.png" ), tr( "&About" ), this );
    aboutAct->setStatusTip( tr( "About Rapidshare Downloader" ) );
    connect( aboutAct, SIGNAL( triggered() ), this, SLOT( about() ) );
}

void RapidDown::createMenus()
{
    fileMenu = menuBar()->addMenu( tr( "&File" ) );
    fileMenu->addAction( loadAct );
    fileMenu->addAction( saveAct );
    fileMenu->addAction( exitAct );

    prefMenu = menuBar()->addMenu( tr( "&Settings" ) );
    prefMenu->addAction( optionsAct );
    prefMenu->addAction( historyAct );

    helpMenu = menuBar()->addMenu( tr( "&Help" ) );
    helpMenu->addAction( aboutAct );
    helpMenu->addAction( aboutQtAct );
}

void RapidDown::createStatusBar()
{
    statusBar()->showMessage( tr( "Ready" ), 2000 );
}

void RapidDown::createUrlGroupBox()
{
    connect( m_durl, SIGNAL( textChanged( const QString & ) ), this, SLOT( updateName() ) );
    connect( m_durl, SIGNAL( returnPressed() ), this, SLOT( startDown() ) );

    connect( m_startDown, SIGNAL( clicked() ), this, SLOT( manualStart() ) );

    connect( m_abortDown, SIGNAL( clicked() ), this, SLOT( abortDown() ) );
    m_abortDown->setEnabled( false );

    connect( m_getURLList, SIGNAL( clicked() ), this, SLOT( getTheList() ) );

    connect( m_getFromFile, SIGNAL( clicked() ), this, SLOT( getFromFile() ) );

    connect( m_getFromClipboardFirst, SIGNAL( clicked() ), this, SLOT( getFromClipboardFirst() ) );

    connect( m_getFromClipboardLast, SIGNAL( clicked() ), this, SLOT( getFromClipboardLast() ) );

}

void RapidDown::createTrayIcon()
{
    m_trayIcon = new TrayIcon(this);
}

void RapidDown::createTrayActions()
{
    startDownAct = new QAction( QIcon( ":/data/start.png" ), tr( "Start Download" ), this );
    connect( startDownAct, SIGNAL( triggered() ), this, SLOT( manualStart() ) );

    abortDownAct = new QAction( QIcon( ":/data/stop.png" ), tr( "Abort Download" ), this );
    connect( abortDownAct, SIGNAL( triggered() ), this, SLOT( abortDown() ) );
    abortDownAct->setEnabled( false );

    editListAct = new QAction( QIcon( ":/data/settings.png" ), tr( "Edit URL List" ), this );
    connect( editListAct, SIGNAL( triggered() ), this, SLOT( getTheList() ) );

    quitAct = new QAction( QIcon( ":/data/quit.png" ), tr( "&Exit" ), this );
    connect( quitAct, SIGNAL( triggered() ), this, SLOT( exitApp() ) );
}

void RapidDown::createProgressGroupBox()
{
    progressBar->setValue( 0 );;
}

void RapidDown::manualStart()
{
    waiting = 0;
    startDown();
}

void RapidDown::loadAppList()
{
    loadList( pref.urlListFileName );
}

void RapidDown::getFromFile()
{
    QString fileName = QFileDialog::getOpenFileName( this, tr( "Open File" ),
                       QDir::homePath(),
                       tr( "Text file (*.txt)" ) );

    loadList( fileName );
}

void RapidDown::getFromClipboardLast()
{
    getFromClipboard(false);
}

void RapidDown::getFromClipboardFirst()
{
    getFromClipboard(true);
}

void RapidDown::getFromClipboard(bool first)
{
    QString txt;
    QStringList lst;
    bool listChanged;

    txt = clipboard->text( QClipboard::Clipboard );
    if ( txt.length() < 1 )
    {
        statusBar()->showMessage( tr( "No URLs set" ), 2000 );
        return;
    }

    lst = txt.split( QRegExp( "[\r\n]" ) );
    listChanged = false;

    for ( QStringList::Iterator i = lst.begin(); i != lst.end(); ++i )
    {
        if (( *i ).contains( "rapidshare.com" ) ) // later list of servers
        {
            // exists in list?
            if ( pasteIfNotInList(( *i ), first ) == true )
                listChanged = true;
        }
    }

    if ( listChanged == true )
    {
        m_urls->setText( QString( "URLs in the list: " + QString::number( urlWorkList.size() ) ) );

        if ( downloadRuns == false )
            statusBar()->showMessage( tr( "The URLs has been added successfully" ), 2000 );

        saveAppList();
        if ( downloadRuns == false )
        {
            if ( pref.autoBeginAfterAdd == true )
            {
                setDownloadRunning( true );
                startDown();
            }
        }
    }
    else if ( downloadRuns == false )
        statusBar()->showMessage( tr( "No URLs set" ), 2000 );
}

int RapidDown::posUrlInList( const QString url )
{
    int pos = -1;
    int n = 0;
    for ( QList<workList>::Iterator i = urlWorkList.begin();i != urlWorkList.end(); ++i, ++n )
    {
        if (( *i ).url == url )
        {
            pos = n;
            break;
        }
    }
    return pos;
}

bool RapidDown::pasteIfNotInList( const QString url, bool atFirst )
{
    if ( posUrlInList( url ) == -1 )
    {
        workList tmp;
        tmp.url = url;
        tmp.nameToSave = "";
        tmp.status = 0;

        if (atFirst)
            urlWorkList.push_front( tmp );
        else
            urlWorkList.push_back( tmp );
        return true;
    }

    return false;
}

// load list from file
void RapidDown::loadList( const QString& fname )
{
    QFile url( fname );
    int size;
    if ( !url.open( QIODevice::ReadOnly | QIODevice::Text ) )
        return;

    QTextStream in( &url );
    QString line = in.readLine();
    while ( !line.isNull() )
    {
        if ( line.contains( "rapidshare.com" ) ) // later list of servers
        {
            pasteIfNotInList( line, false ) ;
        }
        line = in.readLine();
    }
    url.close();

    size = urlWorkList.size();
    m_urls->setText( QString( "URLs in the list: " + QString::number( size ) ) );
    if ( size > 0 )
    {
        inputsEnable( );

        if ( downloadRuns == false )
            statusBar()->showMessage( tr( "The URLs list has been loaded successfully" ), 2000 );

        // Write the first URL of EditList in m_durl if it is empty
        if ( m_durl->text().isEmpty( ) )
            m_durl->setText( urlWorkList.at( 0 ).url );
    }
    else if ( downloadRuns == false )
    {
        statusBar()->showMessage( tr( "No URLs set" ), 2000 );
    }
}

//save list into  file
void RapidDown::saveAppList()
{
    QFile url( pref.urlListFileName );
    url.open( QIODevice::WriteOnly  | QIODevice::Text );
    QTextStream out( &url );

    if ( urlWorkList.size() == 0 )
    {
        url.write( "", 0 );
        if ( downloadRuns == false )
            statusBar()->showMessage( tr( "No URLs set" ), 2000 );
        return;
    }
    else
    {
        for ( int i = 0; i < urlWorkList.size();++i )
        {
            if ( urlWorkList.at( i ).status != 255 )
            {
                QString str = urlWorkList.at( i ).url + "\n";
                out << str;
            }
        }
        if ( downloadRuns == false )
            statusBar()->showMessage( tr( "The URLs list has been saved successfully" ), 2000 );
    }
    url.close();
}

// get the list from editing table
void RapidDown::getTheList( )
{
    int oldSize;
    oldSize = urlWorkList.size();

    if ( !m_durl->text().isEmpty() )
    {
        if ( m_durl->text().contains( "rapidshare.com" ) )
        {
            pasteIfNotInList( m_durl->text(), false );
        }
    }

    eList = new EditList( this );
    eList->newlist = urlWorkList;
    eList->sets = &pref;

    eList->show();
    eList->rebuildList();

    if ( eList->exec() )
    {
        // find old elements in the list, copy in the new list status and name from oldy
        for ( QList<workList>::Iterator i = urlWorkList.begin();i != urlWorkList.end(); ++i )
        {
            for ( QList<workList>::Iterator iedit = eList->newlist.begin();iedit != eList->newlist.end(); ++iedit )
            {
                if (( *iedit ).url.isEmpty() )
                    continue;

                if ((( *iedit ).url == ( *i ).url ) )
                {
                    ( *iedit ) = ( *i );
                    break;
                }
            }
        }
        urlWorkList = eList->newlist;
        int nrs = urlWorkList.size();
        m_urls->setText( QString( "URLs in the list: " + QString::number( nrs ) ) );
    }

    // is not ever correct, exact must be oldList != newList
    if ( urlWorkList.size() != oldSize )
    {
        saveAppList();
        if ( downloadRuns == false )
        {
            if ( pref.autoBeginAfterAdd == true )
            {
                setDownloadRunning( true );
                startDown();
            }
        }
    }

    // Write the first URL of EditList in m_durl if it is empty
    if ( urlWorkList.size() )
    {
        if ( m_durl->text().isEmpty( ) )
            m_durl->setText( urlWorkList.at( 0 ).url );
    }

    delete eList;
    eList = NULL;
}

void RapidDown::inputsEnable( )
{
    m_durl->setEnabled( !downloadRuns );
    m_name->setEnabled( !downloadRuns );
}

void RapidDown::readSettings()
{
    QDate d;
    d=QDate::currentDate();

    QSettings setting( tr( "RapidDown", "Rapidshare Downloader" ) );

    pref.homeSettings = setting.value( "dhome", QDir::homePath() ).toString();
    pref.urlListFileName = setting.value( "durls", QDir::currentPath() + "/RapidDownl.urls" ).toString();

    if ( QFile::exists( pref.urlListFileName ) == false )   // if file name with error
    {
        QString name = QDir::currentPath() + "/RapidDownl.urls";
        setting.setValue( "durls", name );
        pref.urlListFileName = name;
    }

    pref.autostartDownload = setting.value( "autostartd" , false ).toBool();
    pref.autoBeginAfterAdd = setting.value( "autobegadd" , false ).toBool() ;
    pref.autoLoadList = setting.value( "autoloadlist" , true ).toBool() ;
    pref.autoStartMinimized = setting.value( "autominimized" , false ).toBool() ;

    pref.confirmExit = setting.value( "confirmex", true ).toBool();
    pref.confirmHide = setting.value( "confirmhide", true ).toBool();
    pref.confirmDelete = setting.value( "confirmdel", true ).toBool();
    pref.showPopups = setting.value( "showpopups", true ).toBool();

    pref.delCompletFiles = setting.value( "delfiles", true ).toBool();
    pref.delCorruptFiles = setting.value( "delcorrupt", false ).toBool();
    pref.delNotExists = setting.value( "delnotexists", true ).toBool();
    pref.rotations = setting.value( "rotations", true ).toInt();
    pref.reDonlSizeErr = setting.value( "redownifsizecorr", 1 ).toBool() ;
    pref.datum = setting.value( "today" ).toString();
    pref.rapidshareUser = setting.value( "ruser" ).toString();
    pref.rapidsharePass = setting.value( "rpass" ).toString();
    pref.speedLimit = setting.value( "dspeed", 20 ).toInt();
    pref.dayLimitGb = setting.value( "limitdaygb", 1.0 ).toDouble();

    if ( d.toString( "dd.MM.yyyy" ) !=  pref.datum )
    {
        pref.todayDownloaded = 0.0;
    }
    else
    {
        pref.todayDownloaded = setting.value( "todaygb", 0.0 ).toDouble();
    }
    m_downloadedToday->setText( "Downloaded today, GBytes: " + QString::number( pref.todayDownloaded ) );

    pref.waitTimeErr = setting.value( "waiterr", 2 ).toInt() ;
    pref.waitTimeNorm = setting.value( "waitnorm", 0 ).toInt() ;
    pref.nrErrors = setting.value( "nrerr", 3 ).toInt();

    setWindowTitle( tr( PROGRAM_VERSION ) );
    setWindowIcon( QIcon( ":/data/rapid-down.png" ) );
    QSize size = setting.value( "size", QSize( 500, 400 ) ).toSize();
    resize( size );

    if( pref.autoLoadList )
        loadList( pref.urlListFileName );

    static bool isRunning = false;
    if( pref.autoStartMinimized && !isRunning )
        hide();
    else
        show();

    isRunning = true;
}

void RapidDown::writeSettings()
{
    QDate d;
    d=QDate::currentDate();
    QSettings settings( tr( "RapidDown", "Rapidshare Downloader" ) );
    settings.setValue( "pos", pos() );
    settings.setValue( "size", size() );
    settings.setValue( "today", d.toString( "dd.MM.yyyy" ) );
    settings.setValue( "todaygb", pref.todayDownloaded );
}

void RapidDown::closeEvent( QCloseEvent *event )
{
    if ( m_trayIcon->trayIcon->isVisible() )
    {
        if ( pref.confirmHide == true )
        {
            int ret = QMessageBox::question( this, tr( "Confirmation" ), tr( "Exit from application or\nRemove to system tray?" ), "Exit", "Hide" );

            if ( ret == 1 )
            {
                hide();
                event->ignore();
                return;
            }
            else
            {
                for ( int i = 0; i <MAXDOWNLOADS; i++ )
                {
                    if ( process[i] != NULL )
                    {
                        process[i]->kill();
                        process[i] = NULL;
                    }
                }

                writeSettings();
                saveAppList();
                event->accept();
                QApplication::exit();
            }
        }
    }

    exitApp();

}

void RapidDown::exitApp()
{
    if ( pref.confirmExit == true )
    {
        int ret = QMessageBox::question( this, tr( "Confirmation" ), "Exit from application?", "Exit", "Cancel" );
        if ( ret == 1 )
        {
            return;
        }
    }

    for ( int i = 0; i<MAXDOWNLOADS ; i++ )
    {
        if ( process[i] != NULL )
        {
            process[i]->kill();
            process[i] = NULL;
        }
    }

    writeSettings();
    saveAppList();
    QApplication::exit();
}

void RapidDown::startDown()
{
    if ( waiting != 0 )
        return;

    if ( runProcesses == MAXDOWNLOADS )
        return;

    QDate d;
    d=QDate::currentDate();
    QString dstr = d.toString( "dd.MM.yyyy" );
    if ( dstr !=  pref.datum )
    {
        pref.todayDownloaded = 0.0;
        pref.datum = dstr;
    }


    if ( pref.todayDownloaded >= pref.dayLimitGb && pref.dayLimitGb != 0.0 )
    {
        processComplete();

        if( !isVisible() && pref.showPopups )
            m_trayIcon->dayLimitGbReached();

        else
            QMessageBox::warning(this, "Limit reached", "Day limit was reached.");

        return;
    }

    QFile file( appDir + "/.rapidcookies.txt" );

    if ( !file.exists() )
    {
        QProcess *getCookies = new QProcess();
        getCookies->execute( "wget --no-check-certificate --save-cookies " + appDir + "/.rapidcookies.txt --post-data=\"login=" + pref.rapidshareUser + "&password=" + pref.rapidsharePass + "\" https://ssl.rapidshare.com/cgi-bin/premiumzone.cgi" );
    }

    QString rurl = m_durl->text();
    if ( rurl.isEmpty() )
    {
        if ( handleListAtBeginDownload( rurl ) == false )
            return; // nothing in the list
    }

    QString dspeed;
    QString folder = pref.homeSettings;
    sizeFound = false;

    if ( !rurl.isEmpty() && !folder.isEmpty() )
    {
        m_urls->setText( QString( "URLs in the list: " + QString::number( urlWorkList.size() ) ) );

        QFile file;
        QString filename, name;
        filename = rurl;
        name = filename.mid( filename.lastIndexOf( '/' ) + 1 );
        name = folder + name;
        m_durl->setText( filename );
        file.setFileName( filename );
        m_name->setText( name );
        QDir::setCurrent( folder );

        if ( file.exists() )
        {
            statusBar()->showMessage( tr( "There is already a file with this name" ) );
            return;
        }

        setDownloadRunning( true );

        progressBar->setValue( 0 );

        //QProcess *pFound;
        for (int i=0; i<MAXDOWNLOADS;i++)
        {
            if (process[i] != NULL)
                break;
        }
        int i;
        process[i] = new QProcess();
        runProcesses++;
        connect( process[i], SIGNAL( readyReadStandardOutput() ), this, SLOT( displayOutputMsg() ) );
        connect( process[i], SIGNAL( readyReadStandardError() ), this, SLOT( displayProgressMsg() ) );
        connect( process[i], SIGNAL( finished( int, QProcess::ExitStatus ) ), this, SLOT( processFinished( int, QProcess::ExitStatus ) ) );

        QString command;
        if ( pref.speedLimit == 0 )
            command = "wget -c -v --load-cookies " + appDir + "/.rapidcookies.txt " + rurl + " -P " + folder;
        else
            command = "wget -c -v --load-cookies " + appDir + "/.rapidcookies.txt " + rurl + " --limit-rate=" + QString::number( pref.speedLimit ) + "K -P " + folder;

        actualSizeInBytes = 0;
        process[i]->start( command );

    }
    else
        QMessageBox::warning( this, tr( "Error" ), tr( "Please write the name and the password and the download speed\nand the destination folder from preferences menu" ) );
}

void RapidDown::setDownloadRunning( bool runs )
{
    downloadRuns = runs;
    m_abortDown->setEnabled( runs );
    abortDownAct->setEnabled( runs );

    m_startDown->setEnabled( !runs );
    startDownAct->setEnabled( !runs );

    inputsEnable( );
}

void RapidDown::noMoreWait()
{
    waiting = 0;
    if ( downloadRuns == true )
        startDown();
}

void RapidDown::processFinished( int exitCode, QProcess::ExitStatus exitStatus )
{
    QString msg;
    int errCode = 0;

    for (int i=0; i<MAXDOWNLOADS; i++)
    {
        if (process[i] == NULL)
            continue;

        if (process[i]->state() == QProcess::NotRunning)
        {
            runProcesses--;
            process[i] = NULL;
        }
    }

    if ( exitStatus == QProcess::CrashExit )
    {
        msg = tr( "There was an error downloading the file. It may be incomplete." );
        errCode = -1;
    }
    else
    {
        if ( exitCode ==0 )
        {
            if( !isVisible() && pref.showPopups )
                m_trayIcon->finished( downFileName, m_downloadedMB->text() );

            msg = tr( "Download successfully completed." );

            downloadedUrls++;
            m_speed->setText( "Speed:" );
            m_eta->setText( "Finishing in:" );
            m_downloadedMB->setText( "Downloaded MBytes:" );
        }
        else //if ( exitCode > 0 )
        {
            msg =  tr( "Failed. Couldn\'t get the URL of this file." );
            errCode = -2;
        }
    }
//   qDebug() << msg;
    statusBar()->showMessage(( msg ), 5000 );

    QProcess *rmPro = new QProcess();
    rmPro->start( "rm premiumzone.cgi" );

    pref.todayDownloaded += actualSizeInBytes/( 1024.0*1024.0*1024.0 ); // in GBytes
    actualSizeInBytes = 0;
    // if not aborted, then is the download complete?
    if ( downloadRuns == true )
    {
        //if urlList was rebuild, we need it
        QString oldName = m_durl->text();
        int pos = posUrlInList( oldName );
        if ( pos >= 0 )
        {
            switch ( errCode )   // codeErr handling
            {
            case - 1: // download err
                if ( pref.delCorruptFiles == true )
                    urlWorkList.removeAt( pos );
                else   // do not delete
                {
                    workList tmp;
                    // hire timeout
                    tmp = urlWorkList.takeAt( pos );
                    tmp.message = msg;
                    tmp.status++;
                    urlWorkList.append( tmp );   // at end of list
                }
                if ( pref.waitTimeErr != 0 )
                {
                    waiting = pref.waitTimeErr;
                    QTimer *timer = new QTimer( this );
                    connect( timer, SIGNAL( timeout() ), this, SLOT( noMoreWait() ) );
                    statusBar()->showMessage( tr( "Waiting after download error" ), 5000 );
                    timer->start( waiting*1000*60 );
                }
                break;
            case - 2: // url not exists?
                if ( pref.delNotExists == true )
                    urlWorkList.removeAt( pos );
                else   // do not delete
                {
                    workList tmp;
                    tmp = urlWorkList.takeAt( pos );
                    tmp.message = msg;
                    tmp.status = -1;
                    urlWorkList.append( tmp );   // at end of list
                }
                if ( pref.waitTimeErr != 0 )
                {
                    waiting = pref.waitTimeErr;
                    QTimer *timer = new QTimer( this );
                    connect( timer, SIGNAL( timeout() ), this, SLOT( noMoreWait() ) );
                    statusBar()->showMessage( tr( "Waiting after download error" ), 5000 );
                    timer->start( waiting*1000*60 );
                }
                break;
            default:
                if ( pref.delCompletFiles == true )
                    urlWorkList.removeAt( pos );
                else   // do not delete
                {
                    workList tmp;
                    tmp = urlWorkList.takeAt( pos );
                    tmp.message = msg;
                    tmp.status = 255;
                    urlWorkList.append( tmp );
                }
                if ( pref.waitTimeNorm != 0 )
                {
                    waiting = pref.waitTimeNorm;
                    QTimer *timer = new QTimer( this );
                    connect( timer, SIGNAL( timeout() ), this, SLOT( noMoreWait() ) );
                    statusBar()->showMessage( tr( "Waiting after file download" ), 5000 );
                    timer->start( waiting*1000*60 );
                }
                break;
            }
        }

        if ( urlWorkList.size() == 0 )
            processComplete();
        else
        {
            m_durl->setText( urlWorkList.at( 0 ).url );
            if ( eList != NULL )
            {
                eList->newlist = urlWorkList;
                eList->rebuildList();
            }
        }

        msg.setNum( downloadedUrls );
        m_downloadedURLs->setText( tr( "Downloaded URLs: " ) + msg );

        saveAppList(); // and if the list is zero...
        startDown();
    }
}

bool RapidDown::handleListAtBeginDownload( QString &addr )
{
    if ( urlWorkList.size() == 0 )
    {
        processComplete();
        return false;
    }

    if ( urlWorkList.at( 0 ).status != 255 )   // not downloaded
    {
        if ( pref.rotations >0 && urlWorkList.at( 0 ).status == pref.rotations )
            return false;

        addr = urlWorkList.at( 0 ).url;
        return true;
    }

    while ( urlWorkList.at( 0 ).status == 255 && urlWorkList.size() != 0 )
    {
        if ( pref.delCompletFiles == true )
            urlWorkList.removeAt( 0 );
        else   // do not delete, move to last
            urlWorkList.append( urlWorkList.takeAt( 0 ) );
    }

    if ( urlWorkList.size() == 0 )
    {
        processComplete();
        return false;
    }

    if ( pref.rotations >0 && urlWorkList.at( 0 ).status == pref.rotations )
        return false;

    addr = urlWorkList.at( 0 ).url;
    return true; // if something in the list
}


void RapidDown::displayOutputMsg()
{
    for (int i=0; i<MAXDOWNLOADS; i++)
    {
        process[i]->setReadChannel( QProcess::StandardOutput );
        QByteArray msg = process[i]->readAllStandardOutput();
        QString result = msg.data();
    }
}

void RapidDown::displayProgressMsg()
{
    QByteArray msg;
    QString result;
    QProcess* actualP=NULL;

    for (int i=0; i<MAXDOWNLOADS; i++)
    {
        process[i]->setReadChannel( QProcess::StandardError );
        msg = process[i]->readAllStandardError();
        if (msg.length() > 1)
        {
            result = msg.data();
            actualP = process[i];
//       qDebug() << "actual:" <<i;
        }
    }
    if (result.length() <1)
        return;

    int pos;
    int actualPercent = 0;

    // if file or url not exists. is same error
    pos = result.indexOf( "[text/html]" );
    if ( pos > 0 )
    {
        QString name;
        statusBar()->showMessage(( "Link is corrupt" ), 5000 );
        name = m_name->text();
        QFile::remove( name );
        actualP->terminate();
        actualP = NULL;
        return;
    }

    // information after "Length:", undepended from language
    if ( sizeFound == false )
    {
        pos = result.indexOf( QRegExp( ": (\\d+) \\(\\d+" ) );

        if ( pos > 0 )
        {
            int pos2 = result.indexOf( ')', pos );

            if ( pos2 > pos )
            {
                QString strSize;
                QString tmpStr;
                int posSize1, posSize2;
                tmpStr = "Length" + result.mid( pos, pos2 - pos ) + "Bytes)";
                progressText->setText( tmpStr );

                posSize1 = 8;
                posSize2 = tmpStr.indexOf( " ", posSize1 + 1 );
                strSize = tmpStr.mid( posSize1, posSize2 - posSize1 );
                strSize.remove( " " );
                actualSizeInBytes = strSize.toLongLong();
                sizeFound = true;
            }
        }
    }


    pos =  result.indexOf( "%" );
    if ( pos > 0 )
    {
        // Here for the tray icon
        QString value;
        QString mbs;
        QString mbsToday;

        value = result.mid( pos - 3, 3 );
        value.remove( " " );

        if ( !value.isEmpty() )
        {
            actualPercent = value.toInt();
            progressBar->setValue( actualPercent );

            if ( sizeFound )
            {
                if ( progressText->text().contains( "MBytes" ) )
                {
                    actualSize = ( float )( actualSizeInBytes / ( 1024.0 * 1024.0 ) ) * 0.01 * actualPercent;
                    mbs.setNum( actualSize, 'g', 4 );
                    mbsToday.setNum(( actualSize/1024.0 ) + pref.todayDownloaded, 'g', 4 );
                    m_downloadedMB->setText( "Downloaded MBytes: " + mbs );
                    m_downloadedToday->setText( "Downloaded today, GBytes: " + mbsToday );
                    mbs += "MB"; // For the tray icon
                }
                else if ( progressText->text().contains( "KBytes" ) )
                {
                    actualSize = ( float )( actualSizeInBytes / ( 1024.0 ) ) * 0.01 * actualPercent;
                    mbs.setNum( actualSize, 'g', 4 );
                    mbsToday.setNum(( actualSize/1024.0 ) + pref.todayDownloaded, 'g', 4 );
                    m_downloadedMB->setText( "Downloaded KBytes: " + mbs );
                    m_downloadedToday->setText( "Downloaded today (GB): " + mbsToday );
                    mbs += "KB";
                }
            }
        }

        // Speed in Kb/s, Mb/s, b/s
        QString speed;
        int pos2 = result.indexOf( QRegExp("[KMb]"), pos );
        if ( pos2 > pos )
        {
            speed = result.mid( pos + 1, pos2 - pos);
            speed.remove( QRegExp( "[\\s\\n\\r]" ) );
        }
        if ( !speed.isEmpty() )
            m_speed->setText( "Speed: " + speed + "b/s" );


        // ETA
        QString eta;
        int pos3 = 0;

        if ( result.contains( 's' ) )
            pos3 = result.indexOf( 's', pos2 );
        else if ( result.contains( 'm' ) )
            pos3 = result.indexOf( 'm', pos2 );

        if ( pos3 > pos2 )
        {
            eta = result.mid( pos2 + 1, pos3 - pos2 );
            eta.remove( QRegExp( "[\\s\\n\\r]" ) );
        }
        if ( !eta.isEmpty() )
            m_eta->setText( "Finishing in: " + eta );


        m_trayIcon->setTrayIconStats(downFileName, speed, eta, mbs, value);

    }
}

void RapidDown::processComplete()
{
    m_durl->setText( "" );
    m_name->setText( "" );
    m_urls->setText( QString( "URLs in the list: " + QString::number( urlWorkList.size() ) ) );

    m_speed->setText( "Speed:" );
    m_eta->setText( "Finishing in:" );
    m_downloadedMB->setText( "Downloaded MBytes:" );
    setDownloadRunning( false );
    m_trayIcon->setTrayIconStats("", "0", "00:00", "0MB", "0");

    if( !isVisible() && pref.showPopups )
        m_trayIcon->Allfinished(downloadedUrls);
}

void RapidDown::abortDown()
{
    int ret = QMessageBox::warning( this, tr( "Abort Download ?" ),
                                    tr( "Do you went realy to abort the current download" ),
                                    QMessageBox::Yes | QMessageBox::No,
                                    QMessageBox::No );
    if ( ret == ( QMessageBox::No ) )
        return;

    for (int i = 0; i < MAXDOWNLOADS; i++)
    {
        if (process[i] != NULL)
        {
            process[i]->terminate();
            //process[i] = NULL;
        }
    }
    m_durl->setText( "" );

    setDownloadRunning( false );

    progressBar->setValue( 0 );
}

void RapidDown::updateName()
{
    if ( m_abortDown->isEnabled() ) // download is running, no reaction
        if ( downloadRuns == true )
            return;

    //if (( m_durl->text().contains( "rapidshare.com" ) ) )
    if (( !m_durl->text().isEmpty() ) )
    {
        m_startDown->setEnabled( true );
        startDownAct->setEnabled( true );

        if ( m_durl->text().count() > 8 )
        {
            if ( !m_durl->text().startsWith( "http://" ) )
                m_durl->setText( "http://" + m_durl->text() );
        }

        if ( m_durl->text().count() > 25 )
        {
            QString str = m_durl->text().section( '/', -1 );  // str == "file.zip"
            m_name->setText( str );

            if(str.count() > 25)
            {
                str.resize(22);
                str.append("...");
            }

            downFileName = str;
        }
    }
    else
    {
        m_name->clear();
        m_startDown->setEnabled( false );
        startDownAct->setEnabled( false );
    }
}

void RapidDown::saveSettings()
{
    Preferences perefeDialog;
    if ( perefeDialog.exec() )
    {
        readSettings();
    }
}

void RapidDown::showHisory()
{
    History historyDialog;

    historyDialog.show();
    if ( historyDialog.exec() )
    {
        //readSettings();
    }
}

void RapidDown::about()
{
    aboutForm aboutProgram;
    aboutProgram.exec();
}

