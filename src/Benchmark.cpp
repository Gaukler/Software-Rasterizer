#include "pch.h"
#include "RasterizationPublic.h"

void benchmark(const Mesh* mesh, const RenderSettings& settings, RenderTarget& renderTarget) {
	const auto startTime = std::chrono::high_resolution_clock::now();

	std::chrono::microseconds duration = std::chrono::microseconds::zero();

	//On tested Ryzen: due to power management the cpu needs time to clock to full speed
	//do one test run then wait for a second, this leads to much more consisten results
	drawTriangles(mesh->triangles, renderTarget, settings);
	std::this_thread::sleep_for(std::chrono::seconds(1));

	const uint32_t nIterations = 10;
	for (uint32_t i = 0; i < nIterations; i++) {
		const auto startTimeLoop = std::chrono::high_resolution_clock::now();

		drawTriangles(mesh->triangles, renderTarget, settings);

		const auto endTime = std::chrono::high_resolution_clock::now();
		std::chrono::microseconds durationLoop = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTimeLoop);
		std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(durationLoop).count() << " ms" << std::endl;

		duration += durationLoop;

	}
	duration /= nIterations;
	std::cout << "Average Render time: " << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << " ms" << std::endl;
}