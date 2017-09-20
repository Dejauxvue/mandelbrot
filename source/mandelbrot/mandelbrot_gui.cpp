/**
*************************************************************************
*
* @file mandelbrot_gui.cpp
*
* implementation of \bref{mandelbrot_gui}
*
************************************************************************/

#include "mandelbrot_gui.hpp"

#include <viral_gui/gui_image.hpp>
#include <viral_gui/gui_frame.hpp>


#include <viral_core/render_resource.hpp>
#include <viral_core/engine.hpp>
#include <viral_core/file_util.hpp>
#include <viral_core/file.hpp>
#include <viral_core/render_command.hpp>

using namespace viral_gui;
using namespace viral_core;

//////////////////////////////////////////////////////////////////////////
//
// mandelbrot_gui
//
//////////////////////////////////////////////////////////////////////////

mandelbrot_gui::mandelbrot_gui()
	:
	gui_app("mandelbrot",
		"gui/gui_shader.glvs", "gui/gui_shader.glfs", "gui",
		style_paths(), "cursor_pointer",
		element_paths(), "window_area"),
	image_material_id_(new render_material_id("image_render_material")),
	flat_shader_id_(new render_shader_id("flat_shader")),
	image_viewport_(new gui_image("image_viewport", gui_, 
		create_image_style(), 
		image_material_id_.instance_or_throw(),
		vector2i(1920,1080)))
{
	element_cache_.entry<gui_frame>("bg_frame")().
		set_content(image_viewport_);
}

void mandelbrot_gui::initialize_rendering(render_command_queue & queue)
{
	render_shader_data flat_shader_data;
	flat_shader_data.fragment_shader =
		engine_.files().open_file("shaders/unlit_shader.glfs", file::read_only)->read_text();
	flat_shader_data.vertex_shader =
		engine_.files().open_file("shaders/unlit_shader.glvs", file::read_only)->read_text();
	queue.commit(flat_shader_data, flat_shader_id_);
	rendering_initialized_ = true;
}

shared_instance<gui_image_style> mandelbrot_gui::create_image_style() {
	return shared_instance<gui_image_style>(new gui_image_style(vector2f(1.f,1.f), 1,
		true, true, gui_image::aspect_fill, false));
}


void mandelbrot_gui::logics_hook(viral_gui::gui_modal_interaction * modal_interaction)
{
	
}

void mandelbrot_gui::render_hook(render_command_queue & queue)
{
	if (!rendering_initialized_)initialize_rendering(queue);

	render_material_data material_data;
	material_data.shader = flat_shader_id_;
	material_data.unlit = true;
	material_data.blend = render_material_data::blend_alpha;
	static int iteration = 0;
	static float interpolation = 0.f;
	interpolation += 0.1f;
	if (interpolation >= 0.9f)
	{
		interpolation = 1.f - interpolation;
		interpolation = 0.f;
		iteration++;
		//iteration = 1 - iteration;
		
	}
	auto_pointer<image> new_image = mandelbrot_generator::generate_mandelbrot_image_julia_value(vector2i(1920, 1080),
		0.1f, -2.f, -1.2f, 1.f, 1.2f, iteration, &interpolation);
	//auto_pointer<image> new_image = mandelbrot_generator::generate_mandelbrot_image_julia_iter(vector2i(1920, 1080),
		//0.1f, -2.f, -1.2f, 1.f, 1.2f, 20.f, 20);

	image_viewport_->apply_source_image(*new_image, material_data);
}

list<string> mandelbrot_gui::style_paths()
{
	list<string> style_paths;
	style_paths.insert("gui/gui_styles.xml");
	return style_paths;
}


list<string> mandelbrot_gui::element_paths()
{
	list<string> element_paths;
	element_paths.insert("gui/gui_elements.xml");
	return element_paths;
}


