#include "pch.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QIcon>

#include "./Config/ChaopySetting.h"
#include "./Core/ChaopyMonitor.h"
#include "./Core/ChaopyCopy.h"
#include "./UI/AppWindow.h"
#include "version.h"

int main(int argc, char** argv)
{
	QApplication app(argc, argv);

	// Log initialize
	auto logger = spdlog::daily_logger_mt("chaopy_logger", "Logs/chaopy.txt", 2, 30);
	spdlog::set_default_logger(logger);
	spdlog::flush_every(std::chrono::seconds(3));

	// Application information
	app.setApplicationName(APPLICATION_NAME);
	app.setApplicationVersion(
		QString("%1.%2.%3")
		.arg(MAJOR_VERSION)
		.arg(MINOR_VERSION)
		.arg(BUILD_NUMBER));
	app.setWindowIcon(QIcon(":/icon.jpg"));
	
	// Command line options
	QCommandLineOption configOption({ "config", "Config file path", "config.json", app.applicationDirPath() + "/Config/chaopy.json" });
	QCommandLineOption targetOption({ "target", "Target directory", "your/path/xx" });

	// Command line parse
	QCommandLineParser parser;
	parser.setApplicationDescription(APPLICATION_DESCRIPTION);
	parser.addHelpOption();
	parser.addVersionOption();
	parser.addOptions({configOption, targetOption});

	parser.process(app);

	std::string configPath = parser.value(configOption).toUtf8().constData();
	spdlog::info("Config path we use: {}", configPath.c_str());

	Q_ASSERT(!configPath.empty());

	// Init setting
	if (0 != ChaopySetting::GetInstance()->Init(configPath))
	{
		spdlog::info("Setting initialize failed.");
		return 1;
	}
	ChaopySetting::GetInstance()->LoadConfig(false);

	// Create monitor
	auto monitor = std::make_shared<ChaopyMonitor>();

	// Create observer
	auto appWObs = std::make_shared<AppWindow>();
	
	auto chaoCopy = std::make_shared<ChaopyCopy>();

	// Attach observer
	monitor->AttachObs(appWObs);

	monitor->AttachObs(chaoCopy);

	// Run
	int ret = app.exec();

	return ret;
}