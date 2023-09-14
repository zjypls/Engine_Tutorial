//
// Created by z on 2023/9/14.
//
#include <filesystem>
#include "Include/tinyobjloader/tiny_obj_loader.h"

#include "Z/Utils/Model.h"


namespace std {
	template<>
	struct hash<Z::Vertex> {
		size_t operator()(const Z::Vertex &v) const{
			return hash<glm::vec3>()(v.verts) << 1
			       | hash<glm::vec3>()(v.normal)
			       | hash<glm::vec2>()(v.tex);
		}
	};
}

namespace Z {
	//TODO:move to assets system
	Ref<Mesh> Mesh::LoadMesh(const std::filesystem::path &path) {
		auto mesh = CreateRef<Mesh>();
		tinyobj::attrib_t attrib{};
		std::vector<tinyobj::shape_t> shapes{};
		std::vector<tinyobj::material_t> materials;
		std::string warnings,errors;
		if(!tinyobj::LoadObj(&attrib,&shapes,&materials,&warnings,&errors,path.string().c_str())){
			Z_CORE_WARN("Loader give warning : {0}",warnings);
			Z_CORE_ERROR("Loader give errors : {0}",errors);
			return mesh;
		}
		std::unordered_map<Vertex,uint32_t> Vertexes{};
		std::vector<Vertex> vertexes;
		std::vector<uint32_t> indexes;
		for(const auto&shape:shapes){
			for(const auto&indices:shape.mesh.indices){
				Vertex vert{};
				vert.verts={
						attrib.vertices[indices.vertex_index*3+0],
						attrib.vertices[indices.vertex_index*3+1],
						attrib.vertices[indices.vertex_index*3+2]};
				vert.tex={
						attrib.texcoords[indices.texcoord_index*2+0],
						attrib.texcoords[indices.texcoord_index*2+1]};
				vert.normal={
						attrib.normals[indices.normal_index*3+0],
						attrib.normals[indices.normal_index*3+1],
						attrib.normals[indices.normal_index*3+2]};
				if(Vertexes.count(vert)==0){
					Vertexes[vert]=vertexes.size();
					vertexes.push_back(vert);
				}
				indexes.push_back(Vertexes.size());
			}
		}
		auto vertexBuffer=VertexBuffer::Create((float*)vertexes.data(),vertexes.size()*sizeof(Vertex));
		vertexBuffer->SetLayout(BufferLayout{
				{Z::ShaderDataType::Float3,"vertex"},
				{Z::ShaderDataType::Float2,"texCoord"},
				{Z::ShaderDataType::Float3,"normal"}
		});

		auto indexBuffer=IndexBuffer::Create(indexes.data(),indexes.size());
		mesh->vertexArray=VertexArray::Create();
		mesh->vertexArray->AddVertexBuffer(vertexBuffer);
		mesh->vertexArray->SetIndexBuffer(indexBuffer);
		return mesh;
	}

}
