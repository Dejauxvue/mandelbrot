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
#include <viral_gui/gui_editbox.hpp>
#include <viral_gui/gui_value_edit.hpp>
#include <viral_gui/gui_dropdown.hpp>


#include <viral_core/render_resource.hpp>
#include <viral_core/engine.hpp>
#include <viral_core/file_util.hpp>
#include <viral_core/file.hpp>
#include <viral_core/render_command.hpp>
#include <viral_core/log.hpp>

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
	image_material_(create_image_material(flat_shader_id_)),
	image_viewport_(new gui_image("image_viewport", gui_, create_image_style(),
		image(vector2i(1920, 1080)), image_material_)),
	image_task_(0)
{
	element_cache_.entry<gui_frame>("bg_frame")().
		set_content(image_viewport_);

	register_event_callback("start_pause_button", *this, (&mandelbrot_gui::start_pause_visualization));

	parameters_.image_dimensions_ = vector2i(1920, 1080);
	parameters_.interpolate_ = true;
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
	return shared_instance<gui_image_style>(new gui_image_style(vector2f(0.7f, 1.f), 0,
		true, true, gui_image::no_aspect, false));
}

render_material_data mandelbrot_gui::create_image_material(const shared_pointer<render_shader_id>& flat_shader_id)
{
	render_material_data ret;
	ret.shader = flat_shader_id;
	ret.unlit = true;
	ret.blend = render_material_data::blend_alpha;
	return ret;
}

void mandelbrot_gui::update_parameters_from_gui()
{
	parameters_.iterations_ = element_cache_.entry<gui_editbox>("iteration_editbox")().text().to_int();
	parameters_.interpolation_ = element_cache_.entry<gui_value_edit>("interpolation_slider")().value();
	switch (element_cache_.entry<gui_dropdown>("method_dropdown")().selected_index()) {
	case 0:
		parameters_.interpolation_method_ = &mandelbrot_generator::polynomial;
		break;
	case 1:
		parameters_.interpolation_method_ = &mandelbrot_generator::linear_angle_and_abs;
		break;
	case 2:
		parameters_.interpolation_method_ = &mandelbrot_generator::linear_short_angle_and_abs;
		break;
	case 3:
		parameters_.interpolation_method_ = &mandelbrot_generator::linear_xy;
		break;
	default:
		LOG_ERROR(string("Invalid index from method_dropdown: ") 
			+ string(element_cache_.entry<gui_dropdown>("method_dropdown")().selected_index()));
		break;
	}
}

void mandelbrot_gui::update_gui_from_parameters()
{
	element_cache_.entry<gui_editbox>("iteration_editbox")().set_text(string(parameters_.iterations_));
	element_cache_.entry<gui_value_edit>("interpolation_slider")().try_set_value(parameters_.interpolation_);
}



void mandelbrot_gui::logics_hook(viral_gui::gui_modal_interaction * modal_interaction)
{
	MUTEX_SCOPE(visualization_mutex_);
	if (!image_task_) {
		update_parameters_from_gui();
		image_task_.reset(new image_computation_task(parameters_));
		image_task_->start();
	}
	else {
		if (image_task_->thread_has_terminated()) {
			image_task_->join();
			image_viewport_->apply_source_image(
				*image_task_->get_output(), image_material_);
			image_task_.reset();

			if (run_visualization_) {
				parameters_.interpolation_ += element_cache_.entry<gui_editbox>("stepsize_editbox")().text().to_float();
				if (parameters_.interpolation_ >= 1.f)
				{
					parameters_.interpolation_ = 0.f;
					parameters_.iterations_++;
				}
				update_gui_from_parameters();
			}
		}
	}
}

void mandelbrot_gui::render_hook(render_command_queue & queue)
{
	if (!rendering_initialized_)initialize_rendering(queue);
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

void mandelbrot_gui::set_gui_read_only(bool b)
{
	element_cache_.entry<gui_editbox>("stepsize_editbox")().set_read_only(b);
	element_cache_.entry<gui_editbox>("iteration_editbox")().set_read_only(b);
	element_cache_.entry<gui_value_edit>("interpolation_slider")().set_read_only(b);
	element_cache_.entry<gui_value_edit>("interpolation_slider")().slider().set_read_only(b);
}

void mandelbrot_gui::start_pause_visualization(const gui_button_event & event)
{
	MUTEX_SCOPE(visualization_mutex_);
	run_visualization_ = !run_visualization_;
	if (run_visualization_) { 
		element_cache_.entry<gui_button>("start_pause_button")().set_text("pause"); 
		set_gui_read_only(true);
	}
	else { 
		element_cache_.entry<gui_button>("start_pause_button")().set_text("start"); 
		set_gui_read_only(false);
	}
}

mandelbrot_gui::image_computation_task::image_computation_task(const mandelbrot_generator::parameter_set & params)
	:
	parameters_(params)
{
}

viral_core::auto_pointer<viral_core::image>& mandelbrot_gui::image_computation_task::get_output()
{
	return output_image_;
}

void mandelbrot_gui::image_computation_task::task_main()
{
	output_image_ = mandelbrot_generator::generate_mandelbrot_image_julia_value(parameters_);
}
