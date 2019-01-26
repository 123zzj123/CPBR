#include "cglmanager.h"
#include "gameobject.h"
#include "inputmanager.h"
#include "cameraobject.h"
#include "assetmanager.h"
#include "meshcomponent.h"
#include "cameracomponent.h"

#include "demo/freecamera.h"

CGLManager::CGLManager(QWidget *parent) : QOpenGLWidget(parent) {
	cube_object = nullptr;
	//setMouseTracking(true);
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
	core->glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
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
}

void CGLManager::wheelEvent(QWheelEvent* event) {
	InputManager::exec_mouse_wheel_event(event);
}
void CGLManager::mouseMoveEvent(QMouseEvent* event) {
	InputManager::exec_mouse_move_event(event);
}
void CGLManager::mousePressEvent(QMouseEvent* event) {
	InputManager::exec_mouse_press_event(event);
}
void CGLManager::mouseReleaseEvent(QMouseEvent* event) {
	InputManager::exec_mouse_release_event(event);

}
void CGLManager::mouseDoubleClickEvent(QMouseEvent* event) {
	InputManager::exec_mouse_dclick_event(event);
}
