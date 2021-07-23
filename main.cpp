#include <irrlicht.h>
#include <iostream>
#include <cmath>
#include "./IrrExtensions/gui/GUIScrollPane.h"
#include <array>
#include <cwchar>

using namespace std;
using namespace irr;

IrrlichtDevice* device = nullptr;
core::dimension2du wnd_size(1920, 1080);
core::recti params_pane_rect(
	core::vector2di(wnd_size.Width-wnd_size.Width/4, 0),
	core::dimension2di(wnd_size.Width/4, wnd_size.Height*2)
);

struct BalanceWheelParams
{
	f32 gravity = -9.8;
	f32 original_len = 4.f;
	f32 mass = 10.0f;
	f32 elasticity = 500.f;
	f32 friction_coef = 1.0f;
	f32 string_friction_coef = 50.0f;
	f32 original_start_ang = 0.0f;  // in degrees
	core::vector3df pos = core::vector3df(0.0f, 0.0f, 0.0f);
	core::vector3df scale = core::vector3df(1.0f, 1.0f, 1.0f)*0.2;
	f32 radius = 5.0f;
	f32 original_vel = 0.0f;
	f32 push_force = 1000.0f;
	
	u16 params_num = 12;
};

struct CameraParams
{
	f32 offset = 3.0f;
	f32 rot_speed = 30.0f;
};

BalanceWheelParams bw_params;
CameraParams c_params;
	
enum ObjectID
{
	OBJECT_ID_BALANCE_WHEEL = 0,
	OBJECT_ID_PLATFORM,
	OBJECT_ID_LIGHT,
	OBJECT_ID_CAMERA
};

enum GUIEditBox
{
	GUI_EDITBOX_GRAVITY = 0,
	GUI_EDITBOX_ORIGINAL_LENGTH,
	GUI_EDITBOX_MASS,
	GUI_EDITBOX_ELASTICITY,
	GUI_EDITBOX_FRICTION_COEFFICIENT,
	GUI_EDITBOX_STRING_FRICTION_COEFFICIENT,
	GUI_EDITBOX_ORIGINAL_START_ANGLE,
	GUI_EDITBOX_POS_X,
	GUI_EDITBOX_POS_Y,
	GUI_EDITBOX_POS_Z,
	GUI_EDITBOX_SCALE_X,
	GUI_EDITBOX_SCALE_Y,
	GUI_EDITBOX_SCALE_Z,
	GUI_EDITBOX_RADIUS,
	GUI_EDITBOX_ORIGINAL_VELOCITY,
	GUI_EDITBOX_PUSHING_FORCE,
	GUI_EDITBOX_COUNT
};

const std::array<s32, GUI_EDITBOX_COUNT> editbox_ids = {3, 5, 7, 9, 11, 13, 15, 18, 20, 22, 25, 27, 29, 31, 33, 35};

class AppEventReceiver : public IEventReceiver
{
public:
	AppEventReceiver() : isEscPressed(false), isMiddleMousePressed(false), wheel(0.0f), mousePosDelta(0.0f, 0.0f, 0.0f), lastMousePos(0.0f, 0.0f, 0.0f),
						isUpPressed(false), isDownPressed(false), isLeftPressed(false), isRightPressed(false), isResetButtonClicked(false)
	{
	}
	
	virtual bool OnEvent(const SEvent& event)
	{
		switch (event.EventType)
		{
			case EET_KEY_INPUT_EVENT:
			{
				if (event.KeyInput.Key == KEY_ESCAPE)
				{
					isEscPressed = true;
					break;
				}
				
				core::vector2di cursor_pos = device->getCursorControl()->getPosition();
				if (params_pane_rect.isPointInside(cursor_pos))
					break;
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
			}
			case EET_MOUSE_INPUT_EVENT:
			{
				core::vector2di cursor_pos = device->getCursorControl()->getPosition();
				if (params_pane_rect.isPointInside(cursor_pos))
					break;
				
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
			}
			case EET_GUI_EVENT:
			{
				if (event.GUIEvent.EventType == gui::EGET_BUTTON_CLICKED)
					isResetButtonClicked = true;
			}
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
	bool isResetButtonClicked;
};

void createParametersPanelWindow(u16 params_num)
{
	gui::IGUIEnvironment* env = device->getGUIEnvironment();
	gui::IGUISkin* skin = env->getSkin();
	for (s32 i = 0; i < gui::EGDC_COUNT; i++)
	{
		video::SColor color = skin->getColor((gui::EGUI_DEFAULT_COLOR)i);
		color.setAlpha(255);
		skin->setColor((gui::EGUI_DEFAULT_COLOR)i, color);
	}
	
	gui::GUIScrollPane* params_pane = new gui::GUIScrollPane(env, env->getRootGUIElement(), params_pane_rect, 1);
	
	params_pane->showVerticalScrollBar(true);
	params_pane->showHorizontalScrollBar(false);
	params_pane->updateAbsolutePosition();
	
	core::dimension2di params_pane_size = params_pane_rect.getSize();
	s32 width = params_pane_size.Width/6;
	s32 height = wnd_size.Height/12;
	
	env->addButton(core::recti(
		core::vector2di(params_pane_size.Width/2-params_pane_size.Width/2/4, height/2/2), 
		core::dimension2di(params_pane_size.Width/2/4*2, height/2)), 
	params_pane, 36, L"Reset");
	gui::IGUIStaticText* gravity_st = env->addStaticText(L"Gravity:", core::recti(
		core::vector2di(width, height),
		core::dimension2di(params_pane_size.Width, wnd_size.Height/24)
	), false, false, params_pane, 2);
	
	height += wnd_size.Height/24;
	u32 gravity_text_w = gravity_st->getTextWidth()*4;
	core::string<wchar_t> gravity_text((double)bw_params.gravity);
	gui::IGUIEditBox* gravity_edbox = env->addEditBox(gravity_text.c_str(), core::recti(
		core::vector2di(width, height),
		core::dimension2di(gravity_text_w, wnd_size.Height/24)
	), true, params_pane, 3);
	
	height += wnd_size.Height/24 + wnd_size.Height/12;
	env->addStaticText(L"Original Length:", core::recti(
		core::vector2di(width, height),
		core::dimension2di(params_pane_size.Width, wnd_size.Height/24)
	), false, false, params_pane, 4);
	
	height += wnd_size.Height/24;
	core::string<wchar_t> orig_l_text((double)bw_params.original_len);
	env->addEditBox(orig_l_text.c_str(), core::recti(
		core::vector2di(width, height),
		core::dimension2di(gravity_text_w, wnd_size.Height/24)
	), true, params_pane, 5);
	
	height += wnd_size.Height/24 + wnd_size.Height/12;
	env->addStaticText(L"Mass:", core::recti(
		core::vector2di(width, height),
		core::dimension2di(params_pane_size.Width, wnd_size.Height/24)
	), false, false, params_pane, 6);
	
	height += wnd_size.Height/24;
	core::string<wchar_t> mass_text((double)bw_params.mass);
	env->addEditBox(mass_text.c_str(), core::recti(
		core::vector2di(width, height),
		core::dimension2di(gravity_text_w, wnd_size.Height/24)
	), true, params_pane, 7);
	
	height += wnd_size.Height/24 + wnd_size.Height/12;
	env->addStaticText(L"Elasticity:", core::recti(
		core::vector2di(width, height),
		core::dimension2di(params_pane_size.Width, wnd_size.Height/24)
	), false, false, params_pane, 8);
	
	height += wnd_size.Height/24;
	core::string<wchar_t> elasticity_text((double)bw_params.elasticity);
	env->addEditBox(elasticity_text.c_str(), core::recti(
		core::vector2di(width, height),
		core::dimension2di(gravity_text_w, wnd_size.Height/24)
	), true, params_pane, 9);
	
	height += wnd_size.Height/24 + wnd_size.Height/12;
	env->addStaticText(L"Air resistance coefficient:", core::recti(
		core::vector2di(width, height),
		core::dimension2di(params_pane_size.Width, wnd_size.Height/24)
	), false, false, params_pane, 10);
	
	height += wnd_size.Height/24;
	core::string<wchar_t> air_resist_text((double)bw_params.friction_coef);
	env->addEditBox(air_resist_text.c_str(), core::recti(
		core::vector2di(width, height),
		core::dimension2di(gravity_text_w, wnd_size.Height/24)
	), true, params_pane, 11);
	
	height += wnd_size.Height/24 + wnd_size.Height/12;
	env->addStaticText(L"String friction coefficient:", core::recti(
		core::vector2di(width, height),
		core::dimension2di(params_pane_size.Width, wnd_size.Height/24)
	), false, false, params_pane, 12);
	
	height += wnd_size.Height/24;
	core::string<wchar_t> str_fric_text((double)bw_params.string_friction_coef);
	env->addEditBox(str_fric_text.c_str(), core::recti(
		core::vector2di(width, height),
		core::dimension2di(gravity_text_w, wnd_size.Height/24)
	), true, params_pane, 13);
	
	width += gravity_text_w + width;
	height = wnd_size.Height/12;
	env->addStaticText(L"Original start angle:", core::recti(
		core::vector2di(width, height),
		core::dimension2di(params_pane_size.Width, wnd_size.Height/24)
	), false, false, params_pane, 14);
	
	height += wnd_size.Height/24;
	core::string<wchar_t> orig_start_ang_text((double)bw_params.original_start_ang);
	env->addEditBox(orig_start_ang_text.c_str(), core::recti(
		core::vector2di(width, height),
		core::dimension2di(gravity_text_w, wnd_size.Height/24)
	), true, params_pane, 15);
	
	height += wnd_size.Height/24 + wnd_size.Height/12;
	env->addStaticText(L"Position:", core::recti(
		core::vector2di(width, height),
		core::dimension2di(params_pane_size.Width, wnd_size.Height/24)
	), false, false, params_pane, 16);
	
	height += wnd_size.Height/24;
	
	env->addStaticText(L"X:", core::recti(core::vector2di(width, height), core::dimension2di(gravity_text_w/6, wnd_size.Height/24)), false, false, params_pane, 17);
	core::string<wchar_t> pos_x_text((double)bw_params.pos.X);
	env->addEditBox(pos_x_text.c_str(), core::recti(
		core::vector2di(width + gravity_text_w/6, height),
		core::dimension2di(gravity_text_w/6, wnd_size.Height/24)
	), true, params_pane, 18);
	env->addStaticText(L"Y:", core::recti(core::vector2di(width + (gravity_text_w/6)*2, height), core::dimension2di(gravity_text_w/6, wnd_size.Height/24)), false, false, params_pane, 19);
	core::string<wchar_t> pos_y_text((double)bw_params.pos.Y);
	env->addEditBox(pos_y_text.c_str(), core::recti(
		core::vector2di(width + (gravity_text_w/6)*3, height),
		core::dimension2di(gravity_text_w/6, wnd_size.Height/24)
	), true, params_pane, 20);
	env->addStaticText(L"Z:", core::recti(core::vector2di(width + (gravity_text_w/6)*4, height), core::dimension2di(gravity_text_w/6, wnd_size.Height/24)), false, false, params_pane, 21);
	core::string<wchar_t> pos_z_text((double)bw_params.pos.Z);
	env->addEditBox(pos_z_text.c_str(), core::recti(
		core::vector2di(width + (gravity_text_w/6)*5, height),
		core::dimension2di(gravity_text_w/6, wnd_size.Height/24)
	), true, params_pane, 22);
	
	height += wnd_size.Height/24 + wnd_size.Height/12;
	env->addStaticText(L"Scale:", core::recti(
		core::vector2di(width, height),
		core::dimension2di(params_pane_size.Width, wnd_size.Height/24)
	), false, false, params_pane, 23);
	
	height += wnd_size.Height/24;
	
	env->addStaticText(L"X:", core::recti(core::vector2di(width, height), core::dimension2di(gravity_text_w/6, wnd_size.Height/24)), false, false, params_pane, 24);
	core::string<wchar_t> scale_x_text((double)bw_params.scale.X);
	env->addEditBox(scale_x_text.c_str(), core::recti(
		core::vector2di(width + gravity_text_w/6, height),
		core::dimension2di(gravity_text_w/6, wnd_size.Height/24)
	), true, params_pane, 25);
	env->addStaticText(L"Y:", core::recti(core::vector2di(width + (gravity_text_w/6)*2, height), core::dimension2di(gravity_text_w/6, wnd_size.Height/24)), false, false, params_pane, 26);
	core::string<wchar_t> scale_y_text((double)bw_params.scale.Y);
	env->addEditBox(scale_y_text.c_str(), core::recti(
		core::vector2di(width + (gravity_text_w/6)*3, height),
		core::dimension2di(gravity_text_w/6, wnd_size.Height/24)
	), true, params_pane, 27);
	env->addStaticText(L"Z:", core::recti(core::vector2di(width + (gravity_text_w/6)*4, height), core::dimension2di(gravity_text_w/6, wnd_size.Height/24)), false, false, params_pane, 28);
	core::string<wchar_t> scale_z_text((double)bw_params.scale.Z);
	env->addEditBox(scale_z_text.c_str(), core::recti(
		core::vector2di(width + (gravity_text_w/6)*5, height),
		core::dimension2di(gravity_text_w/6, wnd_size.Height/24)
	), true, params_pane, 29);
	
	height += wnd_size.Height/24 + wnd_size.Height/12;
	env->addStaticText(L"Radius:", core::recti(
		core::vector2di(width, height),
		core::dimension2di(params_pane_size.Width, wnd_size.Height/24)
	), false, false, params_pane, 30);
	
	height += wnd_size.Height/24;
	
	core::string<wchar_t> radius_text((double)bw_params.radius);
	env->addEditBox(radius_text.c_str(), core::recti(
		core::vector2di(width, height),
		core::dimension2di(gravity_text_w, wnd_size.Height/24)
	), true, params_pane, 31);
	
	height += wnd_size.Height/24 + wnd_size.Height/12;
	env->addStaticText(L"Original velocity:", core::recti(
		core::vector2di(width, height),
		core::dimension2di(params_pane_size.Width, wnd_size.Height/24)
	), false, false, params_pane, 32);
	
	height += wnd_size.Height/24;
	
	core::string<wchar_t> orig_vel_text((double)bw_params.original_vel);
	env->addEditBox(orig_vel_text.c_str(), core::recti(
		core::vector2di(width, height),
		core::dimension2di(gravity_text_w, wnd_size.Height/24)
	), true, params_pane, 33);
	
	height += wnd_size.Height/24 + wnd_size.Height/12;
	env->addStaticText(L"Pushing force:", core::recti(
		core::vector2di(width, height),
		core::dimension2di(params_pane_size.Width, wnd_size.Height/24)
	), false, false, params_pane, 34);
	
	height += wnd_size.Height/24;
	
	core::string<wchar_t> push_f_text((double)bw_params.push_force);
	env->addEditBox(push_f_text.c_str(), core::recti(
		core::vector2di(width, height),
		core::dimension2di(gravity_text_w, wnd_size.Height/24)
	), true, params_pane, 35);
}

int main()
{
	device = createDevice(video::EDT_OPENGL, wnd_size, 32, false, true);
	
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
	
	createParametersPanelWindow(bw_params.params_num);
	
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
	
	core::vector3df up_vec = (end*-1.0f).normalize();
	bal_wheel->setPosition(beam.end);
	bal_wheel->setScale(bw_params.scale);
	bal_wheel->setRotation(up_vec.getHorizontalAngle());
	
	video::SMaterial material;
	material.MaterialType = video::EMT_LIGHTMAP_LIGHTING;
	material.AntiAliasing = true;
	material.Lighting = true;
	
	bal_wheel->getMaterial(0) = material;
	bal_wheel->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, true);
	bal_wheel->setMaterialTexture(0, vdrv->getTexture(fsys->getAbsolutePath("./textures/wood5.jpeg")));
	
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
	
	scene::ILightSceneNode* light = smgr->addLightSceneNode(0, core::vector3df(bw_params.pos.X + 10.f, bw_params.pos.Y, 0.f), video::SColorf(255.f, 255.f, 255.f, 255.f), 50.f, OBJECT_ID_LIGHT);
	scene::ICameraSceneNode* camera = smgr->addCameraSceneNode(0, core::vector3df(0.0f, -bw_params.original_len/2, bw_params.original_len*2), core::vector3df(0.0f, -bw_params.original_len, 0.0f), OBJECT_ID_CAMERA, true);
	
	//scene::ILightSceneNode* light2 = smgr->addLightSceneNode(0, core::vector3df(-50.0f, 0.0f, 0.0f), video::SColorf(0.0f,0.0f,0.0f,255.0f), 50.f, 5);
	//scene::ILightSceneNode* light3 = smgr->addLightSceneNode(0, core::vector3df(50.0f, 0.0f, 0.0f), video::SColorf(0.0f,0.0f,0.0f,255.0f), 50.f, 6);
	//scene::ILightSceneNode* light4 = smgr->addLightSceneNode(0, core::vector3df(0.0f, 0.0f, -50.0f), video::SColorf(0.0f,0.0f,0.0f,255.0f), 50.f, 7);
	//scene::ILightSceneNode* light5 = smgr->addLightSceneNode(0, core::vector3df(0.0f, 0.0f, 50.0f), video::SColorf(0.0f,0.0f,0.0f,255.0f), 50.f, 8);
	camera->bindTargetAndRotation(true);
	
	if (light == nullptr)
	{
		std::cout << "The light fails to be created!" << std::endl;
		return 1;
	}
	
	smgr->addTextSceneNode(env->getBuiltInFont(), L"X", video::SColor(255, 255, 255, 255), 0, core::vector3df(11.0f, 0.0f, 0.0f), 9);
	smgr->addTextSceneNode(env->getBuiltInFont(), L"Y", video::SColor(255, 255, 255, 255), 0, core::vector3df(0.0f, 11.0f, 0.0f), 10);
	smgr->addTextSceneNode(env->getBuiltInFont(), L"Z", video::SColor(255, 255, 255, 255), 0, core::vector3df(0.0f, 0.0f, 11.0f), 11);
	
	bal_wheel->addShadowVolumeSceneNode(0, 12, true);
	smgr->setShadowColor(video::SColor(150,0,0,0));
	platform->addShadowVolumeSceneNode(0, 13, true);
	
	//createParametersPanelWindow(env);
	
	core::vector3df cur_vel(bw_params.original_vel);
	core::vector3df cur_rel_pos(end);
	core::vector3df cur_push_force(0.0f, 0.0f, 0.0f);
	
	u32 needTime = device->getTimer()->getTime() + 10;
	while(device->run())
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
			core::vector3df nmlzed_target = camera->getTarget() - camera->getPosition();
			nmlzed_target.normalize();
				
			cur_push_force = nmlzed_target * bw_params.push_force;
			receiver.isUpPressed = false;
		}
		else if (receiver.isDownPressed)
		{
			core::vector3df nmlzed_target = camera->getTarget() - camera->getPosition();
			nmlzed_target.normalize();
				
			cur_push_force = nmlzed_target * -bw_params.push_force;
			receiver.isDownPressed = false;
		}
		else if (receiver.isLeftPressed)
		{
			core::vector3df nmlzed_target = camera->getTarget() - camera->getPosition();
			nmlzed_target.normalize();
			nmlzed_target.rotateXZBy(90.0f);
				
			cur_push_force = nmlzed_target * bw_params.push_force;
			receiver.isLeftPressed = false;
		}
		else if (receiver.isRightPressed)
		{
			core::vector3df nmlzed_target = camera->getTarget() - camera->getPosition();
			nmlzed_target.normalize();
			nmlzed_target.rotateXZBy(-90.0f);
				
			cur_push_force = nmlzed_target * bw_params.push_force;
			receiver.isRightPressed = false;
		}
		else if (receiver.isResetButtonClicked)
		{
			for (u32 i = 0; i < (u32)GUI_EDITBOX_COUNT; i++)
			{
				gui::IGUIElement* editbox = env->getRootGUIElement()->getElementFromId(editbox_ids[i], true);
				const wchar_t* param_text = editbox->getText();
				switch ((GUIEditBox)i)
				{
					case GUI_EDITBOX_GRAVITY:
						bw_params.gravity = wcscmp(param_text, L"") != 0 ? wcstof(param_text, 0) : bw_params.gravity;
						break;
					case GUI_EDITBOX_ORIGINAL_LENGTH:
						bw_params.original_len = wcscmp(param_text, L"") != 0 ? wcstof(param_text, 0) : bw_params.original_len;
						break;
					case GUI_EDITBOX_MASS:
						bw_params.mass = wcscmp(param_text, L"") != 0 ? wcstof(param_text, 0) : bw_params.mass;
						break;
					case GUI_EDITBOX_ELASTICITY:
						bw_params.elasticity = wcscmp(param_text, L"") != 0 ? wcstof(param_text, 0) : bw_params.elasticity;
						break;
					case GUI_EDITBOX_FRICTION_COEFFICIENT:
						bw_params.friction_coef = wcscmp(param_text, L"") != 0 ? wcstof(param_text, 0) : bw_params.friction_coef;
						break;
					case GUI_EDITBOX_STRING_FRICTION_COEFFICIENT:
						bw_params.string_friction_coef = wcscmp(param_text, L"") != 0 ? wcstof(param_text, 0) : bw_params.string_friction_coef;
						break;
					case GUI_EDITBOX_ORIGINAL_START_ANGLE:
						bw_params.original_start_ang = wcscmp(param_text, L"") != 0 ? wcstof(param_text, 0) : bw_params.original_start_ang;
						break;
					case GUI_EDITBOX_POS_X:
						bw_params.pos.X = wcscmp(param_text, L"") != 0 ? wcstof(param_text, 0) : bw_params.pos.X;
						break;
					case GUI_EDITBOX_POS_Y:
						bw_params.pos.Y = wcscmp(param_text, L"") != 0 ? wcstof(param_text, 0) : bw_params.pos.Y;
						break;
					case GUI_EDITBOX_POS_Z:
						bw_params.pos.Z = wcscmp(param_text, L"") != 0 ? wcstof(param_text, 0) : bw_params.pos.Z;
						break;
					case GUI_EDITBOX_SCALE_X:
						bw_params.scale.X = wcscmp(param_text, L"") != 0 ? wcstof(param_text, 0) : bw_params.scale.X;
						break;
					case GUI_EDITBOX_SCALE_Y:
						bw_params.scale.Y = wcscmp(param_text, L"") != 0 ? wcstof(param_text, 0) : bw_params.scale.Y;
						break;
					case GUI_EDITBOX_SCALE_Z:
						bw_params.scale.Z = wcscmp(param_text, L"") != 0 ? wcstof(param_text, 0) : bw_params.scale.Z;
						break;
					case GUI_EDITBOX_RADIUS:
						bw_params.radius = wcscmp(param_text, L"") != 0 ? wcstof(param_text, 0) : bw_params.radius;
						break;
					case GUI_EDITBOX_ORIGINAL_VELOCITY:
						bw_params.original_vel = wcscmp(param_text, L"") != 0 ? wcstof(param_text, 0) : bw_params.original_vel;
						break;
					case GUI_EDITBOX_PUSHING_FORCE:
						bw_params.push_force = wcscmp(param_text, L"") != 0 ? wcstof(param_text, 0) : bw_params.push_force;
						break;
				}
			}
			end = core::vector3df(bw_params.original_len*std::sin(core::degToRad(bw_params.original_start_ang)),
				-bw_params.original_len*std::cos(core::degToRad(bw_params.original_start_ang)),
				0.f);
			beam = core::line3df(bw_params.pos, bw_params.pos + end);
			bal_wheel->drop();
			bal_wheel = smgr->addSphereSceneNode(bw_params.radius, 32, 0, OBJECT_ID_BALANCE_WHEEL);
			up_vec = (end*-1.0f).normalize();
			bal_wheel->setPosition(beam.end);
			bal_wheel->setScale(bw_params.scale);
			bal_wheel->setRotation(up_vec.getHorizontalAngle());
	
			material.MaterialType = video::EMT_LIGHTMAP_LIGHTING;
			material.AntiAliasing = true;
			material.Lighting = true;
	
			bal_wheel->getMaterial(0) = material;
			bal_wheel->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, true);
			bal_wheel->setMaterialTexture(0, vdrv->getTexture(fsys->getAbsolutePath("./textures/wood5.jpeg")));
			bal_wheel->addShadowVolumeSceneNode(0, 12, true);
			smgr->setShadowColor(video::SColor(150,0,0,0));
			
			platform->setPosition(core::vector3df(bw_params.pos.X, bw_params.pos.Y - bw_params.original_len - bw_params.radius - 3.f, 0.f));
			light->setPosition(core::vector3df(bw_params.pos.X + 10.f, bw_params.pos.Y, 0.f));
			camera->setPosition(core::vector3df(0.0f, -bw_params.original_len/2, bw_params.original_len*2));
			
			cur_vel = core::vector3df(bw_params.original_vel);
			cur_rel_pos = core::vector3df(end);
			cur_push_force = core::vector3df(0.0f, 0.0f, 0.0f);
			
			receiver.isResetButtonClicked = false;
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
			string_friction_force = -(cur_vel.dotProduct(cur_rel_pos))/(pos_len_sqr)*cur_rel_pos*bw_params.string_friction_coef;
		core::vector3df res_force = core::vector3df(0.0f, bw_params.gravity*bw_params.mass, 0.0f) + elastic_force + friction_force + string_friction_force + cur_push_force;
			
		cur_rel_pos += cur_vel*0.01;
			
		cur_vel += res_force*0.01/bw_params.mass;
			
			
		bal_wheel->setPosition(bw_params.pos + cur_rel_pos);
			
		up_vec = (cur_rel_pos*-1.0f).normalize();
		bal_wheel->setRotation(up_vec.getHorizontalAngle());
			
		vdrv->beginScene(true, true, video::SColor(0, 0, 0, 255));
		vdrv->setTransform(video::ETS_WORLD, core::IdentityMatrix);
			
		video::SMaterial line_material;
		line_material.setFlag(video::EMF_NORMALIZE_NORMALS, true);
		line_material.Thickness = 1.5;
		line_material.Lighting = true;
		line_material.MaterialType = video::EMT_LIGHTMAP_LIGHTING;
		line_material.NormalizeNormals = true;

		vdrv->setMaterial(line_material);
		vdrv->draw3DLine(core::vector3df(-10.0f, 0.0f, 0.0f), core::vector3df(10.0f, 0.0f, 0.0f), video::SColor(202,0,16,255));//video::SColor(252, 0, 20, 255));
		vdrv->draw3DLine(core::vector3df(0.0f, 0.0f, 0.0f), core::vector3df(0.0f, 10.0f, 0.0f), video::SColor(0,126,0,255));//video::SColor(10, 0, 234, 255));
		vdrv->draw3DLine(core::vector3df(0.0f, 0.0f, -10.0f), core::vector3df(0.0f, 0.0f, 10.0f), video::SColor(0,0,146,255));//video::SColor(0, 217, 0, 255));
			
		vdrv->draw3DLine(beam.start, bw_params.pos + cur_rel_pos, video::SColor(255, 255, 255, 255));
		
		smgr->drawAll();
		gui::IGUISkin* skin = env->getSkin();
		skin->draw2DRectangle(0, skin->getColor(gui::EGDC_3D_FACE), core::recti(
			core::vector2di(wnd_size.Width-wnd_size.Width/4, 0),
			core::dimension2di(wnd_size.Width/4, wnd_size.Height)
		));
		env->drawAll();
		vdrv->endScene();
			
		core::stringw caption = L"[Irrlicht Engine] Balance Wheel Demo [";
		caption += vdrv->getName();
		caption += "]";
		/*caption += "; FPS: ";
		caption += vdrv->getFPS();
		caption += "]";*/
			
		device->setWindowCaption(caption.c_str());
	
		while (device->getTimer()->getTime() < needTime) {
			device->getTimer()->tick();
			device->yield();
		}
		needTime += 10;
	}
	
	device->drop();
	
	return 0;
}
