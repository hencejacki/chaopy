#include "pch.h"
#include "ChaopyCopy.h"
#include "../Config/ChaopySetting.h"
#include <QFile>
#include <QDir>

ChaopyCopy::ChaopyCopy()
	: running_(true)
{
	pool_ = std::make_unique<ThreadPool>(3);
	pool_->init();
	start();
}

ChaopyCopy::~ChaopyCopy()
{
	stop();
}

void ChaopyCopy::update(const FileStateNotification_t& stInfo)
{
	{
		std::lock_guard<std::mutex> lock(mtx_);
		task_.push(stInfo);
		cv_.notify_one();
	}
}

void ChaopyCopy::start()
{
	pool_->submit([this]() {
		while (running_)
		{
			std::unique_lock<std::mutex> lock(mtx_);
			cv_.wait(lock, [&] {return !task_.empty() || !running_; });

			if (!running_) break;

			FileStateNotification_t stInfo = task_.front();
			task_.pop();
			
			// Copy
			size_t pos = stInfo.filePath.find_last_of("\\");
			if (std::string::npos == pos)
			{
				continue;
			}
			std::string monitorPath = stInfo.filePath.substr(0, pos);
			std::string target = ChaopySetting::GetInstance()->GetTargetPathByMonitorPath(monitorPath);
			QString targetPath = QString("%1\\%2").arg(target.c_str()).arg(stInfo.fileName.c_str());
			QFile src(stInfo.filePath.c_str());
			if (QFile::exists(targetPath))
			{
				QFile::remove(targetPath);
			}
			if (src.copy(QDir::toNativeSeparators(targetPath)))
			{
				spdlog::info("Move {} into {} success.", stInfo.filePath.c_str(), targetPath.toUtf8().constData());
			}
		}
	});
}

void ChaopyCopy::stop()
{
	running_ = false;
	cv_.notify_all();
	pool_->shutdown();
}

