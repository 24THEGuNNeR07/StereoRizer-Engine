#pragma once

#include <iostream>
#include <glm/glm.hpp>

#include <graphics/Transform.h>
#include <graphics/VertexArray.h>
#include <graphics/VertexBuffer.h>
#include <graphics/ElementBuffer.h>
#include <graphics/Material.h>
#include <graphics/Camera.h>

#include <unordered_map>
#include <string>

namespace stereorizer::graphics
{
	struct RenderObjectData
	{
		const BufferAccessType accessType;
		const BufferCallType callType;

		DrawType drawType;

		RenderObjectData(BufferAccessType accessType = BufferAccessType::STATIC, BufferCallType callType = BufferCallType::DRAW, DrawType drawType = DrawType::TRIANGLES);
	};

	class RenderObject : public Transform
	{

	private:
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 invModel = glm::mat4(-1.0f);
		glm::mat4 transInvModel = glm::mat4(-1.0f);
	protected:
		VertexBuffer* vtxBuffer = nullptr;
		VertexArray* vtxArray = nullptr;
		ElementBuffer* elementBuffer = nullptr;
	public:
		Material* const material;
		RenderObjectData objectParams;
		const uint32_t rObjectID;
		RenderObject(uint32_t id, RenderObjectData objectParams, Material* const material);
		RenderObject(uint32_t id, const RenderObject& from);

		virtual void initObject() = 0;

		bool hasElementBuffer() const;

		const glm::mat4& getModel(bool forcedCalculate = false);
		void updateMVP(FreeCamera& camera);

		void draw(FreeCamera& camera);
		void draw(FreeCamera& camera, Shader& shader);

		const VertexBuffer* getVertexBuffer();
		const VertexArray* getVertexArray();
		const ElementBuffer* getElementBuffer();
	};

	class SimpleRenderObject : public RenderObject
	{
	private:
		std::vector<float> vertices;
		std::vector<uint32_t> attributeSizes, indices;
	public:
		SimpleRenderObject(uint32_t id, RenderObjectData objectParams, Material* const material, std::vector<float> vertices, std::vector<uint32_t> attributeSizes);
		SimpleRenderObject(uint32_t id, RenderObjectData objectParams, Material* const material, std::vector<float> vertices, std::vector<uint32_t> attributeSizes, std::vector<uint32_t> indices);
		SimpleRenderObject(uint32_t id, const SimpleRenderObject& from);
		virtual void initObject() override;
		const std::vector<float>& getVertices();
		const std::vector<uint32_t>& getAttributeSizes();
		const std::vector<uint32_t>& getIndices();
	};
}