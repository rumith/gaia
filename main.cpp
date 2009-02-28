/***************************************************************************
 *   main.cpp                                                              *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *   Copyright 2008-2009 David Parunakian                                  *
 ***************************************************************************/
#include <QtGui/QApplication>
#include <QtGui/QSplashScreen>
#include <QtUiTools/QUiLoader>
#include <QtCore/QFile>

#include <QtGui/QComboBox>
#include <QtGui/QTreeWidget>
#include <QtGui/QListWidget>

#include <QtGui/QRadioButton>
#include <QtGui/QPushButton>

#include <QtGui/QMenuBar>

#define OPERATOR

#include "logic.h"
#include "config.h"

int main(int argc, char **argv) {
	/* Initiate the application */
	QApplication app(argc, argv);
	QApplication::setWindowIcon(QIcon(":/icon.png"));

	/* Load and display the splash screen */
	QPixmap pic(":/splash.png");
	QSplashScreen splash(pic);
	splash.show();
	app.processEvents();

	/* Load the user interface file */
	QUiLoader loader;
	QFile file(":/redbook.ui");
	file.open(QFile::ReadOnly);
	QWidget *redBook = loader.load(&file, 0);
	file.close();

	/* Load the configuration file */
	QrbConfig *config = new QrbConfig(":/redbook.conf");

	/* Populate the menu bar */
	QMenuBar *menuBar = redBook->findChild<QMenuBar*>("menuBar");

#ifdef OPERATOR
	QAction *editAction = menuBar->addAction(config->value("Labels", "edit").toString());
	editAction->setObjectName("editAction");
	QAction *saveAction = menuBar->addAction(config->value("Labels", "save").toString());
	saveAction->setObjectName("saveAction");
	QAction *cancelAction = menuBar->addAction(config->value("Labels", "cancel").toString());
	cancelAction->setObjectName("cancelAction");
	menuBar->addSeparator();
	editAction->setVisible(false);
	saveAction->setVisible(false);
	cancelAction->setVisible(false);
#endif

	QMenu *indexMenu = menuBar->addMenu(config->value("Labels", "index").toString());
	indexMenu->setObjectName("indexMenu");

	QMenu *specMenu = menuBar->addMenu(config->value("Labels", "spec").toString());
	specMenu->setObjectName("specMenu");
	specMenu->menuAction()->setVisible(false);

	QMenu *fontMenu = menuBar->addMenu(config->value("Labels", "font").toString());
	fontMenu->setObjectName("fontMenu");
	fontMenu->menuAction()->setVisible(false);
	QAction *lgAction = fontMenu->addAction(config->value("Labels", "font1").toString());
	QAction *smAction = fontMenu->addAction(config->value("Labels", "font0").toString());

	QAction *helpAction = menuBar->addAction(config->value("Labels", "help").toString());
	helpAction->setObjectName("helpAction");

	/* Construct the main index widget */
	QListWidget *indexList = redBook->findChild<QListWidget*>("indexList");
	indexList->setCursor(Qt::PointingHandCursor);
	indexList->setSelectionMode(QAbstractItemView::NoSelection);

	/* Initiate program logic */
	Logic *logic = new Logic(config, redBook);
	QObject::connect(editAction, SIGNAL(activated()), logic, SLOT(edit()));
	QObject::connect(saveAction, SIGNAL(activated()), logic, SLOT(saveEdit()));
	QObject::connect(cancelAction, SIGNAL(activated()), logic, SLOT(cancelEdit()));
	QObject::connect(smAction, SIGNAL(activated()), logic, SLOT(smallerFont()));
	QObject::connect(lgAction, SIGNAL(activated()), logic, SLOT(largerFont()));
	QObject::connect(helpAction, SIGNAL(activated()), logic, SLOT(showHelp()));

	QObject::connect(redBook->findChild<QPushButton*>("docPrintButton"), SIGNAL(clicked()), logic, SLOT(printDocument()));
	QObject::connect(redBook->findChild<QPushButton*>("docBackButton"), SIGNAL(clicked()), logic, SLOT(showIndex()));
	QObject::connect(redBook->findChild<QComboBox*>("chapterCombo"), SIGNAL(currentIndexChanged(const QString &)), logic, SLOT(viewMultiDocChapter(const QString &)));

	QObject::connect(redBook->findChild<QPushButton*>("speciesBackButton"), SIGNAL(clicked()), logic, SLOT(showIndex()));
	QObject::connect(redBook->findChild<QPushButton*>("latButton"), SIGNAL(clicked()), logic, SLOT(latAlpha()));
	QObject::connect(redBook->findChild<QPushButton*>("rusButton"), SIGNAL(clicked()), logic, SLOT(rusAlpha()));

	QObject::connect(redBook->findChild<QRadioButton*>("radioc0"), SIGNAL(toggled(bool)), logic, SLOT(setTaxoChapter(bool)));
	QObject::connect(redBook->findChild<QRadioButton*>("radioc1"), SIGNAL(toggled(bool)), logic, SLOT(setTaxoChapter(bool)));
	QObject::connect(redBook->findChild<QRadioButton*>("radioc2"), SIGNAL(toggled(bool)), logic, SLOT(setTaxoChapter(bool)));
	QObject::connect(redBook->findChild<QRadioButton*>("radioc3"), SIGNAL(toggled(bool)), logic, SLOT(setTaxoChapter(bool)));
	QObject::connect(redBook->findChild<QRadioButton*>("radioc4"), SIGNAL(toggled(bool)), logic, SLOT(setTaxoChapter(bool)));
	QObject::connect(redBook->findChild<QRadioButton*>("radioc5"), SIGNAL(toggled(bool)), logic, SLOT(setTaxoChapter(bool)));
	QObject::connect(redBook->findChild<QRadioButton*>("radioc6"), SIGNAL(toggled(bool)), logic, SLOT(setTaxoChapter(bool)));
	QObject::connect(redBook->findChild<QRadioButton*>("radioc7"), SIGNAL(toggled(bool)), logic, SLOT(setTaxoChapter(bool)));

	QObject::connect(redBook->findChild<QTreeWidget*>("taxoTree"), SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), logic, SLOT(treeItemSelected(QTreeWidgetItem *)));
	QObject::connect(redBook->findChild<QListWidget*>("alphaList"), SIGNAL(itemDoubleClicked(QListWidgetItem*)), logic, SLOT(listItemSelected(QListWidgetItem *)));

	QObject::connect(redBook->findChild<QPushButton*>("backToListsButton"), SIGNAL(clicked()), logic, SLOT(up()));
	QObject::connect(redBook->findChild<QPushButton*>("prevButton"), SIGNAL(clicked()), logic, SLOT(prevSpecies()));
	QObject::connect(redBook->findChild<QPushButton*>("nextButton"), SIGNAL(clicked()), logic, SLOT(nextSpecies()));
	QObject::connect(redBook->findChild<QPushButton*>("printButton"), SIGNAL(clicked()), logic, SLOT(printSpecies()));
	QObject::connect(redBook->findChild<QListWidget*>("sectionList"), SIGNAL(itemClicked(QListWidgetItem*)), logic, SLOT(setArticle(QListWidgetItem*)));

	QObject::connect(&app, SIGNAL(focusChanged(QWidget*, QWidget*)), logic, SLOT(changeFocus(QWidget*, QWidget*)));

	QObject::connect(indexList, SIGNAL(itemClicked(QListWidgetItem*)), logic, SLOT(indexWidgetClicked(QListWidgetItem*)));

	/* Populate the index */
	logic->initIndex();
	foreach (const QString &param, config->parameters("Index")) {
		QAction *action = indexMenu->addAction(config->value("Index", param).toString());
		action->setData(param);
		QObject::connect(action, SIGNAL(triggered()), logic, SLOT(indexMenuClicked()));
	}

	/* Set advanced style information */
	app.setStyleSheet("QRadioButton::indicator::checked { image: url(\":/radio.png\") } QListWidget#indexList::item::hover { background: qlineargradient(spread:pad, x1:0, y1:1,  x2:0, y2:0, stop:0 rgba(255, 255, 255, 255), stop:0.0157895 rgba(255, 197, 166, 255), stop:0.647368 rgba(255, 255, 255, 255), stop:0.757895 rgba(255, 255, 255, 255), stop:1 rgba(255, 220, 197, 255)); }");

	/* Hide the splash screen and show the main program interface */
	sleep(2);
	redBook->show();
	splash.finish(redBook);
	return app.exec();
}
