/* column_preferences_frame.cpp
 *
 * Wireshark - Network traffic analyzer
 * By Gerald Combs <gerald@wireshark.org>
 * Copyright 1998 Gerald Combs
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "config.h"

#include <glib.h>

#include <epan/column-info.h>
#include <epan/column.h>
#include <epan/prefs.h>
#include <epan/proto.h>

#include <ui/preference_utils.h>

#include <ui/qt/utils/qt_ui_utils.h>
#include "column_preferences_frame.h"
#include <ui_column_preferences_frame.h>
#include <ui/qt/widgets/syntax_line_edit.h>
#include <ui/qt/widgets/field_filter_edit.h>
#include <ui/qt/models/column_list_model.h>
#include "wireshark_application.h"

#include <QComboBox>
#include <QTreeWidgetItemIterator>
#include <QLineEdit>
#include <QKeyEvent>

ColumnPreferencesFrame::ColumnPreferencesFrame(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::ColumnPreferencesFrame)
{
    ui->setupUi(this);

    model_ = new ColumnListModel();
    proxyModel_ = new ColumnProxyModel();
    proxyModel_->setSourceModel(model_);

    int one_em = ui->columnTreeView->fontMetrics().height();
    ui->columnTreeView->setColumnWidth(ColumnListModel::COL_FIELDS, one_em * 10);
    ui->columnTreeView->setColumnWidth(ColumnListModel::COL_OCCURRENCE, one_em * 5);

    ui->columnTreeView->setMinimumWidth(one_em * 20);
    ui->columnTreeView->setMinimumHeight(one_em * 12);

    ui->columnTreeView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->columnTreeView->setDragEnabled(true);
    ui->columnTreeView->viewport()->setAcceptDrops(true);
    ui->columnTreeView->setDropIndicatorShown(true);
    ui->columnTreeView->setDragDropMode(QAbstractItemView::InternalMove);

    ui->newToolButton->setStockIcon("list-add");
    ui->deleteToolButton->setStockIcon("list-remove");

    ui->columnTreeView->setModel(proxyModel_);
    ui->columnTreeView->setItemDelegate(new ColumnTypeDelegate());
    ui->columnTreeView->setSortingEnabled(false);

    ui->columnTreeView->resizeColumnToContents(ColumnListModel::COL_DISPLAYED);
    ui->columnTreeView->resizeColumnToContents(ColumnListModel::COL_TITLE);
    ui->columnTreeView->resizeColumnToContents(ColumnListModel::COL_TYPE);

    connect(ui->columnTreeView->selectionModel(), &QItemSelectionModel::selectionChanged,
        this, &ColumnPreferencesFrame::selectionChanged);
}

ColumnPreferencesFrame::~ColumnPreferencesFrame()
{
    delete ui;
}

void ColumnPreferencesFrame::unstash()
{
    model_->saveColumns();
    wsApp->emitAppSignal(WiresharkApplication::ColumnsChanged);
}

void ColumnPreferencesFrame::on_newToolButton_clicked()
{
    model_->addEntry();
}

void ColumnPreferencesFrame::on_deleteToolButton_clicked()
{
    if ( ui->columnTreeView->selectionModel()->selectedIndexes().count() > 0 )
    {
        QModelIndex selIndex = ui->columnTreeView->selectionModel()->selectedIndexes().at(0);
        model_->deleteEntry(proxyModel_->mapToSource(selIndex).row());
    }
}

void ColumnPreferencesFrame::selectionChanged(const QItemSelection &/*selected*/,
    const QItemSelection &/*deselected*/)
{
    ui->deleteToolButton->setEnabled( ui->columnTreeView->selectionModel()->selectedIndexes().count() > 0 );
}

void ColumnPreferencesFrame::on_chkShowDisplayedOnly_stateChanged(int /*state*/)
{
    proxyModel_->setShowDisplayedOnly(ui->chkShowDisplayedOnly->checkState() == Qt::Checked ? true : false);
}

/*
 * Editor modelines
 *
 * Local Variables:
 * c-basic-offset: 4
 * tab-width: 8
 * indent-tabs-mode: nil
 * End:
 *
 * ex: set shiftwidth=4 tabstop=8 expandtab:
 * :indentSize=4:tabSize=8:noTabs=true:
 */
