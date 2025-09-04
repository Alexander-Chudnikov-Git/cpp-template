#include "main_program.hpp"
#include "option_parser.hpp"
#include "settings_manager.hpp"
#include "spdlog_wrapper.hpp"

#include <chrono>
#include <iomanip>
#include <sstream>

int main(const int argc, const char** argv)
{
#ifndef NDEBUG
	spdlog::set_level(spdlog::level::debug);
#endif

	SPD_INFO_CLASS(COMMON::d_settings_group_application, "Application started");

	auto option_parser	  = UTILS::OptionParser::instance();
	auto settings_manager = UTILS::SettingsManager::instance();

	option_parser->addOption("h,help", "Prints help menu.");
	option_parser->addOption("d,debug", "Prints debug info.");

	option_parser->parseOptions(argc, argv);

	if (option_parser->hasOption("h"))
	{
		option_parser->logHelp();
	}

	if (option_parser->hasOption("d"))
	{
		option_parser->debugLog();
	}

	auto main_program = APP::MainProgram();

	auto timestamp = std::chrono::system_clock::now();

	std::time_t		   now_tt = std::chrono::system_clock::to_time_t(timestamp);
	std::tm			   tm	  = *std::localtime(&now_tt);
	std::ostringstream oss;
	oss << std::put_time(&tm, "%c %Z");

	settings_manager->set_setting("application.last-launch", oss.str());
	settings_manager->save_settings();

	SPD_INFO_CLASS(COMMON::d_settings_group_application, "Application stopped");

	return 0;
}
