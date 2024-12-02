/*
 * File: MonitorThread.h
 * Description:
 * Author: HenceJacki <hencejacki@gmail.com>
 * Date: 20241130
 *
 * Revision History:
 * - 20241130: Initial creation.
 */

#pragma once

#include <QThread>
#include <mutex>
#include <atomic>

#include "../Model/ChaopyModel.h"

class MonitorThread : public QThread
{
	Q_OBJECT

public:
	MonitorThread();
	virtual ~MonitorThread();

public:
	void SetHandles(std::vector<MonitorInfoW_t>&&);

	void Stop();

signals:
	void threadFinished();
	void notificationTriggerd(const FileStateNotification_t&);

protected:
	void run() override;

private:
#ifdef WIN32
	std::vector<MonitorInfoW_t> vecMonitorInfos_;
#endif
	std::atomic<bool> running_;
};