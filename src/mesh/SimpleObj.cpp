#include "mesh/SimpleObj.h"

#include <algorithm>
#include <fstream>
#include <sstream>

namespace {

    struct Vec3f {
        float x;
        float y;
        float z;
    };

    int parseObjVertexIndex(const std::string &token, int positionCount) {
        // token: "12" or "12/34" or "12/34/56" — 只取第一个整数（位置索引），OBJ 为 1-based
        std::string num;
        for (char c: token) {
            if (c == '/') {
                break;
            }
            num.push_back(c);
        }
        if (num.empty()) {
            return 0;
        }
        const int v = std::stoi(num);
        if (v > 0) {
            return v - 1;
        }
        return positionCount + v;
    }

    void buildColorsFromBBox(const std::vector<Vec3f> &pos, std::vector<float> &interleaved) {
        interleaved.clear();
        interleaved.reserve(pos.size() * 6U);

        if (pos.empty()) {
            return;
        }

        float minX = pos[0].x;
        float minY = pos[0].y;
        float minZ = pos[0].z;
        float maxX = minX;
        float maxY = minY;
        float maxZ = minZ;
        for (const Vec3f &p: pos) {
            minX = std::min(minX, p.x);
            minY = std::min(minY, p.y);
            minZ = std::min(minZ, p.z);
            maxX = std::max(maxX, p.x);
            maxY = std::max(maxY, p.y);
            maxZ = std::max(maxZ, p.z);
        }

        const float sx = maxX - minX;
        const float sy = maxY - minY;
        const float sz = maxZ - minZ;
        const float eps = 1e-6f;
        const float invX = (sx > eps) ? 1.0f / sx : 0.0f;
        const float invY = (sy > eps) ? 1.0f / sy : 0.0f;
        const float invZ = (sz > eps) ? 1.0f / sz : 0.0f;

        for (const Vec3f &p: pos) {
            interleaved.push_back(p.x);
            interleaved.push_back(p.y);
            interleaved.push_back(p.z);
            if (sx <= eps && sy <= eps && sz <= eps) {
                interleaved.push_back(0.55f);
                interleaved.push_back(0.55f);
                interleaved.push_back(0.75f);
            } else {
                interleaved.push_back((p.x - minX) * invX);
                interleaved.push_back((p.y - minY) * invY);
                interleaved.push_back((p.z - minZ) * invZ);
            }
        }
    }

}

bool loadSimpleObjFile(const std::string &path, SimpleObjMesh &out) {
    out = SimpleObjMesh{};
    std::ifstream in(path);
    if (!in) {
        return false;
    }

    std::vector<Vec3f> positions;
    std::vector<unsigned int> indices;

    std::string line;
    while (std::getline(in, line)) {
        if (line.empty()) {
            continue;
        }
        size_t start = 0;
        while (start < line.size() && (line[start] == ' ' || line[start] == '\t')) {
            ++start;
        }
        if (start >= line.size() || line[start] == '#') {
            continue;
        }

        std::istringstream iss(line.substr(start));
        std::string cmd;
        iss >> cmd;
        if (cmd == "v") {
            Vec3f p{};
            if (!(iss >> p.x >> p.y >> p.z)) {
                continue;
            }
            positions.push_back(p);
        } else if (cmd == "f") {
            std::vector<int> corner;
            std::string tok;
            while (iss >> tok) {
                try {
                    const int idx = parseObjVertexIndex(tok, static_cast<int>(positions.size()));
                    if (idx < 0 || idx >= static_cast<int>(positions.size())) {
                        continue;
                    }
                    corner.push_back(idx);
                } catch (...) {
                    continue;
                }
            }
            if (corner.size() < 3) {
                continue;
            }
            // 三角扇： (0,1,2), (0,2,3), ...
            for (size_t i = 1; i + 1 < corner.size(); ++i) {
                indices.push_back(static_cast<unsigned int>(corner[0]));
                indices.push_back(static_cast<unsigned int>(corner[i]));
                indices.push_back(static_cast<unsigned int>(corner[i + 1]));
            }
        }
    }

    if (positions.empty() || indices.empty()) {
        return false;
    }

    buildColorsFromBBox(positions, out.vertices);
    out.indices = std::move(indices);
    out.valid = true;
    return true;
}
