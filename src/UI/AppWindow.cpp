#include "pch.h"
#include "AppWindow.h"
#include "../Config/ChaopySetting.h"
#include "../Util/StringUtil.h"
#include <QIcon>
#include <QMenu>
#include <QAction>
#include <QFile>
#include <QDir>
#include <QMessageBox>
#include <QCloseEvent>

AppWindow::AppWindow(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	initUi();
	initLogic();
}

AppWindow::~AppWindow()
{
}

void AppWindow::initUi()
{
	systemTray_ = std::make_unique<QSystemTrayIcon>(QIcon(":/icon.jpg"), this);
	menuTray_ = std::make_unique<QMenu>();

	// Add menu item
	QAction* quitAction = new QAction("Quit", menuTray_.get());
	quitAction->setIcon(QIcon(":/quit.png"));
	menuTray_->addAction(quitAction);

	systemTray_->setContextMenu(menuTray_.get());

	systemTray_->show();

	connect(systemTray_.get(), &QSystemTrayIcon::activated, this, &AppWindow::onTrayActivated);

	connect(quitAction, &QAction::triggered, qApp, &QApplication::quit);

	// Log text edit
	ui.logTextEdit->setReadOnly(true);
}

void AppWindow::initLogic()
{

}

void AppWindow::closeEvent(QCloseEvent* e)
{
	this->hide();
	e->ignore();
}

void AppWindow::update(const FileStateNotification_t& stNotification)
{
	QString logFormat = QString("[%1] is operated on [%3].")
		.arg(stNotification.filePath.c_str())
		.arg(stNotification.lastModifyTime.c_str());
	ui.logTextEdit->appendPlainText(logFormat);
}

void AppWindow::onTrayActivated(QSystemTrayIcon::ActivationReason reason)
{
	switch (reason)
	{
	case QSystemTrayIcon::Unknown:
		break;
	case QSystemTrayIcon::Context:
		break;
	case QSystemTrayIcon::DoubleClick:
		this->show();
		break;
	case QSystemTrayIcon::Trigger:
		break;
	case QSystemTrayIcon::MiddleClick:
		break;
	default:
		break;
	}
}