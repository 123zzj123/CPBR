#pragma once

#include "cmath.h"
#include <string>
#include <vector>

enum SourceType {
	BY_FILE,
	BY_STRING
};

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
	static bool gen_mesh_txt(std::string& res, MeshTxtGenType type, uint depth = 32, SourceType source_type = SourceType::BY_FILE);

	~MeshTxtGen() {}

private:
	MeshTxtGen() {}

	// 2d
	static void gen_triangle_right();
	static void gen_triangle_regular();
	static void gen_rect();
	static void gen_circle(uint depth = 32);

	// 3d
	static void gen_cube();
	static void gen_cone(uint depth = 32);
	static void gen_sphere(uint depth = 5);
	static void gen_cylinder(uint depth = 32);

private:
	static void write(std::ostream& out);

	static void write_one_vertex(std::ostream& out, const MVertex& vertex);
	static void write_one_face(std::ostream& out, uint a, uint b, uint c);

	static void add_one_vertex(const MVertex& x);
	static void add_one_vertex(std::vector<MVertex>& v, const MVertex& x);
	static void add_one_face(uint a, uint b, uint c);
	static void add_one_face(const std::vector<MVertex>& v, std::vector<uint>& idx, uint a, uint b, uint c);

	static void cac_normal();

	static std::vector<MVertex> vertices;
	static std::vector<uint> indices;
};

// ===============================================================================================

class MeshLoader {

public:

	static bool load_mesh_txt(const std::string& src, std::vector<MVertex>& vertices, std::vector<uint>& indices, SourceType source_type = SourceType::BY_FILE);
	static bool load_mesh_obj(const std::string& src, std::vector<MVertex>& vertices, std::vector<uint>& indices, SourceType source_type = SourceType::BY_FILE);

	~MeshLoader(){}

private:
	MeshLoader(){}
};

