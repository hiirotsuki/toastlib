// SPDX-License-Identifier: MIT

#include <windows.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Data.Xml.Dom.h>
#include <winrt/Windows.UI.Notifications.h>
#include <winrt/Windows.ApplicationModel.h>
#include <string>
#include <memory>

#ifndef __BEGIN_DECLS
#define __BEGIN_DECLS extern "C" {
#endif
#ifndef __END_DECLS
#define __END_DECLS }
#endif

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Data::Xml::Dom;
using namespace Windows::UI::Notifications;

namespace {
	std::wstring utf8_to_wide(const char *utf8_str)
	{
		if (!utf8_str)
			return L"";

		int len = MultiByteToWideChar(CP_UTF8, 0, utf8_str, -1, NULL, 0);
		if (len <= 1)
			return L"";
		
		std::wstring wide_str(len - 1, L'\0');
		MultiByteToWideChar(CP_UTF8, 0, utf8_str, -1, &wide_str[0], len);
		return wide_str;
	}

	std::wstring path_to_uri(const std::wstring& path)
	{
		if (path.find(L"://") != std::wstring::npos)
		{
			return path;
		}

		std::wstring uri = L"file:///";
		for (wchar_t c : path)
		{
			if (c == L'\\')
			{
				uri += L'/';
			}
			else
			{
				uri += c;
			}
		}
		return uri;
	}

	XmlDocument create_basic_toast_xml(const std::wstring& title, const std::wstring& message, ToastTemplateType template_type)
	{
		auto toastXml = ToastNotificationManager::GetTemplateContent(template_type);
		auto textElements = toastXml.GetElementsByTagName(L"text");

		if (textElements.Length() >= 1)
		{
			textElements.Item(0).AppendChild(toastXml.CreateTextNode(title.c_str()));
		}
		if (textElements.Length() >= 2)
		{
			textElements.Item(1).AppendChild(toastXml.CreateTextNode(message.c_str()));
		}

		return toastXml;
	}

	int show_toast_notification(XmlDocument& toastXml, const std::wstring& app_id)
	{
		try
		{
			auto toast = ToastNotification(toastXml);
			auto notifier = ToastNotificationManager::CreateToastNotifier(app_id.c_str());
			notifier.Show(toast);
			return 0;
		}
		catch (...)
		{
			return -4;
		}
	}
}

#define TOAST_SUCCESS 0
#define TOAST_ERROR_INVALID_PARAM -1
#define TOAST_ERROR_WINRT_INIT -2
#define TOAST_ERROR_TOAST_CREATION -3
#define TOAST_ERROR_NOTIFICATION -4

#define TOAST_DURATION_SHORT 0
#define TOAST_DURATION_LONG 1

static thread_local char last_error_msg[256] = {0};

extern "C" void WINAPI set_last_error(const char *msg)
{
	if (msg)
	{
		memset(last_error_msg, '\0', sizeof(last_error_msg));
		memcpy(last_error_msg, msg, strlen(msg) - 1);
	}
}

extern "C" __declspec(dllexport) int WINAPI toast_init()
{
	try
	{
		init_apartment();
		return TOAST_SUCCESS;
	}
	catch (...)
	{
		set_last_error("Failed to initialize WinRT apartment");
		return TOAST_ERROR_WINRT_INIT;
	}
}

extern "C" __declspec(dllexport) void WINAPI toast_cleanup()
{
	try
	{
		uninit_apartment();
	}
	catch (...) {}
}

extern "C" __declspec(dllexport) int WINAPI toast_show_simple(const char *title, const char *message, const char *app_id)
{
	if (!title || !message || !app_id)
	{
		set_last_error("Invalid parameter: title, message, and app_id cannot be null");
		return TOAST_ERROR_INVALID_PARAM;
	}

	try
	{
		auto wtitle = utf8_to_wide(title);
		auto wmessage = utf8_to_wide(message);
		auto wapp_id = utf8_to_wide(app_id);

		auto toastXml = create_basic_toast_xml(wtitle, wmessage, ToastTemplateType::ToastText02);

		return show_toast_notification(toastXml, wapp_id);
	}
	catch (...)
	{
		set_last_error("Failed to create or show simple toast notification");
		return TOAST_ERROR_TOAST_CREATION;
	}
}

extern "C" __declspec(dllexport) int WINAPI toast_show_with_duration(const char *title, const char *message, const char *app_id, int duration)
{
	if (!title || !message || !app_id)
	{
		set_last_error("Invalid parameter: title, message, and app_id cannot be null");
		return TOAST_ERROR_INVALID_PARAM;
	}

	try
	{
		auto wtitle = utf8_to_wide(title);
		auto wmessage = utf8_to_wide(message);
		auto wapp_id = utf8_to_wide(app_id);

		auto toastXml = create_basic_toast_xml(wtitle, wmessage, ToastTemplateType::ToastText02);

		auto toastElement = toastXml.DocumentElement();
		if (duration == TOAST_DURATION_LONG) {
			toastElement.SetAttribute(L"duration", L"long");
		} else {
			toastElement.SetAttribute(L"duration", L"short");
		}

		return show_toast_notification(toastXml, wapp_id);
	}
	catch (...)
	{
		set_last_error("Failed to create or show duration toast notification");
		return TOAST_ERROR_TOAST_CREATION;
	}
}

extern "C" __declspec(dllexport) int WINAPI toast_show_with_image(const char *title, const char *message, const char *image_path, const char *app_id, const char *alt_desc)
{
	if (!title || !message || !image_path || !app_id)
	{
		set_last_error("Invalid parameter: title, message, image_path and app_id cannot be null");
		return TOAST_ERROR_INVALID_PARAM;
	}

	try
	{
		auto wtitle = utf8_to_wide(title);
		auto wmessage = utf8_to_wide(message);
		auto wimage_path = utf8_to_wide(image_path);
		auto wapp_id = utf8_to_wide(app_id);

		auto image_uri = path_to_uri(wimage_path);

		auto toastXml = create_basic_toast_xml(wtitle, wmessage, ToastTemplateType::ToastImageAndText02);

		auto imageElements = toastXml.GetElementsByTagName(L"image");
		if (imageElements.Length() > 0)
		{
			auto imageElement = imageElements.Item(0).as<XmlElement>();
			imageElement.SetAttribute(L"src", image_uri.c_str());
			if(!alt_desc)
			{
				imageElement.SetAttribute(L"alt", L"Toast notification image");
			}
			else
			{
				auto walt_desc = utf8_to_wide(alt_desc);
				imageElement.SetAttribute(L"alt", walt_desc);
			}
		}

		return show_toast_notification(toastXml, wapp_id);
	}
	catch (...)
	{
		set_last_error("Failed to create or show image toast notification");
		return TOAST_ERROR_TOAST_CREATION;
	}
}

extern "C" __declspec(dllexport) const char * WINAPI toast_get_last_error()
{
	return last_error_msg;
}
