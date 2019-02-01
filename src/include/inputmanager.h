#pragma once

#include "delegate.h"
#include <QSet>
#include <QFlag>

class QTimer;
class CGLManager;
class QKeyEvent;
class QMosueEvent;

DELEGATE1(KeyPress, void, QKeyEvent*);
DELEGATE1(KeyRelease, void, QKeyEvent*);

DELEGATE0(InputAction, void);
DELEGATE1(InputAxis, void, float);

struct InputData;
struct InputState;
class InputThread;

class InputManager {

public:
	// mouse
	static void mouse_pressed_over();
	// mouse
	static void exec_mouse_pressed_event(QMouseEvent* event, CGLManager* gl);
	static void exec_mouse_release_event(QMouseEvent* event, CGLManager* gl);
	static void exec_mouse_moveeee_event(QMouseEvent* event, CGLManager* gl);
	static void exec_mouse_dbclick_event(QMouseEvent* event, CGLManager* gl);
	static void exec_mouse_wheeeel_event(QWheelEvent* event, CGLManager* gl);
	static void exec_mouse_sgclick();

	// key
	static void exec_key_pressed_event(QKeyEvent* event, CGLManager* gl);
	static void exec_key_release_event(QKeyEvent* event, CGLManager* gl);
	static void exec_key_sgclick();
	static void exec_key_dbclick(QKeyEvent* event);
	static void exec_key_longclick(QKeyEvent* event);

	// binders
	static void map_action(const QString& key, InputState is);
	static void bind_action(const QString& key, DELEGATE_ICLASS(InputAction)* ia);
	static void exec_action();

	static void map_axis(const QString& key, InputState is);
	static void bind_axis(const QString& key, DELEGATE_ICLASS(InputAxis)* ia);
	static void exec_axis();
	static void exec_axis_mouse_move();
	static void exec_axis_mouse_wheel();
	static void exec_axis_key_longclick();

	static void init(CGLManager* gl);
	static void quit();

	static const InputData& get_input_data();							// 当前鼠标的按下次数 -- 实现双击用

	static void cursor_clip();
	static void cursor_unclip();
	static void cursor_move(const QPoint& pos, int move_ignore_count = 1);
	static void cursor_show(const Qt::CursorShape& shape);
	static bool cursor_in_edge();

private:
	// mouse

	static QTimer* timer_mouse_pressed_over;
	//static QTimer* timer_mouse_sgclick;
	//static Qt::MouseButton mouse_sgclick_bt;

	// key
	static QTimer* timer_key_sgclick;
	static int key_sgclick_bt;

	// binders
	static QMap<QString, QVector<InputState> > action_maps;
	static QMap<QString, DELEGATE_ICLASS(InputAction)*> input_actions;

	static QMap<QString, QVector<InputState> > axis_maps;
	static QMap<QString, DELEGATE_ICLASS(InputAxis)*> input_axis;

	static InputData cur_input_data;
	static InputState cur_input_state;

	static CGLManager* gl;

	InputManager(){}

};

#define IM_BIND_ACTION(name, type, obj, func)					\
	DELEGATE_BIND(InputAction, type, type##name##, obj, func);	\
	InputManager::bind_action(#name, type##name##);

#define IM_BIND_AXIS(name, type, obj, func)						\
	DELEGATE_BIND(InputAxis, type, type##name##, obj, func);	\
	InputManager::bind_axis(#name, type##name##);

// ======================================================================

enum InputAxisType {
	NONE = 0x0001,
	MOUSE_X = 0x0002,
	MOUSE_Y = 0x0004,
	WHEEL = 0x0008,
	MOUSE = MOUSE_X | MOUSE_Y,

	KEY_LONG_CLICK = 0x0016

};
Q_DECLARE_FLAGS(InputAxisTypes, InputAxisType)
Q_DECLARE_OPERATORS_FOR_FLAGS(InputAxisTypes)

struct InputState {

	// mouse
	Qt::MouseButtons mouse_pressed;		// 按下事件
	Qt::MouseButtons mouse_released;	// 弹起事件
	Qt::MouseButtons mouse_pressing;	// 长按事件
	//QSet<Qt::MouseButton> mouse_dbpress;
	//QSet<Qt::MouseButton> mouse_sgclick;
	//QSet<Qt::MouseButton> mouse_dbclick;

	// axis can be used
	InputAxisTypes axis_types;
	float axis_scale;

	// key
	QSet<int> key_pressed;
	QSet<int> key_pressing;

	// modifier
	Qt::KeyboardModifier modifier_single_click;
	Qt::KeyboardModifier modifier_double_click;
	Qt::KeyboardModifier modifier_longgg_click;
	
	InputState();
	bool contain(const InputState& is); 
	bool operator == (const InputState& is) const; 
	bool operator < (const InputState& is) const;

};

struct InputData{
	// mouse
	int mouse_pressed_count;

	QPoint mouse_pressed_pos;
	QPoint mouse_last_pos;
	QPoint mouse_cur_pos;
	float mouse_move_delta_x;
	float mouse_move_delta_y;

	// mouse
	bool mouse_left_pressed;
	bool mouse_right_pressed;
	bool mouse_left_dbpressed;
	bool mouse_right_dbpressed;
	bool mouse_rigid;							// 判断 press 和 release 之间有没有移动 - sgclick 用


	float mouse_wheel_delta;
	float mouse_sensitivity;

	int mouse_move_ignore_count;

	// key
	int key_click_count;
	float key_longclick_speed;

	InputData();
};


