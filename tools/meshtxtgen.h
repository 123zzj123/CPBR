#pragma once

#include "cmath.h"
#include <string>
#include <vector>

enum MeshTxtGenType {
	CIRCLE,						// Բ
	RECTANGLE,					// ����
	TRIANGLE_RIGHT,				// ֱ��
	TRIANGLE_REGULAR,			// �ȱ�

	CUBE,						// ������
	CONE,						// ׶��
	SPHERE,						// ����
	CYLINDER					// ����
};

struct MVertex {
	CVector3D position;
	CVector3D normal;
	CVector2D tex_coord;
	MVertex(CVector3D pos = CVector3D(), CVector3D norm = CVector3D(), CVector2D coord = CVector2D()) 
	: position(pos), normal(norm), tex_coord(coord) {}
};

class MeshTxtGen {
public:
	~MeshTxtGen() {}
	static bool gen_mesh_txt(const std::string& path, MeshTxtGenType type, uint depth = 5);

private:
	MeshTxtGen() {}

	// 2d
	static void gen_triangle_right();
	static void gen_triangle_regular();
	static void gen_rect();
	static void gen_circle(uint depth = 5);

	// 3d
	static void gen_cube();

private:
	static void write_to_file(std::ofstream& out);

	static void write_one_vertex(std::ofstream& out, const MVertex& vertex);
	static void write_one_face(std::ofstream& out, uint a, uint b, uint c);

	static void add_one_vertex(const MVertex& x);
	static void add_one_face(uint a, uint b, uint c);

	static void cac_normal();

	static std::vector<MVertex> vertices;
	static std::vector<uint> indices;
};

