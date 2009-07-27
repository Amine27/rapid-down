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
#include "editlist.h"

#define SELECT_ALL 0
#define UNSELECT_ALL 1
#define INVERT_ALL 2

EditList::EditList( QWidget* parent, Qt::WFlags fl )
        : QDialog( parent, fl ), Ui::Dialog()
{
    setupUi( this );

    father = (RapidDown*)parent;
    clipboard = father->clipboard;

    createActions();
    setupContextMenu();

    connect( m_cancel, SIGNAL( clicked() ), this, SLOT( cancel() ) );
    connect( m_ok, SIGNAL( clicked() ), this, SLOT( saveList() ) );
    connect( m_addUrls, SIGNAL(clicked() ), this, SLOT( addUrls() ) );
}

EditList::~EditList()
{
}

void EditList::createActions()
{
    copyAct = new QAction( tr( "Copy to clipboard" ), this );
    copyAct->setIcon( QIcon( ":data/edit-copy.png" ) );
    copyAct->setShortcut( Qt::CTRL | Qt::Key_C );
    connect( copyAct, SIGNAL( triggered() ), this, SLOT( copy() ) );

    pasteAct = new QAction( tr( "Paste from clipboard" ), this );
    pasteAct->setIcon( QIcon( ":data/edit-paste.png" ) );
    pasteAct->setShortcut( Qt::CTRL | Qt::Key_V );
    connect( pasteAct, SIGNAL( triggered() ), this, SLOT( paste() ) );

    deleteAct = new QAction( tr( "Delete selected" ), this );
    deleteAct->setIcon( QIcon( ":data/edit-delete.png" ) );
    deleteAct->setShortcut( Qt::CTRL | Qt::Key_D );
    connect( deleteAct, SIGNAL( triggered() ), this, SLOT( del() ) );

    cutAct = new QAction( tr( "Cut to clipboard" ), this );
    cutAct->setIcon( QIcon( ":data/edit-cut.png" ) );
    cutAct->setShortcut( Qt::CTRL | Qt::Key_X );
    connect( cutAct, SIGNAL( triggered() ), this, SLOT( cut() ) );

    clearAct = new QAction( tr( "Clear list" ), this );
    clearAct->setShortcut( Qt::CTRL | Qt::Key_Y );
    connect( clearAct, SIGNAL( triggered() ), this, SLOT( clear() ) );

    firstSeparator = new QAction( this );
    firstSeparator->setSeparator( true );

    selAct = new QAction( tr( "Select all" ), this );
    selAct->setIcon( QIcon( ":data/edit-select-all.png" ) );
    selAct->setShortcut( Qt::CTRL | Qt::Key_A );
    connect( selAct, SIGNAL( triggered() ), this, SLOT( selAll() ) );

    unselAct = new QAction( tr( "Unselect all" ), this );
    unselAct->setShortcut( Qt::CTRL | Qt::Key_U );
    connect( unselAct, SIGNAL( triggered() ), this, SLOT( unselAll() ) );

    invertAct = new QAction( tr( "Invert all" ), this );
    invertAct->setShortcut( Qt::CTRL | Qt::Key_I );
    connect( invertAct, SIGNAL( triggered() ), this, SLOT( invertAll() ) );

    secondSeparator = new QAction( this );
    secondSeparator->setSeparator( true );

    selHome = new QAction( tr( "Selected at first" ), this );
    selHome->setShortcut( Qt::CTRL | Qt::Key_Home );
    connect( selHome, SIGNAL( triggered() ), this, SLOT( selectedToHome() ) );

    selEnd = new QAction( tr( "Selected at end" ), this );
    selEnd->setShortcut( Qt::CTRL | Qt::Key_End );
    connect( selEnd, SIGNAL( triggered() ), this, SLOT( selectedToEnd() ) );

    selUp = new QAction( tr( "Selected up" ), this );
    selUp->setShortcut( Qt::CTRL | Qt::Key_Up );
    connect( selUp, SIGNAL( triggered() ), this, SLOT( selectedUp() ) );

    selDwn = new QAction( tr( "Selected down" ), this );
    selDwn->setShortcut( Qt::CTRL | Qt::Key_Down );
    connect( selDwn, SIGNAL( triggered() ), this, SLOT( selectedDown() ) );
}

void EditList::setupContextMenu()
{
    addAction( pasteAct );
    addAction( deleteAct );
    addAction( copyAct );
    addAction( cutAct );
    addAction( firstSeparator );
    addAction( selAct );
    addAction( unselAct );
    addAction( invertAct );
    addAction( secondSeparator );
    addAction( selHome );
    addAction( selUp );
    addAction( selDwn );
    addAction( selEnd );
    setContextMenuPolicy( Qt::ActionsContextMenu );
}

void EditList::selectedToHome()
{
    // pos = 0 .. size-1
    int size = listWidget->count(); // speed up
    int actual = 0;

    if ( size < 2 )
        return;

    if ( listWidget->item( 0 )->isSelected() == true )
    {
        listWidget->item( 0 )->setSelected( false );
        actual++;
    }

    for ( int i = 1; i < size; i++ )
    {
        QListWidgetItem* it = listWidget->item( i );
        if ( it == NULL )
            break;

        if (( *it ).isSelected() == true )
            listWidget->insertItem( actual++, listWidget->takeItem( i ) );
    }
}

void EditList::selectedToEnd()
{
    // pos = 0 .. size-1
    int size = listWidget->count(); // for speed up, not in loop
    int actual = size-1;
    if ( size < 2 )
        return;

    if ( listWidget->item( size-1 )->isSelected() == true )
    {
        listWidget->item( size-1 )->setSelected( false );
        actual--;
    }

    for ( int i = size-2; i >= 0; i-- )
    {
        QListWidgetItem* it = listWidget->item( i );
        if ( it == NULL )
            break;

        if (( *it ).isSelected() == true )
            listWidget->insertItem( actual--, listWidget->takeItem( i ) );
    }
}

void EditList::selectedUp()
{
    // pos = 0 .. size-1
    int size = listWidget->count(); // this for speed up
    if ( size < 2 )
        return;

    for ( int i = 1; i < size; i++ )
    {
        QListWidgetItem* it = listWidget->item( i );
        if ( it == NULL )
            break;

        if (( *it ).isSelected() == true )
        {
            listWidget->insertItem( i - 1, listWidget->takeItem( i ) );
            listWidget->item( i - 1 )->setSelected( true );
        }
    }
}

void EditList::selectedDown()
{
    // pos = 0 .. size-1
    int size = listWidget->count(); // for speed up
    if ( size < 2 )
        return;

    for ( int i = size - 2; i >= 0 ; i-- )
    {
        QListWidgetItem* it = listWidget->item( i );
        if ( it == NULL )
            break;

        if (( *it ).isSelected() == true )
        {
            listWidget->insertItem( i + 1, listWidget->takeItem( i ) );
            listWidget->item( i + 1 )->setSelected( true );
        }
    }
}

void EditList::selAll()
{
    selectInTable( SELECT_ALL );
}

void EditList::unselAll()
{
    selectInTable( UNSELECT_ALL );
}

void EditList::invertAll()
{
    selectInTable( INVERT_ALL );
}

void EditList::rebuildList()
{
    oldlist = newlist;
    listWidget->clear(); // better way without clear function, rebuild only changed
    for ( QList<workList>::Iterator i = newlist.begin();i != newlist.end();++i )
    {
        QColor c;
        switch (( *i ).status )
        {
        case 255: //downloaded
            c = Qt::green;
            break;
        case -1: // error
            c = Qt::red;
            break;
        case 0: // init
            c = Qt::white;
            break;
        default: // nr of trying
            c = Qt::yellow;
            break;
        }

        QListWidgetItem *newItem = new QListWidgetItem;
        newItem->setText(( *i ).url );
        newItem->setBackgroundColor( c );
        listWidget->addItem( newItem );
    }
}

/*$SPECIALIZATION$*/
void EditList::reject()
{
    QDialog::reject();
}

void EditList::cancel()
{
    newlist = oldlist;
    accept();
}

void EditList::saveList()
{
    workList elTemp;
    newlist.clear();
    for ( int i = 0; i < listWidget->count(); ++i )
    {
        QListWidgetItem* it = listWidget->item( i );
        if ( it == NULL )
            break;
        elTemp.url = ( *it ).text();
        elTemp.status = 0;
        elTemp.nameToSave = "";
        newlist << elTemp;
    }
    accept();
}

void EditList::copy()
{
    QString str;
    int n = listWidget->count();

    for ( int i = 0; i < n;i++ )
    {
        QListWidgetItem* it = listWidget->item( i );
        if ( it == NULL )
            break;
        if (( *it ).isSelected() == true )
        {
            str += ( *it ).text();
            str += "\n";
            ( *it ).setSelected( false );
        }
    }
    clipboard->setText( str, QClipboard::Clipboard );
}

void EditList::cut()
{
    QString str;
    if ( sets->confirmDelete == true )
    {
        if ( 1 == QMessageBox::question( this, tr( "Cut" ), tr( "Cut links from list?" ), tr( "Yes" ), tr( "No" ) ) )
            return;
    }

    for ( int i = 0; i < listWidget->count(); )
    {
        QListWidgetItem* it = listWidget->item( i );
        if ( it == NULL )
            break;
        if (( *it ).isSelected() == true )
        {
            str += ( *it ).text();
            str += "\n";
            listWidget->takeItem( i );
        }
        else
            i++;
    }
    clipboard->setText( str, QClipboard::Clipboard );
}

void EditList::paste()
{
    QString txt;
    QStringList lst;
    txt = clipboard->text( QClipboard::Clipboard );
    if ( txt.length() < 1 )
        return;

    lst = txt.split( QRegExp( "[\r\n]" ) );

    for ( QStringList::Iterator i = lst.begin(); i != lst.end(); ++i )
    {
        //if (( *i ).contains( "rapidshare.com" ) ) // later list of servers
        //{
            // exists in list?
            bool found;
            found = false;

            for ( int in = 0; in < listWidget->count(); ++in )
            {
                QListWidgetItem* it = listWidget->item( in );
                if (( *it ).text() == ( *i ) )
                {
                    found = true;
                    break;
                }
            }
            if ( found == true )
                continue;

            listWidget->addItem( *i );
        //}
    }

    selectInTable( UNSELECT_ALL );
}

void EditList::clear()
{
    listWidget->clear();
}

void EditList::del()
{
    if ( sets->confirmDelete == true )
    {
        if ( 1 == QMessageBox::question( this, tr( "Delete" ), tr( "Delete links from list?" ), tr( "Yes" ), tr( "No" ) ) )
            return;
    }

    for ( int i = 0; i < listWidget->count(); )
    {
        QListWidgetItem* it = listWidget->item( i );
        if ( it == NULL )
            break;
        if (( *it ).isSelected() == true )
        {
            listWidget->takeItem( i );
        }
        else
            i++;
    }
}

// 0 - select
// 1 - unselect
// 2 - invert
void EditList::selectInTable( int vari )
{
    for ( int b = 0; b < listWidget->count(); ++b )
    {
        QListWidgetItem *actual = listWidget->item( b );
        switch ( vari )
        {
        case SELECT_ALL:
            actual->setSelected( true );
            break;
        case UNSELECT_ALL:
            actual->setSelected( false );
            break;
        case INVERT_ALL:
            actual->setSelected( !actual->isSelected() );
            break;
        default:
            break;
        }
    }
}

void EditList::addUrls()
{
    bool ok;
    QString clipboardTxt;

    if (clipboard->text( QClipboard::Clipboard ).contains( "rapidshare.com" ) )
            clipboardTxt = clipboard->text( QClipboard::Clipboard );

    QString url = QInputDialog::getText(this, tr("Add a URL."),
                                          tr("Rapidshare Downloader will load and parse the URL for further links."), QLineEdit::Normal,
                                           clipboardTxt, &ok);
    if (ok && !url.isEmpty())
    {
        clipboard->setText( url, QClipboard::Clipboard );
        paste();
    }
}
