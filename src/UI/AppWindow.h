#pragma once

#include <QDialog>
#include <QSystemTrayIcon>
#include "ui_AppWindow.h"
#include "../Core/ChaopyObserver.h"

class AppWindow : public QDialog, public ChaopyObserver
{
	Q_OBJECT

public:
	AppWindow(QWidget *parent = nullptr);
	~AppWindow();

private:
	void initUi();
	void initLogic();

private slots:
	void onTrayActivated(QSystemTrayIcon::ActivationReason reason);

protected:
	void closeEvent(QCloseEvent*) override;

	void update(const FileStateNotification_t&) override;

private:
	Ui::AppWindow ui;
	std::unique_ptr<QSystemTrayIcon> systemTray_;
	std::unique_ptr<QMenu> menuTray_;
	std::string curLog;
};
