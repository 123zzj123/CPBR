#include <QTime>
#include <QTimer>
#include <QCursor>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QOpenGLShader>	// Ϊ��ʹ�� wind32 �� ClipCursor
#include "cglmanager.h"
#include "inputmanager.h"

// mouse
QTimer* InputManager::timer_mouse_pressed_over;

// key
QTimer* InputManager::timer_key_sgclick;
int InputManager::key_sgclick_bt = 0;

// binders

QMap<QString, QVector<InputState> > InputManager::axis_maps;
QMap<QString, QVector<InputState> > InputManager::action_maps;

QMap<QString, DELEGATE_ICLASS(InputAxis)*> InputManager::input_axis;
QMap<QString, DELEGATE_ICLASS(InputAction)*> InputManager::input_actions;

InputData InputManager::cur_input_data;
InputState InputManager::cur_input_state;

CGLManager* InputManager::gl;

// mouse
void InputManager::exec_mouse_pressed_event(QMouseEvent* event, CGLManager* gl) {
	// update cur_input_state
	cur_input_state.mouse_pressing = event->buttons();

	if (event->buttons() != (Qt::LeftButton | Qt::RightButton)) {
		// cur_input_state
		cur_input_state.mouse_pressed = event->button();
		// cur_input_data
		cur_input_data.mouse_pressed_pos = event->pos();
		cur_input_data.mouse_pressed_count = (cur_input_data.mouse_pressed_count + 1) % 3;
		timer_mouse_pressed_over->stop();
		timer_mouse_pressed_over->start(300);

		cur_input_data.mouse_last_pos = event->pos();
		cur_input_data.mouse_cur_pos = event->pos();
	} // ֻ��һ��������
	
	exec_axis_mouse_move();
}
void InputManager::exec_mouse_release_event(QMouseEvent* event, CGLManager* gl) {

	cur_input_state.mouse_released = event->button();
	cur_input_state.mouse_pressing ^= event->button();

	exec_axis_mouse_move();
}
void InputManager::exec_mouse_moveeee_event(QMouseEvent* event, CGLManager* gl) {
	if (cur_input_data.mouse_move_ignore_count > 0) {
		--cur_input_data.mouse_move_ignore_count;
		return;
	} // ignore move event called by move cursor

	cur_input_data.mouse_move_delta_x = 180.0f * (event->x() - cur_input_data.mouse_last_pos.x()) / gl->width();
	cur_input_data.mouse_move_delta_y = 180.0f * (event->y() - cur_input_data.mouse_last_pos.y()) / gl->width();
	cur_input_data.mouse_last_pos = event->pos();
	cur_input_data.mouse_cur_pos = event->pos();
	
	exec_axis_mouse_move();
}
void InputManager::exec_mouse_wheeeel_event(QWheelEvent* event, CGLManager* gl) {
	cur_input_data.mouse_wheel_delta = event->delta();
	exec_axis_mouse_wheel();
}
void InputManager::mouse_pressed_over() {
	cur_input_data.mouse_pressed_count = 0;
	timer_mouse_pressed_over->stop();
}

// key
void InputManager::exec_key_pressed_event(QKeyEvent* event, CGLManager* gl) {
	qDebug() <<event->key() <<  "hehe" << endl;
	qDebug() << QTime::currentTime() << endl;
	//cur_input_state.key_longgg_click.insert(event->key());
	//cur_input_data.key_longclick_speed = 1.0f;
}
void InputManager::exec_key_release_event(QKeyEvent* event, CGLManager* gl) {
	/*qDebug() << event->key() << "heihei" << endl;
	qDebug() << QTime::currentTime() << endl;
	
	if (!event->isAutoRepeat()) {						// ��һ�� release �����Ƿ񳤰�
		if (!cur_input_state.key_longgg_click.contains(event->key())) { // û�г�����ǲ�ִ�е���˫������
			if (!timer_key_sgclick->isActive()) {
				timer_key_sgclick->start(300);
				key_sgclick_bt = event->key();
			}
			++cur_input_data.key_click_count;
			if (cur_input_data.key_click_count == 2) {
				timer_key_sgclick->stop();
				cur_input_data.key_click_count = 0;
				exec_key_dbclick(event);
			}
		} // ���������һ��release�� auto_repeat Ҳ�� false -- ���� longgg_clickλ�Ѿ���Ϊ true

		cur_input_data.key_longclick_speed = 0.0f;
		cur_input_state.key_longgg_click.remove(event->key());

	} // û�г��� -- ִ�е���˫������
	else {
		if (!cur_input_state.key_longgg_click.contains(event->key())) {
			cur_input_state.key_longgg_click.insert(event->key());
			exec_key_longclick(event);
		}
	} // ����
	*/
}

void InputManager::exec_key_sgclick() {
	/*cur_input_data.key_click_count = 0;
	timer_key_sgclick->stop();
	cur_input_state.key_single_click.insert(key_sgclick_bt);
	qDebug() << key_sgclick_bt << "sg clicked" << endl;
	*/
}

void InputManager::exec_key_dbclick(QKeyEvent* event) {
	/*qDebug() << event->key() << "db clicked" << endl;
	cur_input_state.key_double_click.insert(event->key());
	*/
}
void InputManager::exec_key_longclick(QKeyEvent* event) {
	/*qDebug() << event->key() << "long clicked" << endl;
	cur_input_data.key_longclick_speed = 1.0f;
	*/
}

// binders
void InputManager::map_action(const QString& key, InputState is) {
	action_maps[key].append(is);
}
void InputManager::bind_action(const QString& key, DELEGATE_ICLASS(InputAction)* ia) {
	input_actions[key] = ia;
}
void InputManager::exec_action() {
	bool flag = false;
	for (auto it = input_actions.cbegin(); it != input_actions.cend(); ++it) {	// ��ÿһ���󶨵Ķ���
		if (!action_maps.count(it.key())) continue;								// map ��û����Ӧ�ļ�λ��
		for (auto itt = action_maps[it.key()].begin(); itt != action_maps[it.key()].end(); ++itt) {
			if (cur_input_state.contain((*itt))) {
				// ֱ��ִ��
				(*it)->invoke();
				flag = true;
			} // ����뵱ǰ�ļ�λ״̬��ͬ�� ��ִ��
		} // �������еļ�λ��
	}

	// ��Щ flag ��Ϊû���¼��󶨶�����ִ�к���� �˴��ֶ���� -- ���bug�е㷳
	cur_input_state.mouse_pressed = Qt::NoButton;
	cur_input_state.mouse_released = Qt::NoButton;
	//cur_input_state.mouse_sgclick.clear();
	//cur_input_state.mouse_dbclick.clear();
	/*cur_input_state.key_single_click.clear();
	cur_input_state.key_double_click.clear();
	if (!cur_input_state.key_longgg_click.isEmpty()) exec_axis_key_longclick();
	*/
}

void InputManager::map_axis(const QString& key, InputState is) {
	axis_maps[key].append(is);
}
void InputManager::bind_axis(const QString& key, DELEGATE_ICLASS(InputAxis)* ia) {
	input_axis[key] = ia;
}
void InputManager::exec_axis() {
	for (auto it = input_axis.cbegin(); it != input_axis.cend(); ++it) {	// ��ÿһ���󶨵Ķ���
		if (!axis_maps.count(it.key())) continue;							// map ��û����Ӧ�ļ�λ��
		for (auto itt = axis_maps[it.key()].begin(); itt != axis_maps[it.key()].end(); ++itt) {
			if (cur_input_state.contain((*itt))) {
				float offset = (*itt).axis_scale;
				// ������ʹ�õ� axis_types
				switch ((*itt).axis_types) {
				case InputAxisType::MOUSE_X: 
					offset *= cur_input_data.mouse_move_delta_x;
					offset *= cur_input_data.mouse_sensitivity;
					cur_input_data.mouse_move_delta_x = 0.0f;
					break;
				case InputAxisType::MOUSE_Y: 
					offset *= cur_input_data.mouse_move_delta_y;
					offset *= cur_input_data.mouse_sensitivity;
					cur_input_data.mouse_move_delta_y = 0.0f;
					break;
				case InputAxisType::WHEEL: 
					offset *= cur_input_data.mouse_wheel_delta;
					offset *= cur_input_data.mouse_sensitivity;
					cur_input_data.mouse_wheel_delta = 0.0f;
					break;
				case InputAxisType::KEY_LONG_CLICK:
					offset *= cur_input_data.key_longclick_speed;
					break;
				case InputAxisType::NONE: break;
				default:break;
				}
				(*it)->invoke(offset);
			} // ����뵱ǰ�ļ�λ״̬��ͬ�� ��ִ��
		} // �������еļ�λ��
	}
	
}
void InputManager::exec_axis_mouse_move() {
	for (auto it = input_axis.cbegin(); it != input_axis.cend(); ++it) {	// ��ÿһ���󶨵Ķ���
		if (!axis_maps.count(it.key())) continue;							// map ��û����Ӧ�ļ�λ��
		for (auto itt = axis_maps[it.key()].begin(); itt != axis_maps[it.key()].end(); ++itt) {
			if (cur_input_state.contain((*itt))) {
				float offset = (*itt).axis_scale;
				// ������ʹ�õ� axis_types
				bool flag = false;
				switch ((*itt).axis_types) {
				case InputAxisType::MOUSE_X: 
					offset *= cur_input_data.mouse_move_delta_x;
					offset *= cur_input_data.mouse_sensitivity;
					cur_input_data.mouse_move_delta_x = 0.0f;
					flag = true;
					break;
				case InputAxisType::MOUSE_Y: 
					offset *= cur_input_data.mouse_move_delta_y;
					offset *= cur_input_data.mouse_sensitivity;
					cur_input_data.mouse_move_delta_y = 0.0f;
					flag = true;
					break;
				default:break;
				}
				if(flag) (*it)->invoke(offset);
			} // ����뵱ǰ�ļ�λ״̬��ͬ�� ��ִ��
		} // �������еļ�λ��
	}
}
void InputManager::exec_axis_mouse_wheel() {
	for (auto it = input_axis.cbegin(); it != input_axis.cend(); ++it) {	// ��ÿһ���󶨵Ķ���
		if (!axis_maps.count(it.key())) continue;							// map ��û����Ӧ�ļ�λ��
		for (auto itt = axis_maps[it.key()].begin(); itt != axis_maps[it.key()].end(); ++itt) {
			if (cur_input_state.contain((*itt))) {
				float offset = (*itt).axis_scale;
				// ������ʹ�õ� axis_types
				bool flag = false;
				switch ((*itt).axis_types) {
				case InputAxisType::WHEEL: 
					offset *= cur_input_data.mouse_wheel_delta;
					offset *= cur_input_data.mouse_sensitivity;
					cur_input_data.mouse_wheel_delta = 0.0f;
					flag = true;
					break;
				default:break;
				}
				if(flag) (*it)->invoke(offset);
			} // ����뵱ǰ�ļ�λ״̬��ͬ�� ��ִ��
		} // �������еļ�λ��
	}
}
void InputManager::exec_axis_key_longclick() {
	for (auto it = input_axis.cbegin(); it != input_axis.cend(); ++it) {	// ��ÿһ���󶨵Ķ���
		if (!axis_maps.count(it.key())) continue;							// map ��û����Ӧ�ļ�λ��
		for (auto itt = axis_maps[it.key()].begin(); itt != axis_maps[it.key()].end(); ++itt) {
			if ((*itt) == cur_input_state) {
				float offset = (*itt).axis_scale;
				// ������ʹ�õ� axis_types
				switch ((*itt).axis_types) {
				case InputAxisType::KEY_LONG_CLICK: 
					offset *= cur_input_data.key_longclick_speed;
					break;
				default:break;
				}
				(*it)->invoke(offset);
			} // ����뵱ǰ�ļ�λ״̬��ͬ�� ��ִ��
		} // �������еļ�λ��
	}

}

void InputManager::cursor_clip() {
	QRect tmp = gl->rect();
	QPoint t_a = gl->mapToGlobal(QPoint(tmp.left(), tmp.top()));
	QPoint t_b = gl->mapToGlobal(QPoint(tmp.right(), tmp.bottom()));
	RECT rect;
	rect.left = t_a.x(); rect.top = t_a.y();
	rect.right = t_b.x(); rect.bottom = t_b.y();
	ClipCursor(&rect);
}
void InputManager::cursor_unclip() {
	ClipCursor(nullptr);
}
void InputManager::cursor_move(const QPoint& pos, int move_ignore_count) {
	if (gl == nullptr) return;
	QCursor::setPos(gl->mapToGlobal(pos));
	cur_input_data.mouse_last_pos = pos;
	cur_input_data.mouse_cur_pos = pos;
	cur_input_data.mouse_move_ignore_count = move_ignore_count;
}
void InputManager::cursor_show(const Qt::CursorShape& shape) {
	if (gl == nullptr) return;
	QCursor cursor(shape);
	gl->setCursor(cursor);
}
bool InputManager::cursor_in_edge() {
	if (cur_input_data.mouse_cur_pos.x() <= qMax(0.0f, cur_input_data.mouse_pressed_pos.x() - 4000.0f)) return true;
	if (cur_input_data.mouse_cur_pos.x() >= qMin(gl->width() - 2.0f, cur_input_data.mouse_pressed_pos.x() + 4000.0f)) return true;
	if (cur_input_data.mouse_cur_pos.y() <= qMax(0.0f, cur_input_data.mouse_pressed_pos.y() - 4000.0f)) return true;
	if (cur_input_data.mouse_cur_pos.y() >= qMin(gl->height() - 2.0f, cur_input_data.mouse_pressed_pos.y() + 4000.0f)) return true;
	return false;
}

void InputManager::init(CGLManager* cgl) {
	gl = cgl;

	timer_mouse_pressed_over = new QTimer(gl);
	gl->connect(timer_mouse_pressed_over, SIGNAL(timeout()), gl, SLOT(mouse_pressed_over()));

	//timer_mouse_sgclick = new QTimer(gl);
	//gl->connect(timer_mouse_sgclick, SIGNAL(timeout()), gl, SLOT(mouse_sgclick()));

	timer_key_sgclick = new QTimer(gl);
	gl->connect(timer_key_sgclick, SIGNAL(timeout()), gl, SLOT(key_sgclick()));

}

void InputManager::quit() {
	// timer_mouse_sgclick->stop();
	// delete timer_mouse_sgclick;

}

const InputData& InputManager::get_input_data() { return cur_input_data; }

// ======================================================================

InputState::InputState() {
	// mouse
	mouse_pressed = Qt::NoButton;
	mouse_released = Qt::NoButton;
	mouse_pressing = Qt::NoButton;

	axis_types = InputAxisType::NONE;
	axis_scale = 1.0f;

	// key
	//key_longgg_click.clear();

	modifier_single_click = Qt::NoModifier;
	modifier_double_click = Qt::NoModifier;
	modifier_longgg_click = Qt::NoModifier;
}

bool InputState::contain(const InputState& is) {
	// mouse
	if (mouse_pressed != (mouse_pressed | is.mouse_pressed)) return false;
	if (mouse_released != (mouse_released | is.mouse_released)) return false;
	if (mouse_pressing != is.mouse_pressing) return false;

	// key

	return true;
}
bool InputState::operator == (const InputState& is) const {
	// mouse
	if (mouse_pressed != is.mouse_pressed) return false;
	if (mouse_released != is.mouse_pressed) return false;
	if (mouse_pressing != is.mouse_pressing) return false;

	// key
	if (key_pressed != is.key_pressed) return false;
	if (key_pressing != is.key_pressing) return false;

	if (modifier_single_click != is.modifier_single_click) return false;
	if (modifier_double_click != is.modifier_double_click) return false;
	if (modifier_longgg_click != is.modifier_longgg_click) return false;
	
	return true;
}
bool InputState::operator < (const InputState& is) const { return !((*this) == is); }

InputData::InputData() {

	// mouse
	mouse_pressed_pos = QPoint(0.0f, 0.0f);
	mouse_last_pos = QPoint(0.0f, 0.0f);
	mouse_cur_pos = QPoint(0.0f, 0.0f);

	mouse_move_delta_x = 0.0f;
	mouse_move_delta_y = 0.0f;
	mouse_wheel_delta = 0.0f;
	mouse_sensitivity = 1.0f;

	mouse_pressed_count = 0;
	mouse_move_ignore_count = 0;

	// key
	key_click_count = 0;
	key_longclick_speed = 0.0f;

}
