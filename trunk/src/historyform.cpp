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


#include "historyform.h"

History::History(QWidget* parent, Qt::WFlags fl)
        : QDialog( parent, fl ), Ui::historyWidget()
{
    setupUi(this);

    proxyModel = new QSortFilterProxyModel;
    proxyModel->setDynamicSortFilter(true);

    model = new QStandardItemModel(0, 4, parent);

    model->setHeaderData(0, Qt::Horizontal, QObject::tr("Date"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("File name"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Size"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("URL"));

    m_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tableView->horizontalHeader()->setStretchLastSection(true);
    m_tableView->verticalHeader()->hide();
    m_tableView->setShowGrid(false);
    m_tableView->setSortingEnabled(true);
    m_tableView->setWordWrap(false);
    m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableView->resizeRowsToContents();
    m_tableView->setAlternatingRowColors(true);

    proxyModel->setSourceModel(model);
    m_tableView->setModel(proxyModel);

    connect(m_findEdit, SIGNAL(textChanged(const QString &)), this, SLOT(findHundler(QString)));
    connect(m_filter, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(filterHundler(QString)));
    connect(m_clearHistory, SIGNAL(clicked()), this, SLOT(clearHistory()));

    file.setFileName(QDir::homePath()+"/RapidDownHistory.xml");
}

History::~History()
{
}

// Read history from XML file
void History::readHistory()
{
     if (!file.open(QIODevice::ReadOnly))
     {
         /*QMessageBox::warning(this, tr("Download History"),
                              tr("Cannot read file %1:\n%2.")
                              .arg(file.fileName())
                              .arg(file.errorString()));*/

         return;
     }

    xmlReader.setDevice(&file);

    QString date, fileName, fileSize, url, id, currentId ="0";

    while (!xmlReader.atEnd())
    {
        xmlReader.readNext();

        if (xmlReader.isStartElement())
        {
            if (xmlReader.name() == "file")
                id = xmlReader.attributes().value("id").toString();
            if (xmlReader.name() == "date")
                date = xmlReader.readElementText();
            else if (xmlReader.name() == "fileName")
                fileName = xmlReader.readElementText();
            else if (xmlReader.name() == "fileSize")
                fileSize = xmlReader.readElementText();
            else if (xmlReader.name() == "url")
                url = xmlReader.readElementText();
        }
        else if (xmlReader.isEndElement() && id != currentId)
        {
            readHistoryItem(date, fileName, fileSize, url);
            currentId = id;
        }
    }

    if (xmlReader.hasError())
    {
        qDebug() << "Error";
        return;
    }

    m_tableView->sortByColumn(0, Qt::DescendingOrder);
    file.close();
}

// Create QAbstractItemModel to the given item
void History::readHistoryItem(QString date, QString fileName, QString fileSize, QString url)
{
    // Convert the Unix timestamp to date
    QDateTime dateTime;dateTime.setTime_t(date.toUInt());
    date = dateTime.toString("dd/MM/yyyy hh:mm:ss");

    int row = model->rowCount();
    model->insertRow(row);
    model->setData(model->index(row, 0), date);
    model->setData(model->index(row, 1), fileName);
    model->setData(model->index(row, 2), fileSize);
    model->setData(model->index(row, 3), url);
}

// Write history into XML file
void History::writeHistory()
{
    if (!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::warning(this, tr("Download History"),
                              tr("Cannot write file %1:\n%2.")
                              .arg(file.fileName())
                              .arg(file.errorString()));
        return;
    }

    xmlWriter.setDevice(&file);
    xmlWriter.setAutoFormatting(true);
    xmlWriter.writeStartDocument();
    xmlWriter.writeStartElement("history");

    for (int i = 0; i < model->rowCount(); ++i)
    {
        xmlWriter.writeStartElement("file");
        QString str;
        xmlWriter.writeAttribute("id", str.setNum(i+1));

        for (int j = 0; j < model->columnCount(); ++j)
            writeHisoryItem(model->item( i, j));

        xmlWriter.writeEndElement();
    }

    xmlWriter.writeEndDocument();

    file.close();
}

// Write the given item into XML file
void History::writeHisoryItem(QStandardItem *item)
{
    if(item->column() == 0)
    {
        // Convert date to Unix timestamp before save it
        QString date;
        xmlWriter.writeTextElement("date", date.setNum(QDateTime::fromString(item->text(), "dd/MM/yyyy hh:mm:ss").toTime_t()));
    }
    else if(item->column() == 1)
        xmlWriter.writeTextElement("fileName", item->text());
    else if(item->column() == 2)
        xmlWriter.writeTextElement("fileSize", item->text());
    else if(item->column() == 3)
        xmlWriter.writeTextElement("url", item->text());
}

void History::findHundler(QString text)
{
    QRegExp::PatternSyntax syntax = QRegExp::PatternSyntax(2);
    Qt::CaseSensitivity caseSensitivity = Qt::CaseInsensitive;
    QRegExp regExp(text, caseSensitivity, syntax);

    proxyModel->setFilterKeyColumn(-1);
    proxyModel->setFilterRegExp(regExp);
}

void History::clearHistory()
{
    if(model->rowCount() == 0)
    {
         QMessageBox::information(this, tr("Clear History"),
                              tr("There is nothing to clear"),
                              QMessageBox::Ok);
         return;
    }

    int ret = QMessageBox::warning(this, tr("Clear History?"),
                              tr("Are you sure you want to clear downloads history?"),
                              QMessageBox::Yes | QMessageBox::No,
                              QMessageBox::No);

    if (ret == QMessageBox::No)
        return;


    if (!file.remove())
    {
        QMessageBox::warning(this, tr("Clear History"),
                              tr("Cannot remove file %1:\n%2.")
                              .arg(file.fileName())
                              .arg(file.errorString()));
        return;
    }

    model->removeRows(0, model->rowCount());
}
