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

#ifndef OPTIONSFORM_H
#define OPTIONSFORM_H

#include <QtGui>
#include <QProcess>

#include "ui_optionsForm.h"

class Preferences : public QDialog, private Ui::optionsDialog
{
    Q_OBJECT

public:
    Preferences ( QWidget* parent = 0, Qt::WFlags fl = 0 );
    ~Preferences();

public slots:

private slots:
    void destinationFolder();

protected slots:

    virtual void reject();
    virtual void saveSettings();

};

#endif

