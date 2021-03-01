#include <irrlicht.h>
#include <iostream>
#include <cmath>

using namespace std;
using namespace irr;

struct BalanceWheelParams
{
	const f32 gravity = -9.8;
	f32 original_len = 40.f;
	f32 mass = 1.f;
	f32 elasticity = 100.f;
	f32 friction_coef = 0.1f;
	f32 string_friction_coef = 1.0f;
	const f32 original_start_ang = 60.f;  // in degrees
	const core::vector3df pos = core::vector3df(0.f, 0.f, 0.f);
	const core::vector3df scale = core::vector3df(0.5, 1.f, 1.f);
	const f32 radius = 5.f;
	const f32 original_vel = 0.f;
	const f32 push_force = 500.0f;
};

struct CameraParams
{
	f32 offset = 3.0f;
	f32 rot_speed = 30.0f;
};

enum ObjectID
{
	OBJECT_ID_BALANCE_WHEEL = 0,
	OBJECT_ID_PLATFORM,
	OBJECT_ID_LIGHT,
	OBJECT_ID_CAMERA
};

gui::IGUIWindow *exit_wnd = nullptr;

class AppEventReceiver : public IEventReceiver
{
public:
	AppEventReceiver() : isEscPressed(false), isMiddleMousePressed(false), wheel(0.0f), mousePosDelta(0.0f, 0.0f, 0.0f), lastMousePos(0.0f, 0.0f, 0.0f),
						isUpPressed(false), isDownPressed(false), isLeftPressed(false), isRightPressed(false)
	{
	}
	
	virtual bool OnEvent(const SEvent& event)
	{
		switch (event.EventType)
		{
			case EET_KEY_INPUT_EVENT:
				if (event.KeyInput.Key == KEY_ESCAPE)
				{
					isEscPressed = true;
					break;
				}
				
				if (exit_wnd != nullptr)
					return false;
				
				switch (event.KeyInput.Key)
				{
					case KEY_UP:
						isUpPressed = true;
						break;
					case KEY_DOWN:
						isDownPressed = true;
						break;
					case KEY_LEFT:
						isLeftPressed = true;
						break;
					case KEY_RIGHT:
						isRightPressed = true;
						break;
					default:
						break;
				}
				break;
			case EET_MOUSE_INPUT_EVENT:
				if (exit_wnd != nullptr)
					return false;
				
				switch (event.MouseInput.Event)
				{
					case EMIE_MOUSE_WHEEL:
						wheel = event.MouseInput.Wheel;
						break;
					case EMIE_MMOUSE_PRESSED_DOWN:
						isMiddleMousePressed = true;
						lastMousePos = core::vector3df((f32)event.MouseInput.X, (f32)event.MouseInput.Y, 0.0f);
						mousePosDelta = core::vector3df(0.0f, 0.0f, 0.0f);
						break;
					case EMIE_MMOUSE_LEFT_UP:
						isMiddleMousePressed = false;
						break;
					case EMIE_MOUSE_MOVED:
						mousePosDelta = core::vector3df((f32)event.MouseInput.X, (f32)event.MouseInput.Y, 0.0f) - lastMousePos;
						lastMousePos = core::vector3df((f32)event.MouseInput.X, (f32)event.MouseInput.Y, 0.0f);
						break;
					default:
						break;
				}
				break;
			default:
				break;
						
			
		}
		
		return false;
	}
	
	bool isEscPressed;
	bool isMiddleMousePressed;
	bool isUpPressed;
	bool isDownPressed;
	bool isLeftPressed;
	bool isRightPressed;
	f32 wheel;
	core::vector3df mousePosDelta;
	core::vector3df lastMousePos;
};
/*class AppEventReceiver : public IEventReceiver
{
public:
	AppEventReceiver() : isEscPressed(false), isMouseWheelChanged(false), wheel(0.f), pressed_btn(nullptr)
	{	
	}
	
	virtual bool OnEvent(const SEvent& event) override
	{
		std::cout << (event.EventType == EET_GUI_EVENT) << std::endl;
		if (event.EventType == EET_KEY_INPUT_EVENT &&
			event.KeyInput.Key == KEY_ESCAPE &&
			event.KeyInput.PressedDown
		)
			isEscPressed = true;
		else if (event.EventType == EET_MOUSE_INPUT_EVENT)
			if (event.MouseInput.Event == EMIE_MOUSE_WHEEL)
			{
				isMouseWheelChanged = true;
				wheel = event.MouseInput.Wheel;
			}
		else if (event.EventType == EET_GUI_EVENT)
		{
			std::cout << "GUI Event Type: " << event.GUIEvent.EventType << std::endl;
			if (event.GUIEvent.EventType == gui::EGET_BUTTON_CLICKED)
				std::cout << "GUI Event Type: " << event.GUIEvent.EventType << std::endl;
				pressed_btn = event.GUIEvent.Element;
				if (pressed_btn == nullptr)
					std::cout << "pressed_btn is nullptr" << std::endl;
		}
			
		return false;
	}
	
	bool isEscPressed;
	bool isMouseWheelChanged;
	f32 wheel;
	gui::IGUIElement* pressed_btn;
};*/

void createExitDialogueWindow(gui::IGUIEnvironment* env)
{
	if (env == nullptr)
		return;
	
	gui::IGUISkin* skin = env->getSkin();
	skin->setColor(gui::EGDC_3D_DARK_SHADOW, video::SColor(255, 255, 255, 255));
	core::recti exit_w_rect(core::position2di(256, 192), core::dimension2di(512, 384));
	exit_wnd = env->addWindow(exit_w_rect, true, 0, 0, 1);
	
	if (exit_wnd == nullptr)
	{
		std::cout << "The exit window fails to be created!" << std::endl;
		return;
	}
	
	gui::IGUIButton* cbut = env->addButton(core::recti(
		core::position2di(512/2-128, 384/2-384/4-64), core::dimension2di(2*128, 64)
	), exit_wnd, 2, L"Continue");
	
	if (cbut == nullptr)
	{
		std::cout << "The continue button fails to be created!" << std::endl;
		return;
	}
	
	cbut->updateAbsolutePosition();
	
	gui::IGUIButton* exit_but = env->addButton(core::recti(
		core::position2di(512/2-128, 384/2+384/4-64), core::dimension2di(2*128, 64)
	), exit_wnd, 3, L"Exit");
	
	if (exit_but == nullptr)
	{
		std::cout << "The exit button fails to be created!" << std::endl;
		return;
	}
	
	exit_but->updateAbsolutePosition();
}

int main()
{
	IrrlichtDevice* device = createDevice(video::EDT_OPENGL, core::dimension2du(1024, 768), 32, false);
	
	if (device == nullptr)
	{
		std::cout << "The device fails to be created!" << std::endl;
		return 1;
	}
	
	scene::ISceneManager* smgr = device->getSceneManager();
	video::IVideoDriver* vdrv = device->getVideoDriver();
	gui::IGUIEnvironment* env = device->getGUIEnvironment();
	io::IFileSystem* fsys = device->getFileSystem();
	
	AppEventReceiver receiver;
	device->setEventReceiver(&receiver);
	
	BalanceWheelParams bw_params;
	CameraParams c_params;
	
	core::vector3df end(
		bw_params.original_len*std::sin(core::degToRad(bw_params.original_start_ang)),
		-bw_params.original_len*std::cos(core::degToRad(bw_params.original_start_ang)),
		0.f
	);
	
	core::line3df beam(bw_params.pos, bw_params.pos + end);
	
	scene::IMeshSceneNode* bal_wheel = smgr->addSphereSceneNode(bw_params.radius, 32, 0, OBJECT_ID_BALANCE_WHEEL);
	
	if (bal_wheel == nullptr)
	{
		std::cout << "The balance wheel fails to be created!" << std::endl;
		return 1;
	}
	
	bal_wheel->setPosition(beam.end);
	bal_wheel->setScale(bw_params.scale);
	bal_wheel->setRotation(core::vector3df(0.f, 0.f, bw_params.original_start_ang));
	
	video::SMaterial material;
	material.MaterialType = video::EMT_LIGHTMAP_LIGHTING;
	material.AntiAliasing = true;
	material.Lighting = true;
	
	bal_wheel->getMaterial(0) = material;
	bal_wheel->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, true);
	bal_wheel->setMaterialTexture(0, vdrv->getTexture(fsys->getAbsolutePath("./textures/wood5.jpeg")));
	
	bal_wheel->addShadowVolumeSceneNode();
	smgr->setShadowColor(video::SColor(150,0,0,0));
	
	scene::IMeshSceneNode* platform = smgr->addMeshSceneNode(
		smgr->getMesh(fsys->getAbsolutePath("./models/stone_square.b3d")),
		0, OBJECT_ID_PLATFORM
	);
	
	if (platform == nullptr)
	{
		std::cout << "The platform fails to be created!" << std::endl;
		return 1;
	}
	
	platform->setPosition(core::vector3df(bw_params.pos.X, bw_params.pos.Y - bw_params.original_len - bw_params.radius - 3.f, 0.f));
	platform->getMaterial(0) = material;
	platform->setMaterialTexture(0, vdrv->getTexture(fsys->getAbsolutePath("./textures/stone2.jpg")));
	platform->addShadowVolumeSceneNode();
	
	scene::ILightSceneNode* light = smgr->addLightSceneNode(0, core::vector3df(bw_params.pos.X + 10.f, bw_params.pos.Y, 0.f), video::SColorf(255.f, 255.f, 255.f, 255.f), 50.f, OBJECT_ID_LIGHT);
	scene::ICameraSceneNode* camera = smgr->addCameraSceneNode(0, core::vector3df(0.0f, -bw_params.original_len/2, bw_params.original_len), core::vector3df(0.0f, -bw_params.original_len, 0.0f), OBJECT_ID_CAMERA, true);
	camera->bindTargetAndRotation(true);
	
	if (light == nullptr)
	{
		std::cout << "The light fails to be created!" << std::endl;
		return 1;
	}
	
	smgr->addTextSceneNode(env->getBuiltInFont(), L"X", video::SColor(255, 255, 255, 255), 0, core::vector3df(55.0f, 0.0f, 0.0f), 5);
	smgr->addTextSceneNode(env->getBuiltInFont(), L"Y", video::SColor(255, 255, 255, 255), 0, core::vector3df(0.0f, 55.0f, 0.0f), 6);
	smgr->addTextSceneNode(env->getBuiltInFont(), L"Z", video::SColor(255, 255, 255, 255), 0, core::vector3df(0.0f, 0.0f, 55.0f), 7);
	
	core::vector3df cur_vel(bw_params.original_vel);
	core::vector3df cur_rel_pos(end);
	core::vector3df cur_push_force(0.0f, 0.0f, 0.0f);
	while(device->run())
	{
		if (device->isWindowActive())
		{
			
			cur_push_force = core::vector3df(0.0f, 0.0f, 0.0f);
			if (receiver.isEscPressed)
			{
				device->closeDevice();
				/*createExitDialogueWindow(device->getGUIEnvironment());
				receiver.isEscPressed = false;*/
			}
			else if (receiver.wheel != 0.0f)
			{
				core::vector3df cur_target = camera->getTarget();
				const core::vector3df& cur_pos = camera->getPosition();
				core::vector3df rel_target = cur_target - cur_pos;
				
				s16 sign = receiver.wheel/std::fabs(receiver.wheel);
				
				if (sign > 0)
					std::cout << "Camera has offset forward" << std::endl;
				else if (sign < 0)
					std::cout << "Camera has offset backward" << std::endl;
				
				core::vector3df cam_offset = rel_target.normalize()*(f32)sign*c_params.offset;
				
				core::vector3df new_pos = cur_pos + cam_offset;
				camera->setPosition(new_pos);
				
				receiver.wheel = 0.0f;
			}
			else if (receiver.isMiddleMousePressed && receiver.mousePosDelta != core::vector3df(0.0f, 0.0f, 0.0f))
			{
				const core::vector3df& cur_pos = camera->getPosition();
				core::vector3df cur_target = camera->getTarget();
				core::vector3df new_rot = (cur_pos - cur_target).getHorizontalAngle() + core::vector3df(-receiver.mousePosDelta.Y/10.0f, -receiver.mousePosDelta.X/10.0f, 0.0f);
				
				core::matrix4 matrix;
				matrix.setRotationDegrees(new_rot);
				
				core::vector3df original_camera_pos(0.0f, 0.0f, (cur_target - cur_pos).getLength());
				matrix.rotateVect(original_camera_pos);
				
				camera->setPosition(cur_target + original_camera_pos);
				receiver.mousePosDelta = core::vector3df(0.0f, 0.0f, 0.0f);
			}
			else if (receiver.isUpPressed)
			{
				core::vector3df nmlzed_target = -camera->getPosition();
				nmlzed_target.normalize();
				
				cur_push_force = nmlzed_target * bw_params.push_force;
				receiver.isUpPressed = false;
			}
			else if (receiver.isDownPressed)
			{
				core::vector3df nmlzed_target = -camera->getPosition();
				nmlzed_target.normalize();
				
				cur_push_force = nmlzed_target * -bw_params.push_force;
				receiver.isDownPressed = false;
			}
			else if (receiver.isLeftPressed)
			{
				core::vector3df nmlzed_target = -camera->getPosition();
				nmlzed_target.normalize();
				nmlzed_target.rotateXZBy(90.0f);
				
				cur_push_force = nmlzed_target * bw_params.push_force;
				receiver.isLeftPressed = false;
			}
			else if (receiver.isRightPressed)
			{
				core::vector3df nmlzed_target = -camera->getPosition();
				nmlzed_target.normalize();
				nmlzed_target.rotateXZBy(-90.0f);
				
				cur_push_force = nmlzed_target * bw_params.push_force;
				receiver.isRightPressed = false;
			}
			/*else if (receiver.isMouseWheelChanged)
			{
				core::vector3df cur_target = camera->getTarget();
				core::vector3df cur_pos = camera->getPosition();
				core::vector3df rel_target = cur_target - cur_pos;
				
				s16 sign = receiver.wheel/std::fabs(receiver.wheel);
				
				if (sign > 0)
					std::cout << "Camera has offset forward" << std::endl;
				else if (sign < 0)
					std::cout << "Camera has offset backward" << std::endl;
				
				core::vector3df cam_offset = rel_target.normalize()*(f32)sign*cam_offset_dist;
				
				core::vector3df new_pos = cur_pos + cam_offset;
				camera->setPosition(new_pos);
				
				receiver.isMouseWheelChanged = false;
				receiver.wheel = 0.f;
			}*/
			/*else if (receiver.pressed_btn != nullptr)
			{
				std::cout << "The button is clicked!" << std::endl;
				s32 id = receiver.pressed_btn->getID();
				
				if (id == 2)
				{
					gui::IGUIElement* exit_wnd = receiver.pressed_btn->getParent();
					exit_wnd->drop();
				}
				else if (id == 3)
					device->closeDevice();
				
				receiver.pressed_btn = nullptr;
			}*/
							
			core::vector3df elastic_force = (cur_rel_pos.getLength() > bw_params.original_len)? cur_rel_pos/(-cur_rel_pos.getLength()) * bw_params.elasticity * (cur_rel_pos.getLength() - bw_params.original_len) :
																								core::vector3df(0.0f, 0.0f, 0.0f);
			core::vector3df friction_force = -cur_vel*bw_params.friction_coef;
			f32 pos_len_sqr = cur_rel_pos.dotProduct(cur_rel_pos);
			core::vector3df string_friction_force;
			if (std::fabs(pos_len_sqr) > 0.01)
				string_friction_force = -(cur_vel.dotProduct(cur_rel_pos))/(pos_len_sqr)*cur_rel_pos/std::sqrt(std::fabs(pos_len_sqr))*bw_params.string_friction_coef;
			core::vector3df res_force = core::vector3df(0.0f, bw_params.gravity*bw_params.mass, 0.0f) + elastic_force + friction_force + string_friction_force + cur_push_force;
			
			cur_rel_pos += cur_vel*0.001;
			
			cur_vel += res_force*0.001/bw_params.mass;
			
			
			bal_wheel->setPosition(bw_params.pos + cur_rel_pos);
			
			vdrv->beginScene(true, true, video::SColor(0, 0, 0, 255));
			vdrv->setTransform(video::ETS_WORLD, core::IdentityMatrix);
			vdrv->draw3DLine(core::vector3df(-50.0f, 0.0f, 0.0f), core::vector3df(50.0f, 0.0f, 0.0f), video::SColor(252, 0, 20, 255));
			vdrv->draw3DLine(core::vector3df(0.0f, -50.0f, 0.0f), core::vector3df(0.0f, 50.0f, 0.0f), video::SColor(10, 0, 234, 255));
			vdrv->draw3DLine(core::vector3df(0.0f, 0.0f, -50.0f), core::vector3df(0.0f, 0.0f, 50.0f), video::SColor(0, 217, 0, 255));
			vdrv->draw3DLine(beam.start, bw_params.pos + cur_rel_pos, video::SColor(255, 255, 255, 255));
			smgr->drawAll();
			env->drawAll();
			vdrv->endScene();
			
			core::stringw caption = L"[Irrlicht Engine] Balance Wheel Demo [";
			caption += vdrv->getName();
			caption += "; FPS: ";
			caption += vdrv->getFPS();
			caption += "]";
			
			device->setWindowCaption(caption.c_str());
		}
	}
	
	device->drop();
	
	return 0;
}
