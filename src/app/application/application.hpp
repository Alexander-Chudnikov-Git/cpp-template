#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include "notification_manager.hpp"
#include "option_manager.hpp"
#include "settings_manager.hpp"

namespace APP
{
class Application
{
public:
	Application(const int argc, const char** argv);
	~Application();

	Application(const Application&)			   = delete;
	Application& operator=(const Application&) = delete;

	int run();

private:
	bool initialize_managers(const int argc, const char** argv);
	bool initialize_app();

	void cleanup();

private:
	std::shared_ptr<UTILS::NotificationManager> m_notification_manager;
	std::shared_ptr<UTILS::SettingsManager>		m_settings_manager;
	std::shared_ptr<UTILS::OptionManager>		m_option_manager;
};
} // namespace APP

#endif // APPLICATION_HPP
