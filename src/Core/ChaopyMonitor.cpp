#include "pch.h"
#include "ChaopyMonitor.h"
#include "MonitorThread.h"
#include "../Config/ChaopySetting.h"
#include "../Util/StringUtil.h"
#include "../Model/ChaopyModel.h"
#include <map>
#include <vector>
#include <QMetaType>

ChaopyMonitor::ChaopyMonitor()
	: monitorInfoCnt_(0UL)
	, mt_(std::make_unique<MonitorThread>())
{
	qRegisterMetaType<FileStateNotification_t>("FileStateNotification_t");
	connect(mt_.get(), &MonitorThread::notificationTriggerd, this, &ChaopyMonitor::notifyObservers);
	initMonitor();
}

ChaopyMonitor::~ChaopyMonitor()
{
	observers_.clear();
	if (mt_->isRunning())
	{
		mt_->exit();
	}
}

void ChaopyMonitor::AttachObs(std::shared_ptr<ChaopyObserver> obs)
{
	observers_.insert(obs);
}

void ChaopyMonitor::notifyObservers(const FileStateNotification_t& stNotification)
{
	std::string configFileName = ChaopySetting::GetInstance()->GetConfigFileName();
	if (0 == std::strcmp(configFileName.c_str(), stNotification.fileName.c_str()))
	{
		mt_->Stop();
		// Reload when config file changed
		ChaopySetting::GetInstance()->LoadConfig(true);
		initMonitor();
		return;
	}

	for (auto &it : observers_)
	{
		it->update(stNotification);
	}
}

void ChaopyMonitor::initMonitor()
{
	auto monitorCfgs = ChaopySetting::GetInstance()->GetMonitorConfigs();

	std::vector<MonitorInfoW_t> vecHandles;
	for (auto& monitorCfg : monitorCfgs)
	{
#ifdef WIN32
		std::wstring monitorPath = StringUtil::ToWString(monitorCfg.path);
		HANDLE hDir = CreateFileW(
			monitorPath.c_str(),
			FILE_LIST_DIRECTORY,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			NULL,
			OPEN_EXISTING,
			FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
			NULL
		);

		if (INVALID_HANDLE_VALUE == hDir)
		{
			spdlog::error("Failed to open directory: [{}], error: {}", monitorCfg.path.c_str(), GetLastError());
			continue;
		}

		MonitorInfoW_t stMonitorInfo{};
		MonitorHandleW info;
		info.hDir = hDir;
		info.buffer = std::make_unique<BYTE[]>(BUFFER_SIZE);

		info.overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		if (NULL == info.overlapped.hEvent) {
			spdlog::error("Failed to create event: {}", GetLastError());
			return;
		}

		BOOL ret = ReadDirectoryChangesW(
			info.hDir,
			info.buffer.get(),
			BUFFER_SIZE,
			monitorCfg.recursive,
			FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_CREATION | FILE_NOTIFY_CHANGE_DIR_NAME,
			NULL,
			&info.overlapped,
			NULL
		);

		if (!ret && ERROR_IO_PENDING != GetLastError())
		{
			spdlog::error("ReadDirectoryChangesW for path-{} failed: {}", monitorCfg.path, GetLastError());
			CloseHandle(hDir);
			CloseHandle(info.overlapped.hEvent);
			continue;
		}

		stMonitorInfo.monitorPath = monitorCfg.path;
		stMonitorInfo.monitorHandle = std::move(info);
		vecHandles.push_back(std::move(stMonitorInfo));
		monitorInfoCnt_++;
#endif
	}

	// Start a thread to wait event triggered
	if (0 == monitorInfoCnt_)
	{
		spdlog::warn("No task to execute.");
		return;
	}

	mt_->SetHandles(std::move(vecHandles));
	mt_->start();
}
