/**
 * 3D Foundation Project
 * Copyright 2019 Smithsonian Institution
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "Options.h"
#include <iostream>

using namespace meshsmith;
using namespace flow;
using std::string;

Options::Options() :
	verbose(false),
	report(false),
	list(false),
	joinVertices(false),
	stripNormals(false),
	stripTexCoords(false),
	scale(1.0f),
	translate(0.0f, 0.0f, 0.0f),
	alignX(Align::None),
	alignY(Align::None),
	alignZ(Align::None),
	flipUV(false),
	useCompression(false),
	objectSpaceNormals(false),
	embedMaps(false),
	compressionLevel(7),
	positionQuantizationBits(14),
	texCoordsQuantizationBits(12),
	normalsQuantizationBits(10),
	genericQuantizationBits(8)
{
	matrix.setIdentity();
}

Result Options::fromJSON(const flow::json& opts)
{
	try {
		input = opts.count("input") ? opts.at("input").get<string>() : string{};
		output = opts.count("output") ? opts.at("output").get<string>() : string{};
		format = opts.count("format") ? opts.at("format").get<string>() : string{};
		verbose = opts.count("verbose") ? opts.at("verbose").get<bool>() : false;
		report = opts.count("report") ? opts.at("report").get<bool>() : false;
		list = opts.count("list") ? opts.at("list").get<bool>() : false;
		joinVertices = opts.count("joinVertices") ? opts.at("joinVertices").get<bool>() : false;
		stripNormals = opts.count("stripNormals") ? opts.at("stripNormals").get<bool>() : false;
		stripTexCoords = opts.count("stripTexCoords") ? opts.at("stripTexCoords").get<bool>() : false;
		swizzle = opts.count("swizzle") ? opts.at("swizzle").get<string>() : string{};
		scale = opts.count("scale") ? opts.at("scale").get<float>() : 1.0f;
		flipUV = opts.count("flipUV") ? opts.at("flipUV").get<bool>() : false;

		if (opts.count("translate")) {
			auto t = opts.at("translate");
			translate.x = t.at(0);
			translate.y = t.at(1);
			translate.z = t.at(2);
		}

		if (opts.count("matrix")) {
			auto t = opts.at("matrix");
			for (size_t i = 0, col = 0; col < 4; ++col) {
				for (size_t row = 0; row < 4; ++row, ++i) {
					matrix[row][col] = t.at(i);
				}
			}
		}

		if (opts.count("alignX")) {
			int align = opts.at("alignX").get<int>();
			alignX = align < 0 ? Align::Start : (align > 0 ? Align::End : Align::Center);
		}
		else {
			alignX = Align::None;
		}

		if (opts.count("alignY")) {
			int align = opts.at("alignY").get<int>();
			alignY = align < 0 ? Align::Start : (align > 0 ? Align::End : Align::Center);
		}
		else {
			alignY = Align::None;
		}

		if (opts.count("alignZ")) {
			int align = opts.at("alignZ").get<int>();
			alignZ = align < 0 ? Align::Start : (align > 0 ? Align::End : Align::Center);
		}
		else {
			alignZ = Align::None;
		}

		if (opts.count("gltfx")) {
			auto gltfx = opts["gltfx"];
			metallicFactor = gltfx.count("metallicFactor") ? gltfx.at("metallicFactor").get<float>() : 0.1f;
			roughnessFactor = gltfx.count("roughnessFactor") ? gltfx.at("roughnessFactor").get<float>() : 0.8f;
			diffuseMap = gltfx.count("diffuseMap") ? gltfx.at("diffuseMap").get<string>() : string{};
			occlusionMap = gltfx.count("occlusionMap") ? gltfx.at("occlusionMap").get<string>() : string{};
			emissiveMap = gltfx.count("emissiveMap") ? gltfx.at("emissiveMap").get<string>() : string{};
			metallicRoughnessMap = gltfx.count("metallicRoughnessMap") ? gltfx.at("metallicRoughnessMap").get<string>() : string{};
			zoneMap = gltfx.count("zoneMap") ? gltfx.at("zoneMap").get<string>() : string{};
			normalMap = gltfx.count("normalMap") ? gltfx.at("normalMap").get<string>() : string{};
			objectSpaceNormals = gltfx.count("objectSpaceNormals") ? gltfx.at("objectSpaceNormals").get<bool>() : false;
			embedMaps = gltfx.count("embedMaps") ? gltfx.at("embedMaps").get<bool>() : false;
			useCompression = gltfx.count("useCompression") ? gltfx.at("useCompression").get<bool>() : false;
		}

		if (opts.count("compression")) {
			auto cmp = opts["compression"];
			compressionLevel = cmp.count("compressionLevel") ? cmp.at("compressionLevel").get<uint32_t>() : 7;
			positionQuantizationBits = cmp.count("positionQuantizationBits") ? cmp.at("positionQuantizationBits").get<uint32_t>() : 14;
			texCoordsQuantizationBits = cmp.count("texCoordsQuantizationBits") ? cmp.at("texCoordsQuantizationBits").get<uint32_t>() : 12;
			normalsQuantizationBits = cmp.count("normalsQuantizationBits") ? cmp.at("normalsQuantizationBits").get<uint32_t>() : 10;
			genericQuantizationBits = cmp.count("genericQuantizationBits") ? cmp.at("genericQuantizationBits").get<uint32_t>() : 8;
		}
	}
	catch (const std::exception& e) {
		return Result::error(e.what());
	}

	return Result::ok();
}

json Options::toJSON() const
{
	json result = {
		{ "verbose", verbose },
	};

	if (!input.empty()) {
		result["input"] = input;
	}
	if (!output.empty()) {
		result["output"] = output;
	}
	if (!format.empty()) {
		result["format"] = format;
	}
	if (report) {
		result["report"] = report;
	}
	if (list) {
		result["list"] = list;
	}
	if (joinVertices) {
		result["joinVertices"] = joinVertices;
	}
	if (stripNormals) {
		result["stripNormals"] = stripNormals;
	}
	if (stripTexCoords) {
		result["stripTexCoords"] = stripTexCoords;
	}
	if (!swizzle.empty()) {
		result["swizzle"] = swizzle;
	}
	if (scale != 1.0f) {
		result["scale"] = scale;
	}
	if (!translate.allZero()) {
		result["translate"] = translate.toJSON();
	}
	if (!matrix.isIdentity()) {
		result["matrix"] = matrix.toJSON(Matrix4f::ColumnMajor);
	}

	json gltfx;
	if (useCompression) {
		gltfx["useCompression"] = useCompression;
	}
	if (!diffuseMap.empty()) {
		gltfx["diffuseMap"] = diffuseMap;
	}
	if (!occlusionMap.empty()) {
		gltfx["occlusionMap"] = occlusionMap;
	}
	if (!normalMap.empty()) {
		gltfx["normalMap"] = normalMap;
	}
	if (objectSpaceNormals) {
		gltfx["objectSpaceNormals"] = objectSpaceNormals;
	}
	if (embedMaps) {
		gltfx["embedMaps"] = true;
	}

	if (!gltfx.empty()) {
		result["gltfx"] = gltfx;
	}

	if (useCompression) {
		json compression = {
			{ "compressionLevel", compressionLevel }
		};

		if (positionQuantizationBits > 0) {
			compression["positionQuantizationBits"] = positionQuantizationBits;
		}
		if (texCoordsQuantizationBits > 0) {
			compression["texCoordsQuantizationBits"] = texCoordsQuantizationBits;
		}
		if (normalsQuantizationBits > 0) {
			compression["normalsQuantizationBits"] = normalsQuantizationBits;
		}
		if (genericQuantizationBits > 0) {
			compression["genericQuantizationBits"] = genericQuantizationBits;
		}

		result["compression"] = compression;
	}

	return result;
}
