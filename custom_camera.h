#include <irrlicht.h>

using namespace std;
using namespace irr;

class AppCameraSceneNode : public scene::ICameraSceneNode
{
public:
	AppCameraSceneNode(s32 id, scene::ISceneManager* smgr, scene::ISceneNode* parent = 0, 
		f32 rot_speed = 30.0f, f32 zoom_speed = 3.0f, const core::vector3df& target = core::vector3df(0.0f, 0.0f, 0.0f),
		const core::vector3df& pos = core::vector3df(0.0f, 0.0f, 0.0f), const core::vector3df& rot = core::vector3df(0.0f, 0.0f, 0.0f), const core::vector3df& scale = core::vector3df(1.0f, 1.0f, 1.0f)
	);
	
	virtual void setProjectionMatrix(const core::matrix4& projection, bool isOrthogonal=false) override;
	
	virtual const core::matrix4& getProjectionMatrix() const override;
	
	virtual const core::matrix4& getViewMatrix() const override;
	
	virtual void setViewMatrixAffector(const core::matrix4& affector) override;
	
	virtual const core::matrix4& getViewMatrixAffector() const override;
	
	virtual bool OnEvent(const SEvent& event) override;
	
	virtual void setRotation(const core::vector3df& rotation) override;
	
	virtual void setTarget(const core::vector3df& pos) override;
	
	virtual const core::vector3df& getTarget() const override;
	
	virtual void setUpVector(const core::vector3df& pos) override;
	
	virtual const core::vector3df& getUpVector() const override;
	
	virtual f32 getNearValue() const override;
	
	virtual f32 getFarValue() const override;
	
	virtual f32 getAspectRatio() const override;
	
	virtual f32 getFOV() const override;
	
	virtual void setNearValue(f32 zn) override;
	
	virtual void setFarValue(f32 zf) override;
	
	virtual void setAspectRatio(f32 aspect) override;
	
	virtual void setFOV(f32 fovy) override;
	
	virtual const scene::SViewFrustum* getViewFrustum() const override;
	
	virtual void setInputReceiverEnabled(bool enabled) override;
	
	virtual bool isInputReceiverEnabled() const override;
	
	virtual void bindTargetAndRotation(bool bound) override;
	
	virtual bool getTargetAndRotationBinding(void) const override;
	
	virtual core::aabbox3df& getBoundingBox() const override;
	
	virtual void render() override;
	
	virtual void OnRegisterSceneNode() override;
	
	void updateTarget();
	
	f32 getRotationSpeed() const;
	
	f32 getZoomSpeed() const;
	
	EMOUSE_INPUT_EVENT getMouseInputEvent() const;
	
	f32 getWheel() const;
	
	bool getMiddlePressedDown() const
	{
		return isMiddlePressedDown;
	}
	
	core::vector2df cursorPosDelta;
private:
	f32 rotSpeed;
	f32 zoomSpeed;
	
	EMOUSE_INPUT_EVENT mouseInputEvent;
	f32 wheel;
	bool isMiddlePressedDown;
	
	core::vector2df lastCursorPos;
	core::vector3df targetPos;
};
