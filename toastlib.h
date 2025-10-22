// SPDX-License-Identifier: MIT

#ifndef TOAST_H
#define TOAST_H

#ifdef __cplusplus
extern "C" {
#endif

#define TOAST_SUCCESS 0
#define TOAST_ERROR_INVALID_PARAM -1
#define TOAST_ERROR_WINRT_INIT -2
#define TOAST_ERROR_TOAST_CREATION -3
#define TOAST_ERROR_NOTIFICATION -4

#define TOAST_DURATION_SHORT 0
#define TOAST_DURATION_LONG 1

#ifdef TOAST_EXPORTS
#define TOAST_API __declspec(dllexport)
#else
#define TOAST_API __declspec(dllimport)
#endif

/**
 * Initialize the toast notification system.
 * Must be called once before using any other toast functions.
 * 
 * @return TOAST_SUCCESS on success, error code on failure
 */
TOAST_API int toast_init(void);

/**
 * Cleanup the toast notification system.
 * Should be called once when done using toast notifications.
 */
TOAST_API void toast_cleanup(void);

/**
 * Show a simple toast notification with title and message.
 * 
 * @param title The title text for the toast
 * @param message The message text for the toast
 * @param app_id The application identifier (can be any string)
 * @return TOAST_SUCCESS on success, error code on failure
 */
TOAST_API int toast_show_simple(const char *title, const char *message, const char *app_id);

/**
 * Show a toast notification with custom duration.
 * 
 * @param title The title text for the toast
 * @param message The message text for the toast
 * @param app_id The application identifier (can be any string)
 * @param duration TOAST_DURATION_SHORT or TOAST_DURATION_LONG
 * @return TOAST_SUCCESS on success, error code on failure
 */
TOAST_API int toast_show_with_duration(const char *title, const char *message, const char *app_id, int duration);

/**
 * Show a toast notification with an image.
 * 
 * @param title The title text for the toast
 * @param message The message text for the toast
 * @param image_path Full path to the image file (supports common formats)
 * @param app_id The application identifier (can be any string)
 * @return TOAST_SUCCESS on success, error code on failure
 */
TOAST_API int toast_show_with_image(const char *title, const char *message, const char *image_path, const char *app_id);

/**
 * Get the last error message.
 * 
 * @return Pointer to null-terminated error message string
 */
TOAST_API const char *toast_get_last_error(void);

#ifdef __cplusplus
}
#endif

#endif
