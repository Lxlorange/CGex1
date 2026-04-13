#pragma once

#include <string>
#include <vector>

// 按行读字符串，只认 v（位置）与 f（面索引）。
// f 支持 "1 2 3"、"1/2 3/4 5/6"、"1/2/3 ..."；多边形按三角扇剖分为三角形。

struct SimpleObjMesh {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    bool valid = false;
};

bool loadSimpleObjFile(const std::string& path, SimpleObjMesh& out);
