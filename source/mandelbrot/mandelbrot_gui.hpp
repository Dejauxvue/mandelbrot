/**
*************************************************************************
*
* @file mandelbrot_gui.hpp
*
* User interface for the functionalities of \bref{mandelbrot_generator}
*
************************************************************************/
#ifndef MANDELBROT_GUI_HPP_INCLUDED
#define MANDELBROT_GUI_HPP_INCLUDED

#include <viral_gui/gui_app.hpp>
#include <viral_gui/gui_image.hpp>

#include <viral_core/render_resource.hpp>
#include <viral_core/shared_pointer.hpp>

#include "mandelbrot_generator.hpp"



/**
*************************************************************************
*
* @class mandelbrot_gui
*
* User interface for the functionalities of \bref{mandelbrot_generator}
*
************************************************************************/
class mandelbrot_gui :
	public viral_gui::gui_app
{
public:
	mandelbrot_gui();

private:
	
	/** initialization*/
	//{
	bool rendering_initialized_ = false;
	void initialize_rendering(viral_core::render_command_queue & queue);
	static viral_core::shared_instance<viral_gui::gui_image_style> create_image_style();
	//}

	virtual void logics_hook(viral_gui::gui_modal_interaction* modal_interaction);
	virtual void render_hook(viral_gui::render_command_queue& queue);

	static viral_core::list<viral_core::string> style_paths();
	static viral_core::list<viral_core::string> element_paths();

	/**rendering necessities*/
	//{
	viral_core::shared_pointer<viral_core::render_shader_id> flat_shader_id_;
	viral_core::shared_pointer<viral_core::render_material_id> image_material_id_;
	//}

	/** gui elements*/
	//{
	const viral_core::shared_pointer<viral_gui::gui_image> image_viewport_;
	//}

	/** callbacks for buttons etc.*/
	//{
	//}

};
#endif//#ifndef MANDELBROT_GUI_HPP_INCLUDED