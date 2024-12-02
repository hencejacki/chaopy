/*
 * File: ChaopySetting.h
 * Description: Setting for software
 * Author: HenceJacki <hencejacki@gmail.com>
 * Date: - 20241125
 *
 * Revision History:
 * - 20241125 -: Initial creation.
 * 
 * Json format:
 {
	"monitors": [
		{
			"path": "your/path/to/monitor",
			"target": "your/path/to/chaopy",
			"recursive": whether monitor recursively or not
		}
	]
}
 * 
 * Example:
{
	"monitors": [
		{
			"path": "E:/AWorkSpace/Chaopy/bin/Debug",
			"target": "E:/AWorkSpace/Chaopy/test",
			"recursive": true
		}
	]
}
 */

#pragma once

//////////////////////////////////////////////////////////////////////////
#include <QSettings>
#include <QDebug>
//////////////////////////////////////////////////////////////////////////
#include "pch.h"
//////////////////////////////////////////////////////////////////////////
#include "../Constant/Constants.h"
#include "ChaopyConfig.h"

class ChaopySetting {
public:
	Q_DISABLE_COPY(ChaopySetting)
	virtual ~ChaopySetting() = default;

	static ChaopySetting* GetInstance();

	int Init(const std::string& configFile);

	void LoadConfig(bool hotLoad = true);

	std::string GetConfigFileName();

	std::vector<ChaopyConfig_t> GetMonitorConfigs();

	std::string GetTargetPathByMonitorPath(const std::string&);

private:
	static bool readJsonFile(QIODevice& device, QSettings::SettingsMap& map);
	static bool writeJsonFile(QIODevice& device, const QSettings::SettingsMap& map);

private:
	ChaopySetting();

private:
	std::unique_ptr<QSettings> setting_;
	std::string configFile_;
	std::vector<ChaopyConfig_t> configs_;
};