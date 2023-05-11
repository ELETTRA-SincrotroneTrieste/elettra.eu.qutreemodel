/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qutreemodel.h"
#include "qutreeitem.h"

#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QtWidgets>

class QuTreeModelPrivate {
public:
    QuTreeModelPrivate() : separator("/"), rootItem(nullptr) {}

    QString separator;
    QuTreeItem *rootItem;
};

QuTreeModel::QuTreeModel(const QStringList &headers, QObject *parent)
    : QAbstractItemModel(parent) {
    d = new QuTreeModelPrivate;
    QVector<QVariant> rootData;
    for (const QString &header : headers)
        rootData << header;

    d->rootItem = new QuTreeItem(rootData);
}
QuTreeModel::~QuTreeModel() {
    delete d->rootItem;
    delete d;
}

int QuTreeModel::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return d->rootItem->columnCount();
}

QVariant QuTreeModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return QVariant();

    QuTreeItem *item = getItem(index);

    return item->data(index.column(), role);
}

Qt::ItemFlags QuTreeModel::flags(const QModelIndex &index) const {
    if (!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsEditable | QAbstractItemModel::flags(index);
}

QuTreeItem *QuTreeModel::getItem(const QModelIndex &index) const {
    if (index.isValid()) {
        QuTreeItem *item = static_cast<QuTreeItem*>(index.internalPointer());
        if (item)
            return item;
    }
    return d->rootItem;
}

QVariant QuTreeModel::headerData(int section, Qt::Orientation orientation,
                                 int role) const {
    if (orientation == Qt::Horizontal)
        return d->rootItem->data(section, role);

    return QVariant();
}

QModelIndex QuTreeModel::index(int row, int column, const QModelIndex &parent) const {
    if (parent.isValid() && parent.column() != 0)
        return QModelIndex();

    QuTreeItem *parentItem = getItem(parent);
    if (!parentItem)
        return QModelIndex();

    QuTreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    return QModelIndex();
}

bool QuTreeModel::insertColumns(int position, int columns, const QModelIndex &parent) {
    beginInsertColumns(parent, position, position + columns - 1);
    const bool success = d->rootItem->insertColumns(position, columns);
    endInsertColumns();

    return success;
}

bool QuTreeModel::insertRows(int position, int rows, const QModelIndex &parent)
{
    QuTreeItem *parentItem = getItem(parent);
    if (!parentItem)
        return false;

    beginInsertRows(parent, position, position + rows - 1);
    const bool success = parentItem->insertChildren(position,
                                                    rows,
                                                    d->rootItem->columnCount());
    endInsertRows();

    return success;
}

QModelIndex QuTreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    QuTreeItem *childItem = getItem(index);
    QuTreeItem *parentItem = childItem ? childItem->parent() : nullptr;

    if (parentItem == d->rootItem || !parentItem)
        return QModelIndex();

    return createIndex(parentItem->childNumber(), 0, parentItem);
}

bool QuTreeModel::removeColumns(int position, int columns, const QModelIndex &parent)
{
    beginRemoveColumns(parent, position, position + columns - 1);
    const bool success = d->rootItem->removeColumns(position, columns);
    endRemoveColumns();

    if (d->rootItem->columnCount() == 0)
        removeRows(0, rowCount());

    return success;
}

bool QuTreeModel::removeRows(int position, int rows, const QModelIndex &parent)
{
    QuTreeItem *parentItem = getItem(parent);
    if (!parentItem)
        return false;

    beginRemoveRows(parent, position, position + rows - 1);
    const bool success = parentItem->removeChildren(position, rows);
    endRemoveRows();

    return success;
}

void QuTreeModel::setSeparator(const QString &s) {
    d->separator = s;
}

QString QuTreeModel::separator() const {
    return d->separator;
}

int QuTreeModel::rowCount(const QModelIndex &parent) const
{
    const QuTreeItem *parentItem = getItem(parent);

    return parentItem ? parentItem->childCount() : 0;
}

bool QuTreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    QuTreeItem *item = getItem(index);
    bool result = item->setData(index.column(), value, role);

    if (result)
        emit dataChanged(index, index, { role} );

    return result;
}

bool QuTreeModel::setHeaderData(int section, Qt::Orientation orientation,
                                const QVariant &value, int role)
{
    if (orientation != Qt::Horizontal)
        return false;

    const bool result = d->rootItem->setData(section, value, role);

    if (result)
        emit headerDataChanged(orientation, section, section);

    return result;
}

QModelIndex QuTreeModel::findItem(const QString& s) {
    return m_find_item(s, 0, QModelIndex(), Qt::DisplayRole);

}

// remove the item matching exactly s
// return the item above the removed one if possible
QModelIndex QuTreeModel::removeItem(const QString &s) {
    const QModelIndex& ri = findItem(s);
    const QModelIndex& parent = ri.parent();
    if(ri.isValid()) {
        removeRows(ri.row(), 1, ri.parent());
    }
    if(parent.isValid()) {
        return index(rowCount(parent) - 1, 0, parent);
    }
    return QModelIndex();
}

QList<QModelIndex> QuTreeModel::findItems(const QString &search, int match_mode, int role) const {
    QList<QModelIndex> il;
    QRegularExpression re(QString(".*%1.*").arg(search));
    il += m_find_items(re, match_mode, QModelIndex(), role);
    return il;
}

QList<QModelIndex> QuTreeModel::m_find_items(const QRegularExpression& re, int match_mode, const QModelIndex& parent, int role) const {
    QList<QModelIndex> il;
    QRegularExpressionMatch ma;
    for(int i = 0; i < rowCount(parent); i++) {
        for(int c = 0; c < columnCount(parent); c++) {
            const QModelIndex &idx = index(i, c, parent);
            ma = re.match(data(idx, role ).toString());
            if(ma.hasMatch())
                il.push_back(idx);
//            printf("r. %d c. %d searched %s next idx valid %s\n", i, c, qstoc(re.pattern()), idx.isValid() ? "YES" : "NO");
            if(idx.isValid())
                il += m_find_items(re, match_mode, idx, role);
        }
    }
    return il;
}

QList<QModelIndex> QuTreeModel::itemsWithChildren(const QModelIndex &parent)  {
    QList<QModelIndex> li;
    if(hasChildren(parent))
        li << parent;
    for(int i = 0; i < rowCount(parent); i++) {
        QModelIndex idx = index(i, 0, parent);
        if(idx.isValid() && hasChildren(idx)) {
            li << idx;
            li += itemsWithChildren(idx);
        }
    }
    return li;
}

QString QuTreeModel::itemRepr(const QModelIndex &idx) {
    QString s;
    QModelIndex i(idx);
    while(i.isValid()) {
        s = i.data().toString()
                + (!s.isEmpty()
                   ? "/" : "")
                + s;
        i = i.parent();
    }
    return s;
}

QModelIndex QuTreeModel::addItem(const QString &s) {
    return m_add_item(s, QModelIndex());
}

QModelIndex QuTreeModel::m_add_item(const QString &s, const QModelIndex &parent) {
    bool ok = true;
    cuprintf("\e[0;32m+ %s under %s\e[0m\n", qstoc(s), parent.isValid() ? qstoc(parent.data(Qt::DisplayRole).toString()) : "-");
#if QT_VERSION < QT_VERSION_CHECK(5, 12, 0)
    QStringList p = s.split(d->separator, QString::SkipEmptyParts);
#else
    QStringList p = s.split(d->separator, Qt::SkipEmptyParts);
#endif
    QModelIndex t;
    if(p.size() > 0) {
        t = m_find_item(p[0], 0, parent, Qt::DisplayRole);
        if(!t.isValid()) {
            if(!parent.isValid()) {
                ok = insertRows(d->rootItem->childCount(), 1, QModelIndex());
                if(ok) {
                    t = index(d->rootItem->childCount() - 1, 0, QModelIndex());
                    ok = setData(t, p[0], Qt::DisplayRole);
                }
            }
            else {
                ok = insertRows(rowCount(parent), 1, parent);
                if(ok) {
                    t = index(rowCount(parent) - 1, parent.column(), parent);
                    ok = setData(t, p[0], Qt::DisplayRole);
                }
            }
        }
        if(ok && p.size() > 1)
            t = m_add_item(s.section('/', 1), t);
    }
    return t;
}

QModelIndex QuTreeModel::m_find_item(const QString &s, int column, const QModelIndex &parent, int role)
{
//    printf("   searching for %s under %s with %d children", qstoc(s),
//           parent.isValid() ? qstoc(parent.data(Qt::DisplayRole).toString()) : "INVALID", parent.isValid() ? rowCount(parent) : -1);
//    if(parent.isValid())
//        printf(" (%s)\n", qstoc(parent.data(Qt::DisplayRole).toString()));
//    else printf("\n");

#if QT_VERSION < QT_VERSION_CHECK(5, 12, 0)
    const QStringList& p = s.split(d->separator, QString::SkipEmptyParts);
#else
   const QStringList& p = s.split(d->separator, Qt::SkipEmptyParts);
#endif
    assert(p.size() > 0);

    if(parent.isValid() && parent.data(role).toString() == p[0])
        return parent;

    if(!parent.isValid()) {
        for(int i = 0; i < d->rootItem->childCount(); i++) {
//            printf("      \e[1,33m searching %s under top level items %d text %s\e[0m\n",
//                   qstoc( p[0]),
//                    d->rootItem->childCount(),
//                    qstoc(d->rootItem->child(i)->data(0, Qt::DisplayRole).toString()));
            QModelIndex idx = index(i, column, QModelIndex());
            if(idx.data(role).toString() == p[0])
                return s.count('/') > 0 ? m_find_item(s.section('/', 1), 0, idx) : idx;
        }
    }
    else {
        for(int i = 0; i < rowCount(parent); i++) {
//            printf("      \e[1,36m searching %s under item with %d children text %s\e[0m\n",
//                   qstoc( p[0]),
//                    rowCount(parent),
//                    qstoc(index(i, 0, parent).data(Qt::DisplayRole).toString()));

            if(index(i, parent.column(), parent).data(role) == p[0])
                return s.count('/') > 0 ? m_find_item(s.section('/', 1), 0, index(i, parent.column(), parent)) : index(i, parent.column(), parent);
        }
    }
//    printf("   \e[1;31m no item found under parent %s while searching %s\e[0m\n",
//           parent.isValid() ? qstoc(parent.data(Qt::DisplayRole).toString()) : "-",  qstoc(s));
    return QModelIndex();
}


void QuTreeModel::setupModelData(const QStringList &lines, QuTreeItem *parent)
{
    QVector<QuTreeItem*> parents;
    QVector<int> indentations;
    parents << parent;
    indentations << 0;

    int number = 0;

    while (number < lines.count()) {
        int position = 0;
        while (position < lines[number].length()) {
            if (lines[number].at(position) != ' ')
                break;
            ++position;
        }

        const QString lineData = lines[number].mid(position).trimmed();

        if (!lineData.isEmpty()) {
            // Read the column data from the rest of the line.
#if QT_VERSION < QT_VERSION_CHECK(5, 12, 0)
            const QStringList columnStrings =
                    lineData.split(QLatin1Char('\t'), QString::SkipEmptyParts);
#else
            const QStringList columnStrings =
                    lineData.split(QLatin1Char('\t'), Qt::SkipEmptyParts);
#endif
            QVector<QVariant> columnData;
            columnData.reserve(columnStrings.size());
            for (const QString &columnString : columnStrings)
                columnData << columnString;

            if (position > indentations.last()) {
                // The last child of the current parent is now the new parent
                // unless the current parent has no children.

                if (parents.last()->childCount() > 0) {
                    parents << parents.last()->child(parents.last()->childCount()-1);
                    indentations << position;
                }
            } else {
                while (position < indentations.last() && parents.count() > 0) {
                    parents.pop_back();
                    indentations.pop_back();
                }
            }

            // Append a new item to the current parent's list of children.
            QuTreeItem *parent = parents.last();
            parent->insertChildren(parent->childCount(), 1, d->rootItem->columnCount());
            for (int column = 0; column < columnData.size(); ++column)
                parent->child(parent->childCount() - 1)->setData(column, columnData[column], Qt::DisplayRole);
        }
        ++number;
    }
}

QAbstractItemModel *QuTreeModel::qmodel() {
    return this;
}
