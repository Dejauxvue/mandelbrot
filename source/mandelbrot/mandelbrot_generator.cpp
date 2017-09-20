/**
*************************************************************************
*
* @file mandelbrot_generator.cpp
*
* implementation of the mandelbrot set computation
*
************************************************************************/

#include "mandelbrot_generator.hpp"

#include <viral_core/geo_util.hpp>

#include <math.h>

using namespace viral_core;

//////////////////////////////////////////////////////////////////////////
//
// mandelbrot_generator
//
//////////////////////////////////////////////////////////////////////////
viral_core::auto_pointer<viral_core::image> mandelbrot_generator::generate_mandelbrot_image_julia_iter(const viral_core::vector2i & image_dimensions, float hsv_color_offset, float real_min, float imaginary_min, float real_max, float imaginary_max, float max_threshold, int max_iter)
{
	auto_pointer<image> ret(new image(image_dimensions));

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

		if (remain_iter == 0) ret->data()[i * 4] = ret->data()[i * 4 + 1] = ret->data()[i * 4 + 2] = 0;
		else {
			float h_value = (float)julia_iter / (float)max_iter + hsv_color_offset;
			h_value = h_value - (int)h_value;//mod h_value
			hsv_to_rgb(h_value, 1.f, 1.f, ret->data()[i * 4], ret->data()[i * 4 + 1], ret->data()[i * 4 + 2]);
		}

		ret->data()[i * 4 + 3] = 255;//Alpha-value
	}
	return ret;
}


viral_core::auto_pointer<viral_core::image> mandelbrot_generator::generate_mandelbrot_image_julia_value(
	const viral_core::vector2i & image_dimensions, float hsv_color_offset, float real_min, float imaginary_min, float real_max, float imaginary_max, int iterations, float* interpolation)
{
	auto_pointer<image> ret(new image(image_dimensions));

	/*computation according to https://de.wikipedia.org/wiki/Mandelbrot-Menge#Programmbeispiel */
	for (int i = 0; i < ret->size().x * ret->size().y; i++) {
		int x_coordinate = i % ret->size().x;
		int y_coordinate = i / ret->size().x;
		float re_part = real_min + (real_max - real_min) * x_coordinate / ret->size().x;
		float im_part = imaginary_min + (imaginary_max - imaginary_min) * y_coordinate / ret->size().y;

		float x = re_part;
		float y = im_part;
		float xx = x * x;
		float yy = y * y;
		float xy = x * y;

		for (int j = 0; j < iterations; j++) {
			x = xx - yy + re_part;
			y = xy + xy + im_part;
			xx = x*x;
			yy = y*y;
			xy = x*y;
		}
		/*interpolation*/
		if (interpolation) {
			complex_pow_real(x, y, 2.f / (2.f - *interpolation), x, y);
			x += *interpolation * re_part;
			y += *interpolation * im_part;
			xx = x*x;
			yy = y*y;
			xy = x*y;
		}

		float v = 1.f / (1.f + 5.f*sqrtf(xx + yy));
		float h_degree = geo_util::atan2(y, x) * geo_constants::radians_to_degrees;
		while (h_degree < 0.0f) h_degree += 360.f;

		hsl_to_rgb(h_degree / 360.f, 1.f, v, ret->data()[i * 4], ret->data()[i * 4 + 1], ret->data()[i * 4 + 2]);


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
		r_out = g_out = b_out = (unsigned char)(v * 255);
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
		r_out = (unsigned char)(v * 255);
		g_out = (unsigned char)(t * 255);
		b_out = (unsigned char)(p * 255);
		break;
	case 1:
		r_out = (unsigned char)(q * 255);
		g_out = (unsigned char)(v * 255);
		b_out = (unsigned char)(p * 255);
		break;
	case 2:
		r_out = (unsigned char)(p * 255);
		g_out = (unsigned char)(v * 255);
		b_out = (unsigned char)(t * 255);
		break;

	case 3:
		r_out = (unsigned char)(p * 255);
		g_out = (unsigned char)(q * 255);
		b_out = (unsigned char)(v * 255);
		break;
	case 4:
		r_out = (unsigned char)(t * 255);
		g_out = (unsigned char)(p * 255);
		b_out = (unsigned char)(v * 255);
		break;
	case 5:
	default:
		r_out = (unsigned char)(v * 255);
		g_out = (unsigned char)(p * 255);
		b_out = (unsigned char)(q * 255);
		break;
	}
}

void mandelbrot_generator::hsl_to_rgb(float h, float s, float l, unsigned char & r_out, unsigned char & g_out, unsigned char & b_out)
{
	/*implementation from http://forums.codeguru.com/showthread.php?421328-RGB-to-HSL-and-vice-versa */
	r_out = 0;
	g_out = 0;
	b_out = 0;

	if (s <= 0.f)
	{
		r_out = l;
		g_out = l;
		b_out = l;
	}
	else
	{
		double temp1 = 0;
		if (l < .5f)
		{
			temp1 = l*(1 + s);
		}
		else
		{
			temp1 = l + s - (l*s);
		}

		double temp2 = 2 * l - temp1;

		double temp3 = 0;
		for (int i = 0; i < 3; i++)
		{
			unsigned char* c = 0;
			switch (i)
			{
			case 0: // red
			{
				temp3 = h + .33333;
				if (temp3 > 1)
					temp3 -= 1;
				c = &r_out;
				break;
			}
			case 1: // green
			{
				temp3 = h;
				c = &g_out;
				break;
			}
			case 2: // blue
			{
				temp3 = h - .33333;
				if (temp3 < 0)
					temp3 += 1;
				c = &b_out;
				break;
			}
			default:
			{

			}
			}
			if ((temp3 * 6) < 1)
				*c = (unsigned char)((temp2 + (temp1 - temp2) * 6 * temp3) * 100);
			else
				if ((temp3 * 2) < 1)
					*c = (unsigned char)(temp1 * 100);
				else
					if ((temp3 * 3) < 2)
						*c = (unsigned char)((temp2 + (temp1 - temp2)*(.66666 - temp3) * 6) * 100);
					else
						*c = (unsigned char)(temp2 * 100);
		}
	}
	r_out = (unsigned char)((((float)r_out) / 100) * 255);
	g_out = (unsigned char)((((float)g_out) / 100) * 255);
	b_out = (unsigned char)((((float)b_out) / 100) * 255);
}

void mandelbrot_generator::complex_pow_real(float a, float b, float x, float & a_out, float & b_out)
{
	float absolute = sqrtf(a*a + b*b);
	absolute = powf(absolute, x);
	float argument = geo_util::atan2(b, a);
	argument *= x;
	a_out = absolute * cosf(argument);
	b_out = absolute * sinf(argument);
}
