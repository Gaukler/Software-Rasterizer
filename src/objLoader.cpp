#include "pch.h"
#include "objLoader.h"

namespace objLoader {

	struct TriangleIndices {
		size_t positionIndices[3];
		size_t normalIndices[3];
		size_t uvIndices[3];
	};

	Mesh* loadOBJ(const std::string& path) {
		std::ifstream file(path, std::ios::binary);
		if (!file.is_open()) {
			std::cout << "Unable to open file " << path << std::endl;
			return nullptr;
		}
		std::string line;
		std::vector<cml::vec3> positionList;
		std::vector<cml::vec3> normalList;
		std::vector<cml::vec2> uvList;
		std::vector<TriangleIndices> triangleIndexList;
		while (std::getline(file, line)) {
			if (line.substr(0, 2) == "v ") {
				cml::vec3 position;
				size_t end = 2;
				for (int i = 0; i < 3; i++) {
					size_t start = line.find_first_not_of(" ", end);
					end = line.find_first_of(" ", start + 1);
					position[i] = std::stof(line.substr(start, end - start));
				}
				positionList.push_back(position);
			}
			else if (line.substr(0, 2) == "vt") {
				cml::vec2 uv;
				size_t end = 2;
				for (int i = 0; i < 2; i++) {
					size_t start = line.find_first_not_of(" ", end);
					end = line.find_first_of(" ", start + 1);
					uv[i] = std::stof(line.substr(start, end - start));
				}
				uvList.push_back(uv);
			}
			else if (line.substr(0, 2) == "vn") {
				cml::vec3 normal;
				size_t end = 2;
				for (int i = 0; i < 3; i++) {
					size_t start = line.find_first_not_of(" ", end);
					end = line.find_first_of(" ", start + 1);
					normal[i] = std::stof(line.substr(start, end - start));
				}
				normalList.push_back(normal);
			}
			else if (line.substr(0, 2) == "f ") {
				size_t end = 1;
				size_t start = line.find_first_not_of(" ", end);
				TriangleIndices triangleIndices;
				for (size_t i = 0; i < 3; i++) {
					end = std::min(line.find_first_of(" ", start + 1), line.find_first_of("/", start + 1));
					std::string indexString = line.substr(start, end - start);
					triangleIndices.positionIndices[i] = (size_t)(std::stoi(indexString)) - 1;
					start = end + 1;

					end = std::min(line.find_first_of(" ", start + 1), line.find_first_of("/", start + 1));
					indexString = line.substr(start, end - start);
					triangleIndices.uvIndices[i] = (size_t)(std::stoi(indexString)) - 1;
					start = end + 1;

					end = std::min(line.find_first_of(" ", start + 1), line.find_first_of("/", start + 1));
					indexString = line.substr(start, end - start);
					triangleIndices.normalIndices[i] = (size_t)(std::stoi(indexString)) - 1;
					start = end + 1;
				}
				triangleIndexList.push_back(triangleIndices);
			}
		}

		Mesh* mesh = new Mesh;
		for (size_t i = 0; i < triangleIndexList.size(); i++) {
			TriangleIndices indices = triangleIndexList[i];
			Triangle t;

			t.v1.position = positionList[indices.positionIndices[0]];
			t.v2.position = positionList[indices.positionIndices[1]];
			t.v3.position = positionList[indices.positionIndices[2]];

			t.v1.normal = normalList[indices.normalIndices[0]];
			t.v2.normal = normalList[indices.normalIndices[1]];
			t.v3.normal = normalList[indices.normalIndices[2]];

			t.v1.uv = uvList[indices.uvIndices[0]];
			t.v2.uv = uvList[indices.uvIndices[1]];
			t.v3.uv = uvList[indices.uvIndices[2]];

			mesh->triangles.push_back(t);
		}

		return mesh;
	}
}