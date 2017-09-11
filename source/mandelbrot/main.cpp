#include "mandelbrot_generator.hpp"

#include <viral_core/file_util.hpp>

using namespace viral_core;
int main() {
	auto image = mandelbrot_generator::generate_mandelbrot_image(vector2i(1920, 1080), 0.1f, -2.f, -1.2f, 1.f, 1.2f, 20.f, 2000);
	disk_file img_file("mandelbrot_output.png", file::file_operation::read_write_truncate);
	image->save_png(img_file);
}