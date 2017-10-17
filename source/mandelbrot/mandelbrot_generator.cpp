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
#include <viral_core/log.hpp>

#include <math.h>

using namespace viral_core;

//////////////////////////////////////////////////////////////////////////
//
// mandelbrot_generator
//
//////////////////////////////////////////////////////////////////////////

void mandelbrot_generator::linear_angle_and_abs(float start_x, float start_y, 
	float goal_x, float goal_y, float interpolation, float & x, float & y)
{
	float start_abs = sqrtf(start_x * start_x + start_y * start_y);
	float next_abs = sqrtf(goal_x * goal_x + goal_y * goal_y);
	float start_angle = geo_util::atan2(start_y, start_x);
	float next_angle = geo_util::atan2(goal_y, goal_x);
	float curr_abs = interpolation * (next_abs - start_abs) + start_abs;
	float curr_angle = interpolation * (next_angle - start_angle) + start_angle;
	x = curr_abs * cosf(curr_angle);
	y = curr_abs * sinf(curr_angle);
}

void mandelbrot_generator::linear_short_angle_and_abs(float start_x, float start_y,
	float goal_x, float goal_y, float interpolation, float & x, float & y)
{
	float start_abs = sqrtf(start_x * start_x + start_y * start_y);
	float next_abs = sqrtf(goal_x * goal_x + goal_y * goal_y);
	float start_angle = geo_util::atan2(start_y, start_x);
	float next_angle = geo_util::atan2(goal_y, goal_x);
	if (next_angle - start_angle > geo_constants::pi) {//go shorter way for angle
		next_angle -= geo_constants::double_pi;
	}
	float curr_abs = interpolation * (next_abs - start_abs) + start_abs;
	float curr_angle = interpolation * (next_angle - start_angle) + start_angle;
	x = curr_abs * cosf(curr_angle);
	y = curr_abs * sinf(curr_angle);
}

void mandelbrot_generator::polynomial(float start_x, float start_y,
	float goal_x, float goal_y, float interpolation, float & x, float & y)
{
	complex_pow_real(start_x, start_y, 2.f / (2.f - interpolation), x, y);
	x += interpolation * (goal_x + start_y*start_y - start_x * start_x);//re_part of c
	y += interpolation * (goal_y - 2.f * start_x * start_y);			//im_part of c
}

void mandelbrot_generator::linear_xy(float start_x, float start_y,
	float goal_x, float goal_y, float interpolation, float & x, float & y)
{
	x = start_x + interpolation * (goal_x - start_x);
	y = start_y + interpolation * (goal_y - start_y);
}

auto_pointer<image> mandelbrot_generator::generate_mandelbrot_image_julia_iter(const parameter_set& params)
{
	auto_pointer<image> ret(new image(params.image_dimensions_));

	/*computation according to https://de.wikipedia.org/wiki/Mandelbrot-Menge#Programmbeispiel */
	for (int i = 0; i < ret->size().x * ret->size().y; i++) {
		int x_coordinate = i % ret->size().x;
		int y_coordinate = i / ret->size().x;
		float im_part = params.imaginary_min_ + (params.imaginary_max_ - params.imaginary_min_) * y_coordinate / ret->size().y;
		float re_part = params.real_min_ + (params.real_max_ - params.real_min_) * x_coordinate / ret->size().x;

		int remain_iter = params.max_iter_;
		float xx = re_part * re_part;
		float yy = im_part * im_part;
		float xy = re_part * im_part;
		float abs_2 = xx + yy;

		while (abs_2 <= params.max_threshold_ && remain_iter > 0) {
			remain_iter--;
			float x = xx - yy + re_part;
			float y = xy + xy + im_part;
			xx = x*x;
			yy = y*y;
			xy = x*y;
			abs_2 = xx + yy;
		}

		int julia_iter = params.max_iter_ - remain_iter;

		if (remain_iter == 0) ret->data()[i * 4] = ret->data()[i * 4 + 1] = ret->data()[i * 4 + 2] = 0;
		else {
			float h_value = (float)julia_iter / (float)params.max_iter_ + params.hsv_color_offset_;
			h_value = h_value - (int)h_value;//mod h_value
			hsv_to_rgb(h_value, 1.f, 1.f, ret->data()[i * 4], ret->data()[i * 4 + 1], ret->data()[i * 4 + 2]);
		}

		ret->data()[i * 4 + 3] = 255;//Alpha-value
	}
	return ret;
}

viral_core::auto_pointer<viral_core::image> mandelbrot_generator::generate_mandelbrot_image_julia_value(
	const parameter_set& params)
{
	//LOG_INFO(string("compuatation with iteration: ") + params.iterations_ + 
		//string(" and interpolation: ") + params.interpolation_);
	auto_pointer<image> ret(new image(params.image_dimensions_));

	/*computation according to https://de.wikipedia.org/wiki/Mandelbrot-Menge#Programmbeispiel */
	for (int i = 0; i < ret->size().x * ret->size().y; i++) {
		int x_coordinate = i % ret->size().x;
		int y_coordinate = i / ret->size().x;
		float re_part = params.real_min_ + (params.real_max_ - params.real_min_) * x_coordinate / ret->size().x;
		float im_part = params.imaginary_min_ 
			+ (params.imaginary_max_ - params.imaginary_min_) * y_coordinate / ret->size().y;

		float x = re_part;
		float y = im_part;
		float xx = x * x;
		float yy = y * y;
		float xy = x * y;

		for (int j = 0; j < params.iterations_; j++) {
			x = xx - yy + re_part;
			y = xy + xy + im_part;
			xx = x*x;
			yy = y*y;
			xy = x*y;
		}
		/*interpolation*/
		if (params.interpolate_) {
			float start_x = x;
			float start_y = y;
			float goal_x = xx - yy + re_part;
			float goal_y = xy + xy + im_part;
			(*params.interpolation_method_)(start_x, start_y, goal_x, goal_y, params.interpolation_, x, y);
			xx = x*x;
			yy = y*y;
			xy = x*y;
		}

		float v = 1.f / (1.f + 5.f*sqrtf(xx + yy));
		float h_degree = geo_util::atan2(y, x) * geo_constants::radians_to_degrees;
		while (h_degree < 0.0f) h_degree += 360.f;

		//rgb_project_2d(h_degree / 360.f, v, ret->data()[i * 4], ret->data()[i * 4 + 1], ret->data()[i * 4 + 2]);
		hsl_to_rgb(h_degree / 360.f, 1.f, v, ret->data()[i * 4], ret->data()[i * 4 + 1], ret->data()[i * 4 + 2]);


		ret->data()[i * 4 + 3] = 255;//Alpha-value
	}
	return ret;
}

void mandelbrot_generator::hsv_to_rgb(float h, float s, float v, 
	unsigned char & r_out, unsigned char & g_out, unsigned char & b_out)
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
		r_out = (unsigned char)(v * 255.f);
		g_out = (unsigned char)(t * 255.f);
		b_out = (unsigned char)(p * 255.f);
		break;
	case 1:
		r_out = (unsigned char)(q * 255.f);
		g_out = (unsigned char)(v * 255.f);
		b_out = (unsigned char)(p * 255.f);
		break;
	case 2:
		r_out = (unsigned char)(p * 255.f);
		g_out = (unsigned char)(v * 255.f);
		b_out = (unsigned char)(t * 255.f);
		break;

	case 3:
		r_out = (unsigned char)(p * 255.f);
		g_out = (unsigned char)(q * 255.f);
		b_out = (unsigned char)(v * 255.f);
		break;
	case 4:
		r_out = (unsigned char)(t * 255.f);
		g_out = (unsigned char)(p * 255.f);
		b_out = (unsigned char)(v * 255.f);
		break;
	case 5:
	default:
		r_out = (unsigned char)(v * 255.f);
		g_out = (unsigned char)(p * 255.f);
		b_out = (unsigned char)(q * 255.f);
		break;
	}
}

void mandelbrot_generator::hsl_to_rgb(float h, float s, float l, 
	unsigned char & r_out, unsigned char & g_out, unsigned char & b_out)
{
	/*implementation from http://forums.codeguru.com/showthread.php?421328-RGB-to-HSL-and-vice-versa */
	r_out = 0;
	g_out = 0;
	b_out = 0;

	if (s <= 0.f)
	{
		r_out = (unsigned char)l * 255.f;
		g_out = (unsigned char)l * 255.f;
		b_out = (unsigned char)l * 255.f;
	}
	else
	{
		double temp1 = 0;
		if (l < .5f)
		{
			temp1 = l*(1.f + s);
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
	r_out = (unsigned char)((((float)r_out) / 100.f) * 255.f);
	g_out = (unsigned char)((((float)g_out) / 100.f) * 255.f);
	b_out = (unsigned char)((((float)b_out) / 100.f) * 255.f);
}

void mandelbrot_generator::rgb_project_2d(float angle, float absolute, 
	unsigned char & r_out, unsigned char & g_out, unsigned char & b_out)
{
	angle = 2.f * geo_constants::pi * angle;
	r_out = (unsigned char) (sinf(angle)*absolute *255.f);
	g_out = 0;
	b_out = (unsigned char)(cosf(angle)*absolute*255.f);
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
