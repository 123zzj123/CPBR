#include "cglmanager.h"
#include "gameobject.h"
#include "inputmanager.h"
#include "cameraobject.h"
#include "assetmanager.h"
#include "meshcomponent.h"
#include "cameracomponent.h"
#include <QMouseEvent>
#include <QWheelEvent>

#include "demo/freecamera.h"

CGLManager::CGLManager(QWidget *parent) : QOpenGLWidget(parent) {
	cube_object = nullptr;

	// init mouse
	//setMouseTracking(true);
	this->setCursor(Qt::CrossCursor);
}

CGLManager::~CGLManager() {
}

void CGLManager::initializeGL() {
	// ��ʼ�� gl_core
	core = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();

	// ԭ�� -- ��ɫ��
	AssetManager::load_shader("triangle", ":/asset/shaders/single.vert", ":/asset/shaders/single.frag");

	// ԭ�� -- ��������
	AssetManager::load_mesh("cube", ":/asset/models/txt/cube.txt");
	// texture

	// ���� gameobject
	auto mc = new MeshComponent(&AssetManager::get_mesh("cube"));
	cube_object = new GameObject(mc);

	// gameobject ����һ����ʾ���
	auto mcc = new MeshComponent(&AssetManager::get_mesh("cube"));
	mcc->attach_to(mc);
	mcc->set_location(QVector3D(1.2f,0.0f, 0.0f));
	mcc->set_scale(QVector3D(0.5f, 0.5f, 0.5f));

	auto mccc = new MeshComponent(&AssetManager::get_mesh("cube"));
	mccc->attach_to(mcc);
	mccc->set_location(QVector3D(0.0f, 2.0f, 0.0f));
	mccc->set_scale(QVector3D(0.5f, 0.5f, 0.f));
	
	// ��ʼ�� camera
	auto camera = new CameraObject();
	auto free_camera = new FreeCamera();
	free_camera->bind_input();
	main_camera = free_camera->get_camera_component();
	free_camera->get_root()->set_location(QVector3D(0.0f, 1.5f, -3.0f));
	free_camera->get_root()->set_roataion(QVector3D(-20.0f, 0.0f, 0.0f));

	// shader ��̬������ֵ
	QMatrix4x4 projection, view;
	view.translate(QVector3D(0.0f, 0.0f, 3.0f));
	projection.perspective(45.0f, (GLfloat)width() / height(), 0.1f, 100.0f);
	auto& t_shader = AssetManager::get_shader("triangle").use();
	t_shader.set_mat4("view", view);
	t_shader.set_mat4("projection", projection);

	// gl ״̬��ʼ��
	core->glEnable(GL_DEPTH_TEST);
	//core->glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}
void CGLManager::resizeGL(int w, int h) {
	core->glViewport(0, 0, w, h);
}
void CGLManager::paintGL() {
	// gl �����ʼ��
	//core->glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	core->glClearColor(205.0f/255.0f, 220.0f/255.0f, 232.0f/255.0f, 1.0f);
	core->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// shader ��̬������ֵ
	//cube_object->get_root()->set_roataion(QVector3D(45.0f, 0.0f, 0.0f));
	auto t_shader = AssetManager::get_shader("triangle").use();
	t_shader.set_mat4("view", main_camera->get_view_mat());

	// render
	t_shader.use();
	if (cube_object != nullptr) {
		cube_object->draw(t_shader);
	}
	
	update(); // ���� paintGL����ѭ������
}

void CGLManager::wheelEvent(QWheelEvent* event) {
	InputManager::exec_mouse_wheel_event(event);
}
void CGLManager::mouseMoveEvent(QMouseEvent* event) {
	// event->x() Խ�� -- �������λ��, �Ҳ�����Խ��������¼�
	if (event->x() <= 0 || event->x() >= width()-2 || event->y() <= 0 || event->y() >= height()-2) {
		QPoint pos = this->mapToGlobal(QPoint(InputManager::mouse_pre_position.x(), InputManager::mouse_pre_position.y()));
		//QCursor cursor(Qt::BlankCursor);
		QCursor cursor(Qt::CrossCursor);
		cursor.setPos(pos);					
		this->setCursor(cursor);
		InputManager::mouse_last_position = InputManager::mouse_pre_position;
		return;
	}

	InputManager::exec_mouse_move_event(event);

	InputManager::mouse_last_position = QVector2D(event->x(), event->y());
	
}
void CGLManager::mousePressEvent(QMouseEvent* event) {
	// set mouse state flag
	if (event->button() == Qt::LeftButton) { InputManager::mouse_left_pressed = true; }
	else if (event->button() == Qt::RightButton) { InputManager::mouse_right_pressed = true; }

	// set cursor
	InputManager::mouse_pre_position = QVector2D(event->x(), event->y());
	InputManager::mouse_last_position = QVector2D(event->x(), event->y());
	//this->setCursor(Qt::BlankCursor);

	// clip cursor
	QRect rect = this->rect();
	QPoint t_a = this->mapToGlobal(QPoint(rect.left(), rect.top()));
	QPoint t_b = this->mapToGlobal(QPoint(rect.right(), rect.bottom()));
	InputManager::clip_cursor(t_a.x(), t_a.y(), t_b.x(), t_b.y());

	InputManager::exec_mouse_press_event(event);

}
void CGLManager::mouseReleaseEvent(QMouseEvent* event) {
	// set mouse state flag
	if (event->button() == Qt::LeftButton) { InputManager::mouse_left_pressed = false; }
	else if (event->button() == Qt::RightButton) { InputManager::mouse_right_pressed = false; }

	// judge if has moved
	QVector2D move_differ = QVector2D(event->x(), event->y()) - InputManager::mouse_pre_position;
	float differ = move_differ.length();
	if (differ <= 1e-3) { InputManager::mouse_moved = false; }
	else { InputManager::mouse_moved = true; }

	InputManager::exec_mouse_release_event(event);

	// reset cursor if no pressed
	if (!InputManager::mouse_left_pressed && !InputManager::mouse_right_pressed) {
		QPoint pos = this->mapToGlobal(QPoint(InputManager::mouse_pre_position.x(), InputManager::mouse_pre_position.y()));
		QCursor cursor(Qt::CrossCursor);
		cursor.setPos(pos);					// �ͷ�ʱ�ص�ԭ������ʱ��λ��
		this->setCursor(cursor);
		InputManager::unclip_cursor();
	}

}
void CGLManager::mouseDoubleClickEvent(QMouseEvent* event) {
	InputManager::exec_mouse_dclick_event(event);
}
