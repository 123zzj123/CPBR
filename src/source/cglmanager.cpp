#include "cglmanager.h"
#include "assetmanager.h"
#include "meshcomponent.h"
#include "gameobject.h"

CGLManager::CGLManager(QWidget *parent) : QOpenGLWidget(parent) {
	cube_object = nullptr;
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
	
	// shader ��̬������ֵ
	QMatrix4x4 projection, view;
	view.translate(QVector3D(0.0f, 0.0f, -3.0f));
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
	auto t_shader = AssetManager::get_shader("triangle").use();
	QMatrix4x4 model;
	model.rotate(45.0f, QVector3D(0.5f, 1.0f, 0.0f));
	t_shader.set_mat4("model", model); 

	// render
	AssetManager::get_shader("triangle").use();
	if (cube_object != nullptr) {
		cube_object->draw();
	}
}
