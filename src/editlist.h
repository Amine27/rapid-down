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


#ifndef EDITLIST_H
#define EDITLIST_H

#include <QDialog>
#include <QContextMenuEvent>
#include <QMenu>
#include <QList>
#include <QMenuBar>


#include "RapidDown.h"
#include "ui_editList.h"

class RapidDown;


class EditList : public QDialog, private Ui::Dialog
{
    Q_OBJECT

public:
    EditList( QWidget* parent = 0, Qt::WFlags fl = 0 );
    ~EditList();
    void rebuildList();

private slots:
    void saveList();
    void cancel();
    void copy();
    void selAll();
    void unselAll();
    void selectedToHome();
    void selectedToEnd();
    void selectedUp();
    void selectedDown();
    void invertAll();
    void clear();
    void cut();
    void paste();
    void del();
    void addUrls();

private:
    void setupContextMenu();
    void createActions();
    void selectInTable( int variation );

private:
    QAction *copyAct;
    QAction *cutAct;
    QAction *pasteAct;
    QAction *firstSeparator;
    QAction *secondSeparator;
    QAction *selAct;
    QAction *unselAct;
    QAction *invertAct;
    QAction *deleteAct;
    QAction *clearAct;
    QAction *selHome;
    QAction *selEnd;
    QAction *selUp;
    QAction *selDwn;
    QClipboard *clipboard;
    RapidDown *father;

protected slots:
    virtual void reject();


public:
    QList <workList> newlist, oldlist;
    struct Settings *sets;
};

#endif

