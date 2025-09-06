#ifndef NOTIFICATION_MANAGER_HPP
#define NOTIFICATION_MANAGER_HPP

#include "manager_singleton.hpp"

#include <string>

namespace UTILS
{
enum class NotificationPriority
{
	MIN		= 1,
	LOW		= 2,
	DEFAULT = 3,
	HIGH	= 4,
	MAX		= 5
};

struct NotificationMessage
{
	std::string				 topic;
	std::string				 message;
	std::string				 title;
	NotificationPriority	 priority		 = NotificationPriority::DEFAULT;
	std::vector<std::string> tags			 = {};
	bool					 enable_markdown = false;
	std::string				 schedule		 = "";
	std::string				 click_action	 = "";
	std::string				 attachment_url	 = "";
	std::string				 email_recipient = "";
	std::vector<std::string> actions		 = {}; // Rework with custom class
};

class NotificationManager : public UTILS::ManagerSingleton<NotificationManager>
{
	friend class ManagerSingleton<NotificationManager>;

private:
	NotificationManager() = default;

	void initialize() override;

public:
	std::string_view get_manager_name() const override;

	~NotificationManager();

	bool send_notification(std::string_view				   topic,
						   std::string_view				   message,
						   std::string_view				   title,
						   NotificationPriority			   priority		   = NotificationPriority::DEFAULT,
						   const std::vector<std::string>& tags			   = {},
						   bool							   enable_markdown = false,
						   std::string_view				   schedule		   = "");
	bool send_notification(const NotificationMessage& notification);

protected:
	static std::mutex m_notification_mutex;
};
} // namespace UTILS

#endif // NOTIFICATION_MANAGER_HPP
