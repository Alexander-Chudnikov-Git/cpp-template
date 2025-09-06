#include "main_program.hpp"
#include "notification_manager.hpp"
#include "option_manager.hpp"
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

	auto option_manager		  = UTILS::OptionManager::instance();
	auto settings_manager	  = UTILS::SettingsManager::instance();
	auto notification_manager = UTILS::NotificationManager::instance();

	option_manager->add_option("h,help", "Prints help menu.");
	option_manager->add_option("d,debug", "Prints debug info.");

	option_manager->parse_options(argc, argv);

	if (option_manager->has_option("h"))
	{
		option_manager->log_help();
	}

	if (option_manager->has_option("d"))
	{
		option_manager->debug_log();
	}

	auto main_program = APP::MainProgram();

	auto timestamp = std::chrono::system_clock::now();

	std::time_t		   now_tt = std::chrono::system_clock::to_time_t(timestamp);
	std::tm			   tm	  = *std::localtime(&now_tt);
	std::ostringstream oss;
	oss << std::put_time(&tm, "%c %Z");

	settings_manager->set_setting("application.last-launch", oss.str());
	settings_manager->set_setting("notification.last-launch", oss.str());
	settings_manager->set_setting("application.last-launch", oss.str());
	settings_manager->set_setting("application.last-launch", oss.str());
	settings_manager->save_settings();

	settings_manager->set_setting("notifications.uri", "https://ntfy.sh/chooisfox-test");

	UTILS::NotificationMessage notification;
	notification.title	 = "Application";
	notification.message = "The application has been started.";
	notification.tags	 = {"warning"};

	notification_manager->send_notification(notification);

	SPD_INFO_CLASS(COMMON::d_settings_group_application, "Application stopped");

	return 0;
}
