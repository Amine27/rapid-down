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

#include "optionsform.h"

Preferences::Preferences( QWidget* parent, Qt::WFlags fl )
        : QDialog( parent, fl ), Ui::optionsDialog()
{
    setupUi( this );

    QSettings setting( tr( "RapidDown", "Rapidshare Downloader" ) );

    tabWidget->setCurrentIndex ( 0);

    QValidator *validator = new QRegExpValidator(QRegExp ("[0-9]+"), this);
    lineEditBand->setValidator(validator);
    lineEditDayLimit->setValidator(validator);
    lineEditWaitError->setValidator(validator);
    lineEditWaitNormal->setValidator(validator);

    m_directory->setText( setting.value( "dhome", QDir::homePath() ).toString() );
    connect( pushDirectory, SIGNAL( clicked() ), this, SLOT( destinationFolder() ) );
    m_listfile->setText( setting.value( "durls" ).toString() );

    m_begAutomat->setChecked( setting.value( "autostartd", false ).toBool() );
    m_begAfterAddition->setChecked( setting.value( "autobegadd", false ).toBool() );
    m_loadListAtStart->setChecked( setting.value( "autoloadlist", true ).toBool() );
    m_startMinimized->setChecked( setting.value( "autominimized", false ).toBool() );

    m_confirmExit->setChecked( setting.value( "confirmex", true ).toBool() );
    m_confirmHide->setChecked( setting.value( "confirmhide", true ).toBool() );
    m_confirmDelList->setChecked( setting.value( "confirmdel", true ).toBool() );
    m_showPopups->setChecked( setting.value( "showpopups", true ).toBool() );

    m_delFiles->setChecked( setting.value( "delfiles", true ).toBool() );
    m_delCorrupt->setChecked( setting.value( "delcorrupt", false ).toBool() );
    m_delNotExists->setChecked( setting.value( "delnotexists", true ).toBool() );
    comboRotations->setCurrentIndex( setting.value( "rotations", true ).toInt() );

    m_redownloadIfSizeCorrupt->setChecked( setting.value( "redownifsizecorr", true ).toBool() );
    lineEditWaitError->setText( QString::number( setting.value( "waiterr", 0 ).toInt() ) );
    comboTryings->setCurrentIndex( setting.value( "nrerr", 3 ).toInt() );
    lineEditWaitNormal->setText( QString::number( setting.value( "waitnorm", 0 ).toInt() ) );

    lineEditBand->setText( QString::number( setting.value( "dspeed", 40 ).toInt() ) );
    lineEditDayLimit->setText( QString::number( setting.value( "limitdaygb", 1.0 ).toDouble() ) );

    groupAccount->setChecked( setting.value( "rapidshareaccount", true ).toBool() );
    lineEditUsername->setText( setting.value( "ruser" ).toString() );
    lineEditPassword->setText( setting.value( "rpass" ).toString() );


    connect( pushOk, SIGNAL( clicked() ), this, SLOT( saveSettings() ) );
    connect( pushCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
}

Preferences::~Preferences()
{
}

/*$SPECIALIZATION$*/
void Preferences::reject()
{
    QDialog::reject();
}

void Preferences::saveSettings()
{
    QSettings setting( tr( "RapidDown", "Rapidshare Downloader" ) );

    setting.setValue( "dhome", m_directory->text() );
    setting.setValue( "durls", m_listfile->text( ) );
    setting.setValue( "autostartd", m_begAutomat->isChecked( ) );
    setting.setValue( "autobegadd", m_begAfterAddition->isChecked( ) );
    setting.setValue( "autoloadlist", m_loadListAtStart->isChecked( ) );
    setting.setValue( "autominimized", m_startMinimized->isChecked( ) );

    setting.setValue( "confirmex", m_confirmExit->isChecked( ) );
    setting.setValue( "confirmhide", m_confirmHide->isChecked( ) );
    setting.setValue( "confirmdel", m_confirmDelList->isChecked() );
    setting.setValue( "showpopups", m_showPopups->isChecked() );

    setting.setValue( "delfiles", m_delFiles->isChecked( ) );
    setting.setValue( "delcorrupt", m_delCorrupt->isChecked( ) );
    setting.setValue( "delnotexists", m_delNotExists->isChecked( ) );
    setting.setValue( "rotations", comboRotations->currentIndex( ) );

    setting.setValue( "redownifsizecorr", m_redownloadIfSizeCorrupt->isChecked() );
    setting.setValue( "waiterr", lineEditWaitError->text( ) );
    setting.setValue( "waitnorm", lineEditWaitNormal->text( ) );
    setting.setValue( "nrerr", comboTryings->currentIndex( ) );

    setting.setValue( "dspeed", lineEditBand->text( ) );
    setting.setValue( "limitdaygb", lineEditDayLimit->text( ) );

    setting.setValue( "rapidshareaccount", groupAccount->isChecked( ) );
    setting.setValue( "ruser", lineEditUsername->text( ) );
    setting.setValue( "rpass", lineEditPassword->text( ) );

    QDialog::accept();
}


void Preferences::destinationFolder()
{
    QString dir = QFileDialog::getExistingDirectory( this, tr( "Select the destination folder" ),
                  QDir::homePath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks );
    m_directory->setText( dir );
}

