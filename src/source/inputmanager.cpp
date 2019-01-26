#include <QCursor>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QOpenGLShader>	// 为了使用 wind32 的 ClipCursor
#include "inputmanager.h"

QMap<QString, DELEGATE_ICLASS(MousePress)*> InputManager::mouse_press_handlers;
QMap<QString, DELEGATE_ICLASS(MouseRelease)*> InputManager::mouse_release_handlers;
QMap<QString, DELEGATE_ICLASS(MouseMove)*> InputManager::mouse_move_handlers;
QMap<QString, DELEGATE_ICLASS(MouseWheel)*> InputManager::mouse_wheel_handlers;
QMap<QString, DELEGATE_ICLASS(MouseDoubleClick)*> InputManager::mouse_dclick_handlers;

QVector2D InputManager::mouse_pre_position = QVector2D(0.0f, 0.0f);
QVector2D InputManager::mouse_last_position = QVector2D(0.0f, 0.0f);
float InputManager::mouse_sensitivity = 0.1f;

bool InputManager::mouse_left_pressed = false;
bool InputManager::mouse_right_pressed = false;
bool InputManager::mouse_moved = false;

void InputManager::bind_mouse_press_event(const QString& key, DELEGATE_ICLASS(MousePress)* mp) {
	mouse_press_handlers[key] = mp;
} 
void InputManager::ubnd_mouse_press_event(const QString& key) {
	mouse_press_handlers.remove(key);
}
void InputManager::exec_mouse_press_event(QMouseEvent* event) {
	for (auto it = mouse_press_handlers.cbegin(); it != mouse_press_handlers.cend(); ++it) {
		(*it)->invoke(event);
	}
}

void InputManager::bind_mouse_release_event(const QString& key, DELEGATE_ICLASS(MouseRelease)* mp) {
	mouse_release_handlers[key] = mp;
}
void InputManager::ubnd_mouse_release_event(const QString& key) {
	mouse_release_handlers.remove(key);
}
void InputManager::exec_mouse_release_event(QMouseEvent* event) {
	for (auto it = mouse_release_handlers.cbegin(); it != mouse_release_handlers.cend(); ++it) {
		(*it)->invoke(event);
	}
}

void InputManager::bind_mouse_move_event(const QString& key, DELEGATE_ICLASS(MouseMove)* mp) {
	mouse_move_handlers[key] = mp;
}
void InputManager::ubnd_mouse_move_event(const QString& key) {
	mouse_move_handlers.remove(key);
}
void InputManager::exec_mouse_move_event(QMouseEvent* event) {
	for (auto it = mouse_move_handlers.cbegin(); it != mouse_move_handlers.cend(); ++it) {
		(*it)->invoke(event);
	}
}

void InputManager::bind_mouse_wheel_event(const QString& key, DELEGATE_ICLASS(MouseWheel)* mp) {
	mouse_wheel_handlers[key] = mp;
}
void InputManager::ubnd_mouse_wheel_event(const QString& key) {
	mouse_wheel_handlers.remove(key);
}
void InputManager::exec_mouse_wheel_event(QWheelEvent* event) {
	for (auto it = mouse_wheel_handlers.cbegin(); it != mouse_wheel_handlers.cend(); ++it) {
		(*it)->invoke(event);
	}
}

void InputManager::bind_mouse_dclick_event(const QString& key, DELEGATE_ICLASS(MouseDoubleClick)* mp) {
	mouse_dclick_handlers[key] = mp;
}
void InputManager::ubnd_mouse_dclick_event(const QString& key) {
	mouse_dclick_handlers.remove(key);
}
void InputManager::exec_mouse_dclick_event(QMouseEvent* event) {
	for (auto it = mouse_dclick_handlers.cbegin(); it != mouse_dclick_handlers.cend(); ++it) {
		(*it)->invoke(event);
	}
}

void InputManager::clip_cursor(int left, int top, int right, int bottom) {
	RECT rect;
	rect.left = left; rect.top = top; 
	rect.right = right; rect.bottom = bottom;
	ClipCursor(&rect);
}
void InputManager::unclip_cursor() {
	ClipCursor(nullptr);
}

