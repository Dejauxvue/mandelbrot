/**
*************************************************************************
*
* @file mandelbrot_generator.cpp
*
* implementation of the mandelbrot set computation
*
************************************************************************/

#include "mandelbrot_generator.hpp"

using namespace viral_core;

viral_core::auto_pointer<viral_core::image> mandelbrot_generator::generate_mandelbrot_image(const viral_core::vector2i & dimensions, float hsv_color_offset, float real_min, float imaginary_min, float real_max, float imaginary_max, float max_threshold, int max_iter)
{
	auto_pointer<image> ret(new image(dimensions));

	/*computation according to https://de.wikipedia.org/wiki/Mandelbrot-Menge#Programmbeispiel */
	for (int i = 0; i < ret->size().x * ret->size().y; i++) {
		int x_coordinate = i % ret->size().x;
		int y_coordinate = i / ret->size().x;
		float im_part = imaginary_min + (imaginary_max - imaginary_min) * y_coordinate / ret->size().y;
		float re_part = real_min + (real_max - real_min) * x_coordinate / ret->size().x;

		int remain_iter = max_iter;
		float xx = re_part * re_part;
		float yy = im_part * im_part;
		float xy = re_part * im_part;
		float abs_2 = xx + yy;

		while (abs_2 <= max_threshold && remain_iter > 0) {
			remain_iter--;
			float x = xx - yy + re_part;
			float y = xy + xy + im_part;
			xx = x*x;
			yy = y*y;
			xy = x*y;
			abs_2 = xx + yy;
		}

		int julia_iter = max_iter - remain_iter;

		if (remain_iter == 0) ret->data()[i * 4] = ret->data()[i * 4 +1] = ret->data()[i * 4 + 2] = 0;
		else { 
			float h_value = (float)julia_iter / (float)max_iter + hsv_color_offset;
			h_value = h_value - (int)h_value;//mod h_value
			hsv_to_rgb(h_value, 1.f, 1.f, ret->data()[i * 4], ret->data()[i * 4 + 1], ret->data()[i * 4 + 2]); 
		}

		ret->data()[i * 4 + 3] = 255;//Alpha-value
	}
	return ret;
}


void mandelbrot_generator::hsv_to_rgb(float h, float s, float v, unsigned char & r_out, unsigned char & g_out, unsigned char & b_out)
{
	/*implementation from https://stackoverflow.com/questions/3018313/algorithm-to-convert-rgb-to-hsv-and-hsv-to-rgb-in-range-0-255-for-both*/
	float hh, p, q, t, ff;
	long i;
	if (s <= 0.0) {       // < is bogus, just shuts up warnings
		r_out= g_out = b_out = v * 255;
		return;
	}
	hh = h * 360.f;
	if (hh >= 360.f) hh = 0.f;
	hh /= 60.f;
	i = (long)hh;
	ff = hh - i;
	p = v * (1.f - s);
	q = v * (1.f - (s * ff));
	t = v * (1.f - (s * (1.f - ff)));

	switch (i) {
	case 0:
		r_out = v * 255;
		g_out = t * 255;
		b_out = p * 255;
		break;
	case 1:
		r_out = q * 255;
		g_out = v * 255;
		b_out = p * 255;
		break;
	case 2:
		r_out = p * 255;
		g_out = v * 255;
		b_out = t * 255;
		break;

	case 3:
		r_out = p * 255;
		g_out = q * 255;
		b_out = v * 255;
		break;
	case 4:
		r_out = t * 255;
		g_out = p * 255;
		b_out = v * 255;
		break;
	case 5:
	default:
		r_out = v * 255;
		g_out = p * 255;
		b_out = q * 255;
		break;
	}
}
