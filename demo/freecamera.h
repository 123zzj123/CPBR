#pragma once

#include "cameraobject.h"

class QKeyEvent;
class QMouseEvent;
class QWheelEvent;

class FreeCamera : public CameraObject {
public:
	FreeCamera();
	~FreeCamera();

	virtual void bind_input();

protected:

	void turn(float offset);
	void turn_over();							// �ƶ���������ʾ���
	void look_up(float offset);

	void move_forward(float offset);
	void move_forward_plane(float offset);
	void move_right(float offset);
	void move_up(float offset);

};
