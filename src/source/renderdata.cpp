#include "renderdata.h"

IMPLEMENT_CLASS(RenderData)

RenderData::RenderData() : vao(0), vbo(0), ebo(0) {
	core = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();
}
RenderData::~RenderData() {
	//if (vao != 0) core->glDeleteVertexArrays(1, &vao);
	//if (vbo != 0) core->glDeleteBuffers(1, &vbo);
	//if (ebo != 0) core->glDeleteBuffers(1, &ebo);

}

void RenderData::init(const QVector<Vertex>& v, const QVector<GLuint>& i) {
	vertices = v;
	indices = i;

	// ��ʼ�� vbo
	core->glGenBuffers(1, &vbo);
	core->glBindBuffer(GL_ARRAY_BUFFER,vbo);
	core->glBufferData(GL_ARRAY_BUFFER, vertices.length() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
	core->glBindBuffer(GL_ARRAY_BUFFER, 0);

	// ��ʼ�� ebo
	if (indices.length() > 0) {
		core->glGenBuffers(1, &ebo);
		core->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		core->glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.length() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);
		core->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	// ��ʼ�� vao
	core->glGenVertexArrays(1, &vao);
	core->glBindVertexArray(vao);
	core->glBindBuffer(GL_ARRAY_BUFFER, vbo);
	if (indices.length() > 0) core->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	
	core->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	core->glEnableVertexAttribArray(0);
	core->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	core->glEnableVertexAttribArray(1);
	core->glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex_coord));
	core->glEnableVertexAttribArray(2);

	core->glBindVertexArray(0);
	core->glBindBuffer(GL_ARRAY_BUFFER, 0);
	if (indices.length() > 0) core->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void RenderData::draw() {
	core->glBindVertexArray(vao);
	if(indices.length() <= 0) core->glDrawArrays(GL_TRIANGLES, 0, vertices.length());
	else core->glDrawElements(GL_TRIANGLES, indices.length(), GL_UNSIGNED_INT, 0);
	core->glBindVertexArray(0);
}
