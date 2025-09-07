#include "application.hpp"

#include <chrono>
#include <iomanip>
#include <lmdb++.h>
#include <sstream>

namespace APP
{
Application::Application(const int argc, const char** argv)
{
	this->initialize_managers(argc, argv);
	this->initialize_app();

	SPD_INFO_CLASS(COMMON::d_settings_group_application, "Application initialized");
}

Application::~Application()
{
	this->cleanup();
}

int Application::run()
{
	try
	{
		auto env = lmdb::env::create();

		std::filesystem::create_directory("./data/");

		env.set_mapsize(1UL * 1024UL * 1024UL);
		env.open("./data/", 0, 0664);

		lmdb::dbi dbi;

		{
			auto wtxn = lmdb::txn::begin(env);
			dbi		  = lmdb::dbi::open(wtxn, nullptr);

			dbi.put(wtxn, "username", "jhacker");
			dbi.put(wtxn, "email", std::string("jhacker@example.org"));
			dbi.put(wtxn, "fullname", std::string_view("J. Random Hacker"));

			wtxn.commit();
		}

		{
			auto rtxn = lmdb::txn::begin(env, nullptr, MDB_RDONLY);

			std::string_view email_view;
			if (dbi.get(rtxn, "email", email_view))
			{
				SPD_INFO_CLASS(COMMON::d_settings_group_application, fmt::format("email: {}", email_view));
			}
			else
			{
				SPD_ERROR_CLASS(COMMON::d_settings_group_application, "email not found");
			}
		}

		{
			auto rtxn = lmdb::txn::begin(env, nullptr, MDB_RDONLY);

			{
				auto			 cursor = lmdb::cursor::open(rtxn, dbi);
				std::string_view key, value;

				if (cursor.get(key, value, MDB_FIRST))
				{
					do
					{
						SPD_INFO_CLASS(COMMON::d_settings_group_application, fmt::format("key: {} : {}", key, value));
					} while (cursor.get(key, value, MDB_NEXT));
				}
			}
		}
	}
	catch (const lmdb::error& e)
	{
		SPD_ERROR_CLASS(COMMON::d_settings_group_application, fmt::format("LMDB Error: {}", e.what()));
		return 1;
	}
	catch (const std::exception& e)
	{
		SPD_ERROR_CLASS(COMMON::d_settings_group_application, fmt::format("Standard Error: {}", e.what()));
		return 1;
	}

	return 0;
}

bool Application::initialize_managers(const int argc, const char** argv)
{
	this->m_option_manager		 = UTILS::OptionManager::instance();
	this->m_settings_manager	 = UTILS::SettingsManager::instance();
	this->m_notification_manager = UTILS::NotificationManager::instance();

	this->m_option_manager->add_option("h,help", "Prints help menu.");
	this->m_option_manager->add_option("d,debug", "Prints debug info.");

	this->m_option_manager->parse_options(argc, argv);

	if (this->m_option_manager->has_option("h"))
	{
		this->m_option_manager->log_help();
	}

	if (this->m_option_manager->has_option("d"))
	{
		this->m_option_manager->debug_log();
	}

	auto timestamp = std::chrono::system_clock::now();

	std::time_t		   now_tt = std::chrono::system_clock::to_time_t(timestamp);
	std::tm			   tm	  = *std::localtime(&now_tt);
	std::ostringstream oss;
	oss << std::put_time(&tm, "%c %Z");

	this->m_settings_manager->set_setting("application.last-launch", oss.str());
	this->m_settings_manager->save_settings();

	UTILS::NotificationMessage notification;
	notification.title	 = "Application";
	notification.message = "The application has been started.";
	notification.tags	 = {"warning"};

	this->m_notification_manager->send_notification(notification);

	return true;
}

bool Application::initialize_app()
{
	return true;
}

void Application::cleanup()
{
	this->m_notification_manager->shutdown();

	SPD_INFO_CLASS(COMMON::d_settings_group_application, "Application cleaned up");
}

} // namespace APP
