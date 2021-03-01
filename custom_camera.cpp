#include "custom_camera.h"

AppCameraSceneNode::AppCameraSceneNode(s32 id, scene::ISceneManager* smgr, scene::ISceneNode* parent, 
		f32 rot_speed, f32 zoom_speed, const core::vector3df& target,
		const core::vector3df& pos, const core::vector3df& rot, const core::vector3df& scale)
	: scene::ICameraSceneNode(parent, smgr, id, pos, rot, scale),
	  rotSpeed(rot_speed), zoomSpeed(zoom_speed), cursorPosDelta(0.0f, 0.0f), lastCursorPos(0.0f, 0.0f), mouseInputEvent(EMIE_COUNT), wheel(0.0f), isMiddlePressedDown(false)
	  {
		  setTarget(target);
	  } 

void AppCameraSceneNode::setProjectionMatrix(const core::matrix4& projection, bool isOrthogonal)
{
}

const core::matrix4& AppCameraSceneNode::getProjectionMatrix() const
{
	return core::matrix4();
}

const core::matrix4& AppCameraSceneNode::getViewMatrix() const
{
	return core::matrix4();
}

void AppCameraSceneNode::setViewMatrixAffector(const core::matrix4& affector)
{
}

const core::matrix4& AppCameraSceneNode::getViewMatrixAffector() const
{
	return core::matrix4();
}

bool AppCameraSceneNode::OnEvent(const SEvent& event)
{
	mouseInputEvent = EMIE_COUNT;
	wheel = 0.0f;
	isMiddlePressedDown = false;
	if (event.EventType == EET_MOUSE_INPUT_EVENT)
		switch (event.MouseInput.Event)
		{
			case EMIE_MOUSE_WHEEL:
				mouseInputEvent = EMIE_MOUSE_WHEEL;
				wheel = event.MouseInput.Wheel;
				break;
			case EMIE_MMOUSE_PRESSED_DOWN:
				mouseInputEvent = EMIE_MMOUSE_PRESSED_DOWN;
				lastCursorPos = core::vector2df((f32)event.MouseInput.X, (f32)event.MouseInput.Y);
				break;
			case EMIE_MOUSE_MOVED:
				mouseInputEvent = EMIE_MOUSE_MOVED;
				core::vector2df cur_cursor_pos = core::vector2df((f32)event.MouseInput.X, (f32)event.MouseInput.Y);
				cursorPosDelta = core::vector2df(cur_cursor_pos - lastCursorPos);
				lastCursorPos = cur_cursor_pos;
				isMiddlePressedDown = true;
				break;
		}
			
	return false;
}

void AppCameraSceneNode::setRotation(const core::vector3df& rotation)
{
	scene::ISceneNode::setRotation(rotation);
}

void AppCameraSceneNode::setTarget(const core::vector3df& pos)
{
	targetPos = pos;
	core::vector3df rel_target = pos - getPosition();
	core::vector3df angles = rel_target.getHorizontalAngle();
		
	setRotation(angles);
}

const core::vector3df& AppCameraSceneNode::getTarget() const
{
	return targetPos;
}

void AppCameraSceneNode::setUpVector(const core::vector3df& pos)
{
}

const core::vector3df& AppCameraSceneNode::getUpVector() const
{
	return core::vector3df(0);
}

f32 AppCameraSceneNode::getNearValue() const
{
	return 0;
}

f32 AppCameraSceneNode::getFarValue() const
{
	return 0;
}

f32 AppCameraSceneNode::getAspectRatio() const
{
	return 0;
}
 
f32 AppCameraSceneNode::getFOV() const
{
	return 0;
}

void AppCameraSceneNode::setNearValue(f32 zn)
{
}

void AppCameraSceneNode::setFarValue(f32 zf)
{
}

void AppCameraSceneNode::setAspectRatio(f32 aspect)
{
}

void AppCameraSceneNode::setFOV(f32 fovy)
{
}

const scene::SViewFrustum* AppCameraSceneNode::getViewFrustum() const
{
	return nullptr;
}

void AppCameraSceneNode::setInputReceiverEnabled(bool enabled)
{
}

bool AppCameraSceneNode::isInputReceiverEnabled() const
{
	return true;
}

void AppCameraSceneNode::bindTargetAndRotation(bool bound)
{
}

bool AppCameraSceneNode::getTargetAndRotationBinding(void) const
{
	return true;
}
	
core::aabbox3df& AppCameraSceneNode::getBoundingBox() const
{
	core::aabbox3df bounding_box;
	return bounding_box;
}

void AppCameraSceneNode::render()
{	
}

void AppCameraSceneNode::OnRegisterSceneNode()
{
	SceneManager->registerNodeForRendering(this, scene::ESNRP_CAMERA);
	
	scene::ISceneNode::OnRegisterSceneNode();
}

void AppCameraSceneNode::updateTarget()
{
	setTarget(getTarget());
}
	
f32 AppCameraSceneNode::getRotationSpeed() const
{
	return rotSpeed;
}
	
f32 AppCameraSceneNode::getZoomSpeed() const
{
	return zoomSpeed;
}
	
EMOUSE_INPUT_EVENT AppCameraSceneNode::getMouseInputEvent() const
{
	return mouseInputEvent;
}
	
f32 AppCameraSceneNode::getWheel() const
{
	return wheel;
}
