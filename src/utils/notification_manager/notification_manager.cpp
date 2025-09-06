#include "notification_manager.hpp"

#include "network_manager.hpp"
#include "settings_manager.hpp"
#include "spdlog/fmt/bundled/format.h"

namespace UTILS
{
std::mutex NotificationManager::m_notification_mutex;

std::string_view NotificationManager::get_manager_name() const
{
	return "Notification Manager";
}

void NotificationManager::initialize()
{}

NotificationManager::~NotificationManager()
{}

bool NotificationManager::send_notification(std::string_view				topic,
											std::string_view				message,
											std::string_view				title,
											NotificationPriority			priority,
											const std::vector<std::string>& tags,
											bool							enable_markdown,
											std::string_view				schedule)
{
	NotificationMessage notification;
	notification.topic			 = topic;
	notification.message		 = message;
	notification.title			 = title;
	notification.priority		 = priority;
	notification.tags			 = tags;
	notification.enable_markdown = enable_markdown;
	notification.schedule		 = schedule;
	return this->send_notification(notification);
}

bool NotificationManager::send_notification(const NotificationMessage& notification)
{
	auto settings_manager = UTILS::SettingsManager::instance();
	if (settings_manager->get_setting<std::string>("notifications.enabled", "false").compare("false"))
	{
		SPD_DEBUG_CLASS(COMMON::d_settings_group_utils, "Unable to send notification, notifications are disabled.");
		return false;
	}

	auto notifications_uri = settings_manager->get_setting<std::string>("notifications.uri", "");
	if (notifications_uri.empty())
	{
		SPD_DEBUG_CLASS(COMMON::d_settings_group_utils, "Unable to send notification, notifications server is empty.");
		return false;
	}

	auto network_manager = UTILS::NetworkManager::instance();

	std::string tags;
	for (auto tag : notification.tags)
	{
		tags += tag + ",";
	}
	if (!tags.empty())
	{
		tags.pop_back();
	}

	std::string actions;
	for (auto action : notification.actions)
	{
		actions += action + ";";
	}
	if (!actions.empty())
	{
		actions.pop_back();
	}

	std::vector<std::pair<std::string, std::string>> headers;
	headers.push_back({"Title", notification.title});
	headers.push_back({"Priority", std::to_string(static_cast<int>(notification.priority))});
	headers.push_back({"Tags", tags});
	headers.push_back({"Markdown", (notification.enable_markdown ? "true" : "false")});
	headers.push_back({"Delay", notification.schedule});
	headers.push_back({"Click", notification.click_action});
	headers.push_back({"Attach", notification.attachment_url});
	headers.push_back({"Email", notification.email_recipient});
	headers.push_back({"Actions", actions});

	NetworkRequest request;
	request.method	= HttpMethod::POST;
	request.url		= notifications_uri;
	request.headers = headers;
	request.body	= notification.message;

	request.username = settings_manager->get_setting<std::string>("notifications.username", "");
	request.password = settings_manager->get_setting<std::string>("notifications.password", "");

	auto response = network_manager->make_request(request);
	if (!response.error.empty())
	{
		SPD_DEBUG_CLASS(COMMON::d_settings_group_utils,
						fmt::format("Unable to send notification. Response code {}: {}", response.http_code, response.error));
		return false;
	}

	return true;
}

} // namespace UTILS
