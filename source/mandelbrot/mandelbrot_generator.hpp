/**
*************************************************************************
*
* @file mandelbrot_generator.hpp
*
* Generation of images of the mandlebrot-set, 
* see https://de.wikipedia.org/wiki/Mandelbrot-Menge
*
************************************************************************/

#ifndef MANDELBROT_GENERATOR_HPP_INCLUDED
#define MANDELBROT_GENERATOR_HPP_INCLUDED

#include <viral_core/image.hpp>

/**
*************************************************************************
*
* @class mandelbrot_generator
*
* visualizes the mandelbrot set
*
************************************************************************/

class mandelbrot_generator {
public:

	/**
	*************************************************************************
	* @class mandelbrot_generator::parameter_set
	* condenses all parameters necessary for a computation
	* depending on the visualizatio method used, some values can be omitted
	************************************************************************/
	class parameter_set {
	public:
		viral_core::vector2i image_dimensions_;
		float hsv_color_offset_ = 0.f;
		float real_min_ = -2.001f;
		float imaginary_min_ = -1.2001f;
		float real_max_ = 1.f;
		float imaginary_max_ = 1.2f;
		float max_threshold_ = 20.f;
		int max_iter_ = 20;
		int iterations_ = 1;
		float interpolation_ = 0.f;
		bool interpolate_ = false;
		/**signature is start_x, start_y, goal_x, goal_y, interpolation, out_x, out_y*/
		void (*interpolation_method_)(float, float, float, float, float, float&, float&) = 0;
	};

	/** interpolation methods for \bref{generate_mandelbrot_image_julia_iter} */
	//{
	static void linear_angle_and_abs(float, float, float, float, float, float&, float&);
	static void linear_short_angle_and_abs(float, float, float, float, float, float&, float&);
	static void polynomial(float, float, float, float, float, float&, float&);
	static void linear_xy(float, float, float, float, float, float&, float&);
	//}

	/**
	* generates an image that shows the mandelbrot set, more specifically an image that
	* encodes the number of julia iterations per pixel
	* - \bref{real_min}, \bref{imaginary_min}, \bref{real_max} and \bref{imaginary_max} 
	*	define the section of the set to paint	
	* -	\bref{max_threshold} and \bref{max_iter} determine termination criteria of the algorithm
	* - \bref{hsv_color_offset} enables the generation of differently colored images
	*/
	static viral_core::auto_pointer<viral_core::image> generate_mandelbrot_image_julia_iter(
		const parameter_set& params);

	/**
	* generates an image that shows the julia value for each pixel for a given number of iterations
	* - \bref{real_min}, \bref{imaginary_min}, \bref{real_max} and \bref{imaginary_max}
	*	define the section of the set to paint
	* - if interpolation is specified, an image is produced from the interpolation between iterations 
	*	and iterations + 1, by z_n^(2/(2-r) + r * c, where c is the first element of the sequence 
	*	and r is the grade of interpolation
	*/
	static viral_core::auto_pointer<viral_core::image> generate_mandelbrot_image_julia_value(
		const parameter_set& params);

private:
	/**
	* converts from [0,1)^3 hsv-space to [0,255)^3 rgb-space
	*/
	static void hsv_to_rgb(float h, float s, float v, 
		unsigned char &r_out, unsigned char &g_out, unsigned char &b_out);

	/**
	* converts from [0,1)^3 hsl-space to [0,255)^3 rgb-space
	*/
	static void hsl_to_rgb(float h, float s, float l, 
		unsigned char &r_out, unsigned char &g_out, unsigned char &b_out);
	
	/**
	* converts from [0,1)^2 to the rb plane in the [0,255)^3 rgb-space
	*/
	static void rgb_project_2d(float angle, float absolute, 
		unsigned char &r_out, unsigned char &g_out, unsigned char &b_out);

	/**
	* computes c^x where c is a complex number given as c= a + bi and x is real
	*/
	static void complex_pow_real(float a, float b, float x, float& a_out, float& b_out);
};

#endif//#ifndef MANDELBROT_GENERATOR_HPP_INCLUDED