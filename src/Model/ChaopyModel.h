#pragma once

#ifdef WIN32	// windows
#include <Windows.h>
#else
#endif
#include <string>
#include <memory>

#ifdef WIN32
const DWORD BUFFER_SIZE = 1024;
#endif

typedef struct FileStateNotification {

	std::string fileName;
	std::string filePath;
#ifdef WIN32
	DWORD fileAction;
#endif
	std::string lastModifyTime;

	FileStateNotification()
		: fileName("")
		, filePath("")
		, fileAction(0)
		, lastModifyTime("")
	{

	}

	FileStateNotification& operator =(const FileStateNotification& other) {
		fileName = other.fileName;
		filePath = other.filePath;
		fileAction = other.fileAction;
		lastModifyTime = other.lastModifyTime;

		return *this;
	}

	FileStateNotification(const FileStateNotification& other)
	{
		operator =(other);
	}

}FileStateNotification_t;

typedef struct MonitorHandleW {
	HANDLE hDir;
	OVERLAPPED overlapped;
	std::unique_ptr<BYTE[]> buffer;

	MonitorHandleW() : hDir(NULL), overlapped(), buffer(nullptr) {
		ZeroMemory(&overlapped, sizeof(overlapped));
	}

	MonitorHandleW(const MonitorHandleW&) = delete;
	MonitorHandleW& operator=(const MonitorHandleW&) = delete;

	MonitorHandleW(MonitorHandleW&& other) noexcept
		: hDir(other.hDir), overlapped(other.overlapped), buffer(std::move(other.buffer)) {
		other.hDir = NULL;
		ZeroMemory(&other.overlapped, sizeof(other.overlapped));
	}

	MonitorHandleW& operator=(MonitorHandleW&& other) noexcept {
		if (this != &other) {
			hDir = other.hDir;
			overlapped = other.overlapped;
			buffer = std::move(other.buffer);
			other.hDir = NULL;
			ZeroMemory(&other.overlapped, sizeof(other.overlapped));
		}
		return *this;
	}
}MonitorHandleW_t;

typedef struct MonitorInfoW
{
	std::string monitorPath;
	MonitorHandleW_t monitorHandle;

	MonitorInfoW()
		: monitorPath("")
		, monitorHandle{}
	{
	}

	MonitorInfoW(MonitorInfoW&& other) noexcept
		: monitorPath(std::move(other.monitorPath))
		, monitorHandle(std::move(other.monitorHandle))
	{
	}

	MonitorInfoW& operator=(MonitorInfoW&& other) noexcept
	{
		if (this != &other) {
			monitorPath = std::move(other.monitorPath);
			monitorHandle = std::move(other.monitorHandle);
		}
		return *this;
	}

} MonitorInfoW_t;
