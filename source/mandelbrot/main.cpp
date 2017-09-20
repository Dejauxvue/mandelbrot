#include <viral_core/file_util.hpp>

#include "mandelbrot_gui.hpp"

using namespace viral_core;
using namespace viral_gui;

int main() {
	//auto image = mandelbrot_generator::generate_mandelbrot_image_julia_iter(vector2i(1920, 1080), 0.1f, -2.f, -1.2f, 1.f, 1.2f, 20.f, 2000);
	//disk_file img_file("mandelbrot_output.png", file::file_operation::read_write_truncate);
	//image->save_png(img_file);
		gui_app::run<mandelbrot_gui>();
		return 0;
}