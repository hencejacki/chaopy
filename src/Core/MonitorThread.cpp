#include "pch.h"
#include "MonitorThread.h"
#include "../Util/StringUtil.h"
#include <QDateTime>

MonitorThread::MonitorThread()
	: running_(true)
{
}

MonitorThread::~MonitorThread()
{
	Stop();
	wait();
}

void MonitorThread::SetHandles(std::vector<MonitorInfoW_t>&& infos)
{
	vecMonitorInfos_ = std::move(infos);
}

void MonitorThread::run()
{
	while (running_)
	{
#ifdef WIN32
		std::vector<HANDLE> handleArr(vecMonitorInfos_.size(), nullptr);
		for (size_t i = 0; i < vecMonitorInfos_.size(); ++i)
		{
			handleArr[i] = vecMonitorInfos_[i].monitorHandle.overlapped.hEvent;
		}
		DWORD dwEvent = WaitForMultipleObjects(
			static_cast<DWORD>(vecMonitorInfos_.size()),
			handleArr.data(),
			FALSE,
			5000
		);

		if (dwEvent == WAIT_FAILED) {
			spdlog::error("Failed to wait: {}", GetLastError());
			continue;
		}

		if (WAIT_TIMEOUT == dwEvent)
		{
			spdlog::warn("Wait to retry: {}", GetLastError());
			continue;
		}

		int pos = static_cast<int>(dwEvent - WAIT_OBJECT_0);
		if (pos < 0 || pos >= static_cast<int>(vecMonitorInfos_.size())) {
			spdlog::error("Invalid event index: {}", pos);
			continue;
		}

		auto& info = vecMonitorInfos_[pos];
		auto& handle = info.monitorHandle;
		auto& event = handle.overlapped.hEvent;
		
		DWORD bytesReturned = 0;
		if (GetOverlappedResult(handle.hDir, &handle.overlapped, &bytesReturned, TRUE)) {
			PFILE_NOTIFY_INFORMATION pNotify = reinterpret_cast<PFILE_NOTIFY_INFORMATION>(handle.buffer.get());
			do {
				std::wstring wStrFileName(pNotify->FileName, pNotify->FileNameLength / sizeof(WCHAR));
				std::string nStrFileName = StringUtil::FromWString(wStrFileName);

				switch (pNotify->Action) {
				case FILE_ACTION_ADDED:
					spdlog::info("Added: {}", nStrFileName);
					break;
				case FILE_ACTION_REMOVED:
					spdlog::info("Removed: {}", nStrFileName);
					break;
				case FILE_ACTION_MODIFIED:
					spdlog::info("Modified: {}", nStrFileName);
					break;
				case FILE_ACTION_RENAMED_OLD_NAME:
					spdlog::info("Renamed (old): {}", nStrFileName);
					break;
				case FILE_ACTION_RENAMED_NEW_NAME:
					spdlog::info("Renamed (new): {}", nStrFileName);
					break;
				default:
					spdlog::warn("Unknown action: {}", pNotify->Action);
					break;
				}

				FileStateNotification_t stNotification{};
				stNotification.fileName = nStrFileName;
				stNotification.filePath = QString("%1\\%2").arg(info.monitorPath.c_str()).arg(nStrFileName.c_str()).toUtf8().constData();
				stNotification.fileAction = pNotify->Action;
				stNotification.lastModifyTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss").toUtf8().constData();
				emit notificationTriggerd(stNotification);

				pNotify = reinterpret_cast<PFILE_NOTIFY_INFORMATION>(
					reinterpret_cast<BYTE*>(pNotify) + pNotify->NextEntryOffset);
			} while (pNotify->NextEntryOffset != 0);

			// Restart monitor
			// DO NOT reuse overlapped anymore!
			ZeroMemory(&handle.overlapped, sizeof(handle.overlapped));
			// Clean up old event
			if (event)
			{
				CloseHandle(event);
			}
			event = CreateEvent(NULL, TRUE, FALSE, NULL);
			if (NULL == event) {
				spdlog::error("Failed to create event: {}", GetLastError());
				return;
			}
			
			BOOL ret = ReadDirectoryChangesW(
				handle.hDir,
				handle.buffer.get(),
				BUFFER_SIZE,
				TRUE,
				FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_CREATION | FILE_NOTIFY_CHANGE_DIR_NAME,
				NULL,
				&handle.overlapped,
				NULL);

			if (!ret && ERROR_IO_PENDING != GetLastError())
			{
				spdlog::error("Failed to reinitialize monitoring: {}", GetLastError());
				CloseHandle(handle.hDir);
				CloseHandle(event);
				continue;
			}

			
		}
		else {
			DWORD error = GetLastError();
			if (error == ERROR_IO_PENDING) {
				spdlog::debug("Operation is still pending.");
			}
			else {
				spdlog::error("GetOverlappedResult failed: {}", error);
			}
		}
#endif
	}
}

void MonitorThread::Stop()
{
	running_ = false;
	for (auto& info : vecMonitorInfos_) {
		if (info.monitorHandle.overlapped.hEvent) {
			CloseHandle(info.monitorHandle.overlapped.hEvent);
			info.monitorHandle.overlapped.hEvent = nullptr;
		}
	}
	vecMonitorInfos_.clear();
}

