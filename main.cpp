#include <irrlicht.h>
#include <iostream>
#include <cmath>

using namespace std;
using namespace irr;

namespace irr
{
	namespace core
	{
		typedef vector3df position3df;
	}
}

struct BalanceWheelParams
{
	const f32 gravity = -9.8;
	f32 original_len = 40.f;
	f32 mass = 1.f;
	f32 elasticity = 1000.f;
	f32 friction_coef = 0.00f;
	f32 string_friction_coef = 50.0f;
	const f32 original_start_ang = 30.f;  // in degrees
	const core::position3df pos = core::position3df(0.f, 0.f, 0.f);
	const core::vector3df scale = core::vector3df(0.5, 1.f, 1.f);
	const f32 radius = 5.f;
	const f32 original_vel = 0.f;
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

class AppEventReceiver : public IEventReceiver
{
public:
	AppEventReceiver() : isEscPressed(false)
	{
	}
	
	virtual bool OnEvent(const SEvent& event)
	{
		if (event.EventType == EET_KEY_INPUT_EVENT &&
			event.KeyInput.Key == KEY_ESCAPE &&
			event.KeyInput.PressedDown
		)
		{
			isEscPressed = true;
		}
		else if (event.EventType == EET_MOUSE_INPUT_EVENT)
			if (event.MouseInput.Event == EMIE_MOUSE_WHEEL)
			{
				isMouseWheelChanged = true;
				wheel = event.MouseInput.Wheel;
			}
		
		return false;
	}
	
	bool isEscPressed;
	bool isMouseWheelChanged;
	f32 wheel;
};

void createExitDialogueWindow(gui::IGUIEnvironment* env)
{
	if (env == nullptr)
		return;
	
	gui::IGUISkin* skin = env->getSkin();
	skin->setColor(gui::EGDC_3D_DARK_SHADOW, video::SColor(255, 255, 255, 255));
	core::recti exit_w_rect(core::position2di(256, 192), core::dimension2di(512, 384));
	gui::IGUIWindow* exit_w = env->addWindow(exit_w_rect, true, 0, 0, 1);
	
	if (exit_w == nullptr)
	{
		std::cout << "The exit window fails to be created!" << std::endl;
		return;
	}
	
	gui::IGUIButton* cbut = env->addButton(core::recti(
		core::position2di(512/2-128, 384/2-384/4-64), core::dimension2di(2*128, 64)
	), exit_w, 2, L"Continue");
	
	if (cbut == nullptr)
	{
		std::cout << "The continue button fails to be created!" << std::endl;
		return;
	}
	
	cbut->updateAbsolutePosition();
	
	gui::IGUIButton* exit_but = env->addButton(core::recti(
		core::position2di(512/2-128, 384/2+384/4-64), core::dimension2di(2*128, 64)
	), exit_w, 3, L"Exit");
	
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
	io::IFileSystem* fsys = device->getFileSystem();
	
	AppEventReceiver receiver;
	device->setEventReceiver(&receiver);
	
	BalanceWheelParams params;
	
	core::position3df end(
		params.original_len*std::sin(core::degToRad(params.original_start_ang)),
		-params.original_len*std::cos(core::degToRad(params.original_start_ang)),
		0.f
	);
	
	core::line3df beam(params.pos, params.pos + end);
	
	std::cout << "end: X: " << end.X << ", Y: " << end.Y << ", Z: " << end.Z << std::endl;
	std::cout << "beam.end: X: " << beam.end.X << ", Y: " << beam.end.Y << ", Z: " << beam.end.Z << std::endl;
	scene::IMeshSceneNode* bal_wheel = smgr->addSphereSceneNode(params.radius, 32, 0, 1);
	
	if (bal_wheel == nullptr)
	{
		std::cout << "The balance wheel fails to be created!" << std::endl;
		return 1;
	}
	
	bal_wheel->setPosition(beam.end);
	bal_wheel->setScale(params.scale);
	bal_wheel->setRotation(core::vector3df(0.f, 0.f, params.original_start_ang));
	
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
		0, 2
	);
	
	if (platform == nullptr)
	{
		std::cout << "The platform fails to be created!" << std::endl;
		return 1;
	}
	
	platform->setPosition(core::position3df(params.pos.X, params.pos.Y - params.original_len - params.radius - 3.f, 0.f));
	platform->getMaterial(0) = material;
	platform->setMaterialTexture(0, vdrv->getTexture(fsys->getAbsolutePath("./textures/stone2.jpg")));
	platform->addShadowVolumeSceneNode();
	
	scene::ILightSceneNode* light = smgr->addLightSceneNode(0, core::position3df(params.pos.X + 10.f, params.pos.Y, 0.f), video::SColorf(255.f, 255.f, 255.f, 255.f), 50.f, 3);
	scene::ICameraSceneNode* camera = smgr->addCameraSceneNodeFPS(0, 100.f, 0.5f, 4);
	camera->setPosition(core::position3df(0.f, params.pos.Y+10.f, 30.f));
	
	if (light == nullptr)
	{
		std::cout << "The light fails to be created!" << std::endl;
		return 1;
	}
	
	f32 cam_offset_dist = 2.5f;
	
	core::vector3df cur_vel(params.original_vel);
	core::position3df cur_rel_pos(end);
	while(device->run())
	{
		if (device->isWindowActive())
		{
			if (receiver.isEscPressed)
			{
				device->closeDevice();
				/*createExitDialogueWindow(device->getGUIEnvironment());
				receiver.isEscPressed = false;*/
			}
			else if (receiver.isMouseWheelChanged)
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
			}
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
							
			core::vector3df elastic_force = cur_rel_pos/(-cur_rel_pos.getLength()) * params.elasticity * (cur_rel_pos.getLength() - params.original_len);
			core::vector3df friction_force = -cur_vel*params.friction_coef;
			f32 pos_len_sqr = cur_rel_pos.dotProduct(cur_rel_pos);
			core::vector3df string_friction_force;
			if (std::fabs(pos_len_sqr) > 0.01)
				string_friction_force = -(cur_vel.dotProduct(cur_rel_pos))/(pos_len_sqr)*cur_rel_pos/std::sqrt(std::fabs(pos_len_sqr))*params.string_friction_coef;
			core::vector3df res_force = core::vector3df(0.0f, params.gravity*params.mass, 0.0f) + elastic_force + friction_force + string_friction_force;
			
			cur_rel_pos += cur_vel*0.001;
			
			cur_vel += res_force*0.001/params.mass;
			
			
			bal_wheel->setPosition(params.pos + cur_rel_pos);
			
			vdrv->beginScene(true, true, video::SColor(0, 0, 0, 255));
			vdrv->setTransform(video::ETS_WORLD, core::IdentityMatrix);
			vdrv->draw3DLine(beam.start, params.pos + cur_rel_pos, video::SColor(255, 255, 255, 255));
			smgr->drawAll();
			device->getGUIEnvironment()->drawAll();
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
