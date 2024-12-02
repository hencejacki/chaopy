#include "pch.h"
#include "ChaopySetting.h"
#include <QIODevice>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonValue>
#include <QStringList>

ChaopySetting::ChaopySetting()
	: setting_(nullptr)
	, configFile_("")
{
}

ChaopySetting* ChaopySetting::GetInstance()
{
	static ChaopySetting ins;
	return &ins;
}

int ChaopySetting::Init(const std::string& configFile)
{
	const QSettings::Format jsonFormat =
		QSettings::registerFormat(DEFAULT_CONFIG_FORMAT, readJsonFile, writeJsonFile);
	setting_ = std::make_unique<QSettings>(QString(configFile.c_str()), jsonFormat);
	configFile_ = configFile;
	return 0;
}

void ChaopySetting::LoadConfig(bool hotLoad)
{
	if (hotLoad)
	{
		// Reset setting's ptr
		setting_.reset();
		const QSettings::Format jsonFormat =
			QSettings::registerFormat(DEFAULT_CONFIG_FORMAT, readJsonFile, writeJsonFile);
		setting_ = std::make_unique<QSettings>(QString(configFile_.data()), jsonFormat);
	}

	const QJsonArray monitors = setting_->value("monitors").toJsonArray();
	if (monitors.isEmpty()) {
		spdlog::info("Config file is empty.");
		return;
	}

	for (auto monitor : monitors)
	{
		if (monitor.isObject())
		{
			ChaopyConfig_t stConfig{};
			stConfig.path = monitor["path"].toString().toUtf8().constData();
			stConfig.target = monitor["target"].toString().toUtf8().constData();
			stConfig.recursive = monitor["recursive"].toBool();

			spdlog::info("Got monitor path-[{}] with recursive-[{}] and target-[{}].", stConfig.path.c_str(), stConfig.recursive, stConfig.target.c_str());
			configs_.push_back(stConfig);
		}
	}
}

std::string ChaopySetting::GetConfigFileName()
{
	size_t pos = configFile_.find_last_of('/');
	if (configFile_.npos == pos)
	{
		spdlog::warn("Error get file name of config file-[{}].", configFile_.c_str());
		return "";
	}
	return configFile_.substr(pos);
}

std::vector<ChaopyConfig_t> ChaopySetting::GetMonitorConfigs()
{
	return configs_;
}

std::string ChaopySetting::GetTargetPathByMonitorPath(const std::string& monitorPath)
{
	std::string ret = "";
	for (auto& cfg : configs_)
	{
		if (0 == std::strcmp(monitorPath.c_str(), cfg.path.c_str()))
		{
			ret = cfg.target;
			break;
		}
	}
	return ret;
}

bool ChaopySetting::readJsonFile(QIODevice& device, QSettings::SettingsMap& map)
{
	QJsonParseError err;
	map = QJsonDocument::fromJson(device.readAll(), &err).toVariant().toMap();
	return err.error == QJsonParseError::NoError;
}

bool ChaopySetting::writeJsonFile(QIODevice& device, const QSettings::SettingsMap& map)
{
	const auto json = QJsonDocument::fromVariant(map).toJson();
	return device.write(json) == static_cast<qint64>(json.size());
}
