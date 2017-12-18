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
#include <viral_gui/gui_event.hpp>
#include <viral_gui/gui_button.hpp>

#include <viral_core/render_resource.hpp>
#include <viral_core/shared_pointer.hpp>
#include <viral_core/thread.hpp>
#include <viral_core/thread_synch.hpp>

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
	/** rendering necessities*/
	//{
	viral_core::shared_pointer<viral_core::render_shader_id> flat_shader_id_;
	viral_core::shared_pointer<viral_core::render_material_id> image_material_id_;
	viral_core::render_material_data image_material_;
	//}
	
	/** initialization*/
	//{
	bool rendering_initialized_ = false;
	void initialize_rendering(viral_core::render_command_queue & queue);
	static viral_core::shared_instance<viral_gui::gui_image_style> create_image_style();
	static viral_core::render_material_data create_image_material
		(const viral_core::shared_pointer<viral_core::render_shader_id>& flat_shader_id);
	//}

	bool run_visualization_ = false;

	/** parameter set for the visualization */
	mandelbrot_generator::parameter_set parameters_;
	void update_parameters_from_gui();
	void update_gui_from_parameters();
	viral_core::mutex visualization_mutex_;

	virtual void logics_hook(viral_gui::gui_modal_interaction* modal_interaction);
	virtual void render_hook(viral_gui::render_command_queue& queue);

	static viral_core::list<viral_core::string> style_paths();
	static viral_core::list<viral_core::string> element_paths();

	/**disables or enables all editable gui_elements*/
	void set_gui_read_only(bool);

	/** gui elements*/
	//{
	const viral_core::shared_pointer<viral_gui::gui_image> image_viewport_;
	//}


	/** callbacks for buttons etc.*/
	//{
	void start_pause_visualization(const viral_gui::gui_button_event& event);
	void write_simulation_to_file(const viral_gui::gui_button_event& event);
	//}

	/**
	*************************************************************************
	*
	* @class mandelbrot_gui::image_computation_task
	*
	* threaded task to compute the mandelbrot image 
	* using \bref{mandelbrot_generator}
	*
	************************************************************************/
	class image_computation_task :
		public viral_core::threaded_task
	{
	public:
		image_computation_task(const mandelbrot_generator::parameter_set& params);
		viral_core::auto_pointer<viral_core::image>& get_output();
	private:
		const mandelbrot_generator::parameter_set parameters_;
		viral_core::auto_pointer<viral_core::image> output_image_;
		virtual void task_main();
	};

	viral_core::auto_pointer<image_computation_task> image_task_;
};
#endif//#ifndef MANDELBROT_GUI_HPP_INCLUDED