/*
 * File: ChaopyMonitor.h
 * Description:
 * Author: HenceJacki <hencejacki@gmail.com>
 * Date: 20241128
 *
 * Revision History:
 * - 20241128: Initial creation.
 */

#pragma once

#include <set>
#include <stdint.h>
#include <QObject>

#include "ChaopyObserver.h"

class MonitorThread;

class ChaopyMonitor : public QObject {
	Q_OBJECT

public:
	ChaopyMonitor();
	virtual ~ChaopyMonitor();

	void AttachObs(std::shared_ptr<ChaopyObserver>);

	void DetachObs(std::shared_ptr<ChaopyObserver>);

private:
	void initMonitor();

public slots:
	void notifyObservers(const FileStateNotification_t&);

	void onMonitorThreadExit();

private:
	std::set<std::shared_ptr<ChaopyObserver>> observers_;
	std::shared_ptr<MonitorThread> mt_;
#ifdef WIN32
	DWORD monitorInfoCnt_;
#endif
};