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
	* generates an image that shows the mandelbrot set 
	* - \bref{real_min}, \bref{imaginary_min}, \bref{real_max} and \bref{imaginary_max} 
	*	define the section of the set to paint	
	* -	\bref{max_threshold} and \bref{max_iter} determine termination criteria of the algorithm
	* - \bref{hsv_color_offset} enables the generation of differently colored images
	*/
	static viral_core::auto_pointer<viral_core::image> generate_mandelbrot_image(const viral_core::vector2i& image_dimensions, float hsv_color_offset, float real_min, float imaginary_min, float real_max, float imaginary_max, float max_threshold, int max_iter);

private:
	/**
	* converts from [0,1)^3 hsv-space to [0,255)^3 rgb-space
	*/
	static void hsv_to_rgb(float h, float s, float v, unsigned char &r_out, unsigned char &g_out, unsigned char &b_out);
};

#endif//#ifndef MANDELBROT_GENERATOR_HPP_INCLUDED